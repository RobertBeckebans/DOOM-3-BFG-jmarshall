/*
===========================================================================

Doom 3 BFG Edition GPL Source Code
Copyright (C) 1993-2012 id Software LLC, a ZeniMax Media company.
Copyright (C) 2021 Justin Marshall

This file is part of the Doom 3 BFG Edition GPL Source Code ("Doom 3 BFG Edition Source Code").  

Doom 3 BFG Edition Source Code is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Doom 3 BFG Edition Source Code is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Doom 3 BFG Edition Source Code.  If not, see <http://www.gnu.org/licenses/>.

In addition, the Doom 3 BFG Edition Source Code is also subject to certain additional terms. You should have received a copy of these additional terms immediately following the terms and conditions of the GNU General Public License which accompanied the Doom 3 BFG Edition Source Code.  If not, please request a copy in writing from id Software at the address below.

If you have questions concerning this license or the applicable additional terms, you may contact in writing id Software LLC, c/o ZeniMax Media Inc., Suite 120, Rockville, Maryland 20850 USA.

===========================================================================
*/

#pragma once

//
// iceMonsterZombieCommandoChaingun
//
class iceMonsterZombieCommandoChaingun : public idAI
{
	CLASS_PROTOTYPE( iceMonsterZombieCommandoChaingun );
public:
	virtual void				Init() override;
	virtual void				AI_Begin() override;

	virtual int					check_attacks() override;
	virtual void				do_attack( int attack_flags ) override;
private:
	stateResult_t				state_Begin( stateParms_t* parms );
	stateResult_t				state_Idle( stateParms_t* parms );
	stateResult_t				combat_dodge_right( stateParms_t* parms );
	stateResult_t				combat_dodge_left( stateParms_t* parms );
	stateResult_t				crouch_attack( stateParms_t* parms );
	stateResult_t				stand_attack( stateParms_t* parms );
private:
	idScriptBool		fire;
	idScriptBool		crouch_fire;
	idScriptBool		step_left;
	idScriptBool		step_right;
	idScriptFloat		nextDodge;
	idScriptFloat		nextAttack;
	idScriptFloat		nextNoFOVAttack;
private:
	idEntity*			combat_node;
	float				attackTime;
};
