/*
===========================================================================

Doom 3 BFG Edition GPL Source Code
Copyright (C) 1993-2012 id Software LLC, a ZeniMax Media company.

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

#ifndef __GAME_WEAPON_H__
#define __GAME_WEAPON_H__

#include "PredictedValue.h"

/*
===============================================================================

	Player Weapon

===============================================================================
*/

extern const idEventDef EV_Weapon_State;

typedef enum
{
	WP_NONE,
	WP_IDLE,
	WP_READY,
	WP_OUTOFAMMO,
	WP_RELOAD,
	WP_HOLSTERED,
	WP_RISING,
	WP_LOWERING,
	WP_FIRE
} weaponStatus_t;

typedef int ammo_t;
static const int AMMO_NUMTYPES = 16;

class idPlayer;

static const int LIGHTID_WORLD_MUZZLE_FLASH = 1;
static const int LIGHTID_VIEW_MUZZLE_FLASH = 100;

class idMoveableItem;

typedef struct
{
	char			name[64];
	char			particlename[128];
	bool			active;
	int				startTime;
	jointHandle_t	joint;			//The joint on which to attach the particle
	bool			smoke;			//Is this a smoke particle
	const idDeclParticle* particle;		//Used for smoke particles
	idFuncEmitter*  emitter;		//Used for non-smoke particles
} WeaponParticle_t;

typedef struct
{
	char			name[64];
	bool			active;
	int				startTime;
	jointHandle_t	joint;
	int				lightHandle;
	renderLight_t	light;
} WeaponLight_t;

class rvmWeaponObject : public idClass
{
public:
	CLASS_PROTOTYPE( rvmWeaponObject );

	virtual void			Init( idWeapon* weapon );

	virtual void			Raise() { }
	virtual void			Idle() { }
	virtual void			Fire() { }
	virtual void			Lower() { }
	virtual void			Reload() { }

	virtual bool			CanFire()
	{
		return true;
	}

	virtual void			ResetStates( void );

	virtual bool			CanSwitchState( void );

	virtual bool			HasWaitSignal( void );

	virtual bool			IsHolstered( void )
	{
		return isHolstered;
	}
	virtual bool			IsRisen( void )
	{
		return isRisen;
	}
protected:
	idWeapon* owner;

	const idSoundShader*	FindSound( const char* name );

	bool					isHolstered;
	bool					isRisen;

	void					Wait( float duration );
protected:
	int	risingState;
	int	loweringState;
	int idleState;
	int firingState;
	int reloadState;
	float waitDuration;
};

class idWeapon : public idAnimatedEntity
{
public:
	CLASS_PROTOTYPE( idWeapon );

	idWeapon();
	virtual					~idWeapon();

	// Init
	void					Spawn();
	void					SetOwner( idPlayer* owner );
	idPlayer*				GetOwner();
	virtual bool			ShouldConstructScriptObjectAtSpawn() const;
	void					SetFlashlightOwner( idPlayer* owner );

	static void				CacheWeapon( const char* weaponName );

	// save games
	void					Save( idSaveGame* savefile ) const;					// archives object for save game file
	void					Restore( idRestoreGame* savefile );					// unarchives object from save game file

	// Weapon definition management
	void					Clear();
	void					GetWeaponDef( const char* objectname, int ammoinclip );
	bool					IsWorldModelReady();

	// GUIs
	const char* 			Icon() const;
	void					UpdateGUI();
	const char* 			PdaIcon() const;
	const char* 			DisplayName() const;
	const char* 			Description() const;

	virtual void			SetModel( const char* modelname );
	bool					GetGlobalJointTransform( bool viewModel, const jointHandle_t jointHandle, idVec3& offset, idMat3& axis );
	void					SetPushVelocity( const idVec3& pushVelocity );
	bool					UpdateSkin();

	bool					IsFiring()
	{
		return isFiring;
	}

	// State control/player interface
	void					Think();
	void					Raise();
	void					PutAway();
	void					Reload();
	void					LowerWeapon();
	void					RaiseWeapon();
	void					HideWeapon();
	void					ShowWeapon();
	void					HideWorldModel();
	void					ShowWorldModel();
	void					OwnerDied();
	void					BeginAttack();
	void					EndAttack();
	bool					IsReady() const;
	bool					IsReloading() const;
	bool					IsHolstered() const;
	bool					ShowCrosshair() const;
	idEntity* 				DropItem( const idVec3& velocity, int activateDelay, int removeDelay, bool died );
	bool					CanDrop() const;
	void					WeaponStolen();
	void					ForceAmmoInClip();

	weaponStatus_t			GetStatus()
	{
		return status;
	};

	// Visual presentation
	void					PresentWeapon( bool showViewModel );
	int						GetZoomFov();
	void					GetWeaponAngleOffsets( int* average, float* scale, float* max );
	void					GetWeaponTimeOffsets( float* time, float* scale );
	bool					BloodSplat( float size );
	void					SetIsPlayerFlashlight( bool bl )
	{
		isPlayerFlashlight = bl;
	}
	void					FlashlightOn();
	void					FlashlightOff();

