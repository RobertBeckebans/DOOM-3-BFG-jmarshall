// Weapon_shotgun.h
//

#pragma once

class iceWeaponShotgun : public iceWeaponObject
{
public:
	CLASS_PROTOTYPE( iceWeaponShotgun );

	virtual void			Init( idWeapon* weapon );

	stateResult_t			Raise( stateParms_t* parms );
	stateResult_t			Lower( stateParms_t* parms );
	stateResult_t			Idle( stateParms_t* parms );
	stateResult_t			Fire( stateParms_t* parms );
	stateResult_t			Reload( stateParms_t* parms );
private:
	float		spread;

	const idSoundShader*		snd_lowammo;
};