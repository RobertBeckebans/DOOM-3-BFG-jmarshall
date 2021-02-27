// Bot.cpp
//

#include "precompiled.h"
#include "../Game_local.h"

idCVar bot_pathdebug( "bot_pathdebug", "0", CVAR_BOOL | CVAR_CHEAT, "force the bot to path to player" );
idCVar bot_goaldist( "bot_goaldist", "20", CVAR_INTEGER | CVAR_CHEAT, "" );
idCVar bot_debugnav( "bot_debugnav", "0", CVAR_BOOL | CVAR_CHEAT, "draws navmesh paths for the bot" );
idCVar bot_showstate( "bot_showstate", "0", CVAR_BOOL | CVAR_CHEAT, "draws the bot state above the bot" );
idCVar bot_debug( "bot_debug", "0", CVAR_BOOL, "shows debug info for the bot" );

CLASS_DECLARATION( idPlayer, rvmBot )
END_CLASS

/*
===================
rvmBot::rvmBot
===================
*/
rvmBot::rvmBot()
{
	//bs.action = NULL;
	hasSpawned = false;
	gameLocal.RegisterBot( this );
}

/*
===================
rvmBot::~rvmBot
===================
*/
rvmBot::~rvmBot()
{
	gameLocal.UnRegisterBot( this );
}

/*
==================
rvmBot::SetEnemy
==================
*/
void rvmBot::SetEnemy( idPlayer* player )
{
	if( bs.enemy == -1 )
	{
		bs.enemy = player->entityNumber;
		//bs.action = &botAIBattleRetreat;
		stateThread.SetState("state_BattleFight");
	}
}

/*
==================
rvmBot::BotUpdateInventory
==================
*/
void rvmBot::BotUpdateInventory( void )
{
	bs.inventory[INVENTORY_ARMOR]			= inventory.armor;
	bs.inventory[INVENTORY_GAUNTLET]		= 1;
	bs.inventory[INVENTORY_SHOTGUN]			= HasWeapon( weapon_shotgun );
	bs.inventory[INVENTORY_MACHINEGUN]		= HasWeapon( weapon_machinegun );
	bs.inventory[INVENTORY_GRENADELAUNCHER] = 0;
	bs.inventory[INVENTORY_ROCKETLAUNCHER]  = HasWeapon( weapon_rocketlauncher );
	bs.inventory[INVENTORY_LIGHTNING]		= 0;
	bs.inventory[INVENTORY_RAILGUN]			= 0;
	bs.inventory[INVENTORY_PLASMAGUN]		= HasWeapon( weapon_plasmagun );
	bs.inventory[INVENTORY_BFG10K]			= 0;
	bs.inventory[INVENTORY_GRAPPLINGHOOK]	= 0;
	bs.inventory[INVENTORY_SHELLS]			= inventory.ammo[idWeapon::GetAmmoNumForName( "ammo_shells" )].Get();
	bs.inventory[INVENTORY_BULLETS]			= inventory.ammo[idWeapon::GetAmmoNumForName( "ammo_clip" )].Get();
	bs.inventory[INVENTORY_GRENADES]		= 0;
	bs.inventory[INVENTORY_CELLS]			= inventory.ammo[idWeapon::GetAmmoNumForName( "ammo_cells" )].Get();
	bs.inventory[INVENTORY_LIGHTNINGAMMO]	= 0;
	bs.inventory[INVENTORY_ROCKETS]			= inventory.ammo[idWeapon::GetAmmoNumForName( "ammo_rockets" )].Get();
	bs.inventory[INVENTORY_SLUGS]			= 0;
	bs.inventory[INVENTORY_BFGAMMO]			= 0;
	bs.inventory[INVENTORY_HEALTH]			= health;
	bs.inventory[INVENTORY_TELEPORTER]		= 0;
	bs.inventory[INVENTORY_MEDKIT]			= 0;
	bs.inventory[INVENTORY_QUAD]			= 0;
	bs.inventory[INVENTORY_ENVIRONMENTSUIT] = 0;
	bs.inventory[INVENTORY_HASTE]			= 0;
	bs.inventory[INVENTORY_INVISIBILITY]	= 0;
	bs.inventory[INVENTORY_REGEN]			= 0;
	bs.inventory[INVENTORY_FLIGHT]			= 0;
	bs.inventory[INVENTORY_REDFLAG]			= 0;
	bs.inventory[INVENTORY_BLUEFLAG]		= 0;
}


