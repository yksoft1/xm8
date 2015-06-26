//
// eXcellent Multi-platform emulator type 8 - 'XM8'
// based on ePC-8801MA
//
// Author (ePC-8801MA) : Takeda.Toshiya
// Author (XM8) : Tanaka.Yasushi
//
// [ menu id ]
//

#ifdef SDL

#ifndef MENUID_H
#define MENUID_H

//
// global id
//
#define MENU_BACK				0
										// back to upper menu

//
// menu id
//
#define MENU_MAIN				1
										// main menu
#define MENU_DRIVE1				2
										// drive1 menu
#define MENU_DRIVE2				3
										// drive2 menu
#define MENU_CMT				4
										// cmt menu
#define MENU_LOAD				5
										// load menu
#define MENU_SAVE				6
										// save menu
#define MENU_SYSTEM				7
										// system menu
#define MENU_VIDEO				8
										// video menu
#define MENU_AUDIO				9
										// audio menu
#define MENU_INPUT				10
										// input menu
#define MENU_RESET				11
										// reset menu
#define MENU_QUIT				12
										// quit menu
#define MENU_SOFTKEY1			13
										// softkey menu 1
#define MENU_SOFTKEY2			14
										// softkey menu 2
#define MENU_SOFTKEY3			15
										// softkey menu 3
#define MENU_SOFTKEY4			16
										// softkey menu 4
#define MENU_FILE				17
										// file menu

//
// main menu
//
#define MENU_MAIN_MIN			100
										// minimum
#define MENU_MAIN_DRIVE1		100
										// drive 1
#define MENU_MAIN_DRIVE2		101
										// drive 2
#define MENU_MAIN_CMT			102
										// cmt
#define MENU_MAIN_LOAD			103
										// load state
#define MENU_MAIN_SAVE			104
										// save state
#define MENU_MAIN_SYSTEM		105
										// system options
#define MENU_MAIN_VIDEO			106
										// video options
#define MENU_MAIN_AUDIO			107
										// audio options
#define MENU_MAIN_INPUT			108
										// input options
#define MENU_MAIN_SCREEN		109
										// full/window screen
#define MENU_MAIN_SPEED			110
										// full/normal speed
#define MENU_MAIN_RESET			111
										// reset
#define MENU_MAIN_QUIT			112
										// quit
#define MENU_MAIN_MAX			199
										// maximum

//
// drive1 menu
//
#define MENU_DRIVE1_MIN			200
										// minimum
#define MENU_DRIVE1_BANK0		200
										// bank 0
#define MENU_DRIVE1_OPEN		264
										// open
#define MENU_DRIVE1_BOTH		265
										// open drive1 & drive2
#define MENU_DRIVE1_EJECT		266
										// eject
#define MENU_DRIVE1_MAX			299
										// maximum

//
// drive2 menu
//
#define MENU_DRIVE2_MIN			300
										// minimum
#define MENU_DRIVE2_BANK0		300
										// bank 0
#define MENU_DRIVE2_OPEN		364
										// open
#define MENU_DRIVE2_BOTH		365
										// open drive1 & drive2
#define MENU_DRIVE2_EJECT		366
										// eject
#define MENU_DRIVE2_MAX			399
										// maximum

//
// cmt menu
//
#define MENU_CMT_MIN			400
										// minimum
#define MENU_CMT_PLAY			401
										// play
#define MENU_CMT_REC			402
										// rec
#define MENU_CMT_EJECT			403
										// eject
#define MENU_CMT_MAX			499
										// maximum

//
// load menu
//
#define MENU_LOAD_MIN			500
										// minimum
#define MENU_LOAD_0				500
										// slot 0
#define MENU_LOAD_1				501
										// slot 1
#define MENU_LOAD_2				502
										// slot 2
#define MENU_LOAD_3				503
										// slot 3
#define MENU_LOAD_4				504
										// slot 4
#define MENU_LOAD_5				505
										// slot 5
#define MENU_LOAD_6				506
										// slot 6
#define MENU_LOAD_7				507
										// slot 7
#define MENU_LOAD_8				508
										// slot 8
#define MENU_LOAD_9				509
										// slot 9
#define MENU_LOAD_MAX			599
										// maximum

//
// save menu
//
#define MENU_SAVE_MIN			600
										// minimum
#define MENU_SAVE_0				600
										// slot 0
#define MENU_SAVE_1				601
										// slot 1
#define MENU_SAVE_2				602
										// slot 2
#define MENU_SAVE_3				603
										// slot 3
#define MENU_SAVE_4				604
										// slot 4
#define MENU_SAVE_5				605
										// slot 5
#define MENU_SAVE_6				606
										// slot 6
#define MENU_SAVE_7				607
										// slot 7
#define MENU_SAVE_8				608
										// slot 8
#define MENU_SAVE_9				609
										// slot 9
#define MENU_SAVE_MAX			699
										// maximum

//
// system menu
//
#define MENU_SYSTEM_MIN			700
										// minimum
#define MENU_SYSTEM_V1S			700
										// V1S mode
#define MENU_SYSTEM_V1H			701
										// V1H mode
#define MENU_SYSTEM_V2			702
										// V2 mode
#define MENU_SYSTEM_N			703
										// N mode
#define MENU_SYSTEM_4M			704
										// clock 4MHz
#define MENU_SYSTEM_8M			705
										// clock 8MHz
#define MENU_SYSTEM_8MH			706
										// clock 8MHz (FE2/MC)
