// BotAI_SeekLTG.cpp
//

#include "precompiled.h"
#pragma hdrstop
#include "../Game_local.h"

/*
=====================
rvmBot::state_SeekLTG
=====================
*/
stateResult_t rvmBot::state_SeekLTG( stateParms_t* parms )
{
	if( BotIsDead( &bs ) )
	{
		stateThread.SetState( "state_Respawn" );
		return SRESULT_DONE_FRAME;
	}

	BotGetItemLongTermGoal( &bs, 0, &bs.currentGoal );

	// No Enemy.
	bs.enemy = -1;

	//if there is an enemy
	if( BotFindEnemy( &bs, -1 ) )
	{
		if( BotWantsToRetreat( &bs ) )
		{
			//keep the current long term goal and retreat
			//AIEnter_Battle_Retreat(bs, "seek ltg: found enemy");
			stateThread.SetState( "state_Retreat" );
			return SRESULT_DONE_FRAME;
		}
		else
		{
			//trap_BotResetLastAvoidReach(bs.ms);
			//empty the goal stack
			botGoalManager.BotEmptyGoalStack( bs.gs );

			//go fight
			stateThread.SetState( "state_BattleFight" );
			return SRESULT_DONE_FRAME;
		}
	}

	return SRESULT_WAIT;
}