/*
===================
rvmBot::Spawn
===================
*/
void rvmBot::Spawn( void )
{
	idStr botName;
	char filename[256];
	int errnum;

	stateThread.SetOwner(this);

	idPlayer::Spawn();

	if( common->IsServer() )
	{
		weapon_machinegun = SlotForWeapon( "weapon_machinegun" );
		weapon_shotgun = SlotForWeapon( "weapon_shotgun" );
		weapon_plasmagun = SlotForWeapon( "weapon_plasmagun" );
		weapon_rocketlauncher = SlotForWeapon( "weapon_rocketlauncher" );

		WP_MACHINEGUN = weapon_machinegun;
		WP_SHOTGUN = weapon_shotgun;
		WP_PLASMAGUN = weapon_plasmagun;
		WP_ROCKET_LAUNCHER = weapon_rocketlauncher;

		botName = spawnArgs.GetString( "botname" );

		aas = gameLocal.GetBotAAS();
		if( aas == NULL )
		{
			gameLocal.Error( "Missing AAS\n" );
			return;
		}

		// Load in the bot character.
		bs.character = botCharacterStatsManager.BotLoadCharacterFromFile( va( "bots/%s_c.c", botName.c_str() ), 1 );
		if( !bs.character )
		{
			gameLocal.Error( "Failed to load character file for bot %s\n", botName.c_str() );
		}

		// Allocate the goal state.
		bs.gs = botGoalManager.BotAllocGoalState( entityNumber );

		// Get the bot items weights file name.
		botCharacterStatsManager.Characteristic_String( bs.character, CHARACTERISTIC_ITEMWEIGHTS, filename, 256 );
		errnum = botGoalManager.BotLoadItemWeights( bs.gs, filename );
		if( errnum != BLERR_NOERROR )
		{
			gameLocal.Error( "Failed to load bot item weights!" );
			botGoalManager.BotFreeGoalState( bs.gs );
			return;
		}

		//allocate a weapon state
		bs.ws = botWeaponInfoManager.BotAllocWeaponState();

		//load the weapon weights
		botCharacterStatsManager.Characteristic_String( bs.character, CHARACTERISTIC_WEAPONWEIGHTS, filename, 256 );
		errnum = botWeaponInfoManager.BotLoadWeaponWeights( bs.ws, filename );
		if( errnum != BLERR_NOERROR )
		{
			//	trap_BotFreeGoalState(bs->gs);
			botWeaponInfoManager.BotFreeWeaponState( bs.ws );
			return;
		}

		bs.client = entityNumber;
		bs.entitynum = entityNumber;
		bs.setupcount = 4;
		bs.entergame_time = Bot_Time();

		hasSpawned = true;

		bs.botinput.respawn = true;

		stateThread.SetState("state_Respawn");
	}
}

/*
===================
rvmBot::Think
===================
*/
void rvmBot::BotMoveToGoalOrigin( void )
{
	bs.botinput.dir = bs.currentGoal.nextMoveOrigin - GetPhysics()->GetOrigin();

	idAngles ang( 0, bs.botinput.dir.ToYaw(), 0 );
	if( bs.enemy >= 0 )
	{
		idPlayer* enemy = gameLocal.entities[bs.enemy]->Cast<idPlayer>();
		if( enemy )
		{
			idVec3 enemyDir = enemy->GetPhysics()->GetOrigin() - GetPhysics()->GetOrigin();
			enemyDir.Normalize();
			idAngles angles = enemyDir.ToAngles();
			angles.pitch = 0;
			angles.roll = 0;
			bs.botinput.viewangles = angles;
		}
		else
		{
			bs.botinput.viewangles = ang;
		}
	}
	else
	{
		bs.botinput.viewangles = ang;
	}
	bs.botinput.speed = pm_runspeed.GetInteger();

	bs.botinput.dir.Normalize();
}

/*
===================
rvmBot::SpawnToPoint
===================
*/
void rvmBot::SpawnToPoint( const idVec3& spawn_origin, const idAngles& spawn_angles )
{
	idPlayer::SpawnToPoint( spawn_origin, spawn_angles );

	if( common->IsServer() )
	{
		bs.ltg_time = 0;
		stateThread.SetState("state_SeekLTG");
	}
}
/*
===================
rvmBot::StateThreadChanged
===================
*/
void rvmBot::StateThreadChanged(void) {
	// Ensure if we are switching states, pop the last goal.
	bs.ltg_time = 0;
}

