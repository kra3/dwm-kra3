/* See LICENSE file for copyright and license details. */

#include "selfrestart.c"
#include "bstack.c"
#include "bstackhoriz.c"

/* appearance */
static const char font[]            = "-*-terminus-medium-r-*-*-12-*-*-*-*-*-*-*";
static const char normbordercolor[] = "#444444";
static const char normbgcolor[]     = "#111111";
static const char normfgcolor[]     = "#268bd2";
static const char selbordercolor[]  = "#444444";
static const char selbgcolor[]      = "#222222";
static const char selfgcolor[]      = "#268bd2";
static const unsigned int borderpx  = 1;        /* border pixel of windows */
static const unsigned int snap      = 32;       /* snap pixel */
static const unsigned int systrayspacing = 2;   /* systray spacing */
static const Bool showsystray       = True;     /* False means no systray */
static const Bool showbar           = True;     /* False means no bar */
static const Bool topbar            = True;     /* False means bottom bar */

/* tagging */
static const char *tags[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9" };

static const Rule rules[] = {
	/* class      instance    title       tags mask     isfloating   monitor */
	{ NULL,                NULL,                NULL,       0,            False,       -1 },
    { "Gimp",              NULL,                NULL,       1 << 8,       True,        -1 },
    { "Thunderbird",       NULL,                NULL,       1 << 3,       False,       -1 },
    { "Firefox",           NULL,                NULL,       1 << 8,       True,        -1 },
    { "Firefox",           "Navigator",         NULL,       1 << 8,       False,       -1 },
    { "Chromium-browser",  "chromium-browser",  NULL,       1 << 2,       False,       -1 },
    { "Sublime_text",       NULL,               NULL,       1 << 1,       False,       -1 },
};

/* layout(s) */
static const float mfact      = 0.55; /* factor of master area size [0.05..0.95] */
static const int nmaster      = 1;    /* number of clients in master area */
static const Bool resizehints = True; /* True means respect size hints in tiled resizals */

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[]=",      tile },    /* first entry is default */
	{ "><>",      NULL },    /* no layout function means floating behavior */
	{ "|D]",      deck },
	{ "[M]",      monocle },
	{ "|||",      bstack },
	{ "===",      bstackhoriz },
};

/* key definitions */
#define MODKEY Mod4Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static const char *dmenucmd[] = { "dmenu_run", "-fn", font, "-nb", normbgcolor, "-nf", normfgcolor, "-sb", selbgcolor, "-sf", selfgcolor, NULL };
static const char *uxtermcmd[]  = { "uxterm", NULL };
static const char *urxvtccmd[]  = { "urxvtc", NULL };
static const char *i3lockcmd[]  = { "i3lock", NULL };

static Key keys[] = {
	/* modifier                     key        function        argument */
	{ MODKEY,                       XK_p,      spawn,          {.v = dmenucmd } },
	{ MODKEY|ControlMask,           XK_Return, spawn,          {.v = uxtermcmd } },
	{ MODKEY|ShiftMask,             XK_Return, spawn,          {.v = urxvtccmd } },
	{ MODKEY|ControlMask|ShiftMask, XK_l,      spawn,          {.v = i3lockcmd } },
    /* on/off the bar */
	{ MODKEY,                       XK_b,      togglebar,      {0} },
    /* rotate focus between windows */
	{ MODKEY,                       XK_j,      focusstack,     {.i = +1 } },
	{ MODKEY,                       XK_k,      focusstack,     {.i = -1 } },
	{ MODKEY,             			XK_Left,   focusstack,     {.i = +1 } },
	{ MODKEY,             			XK_Right,  focusstack,     {.i = -1 } },
    /* rotate through tags - ie., change view to another tag */
	{ MODKEY|ControlMask, 			XK_j,      shiftview,      {.i = -1 } },
	{ MODKEY|ControlMask, 			XK_k,      shiftview,      {.i = +1 } },
	{ MODKEY|ControlMask, 			XK_Left,   shiftview,      {.i = -1 } },
	{ MODKEY|ControlMask, 			XK_Right,  shiftview,      {.i = +1 } },
    /* supposedly adjust number of master area?? */
	{ MODKEY|ControlMask,           XK_i,      incnmaster,     {.i = +1 } },
	{ MODKEY|ControlMask,           XK_d,      incnmaster,     {.i = -1 } },
    /* adjust master area size */
	{ MODKEY,                       XK_h,      setmfact,       {.f = -0.05} },
	{ MODKEY,                       XK_l,      setmfact,       {.f = +0.05} },
    /* change layouts */
	{ MODKEY,                       XK_t,      setlayout,      {.v = &layouts[0]} },
	{ MODKEY,                       XK_f,      setlayout,      {.v = &layouts[1]} },
	{ MODKEY,                       XK_d,      setlayout,      {.v = &layouts[2]} },
	{ MODKEY,                       XK_m,      setlayout,      {.v = &layouts[3]} },
	{ MODKEY,                       XK_v,      setlayout,      {.v = &layouts[4]} },
	{ MODKEY,                       XK_o,      setlayout,      {.v = &layouts[5]} },
	{ MODKEY,                       XK_space,  setlayout,      {0} },
    /* switch floating on/off for focused client */
	{ MODKEY|ShiftMask,             XK_space,  togglefloating, {0} },
    /* kill focused client */
	{ MODKEY|ShiftMask,             XK_c,      killclient,     {0} },
    /* put focused client in master area */
	{ MODKEY,                       XK_Return, zoom,           {0} },
    /* Make current window available in all tags - sticky */
	{ MODKEY|ShiftMask,             XK_0,      tag,            {.ui = ~0 } },
    /* show all clients in current tag */
	{ MODKEY,                       XK_0,      view,           {.ui = ~0 } },
    /* switch to previous tag */
	{ MODKEY,                       XK_Tab,    view,           {0} },
    /* change focused monitor */
	{ MODKEY,                       XK_comma,  focusmon,       {.i = -1 } },
	{ MODKEY,                       XK_period, focusmon,       {.i = +1 } },
    /*  I forgot !! Doesn't have an extra monitor now a days :( */
	{ MODKEY|ShiftMask,             XK_comma,  tagmon,         {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_period, tagmon,         {.i = +1 } },
    /* change to tag */
	TAGKEYS(                        XK_1,                      0)
	TAGKEYS(                        XK_2,                      1)
	TAGKEYS(                        XK_3,                      2)
	TAGKEYS(                        XK_4,                      3)
	TAGKEYS(                        XK_5,                      4)
	TAGKEYS(                        XK_6,                      5)
	TAGKEYS(                        XK_7,                      6)
	TAGKEYS(                        XK_8,                      7)
	TAGKEYS(                        XK_9,                      8)
    /* restart dwm? and quit dwm */
	{ MODKEY|ShiftMask,             XK_r,      self_restart,   {0} },
	{ MODKEY|ShiftMask,             XK_q,      quit,           {0} },
};

/* button definitions */
/* click can be ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[3]} },
	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
	{ ClkStatusText,        0,              Button2,        spawn,          {.v = urxvtccmd } },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
    /* change tag */
	{ ClkTagBar,            0,              Button1,        view,           {0} },
    /* show all clients in another tag simultaneously */
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
    /* move focused client to another tag */
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
    /* show focused client in another tag along too */
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};

