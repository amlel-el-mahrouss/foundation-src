#pragma once

#include "VTable.h"
#include "Foundation.h"

typedef void* BaseCombatWeapon;
typedef void* BaseViewModel;
typedef void* FuncLadder;
typedef void* FlashlightEffect;
typedef void* EconWearable;

class CCommandContext
{
public:
	bool			needsprocessing;

	void*			cmd; // CUserCmd
	int				command_number;

};

#define CHASE_CAM_DISTANCE_MIN	16.0f
#define CHASE_CAM_DISTANCE_MAX	96.0f
#define WALL_OFFSET				6.0f

class CPredictionError
{
public:
	float	time;
	void*	error; // where did it happen? (vector)

};

class CBasePlayer {
public:
	virtual void	Spawn(void) = 0;
	virtual void	SharedSpawn() = 0; // Shared between client and server.
	virtual bool	GetSteamID(void* pID) = 0;

	// IClientEntity overrides.
	virtual void	OnPreDataChanged(void* updateType) = 0;
	virtual void	OnDataChanged(void* updateType) = 0;

	virtual void	PreDataUpdate(void* updateType) = 0;
	virtual void	PostDataUpdate(void* updateType) = 0;

	virtual void	ReceiveMessage(int classID, void* msg) = 0;

	virtual void	OnRestore() = 0;

	virtual void	AddEntity(void) = 0;

	virtual void	MakeTracer(const void* vecTracerSrc, const void* tr, int iTracerType) = 0;

	virtual void	GetToolRecordingState(void* msg) = 0;

	virtual float	GetPlayerMaxSpeed() = 0;

	virtual void	SetAnimationExtension(const char* pExtension) = 0;

	virtual void*	GetViewModel(int viewmodelindex = 0, bool bObserverOK = true) = 0;
	virtual void*	GetActiveWeapon(void) const = 0;
	virtual const char* GetTracerType(void) = 0;

	// View model prediction setup
	virtual void	CalcView(void* eyeOrigin, void* eyeAngles, float& zNear, float& zFar, float& fov) = 0;
	virtual void	CalcViewModelView(const void* eyeOrigin, const void* eyeAngles) = 0;


	// Handle view smoothing when going up stairs
	virtual void	SmoothViewOnStairs(void* eyeOrigin) = 0;
	virtual float	CalcRoll(const void* angles, const void* velocity, float rollangle, float rollspeed) = 0;
	virtual void	CalcViewRoll(void* eyeAngles) = 0;
	virtual void	CreateWaterEffects(void) = 0;

	virtual void	SetPlayerUnderwater(bool state) = 0;
	virtual void	UpdateUnderwaterState(void) = 0;
	virtual bool	IsPlayerUnderwater(void) = 0;

	virtual void*	Weapon_ShootPosition() = 0;
	virtual void	Weapon_DropPrimary(void) = 0;

	virtual void*	GetAutoaimVector(float flScale) = 0;
	virtual void	SetSuitUpdate(const char* name, int fgroup, int iNoRepeat) = 0;

	virtual bool	CreateMove(float flInputSampleTime, void* pCmd) = 0;
	virtual void	AvoidPhysicsProps(void* pCmd) = 0;

	virtual void	PlayerUse(void) = 0;
	virtual void*   FindUseEntity(void) = 0;
	virtual bool	IsUseableEntity(void* pEntity, unsigned int requiredCaps) = 0;

	// Data handlers
	virtual bool	IsPlayer(void) const = 0;
	virtual int		GetHealth() const = 0;

	virtual int		GetBonusProgress() const = 0;
	virtual int		GetBonusChallenge() const = 0;

	// observer mode
	virtual int		GetObserverMode() const = 0;
	virtual void	SetObserverMode(int iNewMode) = 0;
	virtual void*   GetObserverTarget() const = 0;
	virtual void	SetObserverTarget(void* hObserverTarget) = 0;

	virtual bool	AudioStateIsUnderwater(void* vecMainViewOrigin) = 0;


	virtual bool IsObserver() const = 0;
	virtual bool IsHLTV() const = 0;
	virtual bool IsReplay() const = 0;
	virtual void ResetObserverMode() = 0;
	virtual bool IsBot(void) const = 0;

	// Eye position..
	virtual void*		 EyePosition() = 0;
	virtual const void*	EyeAngles() = 0;		// Direction of eyes
	void				 EyePositionAndVectors(void* pPosition, void* pForward, void* pRight, void* pUp) = 0;
	virtual const void* LocalEyeAngles() = 0;		// Direction of eyes

	// This can be overridden to return something other than m_pRagdoll if the mod uses separate 
	// entities for ragdolls.
	virtual void* GetRepresentativeRagdoll() const = 0;

};

typedef bool (*freezecam_func_t)(void);