	// Ammo
	static ammo_t			GetAmmoNumForName( const char* ammoname );
	static const char*		GetAmmoNameForNum( ammo_t ammonum );
	static const char*		GetAmmoPickupNameForNum( ammo_t ammonum );
	ammo_t					GetAmmoType() const;
	int						AmmoAvailable() const;
	int						AmmoInClip() const;
	void					ResetAmmoClip();
	int						ClipSize() const;
	int						LowAmmo() const;
	int						AmmoRequired() const;
	int						AmmoCount() const;
	int						GetGrabberState() const;

	// Flashlight
	idAnimatedEntity* 		GetWorldModel()
	{
		return worldModel.GetEntity();
	}

	virtual void			WriteToSnapshot( idBitMsg& msg ) const;
	virtual void			ReadFromSnapshot( const idBitMsg& msg );

	enum
	{
		EVENT_RELOAD = idEntity::EVENT_MAXEVENTS,
		EVENT_ENDRELOAD,
		EVENT_CHANGESKIN,
		EVENT_MAXEVENTS
	};
	virtual bool			ClientReceiveEvent( int event, int time, const idBitMsg& msg );

	virtual void			ClientPredictionThink();
	virtual void			ClientThink( const int curTime, const float fraction, const bool predict );
	void					MuzzleFlashLight();
	void					RemoveMuzzleFlashlight();

	// Get a global origin and axis suitable for the laser sight or bullet tracing
	// Returns false for hands, grenades, and chainsaw.
	// Can't be const because a frame may need to be created.
	bool					GetMuzzlePositionWithHacks( idVec3& origin, idMat3& axis );

	void					GetProjectileLaunchOriginAndAxis( idVec3& origin, idMat3& axis );

	const idDeclEntityDef* GetDeclEntityDef()
	{
		return weaponDef;
	}

	friend class idPlayer;
public:
	virtual void			CallNativeEvent( idStr& name ) override;

	void					SetState( weaponStatus_t state, int blendFrames );
	void					WeaponState( weaponStatus_t state, int blendFrames );
	void					Event_SetLightParm( int parmnum, float value );
	void					Event_SetLightParms( float parm0, float parm1, float parm2, float parm3 );

	// script events
	void					Event_Clear();
	void					Event_GetOwner();
	void					Event_SetWeaponStatus( float newStatus );
	void					Event_WeaponReady();
	void					Event_WeaponOutOfAmmo();
	void					Event_WeaponReloading();
	void					Event_WeaponHolstered();
	void					Event_WeaponRising();
	void					Event_WeaponLowering();
	void					Event_UseAmmo( int amount );
	void					Event_AddToClip( int amount );
	void					Event_AmmoInClip();
	void					Event_AmmoAvailable();
	void					Event_TotalAmmoCount();
	void					Event_ClipSize();
	void					Event_PlayAnim( int channel, const char* animname, bool loop );
	void					Event_PlayCycle( int channel, const char* animname );
	bool					Event_AnimDone( int channel, int blendFrames );
	void					Event_SetBlendFrames( int channel, int blendFrames );
	void					Event_GetBlendFrames( int channel );
	void					Event_Next();
	void					Event_SetSkin( const char* skinname );
	void					Event_Flashlight( int enable );
	void					Event_GetLightParm( int parmnum );
	void					Event_LaunchProjectiles( int num_projectiles, float spread, float fuseOffset, float launchPower, float dmgPower );
	void					Event_CreateProjectile();
	void					Event_EjectBrass();
	void					Event_Melee();
	void					Event_GetWorldModel();
	void					Event_AllowDrop( int allow );
	void					Event_AutoReload();
	void					Event_NetReload();
	bool					Event_IsInvisible();
	void					Event_NetEndReload();

	void					EnterCinematic();
	void					ExitCinematic();
	void					NetCatchup();

	bool					IsLinked()
	{
		return isLinked;
	}
private:
	weaponStatus_t			status;
	weaponStatus_t			state;
	weaponStatus_t			idealState;
	int						animBlendFrames;
	int						animDoneTime;
	bool					isPlayerFlashlight;
	bool					isLinked;

	// precreated projectile
	idEntity*				projectileEnt;

	idPlayer* 				owner;
	idEntityPtr<idAnimatedEntity>	worldModel;

	// hiding (for GUIs and NPCs)
	int						hideTime;
	float					hideDistance;
	int						hideStartTime;
	float					hideStart;
	float					hideEnd;
	float					hideOffset;
	bool					hide;
	bool					disabled;

	// berserk
	int						berserk;

	// these are the player render view parms, which include bobbing
	idVec3					playerViewOrigin;
	idMat3					playerViewAxis;

	// the view weapon render entity parms
	idVec3					viewWeaponOrigin;
	idMat3					viewWeaponAxis;

	// the muzzle bone's position, used for launching projectiles and trailing smoke
	idVec3					muzzleOrigin;
	idMat3					muzzleAxis;

	idVec3					pushVelocity;

