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

#include "KeyBindings.h"

#include "ProxyHelper.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <windows.h>


#include "pugixml.hpp"
using namespace pugi;
using namespace std;

#define KEY_DOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)
#define KEY_UP(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 0 : 1)


KeyBindings::KeyBindings()
{
	for(int i=0; i<COMMAND_SIZE; i++)
		keybind_list[i].vk = -1;

	VK_codes[VK_KEY_A] = "VK_KEY_A";
}

KeyBindings::~KeyBindings()
{
}

void KeyBindings::LoadXML()
{
	ProxyHelper* helper = new ProxyHelper();

	// get the user_profile
	bool userFound = false;
	char configPath[512];
	helper->GetPath(configPath, "cfg\\keybindings.xml");

	xml_document doc;
	xml_parse_result resultUsers = doc.load_file(configPath);

	KeyBinding keyB;

	if(resultUsers.status == status_ok)
	{
		xml_node xml_bindings = doc.child("bindings");

		for (xml_node xml_binding = xml_bindings.child("binding"); xml_binding; xml_binding = xml_binding.next_sibling("binding"))
		{

			const char_t* str = xml_binding.attribute("command").as_string();


			if(strcmp(str, "SAVE_SCREEN") == 0)
				keyB.command = SAVE_SCREEN;
			
			else if(strcmp(str, "SEPARATION_INC") == 0)
				keyB.command = SEPARATION_INC;

			else if(strcmp(str, "SEPARATION_DEC") == 0)
				keyB.command = SEPARATION_DEC;

			else if(strcmp(str, "CONVERGENCE_INC") == 0)
				keyB.command = CONVERGENCE_INC;

			else if(strcmp(str, "CONVERGENCE_DEC") == 0)
				keyB.command = CONVERGENCE_DEC;

			else if(strcmp(str, "OFFSET_INC") == 0)
				keyB.command = OFFSET_INC;

			else if(strcmp(str, "OFFSET_DEC") == 0)
				keyB.command = OFFSET_DEC;

			else if(strcmp(str, "SCREEN_OFFSET_INC") == 0)
				keyB.command = SCREEN_OFFSET_INC;

			else if(strcmp(str, "SCREEN_OFFSET_DEC") == 0)
				keyB.command = SCREEN_OFFSET_DEC;

			else if(strcmp(str, "DISTORTIONSCALE_INC") == 0)
				keyB.command = DISTORTIONSCALE_INC;

			else if(strcmp(str, "DISTORTIONSCALE_DEC") == 0)
				keyB.command = DISTORTIONSCALE_DEC;

			else if(strcmp(str, "SWAP_EYES") == 0)
				keyB.command = SWAP_EYES;

			else if(strcmp(str, "DEFAULT_VALUES") == 0)
				keyB.command = DEFAULT_VALUES;

			else if(strcmp(str, "PITCH_MUL_INC") == 0)
				keyB.command = PITCH_MUL_INC;

			else if(strcmp(str, "PITCH_MUL_DEC") == 0)
				keyB.command = PITCH_MUL_DEC;

			else if(strcmp(str, "YAW_MUL_INC") == 0)
				keyB.command = YAW_MUL_INC;

			else if(strcmp(str, "YAW_MUL_DEC") == 0)
				keyB.command = YAW_MUL_DEC;

			else if(strcmp(str, "ROLL_MUL_INC") == 0)
				keyB.command = ROLL_MUL_INC;

			else if(strcmp(str, "ROLL_MUL_DEC") == 0)
				keyB.command = ROLL_MUL_DEC;

			else if(strcmp(str, "STEREO_ENABLED_T") == 0)
				keyB.command = STEREO_ENABLED_T;

			else if(strcmp(str, "SHOCT_L_INC") == 0)
				keyB.command = SHOCT_L_INC;

			else if(strcmp(str, "SHOCT_L_DEC") == 0)
				keyB.command = SHOCT_L_DEC;

			else if(strcmp(str, "SHOCT_R_INC") == 0)
				keyB.command = SHOCT_R_INC;

			else if(strcmp(str, "SHOCT_R_DEC") == 0)
				keyB.command = SHOCT_R_DEC;

			else if(strcmp(str, "SHOCT_MODE_T") == 0)
				keyB.command = SHOCT_MODE_T;



			const char_t* vk_str = xml_binding.attribute("vk").as_string();

			if(strcmp(vk_str, "VK_KEY_0") == 0){	keyB.vk = VK_KEY_0; goto vk_mods; }
			if(strcmp(vk_str, "VK_KEY_1") == 0){	keyB.vk = VK_KEY_1; goto vk_mods; }
			if(strcmp(vk_str, "VK_KEY_2") == 0){	keyB.vk = VK_KEY_2; goto vk_mods; }
			if(strcmp(vk_str, "VK_KEY_3") == 0){	keyB.vk = VK_KEY_3; goto vk_mods; }
			if(strcmp(vk_str, "VK_KEY_4") == 0){	keyB.vk = VK_KEY_4; goto vk_mods; }
			if(strcmp(vk_str, "VK_KEY_5") == 0){	keyB.vk = VK_KEY_5; goto vk_mods; }
			if(strcmp(vk_str, "VK_KEY_6") == 0){	keyB.vk = VK_KEY_6; goto vk_mods; }
			if(strcmp(vk_str, "VK_KEY_7") == 0){	keyB.vk = VK_KEY_7; goto vk_mods; }
			if(strcmp(vk_str, "VK_KEY_8") == 0){	keyB.vk = VK_KEY_8; goto vk_mods; }
			if(strcmp(vk_str, "VK_KEY_9") == 0){	keyB.vk = VK_KEY_9; goto vk_mods; }

			if(strcmp(vk_str, "VK_KEY_A") == 0){	keyB.vk = VK_KEY_A; goto vk_mods; }
			if(strcmp(vk_str, "VK_KEY_B") == 0){	keyB.vk = VK_KEY_B; goto vk_mods; }
			if(strcmp(vk_str, "VK_KEY_C") == 0){	keyB.vk = VK_KEY_C; goto vk_mods; }
			if(strcmp(vk_str, "VK_KEY_D") == 0){	keyB.vk = VK_KEY_D; goto vk_mods; }
			if(strcmp(vk_str, "VK_KEY_E") == 0){	keyB.vk = VK_KEY_E; goto vk_mods; }
			if(strcmp(vk_str, "VK_KEY_F") == 0){	keyB.vk = VK_KEY_F; goto vk_mods; }
			if(strcmp(vk_str, "VK_KEY_G") == 0){	keyB.vk = VK_KEY_G; goto vk_mods; }
			if(strcmp(vk_str, "VK_KEY_H") == 0){	keyB.vk = VK_KEY_H; goto vk_mods; }
			if(strcmp(vk_str, "VK_KEY_I") == 0){	keyB.vk = VK_KEY_I; goto vk_mods; }
			if(strcmp(vk_str, "VK_KEY_J") == 0){	keyB.vk = VK_KEY_J; goto vk_mods; }
			if(strcmp(vk_str, "VK_KEY_K") == 0){	keyB.vk = VK_KEY_K; goto vk_mods; }
			if(strcmp(vk_str, "VK_KEY_L") == 0){	keyB.vk = VK_KEY_L; goto vk_mods; }
			if(strcmp(vk_str, "VK_KEY_M") == 0){	keyB.vk = VK_KEY_M; goto vk_mods; }
			if(strcmp(vk_str, "VK_KEY_N") == 0){	keyB.vk = VK_KEY_N; goto vk_mods; }
			if(strcmp(vk_str, "VK_KEY_O") == 0){	keyB.vk = VK_KEY_O; goto vk_mods; }
			if(strcmp(vk_str, "VK_KEY_P") == 0){	keyB.vk = VK_KEY_P; goto vk_mods; }
			if(strcmp(vk_str, "VK_KEY_Q") == 0){	keyB.vk = VK_KEY_Q; goto vk_mods; }
			if(strcmp(vk_str, "VK_KEY_R") == 0){	keyB.vk = VK_KEY_R; goto vk_mods; }
			if(strcmp(vk_str, "VK_KEY_S") == 0){	keyB.vk = VK_KEY_S; goto vk_mods; }
			if(strcmp(vk_str, "VK_KEY_T") == 0){	keyB.vk = VK_KEY_T; goto vk_mods; }
			if(strcmp(vk_str, "VK_KEY_U") == 0){	keyB.vk = VK_KEY_U; goto vk_mods; }
			if(strcmp(vk_str, "VK_KEY_V") == 0){	keyB.vk = VK_KEY_V; goto vk_mods; }
			if(strcmp(vk_str, "VK_KEY_W") == 0){	keyB.vk = VK_KEY_W; goto vk_mods; }
			if(strcmp(vk_str, "VK_KEY_X") == 0){	keyB.vk = VK_KEY_X; goto vk_mods; }
			if(strcmp(vk_str, "VK_KEY_Y") == 0){	keyB.vk = VK_KEY_Y; goto vk_mods; }
			if(strcmp(vk_str, "VK_KEY_Z") == 0){	keyB.vk = VK_KEY_Z; goto vk_mods; }

			if(strcmp(vk_str, "VK_BACK") == 0){	keyB.vk = VK_BACK; goto vk_mods; }
			if(strcmp(vk_str, "VK_TAB") == 0){	keyB.vk = VK_TAB; goto vk_mods; }
			if(strcmp(vk_str, "VK_CLEAR") == 0){	keyB.vk = VK_CLEAR; goto vk_mods; }
			if(strcmp(vk_str, "VK_RETURN") == 0){	keyB.vk = VK_RETURN; goto vk_mods; }
			if(strcmp(vk_str, "VK_SHIFT") == 0){	keyB.vk = VK_SHIFT; goto vk_mods; }
			if(strcmp(vk_str, "VK_CONTROL") == 0){	keyB.vk = VK_CONTROL; goto vk_mods; }
			if(strcmp(vk_str, "VK_MENU") == 0){		keyB.vk = VK_MENU; goto vk_mods; }
			if(strcmp(vk_str, "VK_PAUSE") == 0){	keyB.vk = VK_PAUSE; goto vk_mods; }
			if(strcmp(vk_str, "VK_CAPITAL") == 0){	keyB.vk = VK_CAPITAL; goto vk_mods; }

			if(strcmp(vk_str, "VK_ESCAPE") == 0){	keyB.vk = VK_ESCAPE; goto vk_mods; }
			if(strcmp(vk_str, "VK_SPACE") == 0){	keyB.vk = VK_SPACE; goto vk_mods; }
			if(strcmp(vk_str, "VK_PRIOR") == 0){	keyB.vk = VK_PRIOR; goto vk_mods; }
			if(strcmp(vk_str, "VK_NEXT") == 0){		keyB.vk = VK_NEXT; goto vk_mods; }
			if(strcmp(vk_str, "VK_END") == 0){		keyB.vk = VK_END; goto vk_mods; }
			if(strcmp(vk_str, "VK_HOME") == 0){		keyB.vk = VK_HOME; goto vk_mods; }
			if(strcmp(vk_str, "VK_LEFT") == 0){		keyB.vk = VK_LEFT; goto vk_mods; }
			if(strcmp(vk_str, "VK_UP") == 0){		keyB.vk = VK_UP; goto vk_mods; }
			if(strcmp(vk_str, "VK_RIGHT") == 0){	keyB.vk = VK_RIGHT; goto vk_mods; }
			if(strcmp(vk_str, "VK_DOWN") == 0){		keyB.vk = VK_DOWN; goto vk_mods; }
			if(strcmp(vk_str, "VK_SELECT") == 0){	keyB.vk = VK_SELECT; goto vk_mods; }
			if(strcmp(vk_str, "VK_PRINT") == 0){	keyB.vk = VK_PRINT; goto vk_mods; }
			if(strcmp(vk_str, "VK_EXECUTE") == 0){	keyB.vk = VK_EXECUTE; goto vk_mods; }
			if(strcmp(vk_str, "VK_SNAPSHOT") == 0){	keyB.vk = VK_SNAPSHOT; goto vk_mods; }
			if(strcmp(vk_str, "VK_INSERT") == 0){	keyB.vk = VK_INSERT; goto vk_mods; }
			if(strcmp(vk_str, "VK_DELETE") == 0){	keyB.vk = VK_DELETE; goto vk_mods; }
			if(strcmp(vk_str, "VK_HELP") == 0){		keyB.vk = VK_HELP; goto vk_mods; }

			if(strcmp(vk_str, "VK_LWIN") == 0){	keyB.vk = VK_LWIN; goto vk_mods; }
			if(strcmp(vk_str, "VK_RWIN") == 0){	keyB.vk = VK_RWIN; goto vk_mods; }
			if(strcmp(vk_str, "VK_APPS") == 0){	keyB.vk = VK_APPS; goto vk_mods; }
			if(strcmp(vk_str, "VK_SLEEP") == 0){	keyB.vk = VK_SLEEP; goto vk_mods; }
			if(strcmp(vk_str, "VK_NUMPAD0") == 0){	keyB.vk = VK_NUMPAD0; goto vk_mods; }
			if(strcmp(vk_str, "VK_NUMPAD1") == 0){	keyB.vk = VK_NUMPAD1; goto vk_mods; }
			if(strcmp(vk_str, "VK_NUMPAD2") == 0){	keyB.vk = VK_NUMPAD2; goto vk_mods; }
			if(strcmp(vk_str, "VK_NUMPAD3") == 0){	keyB.vk = VK_NUMPAD3; goto vk_mods; }
			if(strcmp(vk_str, "VK_NUMPAD4") == 0){	keyB.vk = VK_NUMPAD4; goto vk_mods; }
			if(strcmp(vk_str, "VK_NUMPAD5") == 0){	keyB.vk = VK_NUMPAD5; goto vk_mods; }
			if(strcmp(vk_str, "VK_NUMPAD6") == 0){	keyB.vk = VK_NUMPAD6; goto vk_mods; }
			if(strcmp(vk_str, "VK_NUMPAD7") == 0){	keyB.vk = VK_NUMPAD7; goto vk_mods; }
			if(strcmp(vk_str, "VK_NUMPAD8") == 0){	keyB.vk = VK_NUMPAD8; goto vk_mods; }
			if(strcmp(vk_str, "VK_NUMPAD9") == 0){	keyB.vk = VK_NUMPAD9; goto vk_mods; }
			if(strcmp(vk_str, "VK_MULTIPLY") == 0){	keyB.vk = VK_MULTIPLY; goto vk_mods; }
			if(strcmp(vk_str, "VK_ADD") == 0){	keyB.vk = VK_ADD; goto vk_mods; }
			if(strcmp(vk_str, "VK_SEPARATOR") == 0){	keyB.vk = VK_SEPARATOR; goto vk_mods; }
			if(strcmp(vk_str, "VK_SUBTRACT") == 0){	keyB.vk = VK_SUBTRACT; goto vk_mods; }
			if(strcmp(vk_str, "VK_SUBTRACT") == 0){	keyB.vk = VK_SUBTRACT; goto vk_mods; }
			if(strcmp(vk_str, "VK_DECIMAL") == 0){	keyB.vk = VK_DECIMAL; goto vk_mods; }
			if(strcmp(vk_str, "VK_DIVIDE") == 0){	keyB.vk = VK_DIVIDE; goto vk_mods; }
			
			if(strcmp(vk_str, "VK_F1") == 0){	keyB.vk = VK_F1; goto vk_mods; }
			if(strcmp(vk_str, "VK_F2") == 0){	keyB.vk = VK_F2; goto vk_mods; }
			if(strcmp(vk_str, "VK_F3") == 0){	keyB.vk = VK_F3; goto vk_mods; }
			if(strcmp(vk_str, "VK_F4") == 0){	keyB.vk = VK_F4; goto vk_mods; }
			if(strcmp(vk_str, "VK_F5") == 0){	keyB.vk = VK_F5; goto vk_mods; }
			if(strcmp(vk_str, "VK_F6") == 0){	keyB.vk = VK_F6; goto vk_mods; }
			if(strcmp(vk_str, "VK_F7") == 0){	keyB.vk = VK_F7; goto vk_mods; }
			if(strcmp(vk_str, "VK_F8") == 0){	keyB.vk = VK_F8; goto vk_mods; }
			if(strcmp(vk_str, "VK_F9") == 0){	keyB.vk = VK_F9; goto vk_mods; }
			if(strcmp(vk_str, "VK_F10") == 0){	keyB.vk = VK_F10; goto vk_mods; }
			if(strcmp(vk_str, "VK_F11") == 0){	keyB.vk = VK_F11; goto vk_mods; }
			if(strcmp(vk_str, "VK_F12") == 0){	keyB.vk = VK_F12; goto vk_mods; }
			if(strcmp(vk_str, "VK_F13") == 0){	keyB.vk = VK_F13; goto vk_mods; }
			if(strcmp(vk_str, "VK_F14") == 0){	keyB.vk = VK_F14; goto vk_mods; }
			if(strcmp(vk_str, "VK_F15") == 0){	keyB.vk = VK_F15; goto vk_mods; }
			if(strcmp(vk_str, "VK_F16") == 0){	keyB.vk = VK_F16; goto vk_mods; }
			if(strcmp(vk_str, "VK_F17") == 0){	keyB.vk = VK_F17; goto vk_mods; }
			if(strcmp(vk_str, "VK_F18") == 0){	keyB.vk = VK_F18; goto vk_mods; }
			if(strcmp(vk_str, "VK_F19") == 0){	keyB.vk = VK_F19; goto vk_mods; }
			if(strcmp(vk_str, "VK_F20") == 0){	keyB.vk = VK_F20; goto vk_mods; }
			if(strcmp(vk_str, "VK_F21") == 0){	keyB.vk = VK_F21; goto vk_mods; }
			if(strcmp(vk_str, "VK_F22") == 0){	keyB.vk = VK_F22; goto vk_mods; }
			if(strcmp(vk_str, "VK_F23") == 0){	keyB.vk = VK_F23; goto vk_mods; }
			if(strcmp(vk_str, "VK_F24") == 0){	keyB.vk = VK_F24; goto vk_mods; }
			
			if(strcmp(vk_str, "VK_NUMLOCK") == 0){	keyB.vk = VK_NUMLOCK; goto vk_mods; }
			if(strcmp(vk_str, "VK_SCROLL") == 0){	keyB.vk = VK_SCROLL; goto vk_mods; }
			if(strcmp(vk_str, "VK_LSHIFT") == 0){	keyB.vk = VK_LSHIFT; goto vk_mods; }
			if(strcmp(vk_str, "VK_RSHIFT") == 0){	keyB.vk = VK_RSHIFT; goto vk_mods; }
			if(strcmp(vk_str, "VK_LCONTROL") == 0){	keyB.vk = VK_LCONTROL; goto vk_mods; }
			if(strcmp(vk_str, "VK_RCONTROL") == 0){	keyB.vk = VK_RCONTROL; goto vk_mods; }
			if(strcmp(vk_str, "VK_LMENU") == 0){	keyB.vk = VK_LMENU; goto vk_mods; }
			if(strcmp(vk_str, "VK_RMENU") == 0){	keyB.vk = VK_RMENU; goto vk_mods; }
			
			if(strcmp(vk_str, "VK_BROWSER_BACK") == 0){	keyB.vk = VK_BROWSER_BACK; goto vk_mods; }
			if(strcmp(vk_str, "VK_BROWSER_FORWARD") == 0){	keyB.vk = VK_BROWSER_FORWARD; goto vk_mods; }
			if(strcmp(vk_str, "VK_BROWSER_REFRESH") == 0){	keyB.vk = VK_BROWSER_REFRESH; goto vk_mods; }
			if(strcmp(vk_str, "VK_BROWSER_STOP") == 0){	keyB.vk = VK_BROWSER_STOP; goto vk_mods; }
			if(strcmp(vk_str, "VK_BROWSER_SEARCH") == 0){	keyB.vk = VK_BROWSER_SEARCH; goto vk_mods; }
			if(strcmp(vk_str, "VK_BROWSER_FAVORITES") == 0){	keyB.vk = VK_BROWSER_FAVORITES; goto vk_mods; }
			if(strcmp(vk_str, "VK_BROWSER_HOME") == 0){	keyB.vk = VK_BROWSER_HOME; goto vk_mods; }
			if(strcmp(vk_str, "VK_VOLUME_MUTE") == 0){	keyB.vk = VK_VOLUME_MUTE; goto vk_mods; }
			if(strcmp(vk_str, "VK_VOLUME_DOWN") == 0){	keyB.vk = VK_VOLUME_DOWN; goto vk_mods; }
			if(strcmp(vk_str, "VK_VOLUME_UP") == 0){	keyB.vk = VK_VOLUME_UP; goto vk_mods; }
			if(strcmp(vk_str, "VK_MEDIA_NEXT_TRACK") == 0){	keyB.vk = VK_MEDIA_NEXT_TRACK; goto vk_mods; }
			if(strcmp(vk_str, "VK_MEDIA_PREV_TRACK") == 0){	keyB.vk = VK_MEDIA_PREV_TRACK; goto vk_mods; }
			if(strcmp(vk_str, "VK_MEDIA_STOP") == 0){	keyB.vk = VK_MEDIA_STOP; goto vk_mods; }
			if(strcmp(vk_str, "VK_MEDIA_PLAY_PAUSE") == 0){	keyB.vk = VK_MEDIA_PLAY_PAUSE; goto vk_mods; }
			if(strcmp(vk_str, "VK_LAUNCH_MAIL") == 0){	keyB.vk = VK_LAUNCH_MAIL; goto vk_mods; }
			if(strcmp(vk_str, "VK_LAUNCH_MEDIA_SELECT") == 0){	keyB.vk = VK_LAUNCH_MEDIA_SELECT; goto vk_mods; }
			if(strcmp(vk_str, "VK_LAUNCH_APP1") == 0){	keyB.vk = VK_LAUNCH_APP1; goto vk_mods; }
			if(strcmp(vk_str, "VK_LAUNCH_APP2") == 0){	keyB.vk = VK_LAUNCH_APP2; goto vk_mods; }
			if(strcmp(vk_str, "VK_OEM_1") == 0){	keyB.vk = VK_OEM_1; goto vk_mods; }
			if(strcmp(vk_str, "VK_OEM_PLUS") == 0){	keyB.vk = VK_OEM_PLUS; goto vk_mods; }
			if(strcmp(vk_str, "VK_OEM_COMMA") == 0){	keyB.vk = VK_OEM_COMMA; goto vk_mods; }
			if(strcmp(vk_str, "VK_OEM_MINUS") == 0){	keyB.vk = VK_OEM_MINUS; goto vk_mods; }
			if(strcmp(vk_str, "VK_OEM_PERIOD") == 0){	keyB.vk = VK_OEM_PERIOD; goto vk_mods; }
			if(strcmp(vk_str, "VK_OEM_2") == 0){	keyB.vk = VK_OEM_2; goto vk_mods; }
			if(strcmp(vk_str, "VK_OEM_3") == 0){	keyB.vk = VK_OEM_3; goto vk_mods; }
			if(strcmp(vk_str, "VK_OEM_4") == 0){	keyB.vk = VK_OEM_4; goto vk_mods; }
			if(strcmp(vk_str, "VK_OEM_5") == 0){	keyB.vk = VK_OEM_5; goto vk_mods; }
			if(strcmp(vk_str, "VK_OEM_6") == 0){	keyB.vk = VK_OEM_6; goto vk_mods; }
			if(strcmp(vk_str, "VK_OEM_7") == 0){	keyB.vk = VK_OEM_7; goto vk_mods; }
			if(strcmp(vk_str, "VK_OEM_8") == 0){	keyB.vk = VK_OEM_8; goto vk_mods; }
			if(strcmp(vk_str, "VK_OEM_AX") == 0){	keyB.vk = VK_OEM_AX; goto vk_mods; }
			if(strcmp(vk_str, "VK_OEM_102") == 0){	keyB.vk = VK_OEM_102; goto vk_mods; }
			if(strcmp(vk_str, "VK_ICO_HELP") == 0){	keyB.vk = VK_ICO_HELP; goto vk_mods; }
			if(strcmp(vk_str, "VK_ICO_00") == 0){	keyB.vk = VK_ICO_00; goto vk_mods; }
			
vk_mods:

			keyB.shift = xml_binding.attribute("shift").as_bool();
			keyB.ctrl = xml_binding.attribute("ctrl").as_bool();
			keyB.alt = xml_binding.attribute("alt").as_bool();
			keybind_list[keyB.command] = keyB;

		}
	}
}

bool KeyBindings::CommandPressed(int commandID)
{
	if(commandID < 0 || commandID >= COMMAND_SIZE)
		return false;		// input out of range

	if(KEY_DOWN(keybind_list[commandID].vk) && KEY_DOWN(VK_CONTROL) == keybind_list[commandID].ctrl &&
		KEY_DOWN(VK_SHIFT) == keybind_list[commandID].shift && KEY_DOWN(VK_MENU) == keybind_list[commandID].alt)
	{
		return true;
	}

	return false;
}