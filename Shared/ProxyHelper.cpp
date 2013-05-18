/********************************************************************
Vireio Perception: Open-Source Stereoscopic 3D Driver
Copyright (C) 2012 Andres Hernandez

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
********************************************************************/

#include "ProxyHelper.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <windows.h>

using namespace pugi;

HRESULT RegGetString(HKEY hKey, LPCTSTR szValueName, LPTSTR * lpszResult);

void Log( const char* str )
{
    OutputDebugString(str);
}

void LogF( const char* szFormat, ... )
{
	char szBuff[1024];
	va_list arg;
	va_start(arg, szFormat);
	_vsnprintf_s(szBuff, _TRUNCATE, szFormat, arg);
	va_end(arg);

    Log(szBuff);
}

HRESULT RegGetString(HKEY hKey, LPCTSTR szValueName, LPTSTR * lpszResult) {
 
    // Given a HKEY and value name returns a string from the registry.
    // Upon successful return the string should be freed using free()
    // eg. RegGetString(hKey, TEXT("my value"), &szString);
 
    DWORD dwType=0, dwDataSize=0, dwBufSize=0;
    LONG lResult;
 
    // Incase we fail set the return string to null...
    if (lpszResult != NULL) *lpszResult = NULL;
 
    // Check input parameters...
    if (hKey == NULL || lpszResult == NULL) return E_INVALIDARG;
 
    // Get the length of the string in bytes (placed in dwDataSize)...
    lResult = RegQueryValueEx(hKey, szValueName, 0, &dwType, NULL, &dwDataSize );
 
    // Check result and make sure the registry value is a string(REG_SZ)...
    if (lResult != ERROR_SUCCESS) return HRESULT_FROM_WIN32(lResult);
    else if (dwType != REG_SZ)    return DISP_E_TYPEMISMATCH;
 
    // Allocate memory for string - We add space for a null terminating character...
    dwBufSize = dwDataSize + (1 * sizeof(TCHAR));
    *lpszResult = (LPTSTR)malloc(dwBufSize);
 
    if (*lpszResult == NULL) return E_OUTOFMEMORY;
 
    // Now get the actual string from the registry...
    lResult = RegQueryValueEx(hKey, szValueName, 0, &dwType, (LPBYTE) *lpszResult, &dwDataSize );
 
    // Check result and type again.
    // If we fail here we must free the memory we allocated...
    if (lResult != ERROR_SUCCESS) { free(*lpszResult); return HRESULT_FROM_WIN32(lResult); }
    else if (dwType != REG_SZ)    { free(*lpszResult); return DISP_E_TYPEMISMATCH; }
 
    // We are not guaranteed a null terminated string from RegQueryValueEx.
    // Explicitly null terminate the returned string...
    (*lpszResult)[(dwBufSize / sizeof(TCHAR)) - 1] = TEXT('\0');
 
    return NOERROR;
}

ProxyHelper::ProxyHelper()
	: baseDirLoaded(false)
{
}

char* ProxyHelper::GetBaseDir()
{
	if (baseDirLoaded == true)
	{
		return baseDir;
	}

	HKEY hKey;
	LPCTSTR sk = TEXT("SOFTWARE\\Vireio\\Perception");

	LONG openRes = RegOpenKeyEx(HKEY_CURRENT_USER, sk, 0, KEY_QUERY_VALUE , &hKey);

	if (openRes != ERROR_SUCCESS) 
	{
		return "";
	}

	HRESULT hr = RegGetString(hKey, TEXT("BasePath"), &baseDir);
	if (FAILED(hr)) 
	{
		return "";
	} 
	else 
	{
		baseDirLoaded = true;
	}

	return baseDir;
}

char* ProxyHelper::GetTargetExe()
{
	HKEY hKey;
	LPCTSTR sk = TEXT("SOFTWARE\\Vireio\\Perception");

	LONG openRes = RegOpenKeyEx(HKEY_CURRENT_USER, sk, 0, KEY_QUERY_VALUE , &hKey);

	if (openRes != ERROR_SUCCESS) 
	{
		return "";
	}

	HRESULT hr = RegGetString(hKey, TEXT("TargetExe"), &targetExe);
	if (FAILED(hr)) 
	{
		return "";
	}

	return targetExe;
}

