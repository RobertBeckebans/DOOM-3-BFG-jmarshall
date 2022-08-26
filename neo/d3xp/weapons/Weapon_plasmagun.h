// Weapon_plasmagun.h
//

#pragma once

class iceWeaponPlasmaGun : public iceWeaponObject
{
public:
	CLASS_PROTOTYPE( iceWeaponPlasmaGun );

	virtual void			Init( idWeapon* weapon );

	stateResult_t			Raise( stateParms_t* parms );
	stateResult_t			Lower( stateParms_t* parms );
	stateResult_t			Idle( stateParms_t* parms );
	stateResult_t			Fire( stateParms_t* parms );
	stateResult_t			Reload( stateParms_t* parms );
private:
	float					spread;

	const idSoundShader*		snd_lowammo;
};