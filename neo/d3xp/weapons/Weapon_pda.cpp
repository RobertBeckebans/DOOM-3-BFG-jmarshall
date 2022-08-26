// Weapon_pda.cpp
//

#include "precompiled.h"
#pragma hdrstop
#include "../Game_local.h"

CLASS_DECLARATION( iceWeaponObject, iceWeaponPDA )
END_CLASS

/*
================
iceWeaponPDA::Init
================
*/
void iceWeaponPDA::Init( idWeapon* weapon )
{
	iceWeaponObject::Init( weapon );
}

/*
================
iceWeaponPDA::Raise
================
*/
stateResult_t iceWeaponPDA::Raise( stateParms_t* parms )
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
			if( owner->Event_AnimDone( ANIMCHANNEL_ALL, 0 ) )
			{
				return SRESULT_DONE;
			}
			return SRESULT_WAIT;
	}

	return SRESULT_ERROR;
}

/*
================
iceWeaponPDA::Lower
================
*/
stateResult_t iceWeaponPDA::Lower( stateParms_t* parms )
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
iceWeaponPDA::Idle
================
*/
stateResult_t iceWeaponPDA::Idle( stateParms_t* parms )
{
	enum IdleState
	{
		IDLE_NOTSET = 0,
		IDLE_WAIT
	};

	switch( parms->stage )
	{
		case IDLE_NOTSET:
			owner->GetOwner()->Event_OpenPDA();
			parms->stage = IDLE_WAIT;
			return SRESULT_WAIT;

		case IDLE_WAIT:
			if( !owner->GetOwner()->objectiveSystemOpen )
			{
				//owner->WeaponState( WP_LOWERING, 0 );
				owner->LowerWeapon();
				return SRESULT_DONE;
			}
			return SRESULT_WAIT;
	}
}

/*
================
iceWeaponPDA::Fire
================
*/
stateResult_t iceWeaponPDA::Fire( stateParms_t* parms )
{
	return SRESULT_DONE;
}

/*
================
iceWeaponPDA::Reload
================
*/
stateResult_t iceWeaponPDA::Reload( stateParms_t* parms )
{
	return SRESULT_DONE;
}
