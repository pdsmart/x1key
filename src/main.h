/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Name:            main.h
// Created:         Jan 2022
// Version:         v1.0
// Author(s):       Philip Smart
// Description:     X1 Keyboard logic.
//                  This source file was originally written by Sato Kyoichi and has been translated and
//                  changes made to accommodate the updated hardware key. Please see this repository
//                  for the original source if needed.
//                  It contains the startup code to configure the I/O and timers, initialise the sub-
//                  modules and then start key conversion, incoming keys are mapped realtime and sent
//                  immediately to the X1.
// Credits:         Kyoichi Sato http://kyoutan.jpn.org/ - Firmware based on and uses components of
//                  X1Keybord by Kyoichi Sato.
// Copyright:       Changes from original source (c) 2022 Philip Smart <philip.smart@net2net.org>
//
// History:         Jan 2022 - Initial write.
//
// Notes:           See Makefile to enable/disable conditional components
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
// This source file is free software: you can redistribute it and#or modify
// it under the terms of the GNU General Public License as published
// by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This source file is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
/////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef MAIN_H
#define MAIN_H

#ifdef __cplusplus
    extern "C" {
#endif

#pragma section rom flash_data

// Version string.
const char VERSION[] = "v1.0";

#pragma section rom rom

// Prototypes.
void main(void);
void osc_init(void);
void io_init(void);

#ifdef __cplusplus
}
#endif
#endif // MAIN_H
