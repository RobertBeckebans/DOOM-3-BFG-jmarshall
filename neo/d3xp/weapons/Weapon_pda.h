// Weapon_pda.h
//

#pragma once

class iceWeaponPDA : public iceWeaponObject
{
public:
	CLASS_PROTOTYPE( iceWeaponPDA );

	virtual void			Init( idWeapon* weapon );

	stateResult_t			Raise( stateParms_t* parms );
	stateResult_t			Lower( stateParms_t* parms );
	stateResult_t			Idle( stateParms_t* parms );
	stateResult_t			Fire( stateParms_t* parms );
	stateResult_t			Reload( stateParms_t* parms );
};