#define MENU_SYSTEM_WAIT		707
										// memory wait
#define MENU_SYSTEM_ROMVER		708
										// rom version
#define MENU_SYSTEM_MODE		797
										// mode radio
#define MENU_SYSTEM_CLOCK		798
										// clock radio
#define MENU_SYSTEM_MAX			799
										// maximum

//
// video menu
//
#define MENU_VIDEO_MIN			800
										// minimum
#define MENU_VIDEO_640			801
										// x1.0
#define MENU_VIDEO_960			802
										// x1.5
#define MENU_VIDEO_1280			803
										// x2.0
#define MENU_VIDEO_1600			804
										// x2.5
#define MENU_VIDEO_1920			805
										// x3.0
#define MENU_VIDEO_SKIP0		806
										// 0 frame skip (ex:60fps)
#define MENU_VIDEO_SKIP1		807
										// 1 frame skip (ex:30fps)
#define MENU_VIDEO_SKIP2		808
										// 2 frame skip (ex:20fps)
#define MENU_VIDEO_SKIP3		809
										// 3 frame skip (ex:15fps)
#define MENU_VIDEO_15K			810
										// 15kHz monitor
#define MENU_VIDEO_24K			811
										// 24kHz monitor
#define MENU_VIDEO_SCANLINE		812
										// scanline
#define MENU_VIDEO_BRIGHTNESS	813
										// brightness
#define MENU_VIDEO_STATUSCHK	814
										// status line
#define MENU_VIDEO_STATUSALPHA	815
										// status alpha
#define MENU_VIDEO_SCALEFILTER	816
										// scaling filter
#define MENU_VIDEO_WINDOW		896
										// window radio
#define MENU_VIDEO_SKIP			897
										// skip radio
#define MENU_VIDEO_MONITOR		898
										// monitor radio
#define MENU_VIDEO_MAX			899
										// maximum

//
// audio menu
//
#define MENU_AUDIO_MIN			900
										// minimum
#define MENU_AUDIO_44100		901
										// 44100Hz
#define MENU_AUDIO_48000		902
										// 48000Hz
#define MENU_AUDIO_55467		903
										// 55467Hz
#define MENU_AUDIO_88200		904
										// 88200Hz
#define MENU_AUDIO_96000		905
										// 96000Hz
#define MENU_AUDIO_BUFFER		906
										// buffer
#define MENU_AUDIO_OPN			907
										// YM2203 (SR/FR/TR/MR/FH/MH/FE/FE2/VA/DO)
#define MENU_AUDIO_OPNA			908
										// YM2608 (FA/MA/MA2/MC/VA2/VA3/DO+)
#define MENU_AUDIO_FREQ			997
										// frequency radio
#define MENU_AUDIO_SBII			998
										// sound board II radio
#define MENU_AUDIO_MAX			999
										// maximum

//
// input menu
//
#define MENU_INPUT_MIN			1000
										// minimum
#define MENU_INPUT_SOFTKEY1		1001
										// softkey 1
#define MENU_INPUT_SOFTKEY2		1002
										// softkey 2
#define MENU_INPUT_SOFTKEY3		1003
										// softkey 3
#define MENU_INPUT_SOFTKEY4		1004
										// softkey 4
#define MENU_INPUT_SOFTBRI		1005
										// softkey brightness
#define MENU_INPUT_SOFTALPHA	1006
										// softkey alpha
#define MENU_INPUT_SOFTTIME		1007
										// softkey timeout
#define MENU_INPUT_JOYENABLE	1008
										// joystick enable
#define MENU_INPUT_JOYSWAP		1009
										// joystick button swap
#define MENU_INPUT_JOYKEY		1010
										// joystick to keyboard
#define MENU_INPUT_MOUSETIME	1011
										// mouse timeout
#define MENU_INPUT_KEYENABLE	1012
										// keyboard enable
#define MENU_INPUT_MAX			1099
										// maximum

//
// reset menu
//
#define MENU_RESET_MIN			1100
										// minimum
#define MENU_RESET_YES			1101
										// yes
#define MENU_RESET_NO			1102
										// no
#define MENU_RESET_MAX			1199
										// maximum

//
// quit menu
//
#define MENU_QUIT_MIN			1200
										// minimum
#define MENU_QUIT_YES			1201
										// yes
#define MENU_QUIT_NO			1202
										// no
#define MENU_QUIT_MAX			1299
										// maximum

//
// softkey menu
//
#define MENU_SOFTKEY_MIN		1300
										// minimum
#define MENU_SOFTKEY_0			1300
										// softkey type 0
#define MENU_SOFTKEY_1			1301
										// softkey type 1
#define MENU_SOFTKEY_2			1302
										// softkey type 2
#define MENU_SOFTKEY_3			1303
										// softkey type 3
#define MENU_SOFTKEY_4			1304
										// softkey type 4
#define MENU_SOFTKEY_5			1305
										// softkey type 5
#define MENU_SOFTKEY_6			1306
										// softkey type 6
#define MENU_SOFTKEY_7			1307
										// softkey type 7
#define MENU_SOFTKEY_8			1308
										// softkey type 8
#define MENU_SOFTKEY_9			1309
										// softkey type 9
#define MENU_SOFTKEY_10			1310
										// softkey type 10
#define MENU_SOFTKEY_11			1311
										// softkey type 11
#define MENU_SOFTKEY_MAX		1399
										// maximum

//
// file menu
//
#define MENU_FILE_MIN			10000
										// minimum

#endif // MENUID_H

#endif // SDL