	// weapon definition
	// we maintain local copies of the projectile and brass dictionaries so they
	// do not have to be copied across the DLL boundary when entities are spawned
	const idDeclEntityDef* 	weaponDef;
	const idDeclEntityDef* 	meleeDef;
	idDict					projectileDict;
	float					meleeDistance;
	idStr					meleeDefName;
	idDict					brassDict;
	int						brassDelay;
	idStr					icon;
	idStr					pdaIcon;
	idStr					displayName;
	idStr					itemDesc;

	// view weapon gui light
	renderLight_t			guiLight;
	int						guiLightHandle;

	// muzzle flash
	renderLight_t			muzzleFlash;		// positioned on view weapon bone
	int						muzzleFlashHandle;

	renderLight_t			worldMuzzleFlash;	// positioned on world weapon bone
	int						worldMuzzleFlashHandle;

	float					fraccos;
	float					fraccos2;

	idVec3					flashColor;
	int						muzzleFlashEnd;
	int						flashTime;
	bool					lightOn;
	bool					silent_fire;
	bool					allowDrop;

	// effects
	bool					hasBloodSplat;

	// weapon kick
	int						kick_endtime;
	int						muzzle_kick_time;
	int						muzzle_kick_maxtime;
	idAngles				muzzle_kick_angles;
	idVec3					muzzle_kick_offset;

	// ammo management
	ammo_t					ammoType;
	int						ammoRequired;		// amount of ammo to use each shot.  0 means weapon doesn't need ammo.
	int						clipSize;			// 0 means no reload
	idPredictedValue< int >	ammoClip;
	int						lowAmmo;			// if ammo in clip hits this threshold, snd_
	bool					powerAmmo;			// true if the clip reduction is a factor of the power setting when
	// a projectile is launched
	// mp client
	bool					isFiring;

	// zoom
	int						zoomFov;			// variable zoom fov per weapon

	// joints from models
	jointHandle_t			barrelJointView;
	jointHandle_t			flashJointView;
	jointHandle_t			ejectJointView;
	jointHandle_t			guiLightJointView;
	jointHandle_t			ventLightJointView;

	jointHandle_t			flashJointWorld;
	jointHandle_t			barrelJointWorld;
	jointHandle_t			ejectJointWorld;

	jointHandle_t			smokeJointView;

	idHashTable<WeaponParticle_t>	weaponParticles;
	idHashTable<WeaponLight_t>		weaponLights;

	// sound
	const idSoundShader* 	sndHum;

	// new style muzzle smokes
	const idDeclParticle* 	weaponSmoke;			// null if it doesn't smoke
	int						weaponSmokeStartTime;	// set to gameLocal.time every weapon fire
	bool					continuousSmoke;		// if smoke is continuous ( chainsaw )
	const idDeclParticle*   strikeSmoke;			// striking something in melee
	int						strikeSmokeStartTime;	// timing
	idVec3					strikePos;				// position of last melee strike
	idMat3					strikeAxis;				// axis of last melee strike
	int						nextStrikeFx;			// used for sound and decal ( may use for strike smoke too )

	// nozzle effects
	bool					nozzleFx;			// does this use nozzle effects ( parm5 at rest, parm6 firing )
	// this also assumes a nozzle light atm
	int						nozzleFxFade;		// time it takes to fade between the effects
	int						lastAttack;			// last time an attack occured
	renderLight_t			nozzleGlow;			// nozzle light
	int						nozzleGlowHandle;	// handle for nozzle light

	idVec3					nozzleGlowColor;	// color of the nozzle glow
	const idMaterial* 		nozzleGlowShader;	// shader for glow light
	float					nozzleGlowRadius;	// radius of glow light

	// weighting for viewmodel angles
	int						weaponAngleOffsetAverages;
	float					weaponAngleOffsetScale;
	float					weaponAngleOffsetMax;
	float					weaponOffsetTime;
	float					weaponOffsetScale;

	// flashlight
	void					AlertMonsters();

	// Visual presentation
	void					InitWorldModel( const idDeclEntityDef* def );
	void					MuzzleRise( idVec3& origin, idMat3& axis );
	void					UpdateNozzleFx();
	void					UpdateFlashPosition();

	idGrabber				grabber;
	int						grabberState;
public:
	void					Event_Grabber( int enable );
	int						Event_GrabberHasTarget();
	void					Event_GrabberSetGrabDistance( float dist );
	void					Event_LaunchProjectilesEllipse( int num_projectiles, float spreada, float spreadb, float fuseOffset, float power );
	void					Event_LaunchPowerup( const char* powerup, float duration, int useAmmo );

	void					Event_StartWeaponSmoke();
	void					Event_StopWeaponSmoke();

	void					Event_StartWeaponParticle( const char* name );
	void					Event_StopWeaponParticle( const char* name );

	void					Event_StartWeaponLight( const char* name );
	void					Event_StopWeaponLight( const char* name );
private:
	rvmWeaponObject* currentWeaponObject;
};

ID_INLINE bool idWeapon::IsWorldModelReady()
{
	return ( worldModel.GetEntity() != NULL );
}

ID_INLINE idPlayer* idWeapon::GetOwner()
{
	return owner;
}

#endif /* !__GAME_WEAPON_H__ */