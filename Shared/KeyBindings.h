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

#ifndef KEYBINDINGS_H_INCLUDED
#define KEYBINDINGS_H_INCLUDED

#include <map>

#include <windows.h>


// add keycodes not included in WinUser.h
#define VK_KEY_0	0x30
#define VK_KEY_1	0x31
#define VK_KEY_2	0x32
#define VK_KEY_3	0x33
#define VK_KEY_4	0x34
#define VK_KEY_5	0x35
#define VK_KEY_6	0x36
#define VK_KEY_7	0x37
#define VK_KEY_8	0x38
#define VK_KEY_9	0x39

#define VK_KEY_A	0x41
#define VK_KEY_B	0x42
#define VK_KEY_C	0x43
#define VK_KEY_D	0x44
#define VK_KEY_E	0x45
#define VK_KEY_F	0x46
#define VK_KEY_G	0x47
#define VK_KEY_H	0x48
#define VK_KEY_I	0x49
#define VK_KEY_J	0x4A
#define VK_KEY_K	0x4B
#define VK_KEY_L	0x4C
#define VK_KEY_M	0x4D
#define VK_KEY_N	0x4E
#define VK_KEY_O	0x4F
#define VK_KEY_P	0x50
#define VK_KEY_Q	0x51
#define VK_KEY_R	0x52
#define VK_KEY_S	0x53
#define VK_KEY_T	0x54
#define VK_KEY_U	0x55
#define VK_KEY_V	0x56
#define VK_KEY_W	0x57
#define VK_KEY_X	0x58
#define VK_KEY_Y	0x59
#define VK_KEY_Z	0x5A

struct KeyBinding;

class KeyBindings
{
public:
	KeyBindings();
	~KeyBindings();


	void LoadXML();
	bool CommandPressed(int commandID);


	static enum Command
	{
		SAVE_SCREEN,
		SEPARATION_INC,
		SEPARATION_DEC,
		CONVERGENCE_INC,
		CONVERGENCE_DEC,
		OFFSET_INC,
		OFFSET_DEC,
		DISTORTIONSCALE_INC,
		DISTORTIONSCALE_DEC,
		SCREEN_OFFSET_INC,
		SCREEN_OFFSET_DEC,

		SWAP_EYES,
		DEFAULT_VALUES,

		PITCH_MUL_INC,
		PITCH_MUL_DEC,
		YAW_MUL_INC,
		YAW_MUL_DEC,
		ROLL_MUL_INC,
		ROLL_MUL_DEC,

		STEREO_ENABLED_T,
		SHOCT_L_INC,
		SHOCT_L_DEC,
		SHOCT_R_INC,
		SHOCT_R_DEC,
		SHOCT_MODE_T,
		COMMAND_SIZE
	};

	struct KeyBinding
	{
		int command;
		int vk;
		bool shift;
		bool alt;
		bool ctrl;
	};

	std::map<int, std::string> VK_codes;

	KeyBinding keybind_list[COMMAND_SIZE];
	//KeyBinding keybind_list[256];
};


#endif