void ProxyHelper::GetPath(char* newFolder, char* path)
{
	strcpy_s(newFolder, 512, GetBaseDir());
	strcat_s(newFolder, 512, path);
}


bool ProxyHelper::LoadConfig(ProxyConfig& config)
{
	bool fileFound = false;

	// set defaults
	config.game_type = 0;
	config.stereo_mode = 0;
	config.tracker_mode = 0;
	config.separation = 0.0f;
	config.convergence = 0.0f;
	config.swap_eyes = false;
	config.aspect_multiplier = 1.0f;

	// load the base dir for the app
	GetBaseDir();
	LogF( "[ProxyHelper] Base directory for Vireio: %s", baseDir );
	// get global config
	char configPath[512];
	GetPath(configPath, "cfg\\config.xml");

	xml_document docConfig;
	xml_parse_result resultConfig = docConfig.load_file(configPath);

	if(resultConfig.status == status_ok)
	{
		xml_node xml_config = docConfig.child("config");

		config.stereo_mode = xml_config.attribute("stereo_mode").as_int();
		config.aspect_multiplier = xml_config.attribute("aspect_multiplier").as_float();
		config.tracker_mode = xml_config.attribute("tracker_mode").as_int();

		LogF( "[ProxyHelper] Loaded config.xml, stereo mode: %i", config.stereo_mode );

		fileFound = true;
	}
	else
	{
		Log( "[ProxyHelper] Failed to load config.xml!" );
	}
	
	// get the target exe
	GetTargetExe();

	std::pair<bool, xml_node> profile = GetProfile(targetExe);

	if (profile.first && profile.second)
	{
		xml_node game_profile = profile.second;

		config.game_type = game_profile.attribute("game_type").as_int();
		config.separation = game_profile.attribute("separation").as_float();
		config.convergence = game_profile.attribute("convergence").as_float();
		config.swap_eyes = game_profile.attribute("swap_eyes").as_bool();
		config.yaw_multiplier = game_profile.attribute("yaw_multiplier").as_float();
		config.pitch_multiplier = game_profile.attribute("pitch_multiplier").as_float();
		config.roll_multiplier = game_profile.attribute("roll_multiplier").as_float();
		config.proxy_enabled = game_profile.attribute("proxy_enabled").as_bool();

		strcpy_s( config.proxy_dll, game_profile.attribute("proxy_dll").as_string() );

		LogF( "[ProxyHelper] Loaded basic information from profile (game type %d)", config.game_type );
	}

	LoadUserConfig(config);

	return fileFound && profile.first;
}

std::pair<bool, xml_node> ProxyHelper::GetProfile(char* name)
{
	// get the profile
	bool profileFound = false;
	char profilePath[512];
	GetPath(profilePath, "cfg\\profiles.xml");

	xml_document docProfiles;
	xml_parse_result resultProfiles = docProfiles.load_file(profilePath);
	xml_node found_profile;

	if(resultProfiles.status == status_ok)
	{
		xml_node xml_profiles = docProfiles.child("profiles");

		for (xml_node profile = xml_profiles.child("profile"); profile; profile = profile.next_sibling("profile"))
		{
			if(strcmp(name, profile.attribute("game_exe").value()) == 0)
			{
				LogF( "[ProxyHelper] Found a game profile for %s", name );
				profileFound = true;
				found_profile = profile;
				break;
			}
		}
	}

	return std::make_pair( profileFound, found_profile );
}

bool ProxyHelper::SaveConfig(int mode, float aspect)
{
	// load the base dir for the app
	GetBaseDir();

	// get global config
	char configPath[512];
	GetPath(configPath, "cfg\\config.xml");

	xml_document docConfig;
	xml_parse_result resultConfig = docConfig.load_file(configPath);

	if(resultConfig.status == status_ok)
	{
		xml_node xml_config = docConfig.child("config");

		if(mode >= 0)
			xml_config.attribute("stereo_mode") = mode;

		if(aspect >= 0.0f)
			xml_config.attribute("aspect_multiplier") = aspect;

		docConfig.save_file(configPath);

		return true;
	}
	
	return false;
}

bool ProxyHelper::SaveConfig2(int mode)
{
	// load the base dir for the app
	GetBaseDir();

	// get global config
	char configPath[512];
	GetPath(configPath, "cfg\\config.xml");

	xml_document docConfig;
	xml_parse_result resultConfig = docConfig.load_file(configPath);

	if(resultConfig.status == status_ok)
	{
		xml_node xml_config = docConfig.child("config");

		if(mode >= 0)
			xml_config.attribute("tracker_mode") = mode;

		docConfig.save_file(configPath);

		return true;
	}
	
	return false;
}