/*
===================
rvmBot::ServerThink
===================
*/
void rvmBot::ServerThink( void )
{
	bs.origin = GetPhysics()->GetOrigin();
	bs.eye = GetEyePosition();
	bs.thinktime = Bot_Time();
	bs.botinput.actionflags = 0;

	BotUpdateInventory();

	if( bot_pathdebug.IsModified() )
	{
		if( bot_pathdebug.GetBool() )
		{
			bs.currentGoal.origin = gameLocal.GetLocalPlayer()->GetPhysics()->GetOrigin();
			bs.currentGoal.framenum = gameLocal.framenum;
		}

		bot_pathdebug.ClearModified();
		bot_pathdebug.SetBool( false );
	}

	stateThread.Execute();

	// If we are moving along a set of waypoints, let's move along.
	aasPath_t path;
	//int myArea = aas->PointAreaNum(GetOrigin());
	int goalArea = aas->PointAreaNum( bs.currentGoal.origin );
	idVec3 org = bs.origin;
	int curAreaNum = aas->AdjustPositionAndGetArea( org );

	if( bot_debug.GetBool() )
	{
		if (bs.useRandomPosition)
		{
			aas->ShowWalkPath(GetOrigin(), goalArea, bs.random_move_position);
		}
		else
		{
			aas->ShowWalkPath(GetOrigin(), goalArea, bs.currentGoal.origin);
		}

		aas->ShowArea( GetOrigin() );
	}

	if (bs.useRandomPosition)
	{
		aas->WalkPathToGoal(path, curAreaNum, org, goalArea, bs.random_move_position, TFL_WALK | TFL_AIR);
	}
	else
	{
		aas->WalkPathToGoal(path, curAreaNum, org, goalArea, bs.currentGoal.origin, TFL_WALK | TFL_AIR);
	}
	

	bs.currentGoal.nextMoveOrigin = path.moveGoal;

	BotMoveToGoalOrigin();

	bs.viewangles = bs.botinput.viewangles;

	//if (bot_debugnav.GetBool() && navWaypoints.Num() > 1)
	//{
	//	if (currentWaypoint < navWaypoints.Num())
	//	{
	//		for (int i = currentWaypoint + 1; i < navWaypoints.Num(); i++)
	//		{
	//			idVec4 color_white(255, 255, 255, 255);
	//			gameRenderWorld->DebugArrow(color_white, navWaypoints[i - 1], navWaypoints[i], 10);
	//		}
	//	}
	//
	//	idVec4 color_red(255, 0, 0, 255);
	//	idVec3 box_debug_extents(20, 20, 20);
	//	idMat3 rotation;
	//
	//	rotation.Identity();
	//
	//	idBox start_box(GetPhysics()->GetOrigin(), box_debug_extents, rotation);
	//	idBox end_box(bs.currentGoal.origin, box_debug_extents, rotation);
	//
	//	gameRenderWorld->DebugBox(color_red, start_box);
	//	gameRenderWorld->DebugBox(color_red, end_box);
	//}

	if( bot_showstate.GetBool() )
	{
		//idMat3 axis = GetPhysics()->GetAxis();
		//idVec4 color_white( 1, 1, 1, 1 );
		//gameRenderWorld->DrawTextA( bs.action->GetName(), GetPhysics()->GetOrigin() + idVec3( 0, 0, 100 ), 0.5f, color_white, axis );
	}

	bs.useRandomPosition = false;
	bs.attackerEntity = NULL; // Has to be consumed immedaitly.
	bs.botinput.weapon = bs.weaponnum;
}

/*
=======================
rvmBot::Damage
=======================
*/
void rvmBot::Damage( idEntity* inflictor, idEntity* attacker, const idVec3& dir, const char* damageDefName, const float damageScale, const int location )
{
	idPlayer::Damage( inflictor, attacker, dir, damageDefName, damageScale, location );

	if (health <= 0)
	{
		idPlayer* player = attacker->Cast<idPlayer>();
		if (player)
		{
			BotSendChatMessage(DEATH, player->netname );
		}
	}

	bs.attackerEntity = attacker;
}

/*
=======================
rvmBot::InflictedDamageEvent
=======================
*/
void rvmBot::InflictedDamageEvent(idEntity* target) {
	idPlayer* player = target->Cast<idPlayer>();

	// Don't flood the chat with death and insults.
	if (!player->IsBot() && player->health <= 0)
	{
		BotSendChatMessage(KILL, player->netname);
	}
}

/*
=======================
rvmBot::PresenceTypeBoundingBox
=======================
*/
void rvmBot::PresenceTypeBoundingBox( int presencetype, idVec3& mins, idVec3& maxs )
{
	int index;
	//bounding box size for each presence type
	//idVec3 boxmins[3] = { {0, 0, 0}, {-15, -15, -24}, {-15, -15, -24} };
	//idVec3 boxmaxs[3] = { {0, 0, 0}, { 15,  15,  32}, { 15,  15,   8} };

	idVec3 boxmins[3];
	idVec3 boxmaxs[3];

	boxmins[0] = idVec3( 0, 0, 0 );
	boxmins[1] = idVec3( -15, -15, -24 );
	boxmins[2] = idVec3( -15, -15, -24 );

	boxmaxs[0] = idVec3( 0, 0, 0 );
	boxmaxs[1] = idVec3( 15, 15, 32 );
	boxmaxs[2] = idVec3( 15, 15, 8 );


	if( presencetype == PRESENCE_NORMAL )
	{
		index = 1;
	}
	else if( presencetype == PRESENCE_CROUCH )
	{
		index = 2;
	}
	else
	{
		//botimport.Print(PRT_FATAL, "AAS_PresenceTypeBoundingBox: unknown presence type\n");
		index = 2;
	}
	mins = boxmins[index];
	maxs = boxmaxs[index];
}


/*
===================
rvmBot::Think
===================
*/
void rvmBot::Think( void )
{
	if( !hasSpawned )
	{
		return;
	}

	if( common->IsServer() )
	{
		ServerThink();

		if( bot_debug.GetBool() )
		{
			idVec4 color = idVec4( 1, 1, 1, 1 );
			idBounds bounds = idBounds( idVec3( -10, -10, -10 ), idVec3( 10, 10, 10 ) );
			common->RW()->DebugBounds( color, bounds, GetOrigin() );
		}
	}

	deltaViewAngles.Zero();

	idPlayer::Think();
}