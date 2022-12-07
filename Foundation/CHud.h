#pragma once

#include "VTable.h"
#include "Foundation.h"

constexpr auto NET_MESSAGE_BITS = 6;
constexpr auto IN_ATTACK = (1 << 0);
constexpr auto IN_JUMP = (1 << 1);
constexpr auto IN_DUCK = (1 << 2);
constexpr auto IN_FORWARD = (1 << 3);
constexpr auto IN_BACK = (1 << 4);
constexpr auto IN_USE = (1 << 5);
constexpr auto IN_CANCEL = (1 << 6);
constexpr auto IN_LEFT = (1 << 7);
constexpr auto IN_RIGHT = (1 << 8);
constexpr auto IN_MOVELEFT = (1 << 9);
constexpr auto IN_MOVERIGHT = (1 << 10);
constexpr auto IN_ATTACK2 = (1 << 11);
constexpr auto IN_RUN = (1 << 12);
constexpr auto IN_RELOAD = (1 << 13);
constexpr auto IN_ALT1 = (1 << 14);
constexpr auto IN_SCORE = (1 << 15);

enum
{
	PARTITION_ENGINE_SOLID_EDICTS = (1 << 0),		// every edict_t that isn't SOLID_TRIGGER or SOLID_NOT (and static props)
	PARTITION_ENGINE_TRIGGER_EDICTS = (1 << 1),		// every edict_t that IS SOLID_TRIGGER
	PARTITION_CLIENT_SOLID_EDICTS = (1 << 2),
	PARTITION_CLIENT_RESPONSIVE_EDICTS = (1 << 3),		// these are client-side only objects that respond to being forces, etc.
	PARTITION_ENGINE_NON_STATIC_EDICTS = (1 << 4),		// everything in solid & trigger except the static props, includes SOLID_NOTs
	PARTITION_CLIENT_STATIC_PROPS = (1 << 5),
	PARTITION_ENGINE_STATIC_PROPS = (1 << 6),
	PARTITION_CLIENT_NON_STATIC_EDICTS = (1 << 7),		// everything except the static props
};

//
// Commands for the screen shake effect.
//

struct ScreenShake
{
	int		command;
	float	amplitude;
	float	frequency;
	float	duration;
};

enum ShakeCommand
{
	SHAKE_START = 0,		// Starts the screen shake for all players within the radius.
	SHAKE_STOP,				// Stops the screen shake for all players within the radius.
	SHAKE_AMPLITUDE,		// Modifies the amplitude of an active screen shake for all players within the radius.
	SHAKE_FREQUENCY,		// Modifies the frequency of an active screen shake for all players within the radius.
	SHAKE_START_RUMBLEONLY,	// Starts a shake effect that only rumbles the controller, no screen effect.
	SHAKE_START_NORUMBLE,	// Starts a shake that does NOT rumble the controller.
};

#define FFADE_IN			0x0001		// Just here so we don't pass 0 into the function
#define FFADE_OUT			0x0002		// Fade out (not in)
#define FFADE_MODULATE		0x0004		// Modulate (don't blend)
#define FFADE_STAYOUT		0x0008		// ignores the duration, stays faded out until new ScreenFade message received
#define FFADE_PURGE			0x0010		// Purges all other fades, replacing them with this one

#define SCREENFADE_FRACBITS		9		// which leaves 16-this for the integer part

// This structure is sent over the net to describe a screen fade event
struct ScreenFade
{
	unsigned short 	duration;		// FIXED 16 bit, with SCREENFADE_FRACBITS fractional, seconds duration
	unsigned short 	holdTime;		// FIXED 16 bit, with SCREENFADE_FRACBITS fractional, seconds duration until reset (fade & hold)
	short			fadeFlags;		// flags
	char			r, g, b, a;		// fade to color ( max alpha )
};

typedef unsigned long HFont;

#define OUT_Z_CAP(SIZE) _Out_z_cap_(SIZE)
#define OUT_Z_BYTECAP(SIZE) _Out_z_bytecap_(SIZE)

#define XRES(x)	( x  * ( ( float )ScreenWidth() / 640.0 ) )
#define YRES(y)	( y  * ( ( float )ScreenHeight() / 480.0 ) )

enum
{
	LIGHT_INDEX_TE_DYNAMIC = 0x10000000,
	LIGHT_INDEX_PLAYER_BRIGHT = 0x20000000,
	LIGHT_INDEX_MUZZLEFLASH = 0x40000000,
};

#define MAX_SPHERE_QUERY	256

typedef void (v_func_t)(void);
typedef void* (undefined_func_t)();

class SourceChatLine
{
public:
	virtual void	ApplySchemeSettings(void* pScheme) = 0;
	virtual void    PerformFadeout(void) = 0;
	virtual void	MsgFunc_SayText(void* msg) = 0;

};

//-----------------------------------------------------------------------------
// Purpose: The prompt and text entry area for chat messages
//-----------------------------------------------------------------------------
class SourceChatInputLine
{
public:
	virtual void	ApplySchemeSettings(void* pScheme) = 0;

};

class CHudChat
{
public:
	virtual void	CreateChatInputLine(void) = 0;
	virtual void	CreateChatLines(void) = 0;

	virtual void	Init(void) = 0;
	virtual void	Reset(void) = 0;
	virtual void	ApplySchemeSettings(void* pScheme) = 0;

	virtual void	MsgFunc_SayText(void* msg);
	virtual void	MsgFunc_TextMsg(void* msg);

	virtual void	ChatPrintf(int iPlayerIndex, const char* fmt, ...) = 0;

	virtual int		GetChatInputOffset(void) = 0;

};
