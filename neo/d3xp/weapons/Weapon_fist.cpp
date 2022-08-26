// weapon_fist.cpp
//

#include "precompiled.h"
#pragma hdrstop
#include "../Game_local.h"

CLASS_DECLARATION( iceWeaponObject, iceWeaponFist )
END_CLASS

#define RIFLE_NUMPROJECTILES	1

// blend times
#define FISTS_IDLE_TO_LOWER		4
#define FISTS_IDLE_TO_PUNCH		0
#define FISTS_RAISE_TO_IDLE		4
#define FISTS_PUNCH_TO_IDLE		1


/*
================
iceWeaponFist::Init
================
*/
void iceWeaponFist::Init( idWeapon* weapon )
{
	iceWeaponObject::Init( weapon );
}

/*
================
iceWeaponFist::Raise
================
*/
stateResult_t iceWeaponFist::Raise( stateParms_t* parms )
{
	enum RisingState
	{
		RISING_NOTSET = 0,
		RISING_WAIT
	};

	switch( parms->stage )
	{
		case RISING_NOTSET:
			owner->Event_PlayAnim( ANIMCHANNEL_ALL, "raise", false );
			parms->stage = RISING_WAIT;
			return SRESULT_WAIT;

		case RISING_WAIT:
			if( owner->Event_AnimDone( ANIMCHANNEL_ALL, FISTS_RAISE_TO_IDLE ) )
			{
				return SRESULT_DONE;
			}
			return SRESULT_WAIT;
	}

	return SRESULT_ERROR;
}

/*
================
iceWeaponFist::Lower
================
*/
stateResult_t iceWeaponFist::Lower( stateParms_t* parms )
{
	enum LoweringState
	{
		LOWERING_NOTSET = 0,
		LOWERING_WAIT
	};

	switch( parms->stage )
	{
		case LOWERING_NOTSET:
			owner->Event_PlayAnim( ANIMCHANNEL_ALL, "putaway", false );
			parms->stage = LOWERING_WAIT;
			return SRESULT_WAIT;

		case LOWERING_WAIT:
			if( owner->Event_AnimDone( ANIMCHANNEL_ALL, 0 ) )
			{
				SetState( "Holstered" );
				return SRESULT_DONE;
			}
			return SRESULT_WAIT;
	}

	return SRESULT_ERROR;
}

/*
================
iceWeaponFist::Idle
================
*/
stateResult_t iceWeaponFist::Idle( stateParms_t* parms )
{
	enum IdleState
	{
		IDLE_NOTSET = 0,
		IDLE_WAIT
	};

	switch( parms->stage )
	{
		case IDLE_NOTSET:
			owner->Event_PlayCycle( ANIMCHANNEL_ALL, "idle" );
			parms->stage = IDLE_WAIT;
			return SRESULT_WAIT;

		case IDLE_WAIT:
			// Do nothing.
			return SRESULT_DONE;
	}

	return SRESULT_ERROR;
}

/*
================
iceWeaponFist::Fire
================
*/
stateResult_t iceWeaponFist::Fire( stateParms_t* parms )
{
	enum FIRE_State
	{
		FIRE_NOTSET = 0,
		FIRE_MELEE,
		FIRE_WAIT
	};

	switch( parms->stage )
	{
		case FIRE_NOTSET:
			owner->Event_PlayAnim( ANIMCHANNEL_ALL, GetFireAnim(), false );
			parms->stage = FIRE_MELEE;
			parms->Wait( 0.1f );
			return SRESULT_WAIT;

		case FIRE_MELEE:
			owner->Event_Melee();
			parms->stage = FIRE_WAIT;
			return SRESULT_WAIT;

		case FIRE_WAIT:
			if( owner->Event_AnimDone( ANIMCHANNEL_ALL, 0 ) )
			{
				side = !side;
				return SRESULT_DONE;
			}
			return SRESULT_WAIT;
	}

	return SRESULT_ERROR;
}

/*
================
iceWeaponFist::Reload
================
*/
stateResult_t iceWeaponFist::Reload( stateParms_t* parms )
{
	return SRESULT_DONE;
}

/*
================
iceWeaponFist::GetFireAnim
================
*/
const char* iceWeaponFist::GetFireAnim()
{
	if( side )
	{
		return "punch_left";
	}
	else
	{
		return "punch_right";
	}
}