bool ProxyHelper::LoadUserConfig(ProxyConfig& config)
{
	config.centerlineL = 0.0f;
	config.centerlineR = 0.0f;
	// get the user_profile
	bool userFound = false;
	char usersPath[512];
	GetPath(usersPath, "cfg\\users.xml");

	xml_document docUsers;
	xml_parse_result resultUsers = docUsers.load_file(usersPath);
	xml_node users;
	xml_node userProfile;

	if(resultUsers.status == status_ok)
	{
		xml_node xml_user_profiles = docUsers.child("users");

		for (xml_node user_profile = xml_user_profiles.child("user"); user_profile; user_profile = user_profile.next_sibling("user"))
		{
			if(strcmp("default", user_profile.attribute("user_name").value()) == 0)
			{
				LogF( "[ProxyHelper] Loading specific user (name: %s)", user_profile.attribute("user_name").value() );
				userProfile = user_profile;
				userFound = true;
				config.centerlineL = userProfile.attribute("centerlineL").as_float();
				config.centerlineR = userProfile.attribute("centerlineR").as_float();
				break;
			}
		}
	}
	return userFound;
}

bool ProxyHelper::SaveUserConfig(float centerlineL, float centerlineR)
{
	// get the profile
	bool profileFound = false;
	bool profileSaved = false;
	char profilePath[512];
	GetPath(profilePath, "cfg\\users.xml");

	xml_document docProfiles;
	xml_parse_result resultProfiles = docProfiles.load_file(profilePath);
	xml_node profile;
	xml_node gameProfile;

	if(resultProfiles.status == status_ok)
	{
		xml_node xml_profiles = docProfiles.child("users");

		for (xml_node profile = xml_profiles.child("user"); profile; profile = profile.next_sibling("user"))
		{
			if(strcmp("default", profile.attribute("user_name").value()) == 0)
			{
				LogF( "[ProxyHelper] Found specific user (name: %s)", profile.attribute("user_name").value() );
				gameProfile = profile;
				profileFound = true;
				break;
			}
		}
	}

	if(resultProfiles.status == status_ok && profileFound && gameProfile)
	{
		Log( "[ProxyHelper] Saved user settings to profile" );

		gameProfile.attribute("centerlineL") = centerlineL;
		gameProfile.attribute("centerlineR") = centerlineR;

		docProfiles.save_file(profilePath);

		profileSaved = true;
	}

	return profileSaved;
}


bool ProxyHelper::GetConfig(int& mode, int& mode2)
{
	// load the base dir for the app
	GetBaseDir();

	// get global config
	char configPath[512];
	GetPath(configPath, "cfg\\config.xml");

	xml_document docConfig;
	xml_parse_result resultConfig = docConfig.load_file(configPath);

	if(resultConfig.status == status_ok)
	{
		xml_node xml_config = docConfig.child("config");

		mode = xml_config.attribute("stereo_mode").as_int();
		mode2 = xml_config.attribute("tracker_mode").as_int();

		return true;
	}
	
	return false;
}

bool ProxyHelper::SaveProfile(float sep, float conv, bool swap, float yaw, float pitch, float roll)
{
	// get the target exe
	GetTargetExe();

	// get the profile
	bool profileFound = false;
	bool profileSaved = false;
	char profilePath[512];
	GetPath(profilePath, "cfg\\profiles.xml");

	std::pair<bool, xml_node> profile = GetProfile(targetExe);

	if (profile.first && profile.second)
	{
		xml_document docProfiles;
		docProfiles.load_file(profilePath);
		xml_node game_profile = profile.second;

		game_profile.attribute("separation") = sep;
		game_profile.attribute("convergence") = conv;
		game_profile.attribute("swap_eyes") = swap;
		game_profile.attribute("yaw_multiplier") = yaw;
		game_profile.attribute("pitch_multiplier") = pitch;
		game_profile.attribute("roll_multiplier") = roll;

		docProfiles.save_file(profilePath);

		profileSaved = true;
		LogF( "[ProxyHelper] Saved a game profile for %s", targetExe );
	}

	return profileSaved;
}