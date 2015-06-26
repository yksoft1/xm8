//
// eXcellent Multi-Platform emulator type 8 - 'XM8'
// based on ePC-8801MA
//
// Author (ePC-8801MA) : Takeda.Toshiya
// Author (XM8) : Tanaka.Yasushi
//
// [ menu driver ]
//

#ifdef SDL

#include "os.h"
#include "common.h"
#include "vm.h"
#include "app.h"
#include "setting.h"
#include "platform.h"
#include "video.h"
#include "font.h"
#include "input.h"
#include "diskmgr.h"
#include "tapemgr.h"
#include "menulist.h"
#include "menuid.h"
#include "menu.h"

//
// Menu()
// constructor
//
Menu::Menu(App *a)
{
	// save parameter
	app = a;

	// object
	platform = NULL;
	setting = NULL;
	video = NULL;
	list = NULL;
	diskmgr = NULL;
	tapemgr = NULL;

	// top id
	top_id = MENU_BACK;

	// file selector
	file_dir[0] = '\0';
	file_target[0] = '\0';
	file_id = MENU_BACK;

	// softkey
	softkey_id = MENU_INPUT_SOFTKEY1;
}

//
// ~Menu()
// destructor
//
Menu::~Menu()
{
	Deinit();
}

//
// Init()
// initialize
//
bool Menu::Init()
{
	// object
	platform = app->GetPlatform();
	setting = app->GetSetting();
	video = app->GetVideo();

	// create menu list
	list = new MenuList(app);
	if (list->Init() == false) {
		return false;
	}

	return true;
}

//
// Deinit()
// deinitialize
//
void Menu::Deinit()
{
	if (list != NULL) {
		list->Deinit();
		delete list;
		list = NULL;
	}
}

//
// EnterMenu()
// enter menu
//
void Menu::EnterMenu(int menu_id)
{
	// get disk manager if NULL
	if (diskmgr == NULL) {
		diskmgr = app->GetDiskManager();
	}

	// get tape manager if NULL
	if (tapemgr == NULL) {
		tapemgr = app->GetTapeManager();
	}

	// save menu id
	top_id = menu_id;

	// initialize softkey id
	softkey_id = MENU_INPUT_SOFTKEY1;

	// inform list of starting menu
	list->EnterMenu();

	// each menu
	switch (menu_id) {
	// main menu
	case MENU_MAIN:
		EnterMain(MENU_MAIN_DRIVE1);
		break;

	// quit menu
	case MENU_QUIT:
		EnterQuit();
		break;

	// others
	default:
		break;
	}
}

//
// UpdateMenu()
// update main menu
//
void Menu::UpdateMenu()
{
	// valid menu ?
	if (list == NULL) {
		return;
	}

	// main menu ?
	if (top_id != MENU_MAIN) {
		return;
	}

#ifndef __ANDROID__
	// screen
	if (app->IsFullScreen() == true) {
		list->SetText(MENU_MAIN_SCREEN, "Window Screen");
	}
	else {
		list->SetText(MENU_MAIN_SCREEN, "Full Screen");
	}
#endif // !__ANDROID__

	// speed
	if (app->IsFullSpeed() == true) {
		list->SetText(MENU_MAIN_SPEED, "Normal Speed");
	}
	else {
		list->SetText(MENU_MAIN_SPEED, "Full Speed");
	}
}

//
// ProcessMenu()
// process menu
//
void Menu::ProcessMenu()
{
	list->ProcessMenu();
}

//
// EnterMain()
// enter main menu
//
void Menu::EnterMain(int id)
{
	char title[80];
	Uint32 ver;

	// get version
	ver = app->GetAppVersion();
	sprintf(title, "<< XM8 Ver.%1d.%1d%1d User Interface >>",
		((ver >> 8) & 0x0f),
		((ver >> 4) & 0x0f),
		ver & 0x0f);

	list->SetTitle(title, MENU_MAIN);
	list->AddButton("Drive 1", MENU_MAIN_DRIVE1);
	list->AddButton("Drive 2", MENU_MAIN_DRIVE2);
	list->AddButton("CMT", MENU_MAIN_CMT);
	list->AddButton("Load State", MENU_MAIN_LOAD);
	list->AddButton("Save State", MENU_MAIN_SAVE);
	list->AddButton("System Options", MENU_MAIN_SYSTEM);
	list->AddButton("Video Options", MENU_MAIN_VIDEO);
	list->AddButton("Audio Options", MENU_MAIN_AUDIO);
	list->AddButton("Input Options", MENU_MAIN_INPUT);

#ifndef __ANDROID__
	// screen
	if (app->IsFullScreen() == true) {
		list->AddButton("Window Screen", MENU_MAIN_SCREEN);
	}
	else {
		list->AddButton("Full Screen", MENU_MAIN_SCREEN);
	}
#endif // !__ANDROID__

	// speed
	if (app->IsFullSpeed() == true) {
		list->AddButton("Normal Speed", MENU_MAIN_SPEED);
	}
	else {
		list->AddButton("Full Speed\n", MENU_MAIN_SPEED);
	}

	list->AddButton("Reset", MENU_MAIN_RESET);
	list->AddButton("Quit XM8", MENU_MAIN_QUIT);

	// set focus
	list->SetFocus(id);
}

//
// EnterDrive1()
// enter drive1 menu
//
void Menu::EnterDrive1(int id)
{
	int banks;
	int loop;

	list->SetTitle("<< Drive 1 >>", MENU_DRIVE1);

	if (diskmgr[0]->IsOpen() == true) {
		// disk banks
		banks = diskmgr[0]->GetBanks();
		for (loop=0; loop<banks; loop++) {
			list->AddButton(diskmgr[0]->GetName(loop), MENU_DRIVE1_BANK0 + loop);
		}

		// current bank -> focus
		if (id == MENU_BACK) {
			id = MENU_DRIVE1_BANK0 + diskmgr[0]->GetBank();
		}
	}

	// open
	list->AddButton("(Open)", MENU_DRIVE1_OPEN);
	list->AddButton("(Open 1 & 2)", MENU_DRIVE1_BOTH);

	// eject
	if (diskmgr[0]->IsOpen() == true) {
		list->AddButton("(Eject)", MENU_DRIVE1_EJECT);
	}

	// set focus
	if (id == MENU_BACK) {
		id = MENU_DRIVE1_OPEN;
	}
	list->SetFocus(id);
}

//
// EnterDrive2()
// enter drive2 menu
//
void Menu::EnterDrive2(int id)
{
	int banks;
	int loop;

	list->SetTitle("<< Drive 2 >>", MENU_DRIVE2);

	if (diskmgr[1]->IsOpen() == true) {
		// disk banks
		banks = diskmgr[1]->GetBanks();
		for (loop=0; loop<banks; loop++) {
			list->AddButton(diskmgr[1]->GetName(loop), MENU_DRIVE2_BANK0 + loop);
		}

		// current bank -> focus
		if (id == MENU_BACK) {
			id = MENU_DRIVE2_BANK0 + diskmgr[1]->GetBank();
		}
	}

	// open
	list->AddButton("(Open)", MENU_DRIVE2_OPEN);
	list->AddButton("(Open 1 & 2)", MENU_DRIVE2_BOTH);

	// eject
	if (diskmgr[1]->IsOpen() == true) {
		list->AddButton("(Eject)", MENU_DRIVE2_EJECT);
	}

	// set focus
	if (id == MENU_BACK) {
		id = MENU_DRIVE2_OPEN;
	}
	list->SetFocus(id);
}

//
// EnterCmt()
// enter cmt menu
//
void Menu::EnterCmt(int id)
{
	list->SetTitle("<< CMT >>", MENU_CMT);

	// play and rec
	list->AddButton("Play", MENU_CMT_PLAY);
	list->AddButton("Rec", MENU_CMT_REC);

	// eject
	if ((tapemgr->IsPlay() == true) || (tapemgr->IsRec() == true)) {
		list->AddButton("Eject", MENU_CMT_EJECT);

		// eject -> focus
		if (id == MENU_BACK) {
			id = MENU_CMT_EJECT;
		}
	}

	// set focus
	if (id == MENU_BACK) {
		id = MENU_CMT_PLAY;
	}
	list->SetFocus(id);
}

//
// EnterLoad()
// enter load menu
//
void Menu::EnterLoad()
{
	int id;
	int last;
	int slot;
	cur_time_t ct;
	char textbuf[64];
	char timebuf[64];

	list->SetTitle("<< Load State >>", MENU_LOAD);

	// default focus
	id = MENU_LOAD_0;

	for (slot=0; slot<10; slot++) {
		if (slot == 0) {
			strcpy(textbuf, "Slot 0 (AUTO)");
		}
		else {
			sprintf(textbuf, "Slot %d       ", slot);
		}
		if (app->GetStateTime(slot, &ct) == true) {
			sprintf(timebuf, "%02d-%02d-%02d %02d:%02d",
				ct.year % 100,
				ct.month,
				ct.day,
				ct.hour,
				ct.minute);
			strcat(textbuf, timebuf);
		}

		list->AddButton(textbuf, MENU_LOAD_0 + slot);
	}

	// set focus
	last = setting->GetStateNum();
	if ((last >= 0) && (last <= 9)) {
		id = MENU_LOAD_0 + last;
	}
	list->SetFocus(id);
}

//
// EnterSave()
// enter save menu
//
void Menu::EnterSave()
{
	int id;
	int last;
	int slot;
	cur_time_t ct;
	char textbuf[64];
	char timebuf[64];

	list->SetTitle("<< Save State >>", MENU_SAVE);

	// default focus
	id = MENU_SAVE_0;

	for (slot=0; slot<10; slot++) {
		if (slot == 0) {
			strcpy(textbuf, "Slot 0 (AUTO)");
		}
		else {
			sprintf(textbuf, "Slot %d       ", slot);
		}
		if (app->GetStateTime(slot, &ct) == true) {
			sprintf(timebuf, "%02d-%02d-%02d %02d:%02d",
				ct.year % 100,
				ct.month,
				ct.day,
				ct.hour,
				ct.minute);
			strcat(textbuf, timebuf);
		}

		list->AddButton(textbuf, MENU_SAVE_0 + slot);
	}

	// set focus
	last = setting->GetStateNum();
	if ((last >= 0) && (last <= 9)) {
		id = MENU_SAVE_0 + last;
	}
	list->SetFocus(id);
}

//
// EnterSystem()
// enter system menu
//
void Menu::EnterSystem()
{
	int id;
	Uint8 ver[2];
	Font *font;
	bool add;

	list->SetTitle("<< System Options >>", MENU_SYSTEM);

	// default focus
	id = MENU_SYSTEM_V1S;

	list->AddRadioButton("V1S mode (w/reset)", MENU_SYSTEM_V1S, MENU_SYSTEM_MODE);
	list->AddRadioButton("V1H mode (w/reset)", MENU_SYSTEM_V1H, MENU_SYSTEM_MODE);
	list->AddRadioButton("V2  mode (w/reset)", MENU_SYSTEM_V2, MENU_SYSTEM_MODE);
	list->AddRadioButton("N   mode (w/reset)", MENU_SYSTEM_N, MENU_SYSTEM_MODE);
	list->AddRadioButton("Clock 4MHz  (w/reset)", MENU_SYSTEM_4M, MENU_SYSTEM_CLOCK);
	list->AddRadioButton("Clock 8MHz  (w/reset)", MENU_SYSTEM_8M, MENU_SYSTEM_CLOCK);
	list->AddRadioButton("Clock 8MHzH (FE2/MC,w/reset)", MENU_SYSTEM_8MH, MENU_SYSTEM_CLOCK);
	list->AddCheckButton("Memory wait (w/reset)", MENU_SYSTEM_WAIT);

	// rom version
	font = app->GetFont();
	ver[0] = font->GetROMVersion(1);
	ver[1] = font->GetROMVersion(2);
	add = false;

	if ((ver[0] < 0x32) && (add == false)) {
		list->AddButton("ROM: PC-8801", MENU_SYSTEM_ROMVER);
		add = true;
	}
	if ((ver[0] == 0x33) && (add == false)) {
		list->AddButton("ROM: PC-8801mkII", MENU_SYSTEM_ROMVER);
		add = true;
	}
	if ((ver[0] == 0x34) && (add == false)) {
		list->AddButton("ROM: PC-8801mkIISR/TR\n", MENU_SYSTEM_ROMVER);
		add = true;
	}
	if ((ver[0] < 0x38) && (add == false)) {
		list->AddButton("ROM: PC-8801mkIIFR/MR\n", MENU_SYSTEM_ROMVER);
		add = true;
	}
	if ((ver[0] == 0x38) && (add == false)) {
		list->AddButton("ROM: PC-8801FH/MH\n", MENU_SYSTEM_ROMVER);
		add = true;
	}
	if ((ver[1] < 0x31) && (add == false)) {
		list->AddButton("ROM: PC-8801FA/MA\n", MENU_SYSTEM_ROMVER);
		add = true;
	}
	if ((ver[1] == 0x31) && (add == false)) {
		list->AddButton("ROM: PC-8801FE/MA2\n", MENU_SYSTEM_ROMVER);
		add = true;
	}
	if ((ver[1] == 0x32) && (add == false)) {
		list->AddButton("ROM: PC-98DO\n", MENU_SYSTEM_ROMVER);
		add = true;
	}
	if ((ver[1] == 0x33) && (add == false)) {
		list->AddButton("ROM: PC-8801FE2/MC\n", MENU_SYSTEM_ROMVER);
		add = true;
	}
	if ((ver[1] > 0x33) && (add == false)) {
		list->AddButton("ROM: PC-98DO+\n", MENU_SYSTEM_ROMVER);
		add = true;
	}

	// mode
	switch (setting->GetSystemMode()) {
	case SETTING_V1S_MODE:
		list->SetRadio(MENU_SYSTEM_V1S, MENU_SYSTEM_MODE);
		id = MENU_SYSTEM_V1S;
		break;
	case SETTING_V1H_MODE:
		list->SetRadio(MENU_SYSTEM_V1H, MENU_SYSTEM_MODE);
		id = MENU_SYSTEM_V1H;
		break;
	case SETTING_V2_MODE:
		list->SetRadio(MENU_SYSTEM_V2, MENU_SYSTEM_MODE);
		id = MENU_SYSTEM_V2;
		break;
	case SETTING_N_MODE:
		list->SetRadio(MENU_SYSTEM_N, MENU_SYSTEM_MODE);
		id = MENU_SYSTEM_N;
		break;
	default:
		break;
	}

	// clock
	switch (setting->GetCPUClock()) {
	case 4:
		list->SetRadio(MENU_SYSTEM_4M, MENU_SYSTEM_CLOCK);
		break;
	case 8:
		if (setting->Is8HMode() == true) {
			list->SetRadio(MENU_SYSTEM_8MH, MENU_SYSTEM_CLOCK);
		}
		else {
			list->SetRadio(MENU_SYSTEM_8M, MENU_SYSTEM_CLOCK);
		}
		break;
	default:
		break;
	}

	// wait
	list->SetCheck(MENU_SYSTEM_WAIT, setting->HasMemoryWait());

	// set focus
	list->SetFocus(id);
}

//
// EnterVideo()
// enter video menu
//
void Menu::EnterVideo()
{
	int id;
	const char *quality;

	list->SetTitle("<< Video Options >>", MENU_VIDEO);

	// default focus
	id = MENU_VIDEO_SKIP0;

#ifndef __ANDROID__
	// window size
	list->AddRadioButton("Window x1.0 (640x400)", MENU_VIDEO_640, MENU_VIDEO_WINDOW);
	list->AddRadioButton("Window x1.5 (960x600)", MENU_VIDEO_960, MENU_VIDEO_WINDOW);
	list->AddRadioButton("Window x2.0 (1280x800)", MENU_VIDEO_1280, MENU_VIDEO_WINDOW);
	list->AddRadioButton("Window x2.5 (1600x1000)", MENU_VIDEO_1600, MENU_VIDEO_WINDOW);
	list->AddRadioButton("Window x3.0 (1920x1200)", MENU_VIDEO_1920, MENU_VIDEO_WINDOW);
#endif // !__ANDROID__

	// skip frame
	list->AddRadioButton("No frame skip", MENU_VIDEO_SKIP0, MENU_VIDEO_SKIP);
	list->AddRadioButton("1  frame skip", MENU_VIDEO_SKIP1, MENU_VIDEO_SKIP);
	list->AddRadioButton("2  frame skip", MENU_VIDEO_SKIP2, MENU_VIDEO_SKIP);
	list->AddRadioButton("3  frame skip", MENU_VIDEO_SKIP3, MENU_VIDEO_SKIP);

	// monitor type
	list->AddRadioButton("15kHz monitor (w/reset)", MENU_VIDEO_15K, MENU_VIDEO_MONITOR);
	list->AddRadioButton("24kHz monitor (w/reset)", MENU_VIDEO_24K, MENU_VIDEO_MONITOR);

	// else
	list->AddCheckButton("Scan line", MENU_VIDEO_SCANLINE);
	list->AddSlider("Brightness", MENU_VIDEO_BRIGHTNESS, 0x40, 0xff, 1);

	// status line
	list->AddCheckButton("Status area\n", MENU_VIDEO_STATUSCHK);
	list->AddSlider("Status transparency", MENU_VIDEO_STATUSALPHA, 0, 0xff, 1);

#ifdef _WIN32
	// scaling quality
	list->AddCheckButton("Scaling filter", MENU_VIDEO_SCALEFILTER);
#endif // _WIN32

	// skip frame
	switch (setting->GetSkipFrame()) {
	case 0:
		list->SetRadio(MENU_VIDEO_SKIP0, MENU_VIDEO_SKIP);
		id = MENU_VIDEO_SKIP0;
		break;
	case 1:
		list->SetRadio(MENU_VIDEO_SKIP1, MENU_VIDEO_SKIP);
		id = MENU_VIDEO_SKIP1;
		break;
	case 2:
		list->SetRadio(MENU_VIDEO_SKIP2, MENU_VIDEO_SKIP);
		id = MENU_VIDEO_SKIP2;
		break;
	case 3:
		list->SetRadio(MENU_VIDEO_SKIP3, MENU_VIDEO_SKIP);
		id = MENU_VIDEO_SKIP3;
		break;
	default:
		break;
	}

#ifndef __ANDROID__
	// window size
	switch (setting->GetWindowWidth()) {
	case 640:
		list->SetRadio(MENU_VIDEO_640, MENU_VIDEO_WINDOW);
		id = MENU_VIDEO_640;
		break;
	case 960:
		list->SetRadio(MENU_VIDEO_960, MENU_VIDEO_WINDOW);
		id = MENU_VIDEO_960;
		break;
	case 1280:
		list->SetRadio(MENU_VIDEO_1280, MENU_VIDEO_WINDOW);
		id = MENU_VIDEO_1280;
		break;
	case 1600:
		list->SetRadio(MENU_VIDEO_1600, MENU_VIDEO_WINDOW);
		id = MENU_VIDEO_1600;
		break;
	case 1920:
		list->SetRadio(MENU_VIDEO_1920, MENU_VIDEO_WINDOW);
		id = MENU_VIDEO_1920;
		break;
	default:
		break;
	}
#endif // !__ANDROID__

	// monitor type
	if (setting->IsLowReso() == true) {
		list->SetRadio(MENU_VIDEO_15K, MENU_VIDEO_MONITOR);
	}
	else {
		list->SetRadio(MENU_VIDEO_24K, MENU_VIDEO_MONITOR);
	}

	// scanline
	list->SetCheck(MENU_VIDEO_SCANLINE, setting->HasScanline());

	// brightness
	list->SetSlider(MENU_VIDEO_BRIGHTNESS, setting->GetBrightness());

	// status line
	list->SetCheck(MENU_VIDEO_STATUSCHK, setting->HasStatusLine());
	list->SetSlider(MENU_VIDEO_STATUSALPHA, setting->GetStatusAlpha());

#ifdef _WIN32
	// scaling quality
	quality = setting->GetScaleQuality();
	if (quality[0] == '0') {
		list->SetCheck(MENU_VIDEO_SCALEFILTER, false);
	}
	else {
		list->SetCheck(MENU_VIDEO_SCALEFILTER, true);
	}
#endif // _WIN32

	// set focus
	list->SetFocus(id);
}

//
// EnterAudio()
// enter audio menu
//
void Menu::EnterAudio()
{
	int id;

	list->SetTitle("<< Audio Options >>", MENU_AUDIO);

	// default focus
	id = MENU_AUDIO_44100;

	list->AddRadioButton("Freq. 44100Hz", MENU_AUDIO_44100, MENU_AUDIO_FREQ);
	list->AddRadioButton("Freq. 48000Hz", MENU_AUDIO_48000, MENU_AUDIO_FREQ);
#ifndef __ANDROID__
	list->AddRadioButton("Freq. 55467Hz", MENU_AUDIO_55467, MENU_AUDIO_FREQ);
	list->AddRadioButton("Freq. 88200Hz", MENU_AUDIO_88200, MENU_AUDIO_FREQ);
	list->AddRadioButton("Freq. 96000Hz", MENU_AUDIO_96000, MENU_AUDIO_FREQ);
#endif // !__ANDROID__
	list->AddSlider("Audio Buffer", MENU_AUDIO_BUFFER, 80, 500, 10);
	list->AddRadioButton("YM2203(OPN)  (w/reset)", MENU_AUDIO_OPN, MENU_AUDIO_SBII);
	list->AddRadioButton("YM2608(OPNA) (w/reset)", MENU_AUDIO_OPNA, MENU_AUDIO_SBII);

	// frequency
	switch (setting->GetAudioFreq()) {
	case 44100:
		list->SetRadio(MENU_AUDIO_44100, MENU_AUDIO_FREQ);
		id = MENU_AUDIO_44100;
		break;
	case 48000:
		list->SetRadio(MENU_AUDIO_48000, MENU_AUDIO_FREQ);
		id = MENU_AUDIO_48000;
		break;
#ifndef __ANDROID__
	case 55467:
		list->SetRadio(MENU_AUDIO_55467, MENU_AUDIO_FREQ);
		id = MENU_AUDIO_55467;
		break;
	case 88200:
		list->SetRadio(MENU_AUDIO_88200, MENU_AUDIO_FREQ);
		id = MENU_AUDIO_88200;
		break;
	case 96000:
		list->SetRadio(MENU_AUDIO_96000, MENU_AUDIO_FREQ);
		id = MENU_AUDIO_96000;
		break;
	default:
		break;
#endif // !__ANDROID__
	}

	// audio buffer
	list->SetSlider(MENU_AUDIO_BUFFER, setting->GetAudioBuffer());

	// sound board II
	if (setting->HasOPNA() == true) {
		list->SetRadio(MENU_AUDIO_OPNA, MENU_AUDIO_SBII);
	}
	else {
		list->SetRadio(MENU_AUDIO_OPN, MENU_AUDIO_SBII);
	}

	// set focus
	list->SetFocus(id);
}

//
// EnterInput()
// enter input menu
//
void Menu::EnterInput()
{
	list->SetTitle("<< Input Options >>", MENU_INPUT);

	list->AddButton("Softkey type 1", MENU_INPUT_SOFTKEY1);
	list->AddButton("Softkey type 2", MENU_INPUT_SOFTKEY2);
	list->AddButton("Softkey type 3", MENU_INPUT_SOFTKEY3);
	list->AddButton("Softkey type 4", MENU_INPUT_SOFTKEY4);
	list->AddSlider("Softkey transparency", MENU_INPUT_SOFTALPHA, 0, 0xff, 1);
	list->AddSlider("Softkey timeout", MENU_INPUT_SOFTTIME, 400, 20000, 200);
#ifdef __ANDROID__
	list->AddCheckButton("Keyboard enable", MENU_INPUT_KEYENABLE);
#endif // __ANDROID__
	list->AddCheckButton("Joystick enable", MENU_INPUT_JOYENABLE);
	list->AddCheckButton("Joystick to keyboard", MENU_INPUT_JOYKEY);
	list->AddCheckButton("Joystick button swap", MENU_INPUT_JOYSWAP);
#ifndef __ANDROID__
	list->AddSlider("Mouse timeout", MENU_INPUT_MOUSETIME, 400, 20000, 200);
#endif // !__ANDROID__

	// softkey
	list->SetSlider(MENU_INPUT_SOFTALPHA, setting->GetSoftKeyAlpha());
	list->SetSlider(MENU_INPUT_SOFTTIME, setting->GetSoftKeyTime());

#ifdef __ANDROID__
	// keyboard
	list->SetCheck(MENU_INPUT_KEYENABLE, setting->IsKeyEnable());
#endif // __ANDROID__

	// joystick
	list->SetCheck(MENU_INPUT_JOYENABLE, setting->IsJoyEnable());
	list->SetCheck(MENU_INPUT_JOYKEY, setting->IsJoyKey());
	list->SetCheck(MENU_INPUT_JOYSWAP, setting->IsJoySwap());

#ifndef __ANDROID__
	// mouse
	list->SetSlider(MENU_INPUT_MOUSETIME, setting->GetMouseTime());
#endif // !__ANDROID__

	// focus
	list->SetFocus(softkey_id);
}

//
// EnterReset()
// enter reset menu
//
void Menu::EnterReset()
{
	list->SetTitle("<< Reset >>", MENU_RESET);

	list->AddButton("Yes (Reset)", MENU_RESET_YES);
	list->AddButton("No", MENU_RESET_NO);
}

//
// EnterQuit()
// enter quit menu
//
void Menu::EnterQuit()
{
	list->SetTitle("<< Quit XM8 >>", MENU_QUIT);

	list->AddButton("Yes (Quit)", MENU_QUIT_YES);
	list->AddButton("No", MENU_QUIT_NO);
}

//
// EnterSoftKey()
// enter softkey menu
//
void Menu::EnterSoftKey()
{
	char title[64];
	int set;
	int id;

	// get set
	set = softkey_id - MENU_INPUT_SOFTKEY1;

	sprintf(title, "<< Softkey type %d >>", set + 1);
	list->SetTitle(title, MENU_SOFTKEY1 + set);

	list->AddButton("(LR)Full", MENU_SOFTKEY_0);
	list->AddButton("(L)Cursor   (R)Ten", MENU_SOFTKEY_1);
	list->AddButton("(L)Function (R)Ten", MENU_SOFTKEY_2);
	list->AddButton("(L)Action   (R)2468", MENU_SOFTKEY_3);
	list->AddButton("(L)2468     (R)Action", MENU_SOFTKEY_4);
	list->AddButton("(L)ZX       (R)2468", MENU_SOFTKEY_5);
	list->AddButton("(L)2468     (R)ZX", MENU_SOFTKEY_6);
	list->AddButton("(L)Ten      (R)Cursor", MENU_SOFTKEY_7);
	list->AddButton("(L)Ten      (R)Function", MENU_SOFTKEY_8);
	list->AddButton("(L)Ten", MENU_SOFTKEY_9);
	list->AddButton("            (R)Ten", MENU_SOFTKEY_10);
	list->AddButton("(None)", MENU_SOFTKEY_11);

	// set focus
	id = MENU_SOFTKEY_0 + setting->GetSoftKeySet(set);
	list->SetFocus(id);
}

//
// EnterFile()
// enter file menu
//
void Menu::EnterFile()
{
	const char *name;
	int id;
	Uint32 info;

	// title
	list->SetTitle(file_dir, MENU_FILE, true);

	// first
	name = platform->FindFirst(file_dir, &info);

	// loop
	id = MENU_FILE_MIN;
	while (name != NULL) {
		list->AddButton(name, id);
		list->SetUser(id, info);
		name = platform->FindNext(&info);
		id++;
	}
}

//
// Command()
// command
//
void Menu::Command(bool down, int id)
{
	// back
	if (id == MENU_BACK) {
		if (down == false) {
			CmdBack();
		}
		return;
	}

	// main menu
	if ((id >= MENU_MAIN_MIN) && (id <= MENU_MAIN_MAX)) {
		if (down == false) {
			CmdMain(id);
		}
		return;
	}

	// drive1 menu
	if ((id >= MENU_DRIVE1_MIN) && (id <= MENU_DRIVE1_MAX)) {
		if (down == false) {
			CmdDrive1(id);
		}
		return;
	}

	// drive2 menu
	if ((id >= MENU_DRIVE2_MIN) && (id <= MENU_DRIVE2_MAX)) {
		if (down == false) {
			CmdDrive2(id);
		}
		return;
	}

	// cmt menu
	if ((id >= MENU_CMT_MIN) && (id <= MENU_CMT_MAX)) {
		if (down == false) {
			CmdCmt(id);
		}
		return;
	}

	// load menu
	if ((id >= MENU_LOAD_MIN) && (id <= MENU_LOAD_MAX)) {
		if (down == false) {
			CmdLoad(id);
		}
		return;
	}

	// save menu
	if ((id >= MENU_SAVE_MIN) && (id <= MENU_SAVE_MAX)) {
		if (down == false) {
			CmdSave(id);
		}
		return;
	}

	// system menu
	if ((id >= MENU_SYSTEM_MIN) && (id <= MENU_SYSTEM_MAX)) {
		if (down == false) {
			CmdSystem(id);
		}
		return;
	}

	// video menu
	if ((id >= MENU_VIDEO_MIN) && (id <= MENU_VIDEO_MAX)) {
		CmdVideo(down, id);
		return;
	}

	// audio menu
	if ((id >= MENU_AUDIO_MIN) && (id <= MENU_AUDIO_MAX)) {
		CmdAudio(down, id);
		return;
	}

	// input menu
	if ((id >= MENU_INPUT_MIN) && (id <= MENU_INPUT_MAX)) {
		CmdInput(down, id);
		return;
	}

	// reset menu
	if ((id >= MENU_RESET_MIN) && (id <= MENU_RESET_MAX)) {
		if (down == false) {
			CmdReset(id);
		}
		return;
	}

	// quit menu
	if ((id >= MENU_QUIT_MIN) && (id <= MENU_QUIT_MAX)) {
		if (down == false) {
			CmdQuit(id);
		}
		return;
	}

	// softkey menu
	if ((id >= MENU_SOFTKEY_MIN) && (id <= MENU_SOFTKEY_MAX)) {
		if (down == false) {
			CmdSoftKey(id);
		}
		return;
	}

	// file menu
	if (id >= MENU_FILE_MIN) {
		if (down == false) {
			CmdFile(id);
		}
	}
}

//
// CmdBack()
// command (back)
//
void Menu::CmdBack()
{
	switch (list->GetID()) {
	// main menu
	case MENU_MAIN:
		app->LeaveMenu();
		break;

	// drive1 menu
	case MENU_DRIVE1:
		EnterMain(MENU_MAIN_DRIVE1);
		break;

	// drive2 menu
	case MENU_DRIVE2:
		EnterMain(MENU_MAIN_DRIVE2);
		break;

	// cmt menu
	case MENU_CMT:
		EnterMain(MENU_MAIN_CMT);
		break;

	// load menu
	case MENU_LOAD:
		EnterMain(MENU_MAIN_LOAD);
		break;

	// save menu
	case MENU_SAVE:
		EnterMain(MENU_MAIN_SAVE);
		break;

	// system menu
	case MENU_SYSTEM:
		EnterMain(MENU_MAIN_SYSTEM);
		break;

	// video menu
	case MENU_VIDEO:
		EnterMain(MENU_MAIN_VIDEO);
		break;

	// audio menu
	case MENU_AUDIO:
		EnterMain(MENU_MAIN_AUDIO);
		break;

	// input menu
	case MENU_INPUT:
		EnterMain(MENU_MAIN_INPUT);
		break;

	// reset menu
	case MENU_RESET:
		EnterMain(MENU_MAIN_RESET);
		break;

	// quit menu
	case MENU_QUIT:
		if (top_id == MENU_QUIT) {
			app->LeaveMenu();
		}
		else {
			EnterMain(MENU_MAIN_QUIT);
		}
		break;

	// softkey menu
	case MENU_SOFTKEY1:
	case MENU_SOFTKEY2:
	case MENU_SOFTKEY3:
	case MENU_SOFTKEY4:
		EnterInput();
		break;

	// file menu
	case MENU_FILE:
		switch (file_id) {
		// from drive1 menu
		case MENU_DRIVE1_OPEN:
			EnterDrive1(MENU_DRIVE1_OPEN);
			break;
		case MENU_DRIVE1_BOTH:
			EnterDrive1(MENU_DRIVE1_BOTH);
			break;

		// from drive2 menu
		case MENU_DRIVE2_OPEN:
			EnterDrive2(MENU_DRIVE2_OPEN);
			break;
		case MENU_DRIVE2_BOTH:
			EnterDrive2(MENU_DRIVE2_BOTH);
			break;

		// from cmt menu
		case MENU_CMT_PLAY:
			EnterCmt(MENU_CMT_PLAY);
			break;
		case MENU_CMT_REC:
			EnterCmt(MENU_CMT_REC);
			break;

		// default
		default:
			// fail safe
			EnterMain(MENU_MAIN_DRIVE1);
			break;
		}
		break;

	default:
		// fail safe
		EnterMain(MENU_MAIN_DRIVE1);
		break;
	}
}

//
// CmdMain()
// command (main)
//
void Menu::CmdMain(int id)
{
	switch (id) {
	// drive 1
	case MENU_MAIN_DRIVE1:
		EnterDrive1(MENU_BACK);
		break;

	// drive 2
	case MENU_MAIN_DRIVE2:
		EnterDrive2(MENU_BACK);
		break;

	// cmt
	case MENU_MAIN_CMT:
		EnterCmt(MENU_BACK);
		break;

	// load state
	case MENU_MAIN_LOAD:
		EnterLoad();
		break;

	// save state
	case MENU_MAIN_SAVE:
		EnterSave();
		break;

	// system options
	case MENU_MAIN_SYSTEM:
		EnterSystem();
		break;

	// video options
	case MENU_MAIN_VIDEO:
		EnterVideo();
		break;

	// audio options
	case MENU_MAIN_AUDIO:
		EnterAudio();
		break;

	// input options
	case MENU_MAIN_INPUT:
		EnterInput();
		break;

#ifndef __ANDROID__
	// screen
	case MENU_MAIN_SCREEN:
		if (app->IsFullScreen() == true) {
			app->WindowScreen();
		}
		else {
			app->FullScreen();
		}
		break;
#endif // !__ANDROID__

	// speed
	case MENU_MAIN_SPEED:
		if (app->IsFullSpeed() == true) {
			app->NormalSpeed();
		}
		else {
			app->FullSpeed();
		}
		break;

	// reset
	case MENU_MAIN_RESET:
		EnterReset();
		break;

	// quit
	case MENU_MAIN_QUIT:
		EnterQuit();
		break;

	default:
		break;
	}
}

//
// CmdDrive1()
// command (drive1)
//
void Menu::CmdDrive1(int id)
{
	switch (id) {
	// open
	case MENU_DRIVE1_OPEN:
		file_id = MENU_DRIVE1_OPEN;
		strcpy(file_dir, app->GetDiskDir(0));
		EnterFile();
		break;

	// open 1 & 2
	case MENU_DRIVE1_BOTH:
		file_id = MENU_DRIVE1_BOTH;
		strcpy(file_dir, app->GetDiskDir());
		EnterFile();
		break;

	// eject
	case MENU_DRIVE1_EJECT:
		if (diskmgr[0]->IsOpen() == true) {
			diskmgr[0]->Close();
			app->LeaveMenu();
		}
		break;

	default:
		id -= MENU_DRIVE1_BANK0;
		if (diskmgr[0]->GetBank() != id) {
			diskmgr[0]->SetBank(id);
			app->LeaveMenu();
		}
		break;
	}
}

//
// CmdDrive2()
// command (drive2)
//
void Menu::CmdDrive2(int id)
{
	switch (id) {
	// open
	case MENU_DRIVE2_OPEN:
		file_id = MENU_DRIVE2_OPEN;
		strcpy(file_dir, app->GetDiskDir(1));
		EnterFile();
		break;

	// open 1 & 2
	case MENU_DRIVE2_BOTH:
		file_id = MENU_DRIVE2_BOTH;
		strcpy(file_dir, app->GetDiskDir());
		EnterFile();
		break;

	// eject
	case MENU_DRIVE2_EJECT:
		if (diskmgr[1]->IsOpen() == true) {
			diskmgr[1]->Close();
			app->LeaveMenu();
		}
		break;

	default:
		id -= MENU_DRIVE2_BANK0;
		if (diskmgr[1]->GetBank() != id) {
			diskmgr[1]->SetBank(id);
			app->LeaveMenu();
		}
		break;
	}
}

//
// CmdCmt()
// command (cmt)
//
void Menu::CmdCmt(int id)
{
	switch (id) {
	// play
	case MENU_CMT_PLAY:
		file_id = MENU_CMT_PLAY;
		strcpy(file_dir, app->GetTapeDir());
		EnterFile();
		break;

	// rec
	case MENU_CMT_REC:
		file_id = MENU_CMT_REC;
		strcpy(file_dir, app->GetTapeDir());
		EnterFile();
		break;

	// eject
	case MENU_CMT_EJECT:
		if ((tapemgr->IsPlay() == true) || (tapemgr->IsRec() == true)) {
			tapemgr->Eject();
			app->LeaveMenu();
		}
		break;
	}
}

//
// CmdLoad()
// command (load)
//
void Menu::CmdLoad(int id)
{
	char textprev[64];
	char textbuf[64];

	// save last id
	id -= MENU_LOAD_0;
	setting->SetStateNum(id);

	// update text
	strcpy(textprev, list->GetText(MENU_LOAD_0 + id));
	sprintf(textbuf, "Slot %d Loading...\n", id);
	list->SetText(MENU_LOAD_0 + id, textbuf);

	// draw
	Draw();
	video->Draw();

	// load
	if (app->Load(id) == true) {
		// after load, save last id again
		setting->SetStateNum(id);
		app->LeaveMenu();
	}
	else {
		// load error
		list->SetText(MENU_LOAD_0 + id, textprev);
	}
}

//
// CmdSave()
// command (save)
//
void Menu::CmdSave(int id)
{
	char textprev[64];
	char textbuf[64];

	// save last id
	id -= MENU_SAVE_0;
	setting->SetStateNum(id);

	// update text
	strcpy(textprev, list->GetText(MENU_SAVE_0 + id));
	sprintf(textbuf, "Slot %d Saving...\n", id);
	list->SetText(MENU_SAVE_0 + id, textbuf);

	// draw
	Draw();
	video->Draw();

	// save
	if (app->Save(id) == true) {
		app->LeaveMenu();
	}
	else {
		// save error
		list->SetText(MENU_SAVE_0 + id, textprev);
	}
}

//
// CmdSystem()
// command (system)
//
void Menu::CmdSystem(int id)
{
	bool wait;

	switch (id) {
	// V1S
	case MENU_SYSTEM_V1S:
		list->SetRadio(MENU_SYSTEM_V1S, MENU_SYSTEM_MODE);
		setting->SetSystemMode(SETTING_V1S_MODE);
		break;

	// V1H
	case MENU_SYSTEM_V1H:
		list->SetRadio(MENU_SYSTEM_V1H, MENU_SYSTEM_MODE);
		setting->SetSystemMode(SETTING_V1H_MODE);
		break;

	// V2
	case MENU_SYSTEM_V2:
		list->SetRadio(MENU_SYSTEM_V2, MENU_SYSTEM_MODE);
		setting->SetSystemMode(SETTING_V2_MODE);
		break;

	// N
	case MENU_SYSTEM_N:
		list->SetRadio(MENU_SYSTEM_N, MENU_SYSTEM_MODE);
		setting->SetSystemMode(SETTING_N_MODE);
		break;

	// 4MHz
	case MENU_SYSTEM_4M:
		list->SetRadio(MENU_SYSTEM_4M, MENU_SYSTEM_CLOCK);
		setting->SetCPUClock(4);
		setting->Set8HMode(false);
		break;

	// 8MHz
	case MENU_SYSTEM_8M:
		list->SetRadio(MENU_SYSTEM_8M, MENU_SYSTEM_CLOCK);
		setting->SetCPUClock(8);
		setting->Set8HMode(false);
		break;

	// 8MHzH
	case MENU_SYSTEM_8MH:
		list->SetRadio(MENU_SYSTEM_8MH, MENU_SYSTEM_CLOCK);
		setting->SetCPUClock(8);
		setting->Set8HMode(true);
		break;

	// Memory wait
	case MENU_SYSTEM_WAIT:
		wait = setting->HasMemoryWait();
		if (wait == true) {
			list->SetCheck(MENU_SYSTEM_WAIT, false);
			setting->SetMemoryWait(false);
		}
		else {
			list->SetCheck(MENU_SYSTEM_WAIT, true);
			setting->SetMemoryWait(true);
		}
		break;

	// ROM version
	case MENU_SYSTEM_ROMVER:
		break;

	default:
		break;
	}
}

//
// CmdVideo()
// command (video)
//
void Menu::CmdVideo(bool down, int id)
{
	bool lowreso;
	bool radio;
	bool scanline;
	bool status;
	const char *quality;
	int width;

	// initialize
	width = 0;

	switch (id) {
#ifndef __ANDROID__
	// window x1.0
	case MENU_VIDEO_640:
		if (down == false) {
			list->SetRadio(MENU_VIDEO_640, MENU_VIDEO_WINDOW);
			width = 640;
		}
		break;

	// window x1.5
	case MENU_VIDEO_960:
		if (down == false) {
			list->SetRadio(MENU_VIDEO_960, MENU_VIDEO_WINDOW);
			width = 960;
		}
		break;

	// window x2.0
	case MENU_VIDEO_1280:
		if (down == false) {
			list->SetRadio(MENU_VIDEO_1280, MENU_VIDEO_WINDOW);
			width = 1280;
		}
		break;

	// window x2.5
	case MENU_VIDEO_1600:
		if (down == false) {
			list->SetRadio(MENU_VIDEO_1600, MENU_VIDEO_WINDOW);
			width = 1600;
		}
		break;

	// window x3.0
	case MENU_VIDEO_1920:
		if (down == false) {
			list->SetRadio(MENU_VIDEO_1920, MENU_VIDEO_WINDOW);
			width = 1920;
		}
		break;
#endif // !__ANDROID__

	// 0 frame skip
	case MENU_VIDEO_SKIP0:
		if (down == false) {
			list->SetRadio(MENU_VIDEO_SKIP0, MENU_VIDEO_SKIP);
			setting->SetSkipFrame(0);
		}
		break;

	// 0 frame skip
	case MENU_VIDEO_SKIP1:
		if (down == false) {
			list->SetRadio(MENU_VIDEO_SKIP1, MENU_VIDEO_SKIP);
			setting->SetSkipFrame(1);
		}
		break;

	// 2 frame skip
	case MENU_VIDEO_SKIP2:
		if (down == false) {
			list->SetRadio(MENU_VIDEO_SKIP2, MENU_VIDEO_SKIP);
			setting->SetSkipFrame(2);
		}
		break;

	// 3 frame skip
	case MENU_VIDEO_SKIP3:
		if (down == false) {
			list->SetRadio(MENU_VIDEO_SKIP3, MENU_VIDEO_SKIP);
			setting->SetSkipFrame(3);
		}
		break;

	// 15kHz monitor
	case MENU_VIDEO_15K:
		if (down == false) {
			list->SetRadio(MENU_VIDEO_15K, MENU_VIDEO_MONITOR);
		}
		break;

	// 24kHz monitor
	case MENU_VIDEO_24K:
		if (down == false) {
			list->SetRadio(MENU_VIDEO_24K, MENU_VIDEO_MONITOR);
		}
		break;

	// scan line
	case MENU_VIDEO_SCANLINE:
		if (down == false) {
			scanline = setting->HasScanline();
			if (scanline == true) {
				setting->SetScanline(false);
				list->SetCheck(MENU_VIDEO_SCANLINE, false);
			}
			else {
				setting->SetScanline(true);
				list->SetCheck(MENU_VIDEO_SCANLINE, true);
			}
		}
		break;

	// brightness
	case MENU_VIDEO_BRIGHTNESS:
		setting->SetBrightness((Uint8)list->GetSlider(MENU_VIDEO_BRIGHTNESS));
		break;

	// status line
	case MENU_VIDEO_STATUSCHK:
		if (down == false) {
			status = setting->HasStatusLine();
			if (status == true) {
				setting->SetStatusLine(false);
				list->SetCheck(MENU_VIDEO_STATUSCHK, false);
			}
			else {
				setting->SetStatusLine(true);
				list->SetCheck(MENU_VIDEO_STATUSCHK, true);
			}

			// to resize window
			width = setting->GetWindowWidth();
		}
		break;

	// staus alpha
	case MENU_VIDEO_STATUSALPHA:
		setting->SetStatusAlpha((Uint8)list->GetSlider(MENU_VIDEO_STATUSALPHA));
		break;

#ifdef _WIN32
	// scaling quality
	case MENU_VIDEO_SCALEFILTER:
		if (down == false) {
			quality = setting->GetScaleQuality();
			if (quality[0] != '0') {
				setting->SetScaleQuality(0);
				list->SetCheck(MENU_VIDEO_SCALEFILTER, false);
				video->RebuildTexture(false);
			}
			else {
				setting->SetScaleQuality(2);
				list->SetCheck(MENU_VIDEO_SCALEFILTER, true);
				video->RebuildTexture(false);
			}
		}
		break;
#endif // _WIN32

	default:
		break;
	}

	// reset & leave menu
	if (down == false) {
		// window size
		if (width != 0) {
			setting->SetWindowWidth(width);
			app->SetWindowWidth();
		}

		// monitor type
		lowreso = setting->IsLowReso();
		radio = list->GetRadio(MENU_VIDEO_15K);
		if (lowreso != radio) {
			// not equal
			setting->SetLowReso(radio);
			app->Reset();
			app->LeaveMenu();
			return;
		}
	}
}

//
// CmdAudio()
// command (audio)
//
void Menu::CmdAudio(bool down, int id)
{
	int freq;
	int buf;
	bool opna;
	bool leave;

	// get current frequency
	freq = setting->GetAudioFreq();

	// get current buffer
	buf = setting->GetAudioBuffer();

	// leave flag
	leave = false;

	switch (id) {
	// 44100Hz
	case MENU_AUDIO_44100:
		if (down == false) {
			list->SetRadio(MENU_AUDIO_44100, MENU_AUDIO_FREQ);
			setting->SetAudioFreq(44100);
		}
		break;

	// 48000Hz
	case MENU_AUDIO_48000:
		if (down == false) {
			list->SetRadio(MENU_AUDIO_48000, MENU_AUDIO_FREQ);
			setting->SetAudioFreq(48000);
		}
		break;

#ifndef __ANDROID__
	// 55467Hz
	case MENU_AUDIO_55467:
		if (down == false) {
			list->SetRadio(MENU_AUDIO_55467, MENU_AUDIO_FREQ);
			setting->SetAudioFreq(55467);
		}
		break;

	// 88200Hz
	case MENU_AUDIO_88200:
		if (down == false) {
			list->SetRadio(MENU_AUDIO_88200, MENU_AUDIO_FREQ);
			setting->SetAudioFreq(88200);
		}
		break;

	// 96000Hz
	case MENU_AUDIO_96000:
		if (down == false) {
			list->SetRadio(MENU_AUDIO_96000, MENU_AUDIO_FREQ);
			setting->SetAudioFreq(96000);
		}
		break;
#endif // !__ANDROID__

	// Buffer
	case MENU_AUDIO_BUFFER:
		setting->SetAudioBuffer(list->GetSlider(MENU_AUDIO_BUFFER));
		break;

	// YM2203(OPN)
	case MENU_AUDIO_OPN:
		if (down == false) {
			list->SetRadio(MENU_AUDIO_OPN, MENU_AUDIO_SBII);

			opna =setting->HasOPNA();
			if (opna == true) {
				setting->SetOPNA(false);
				app->ChangeSystem();
				leave = true;
			}
		}
		break;

	// YM2608(OPNA);
	case MENU_AUDIO_OPNA:
		if (down == false) {
			list->SetRadio(MENU_AUDIO_OPNA, MENU_AUDIO_SBII);

			opna =setting->HasOPNA();
			if (opna == false) {
				setting->SetOPNA(true);
				app->ChangeSystem();
				leave = true;
			}
		}
		break;

	default:
		break;
	}

	// rebuild audio component if freq or buffer has been changed
	if ((setting->GetAudioFreq() != freq) || (setting->GetAudioBuffer() != buf)) {
		app->ChangeAudio();
	}

	// leave menu
	if (leave == true) {
		app->LeaveMenu();
	}
}

//
// CmdInput()
// command (input)
//
void Menu::CmdInput(bool down, int id)
{
	bool enable;
	bool swap;

	// softkey type
	if ((id >= MENU_INPUT_SOFTKEY1) && (id <= MENU_INPUT_SOFTKEY4)) {
		if (down == false) {
			softkey_id = id;
			EnterSoftKey();
		}
		return;
	}

	switch (id) {
	// softkey alpha level
	case MENU_INPUT_SOFTALPHA:
		setting->SetSoftKeyAlpha((Uint8)list->GetSlider(MENU_INPUT_SOFTALPHA));
		break;

	// softkey timeout
	case MENU_INPUT_SOFTTIME:
		setting->SetSoftKeyTime((Uint32)list->GetSlider(MENU_INPUT_SOFTTIME));
		break;

#ifdef __ANDROID__
	// keyboard enable
	case MENU_INPUT_KEYENABLE:
		if (down == false) {
			enable = list->GetCheck(MENU_INPUT_KEYENABLE);
			if (enable == true) {
				list->SetCheck(MENU_INPUT_KEYENABLE, false);
				setting->SetKeyEnable(false);
			}
			else {
				list->SetCheck(MENU_INPUT_KEYENABLE, true);
				setting->SetKeyEnable(true);
			}
		}
		break;
#endif // __ANDROID__

	// joystick enable
	case MENU_INPUT_JOYENABLE:
		if (down == false) {
			enable = list->GetCheck(MENU_INPUT_JOYENABLE);
			if (enable == true) {
				list->SetCheck(MENU_INPUT_JOYENABLE, false);
				setting->SetJoyEnable(false);
			}
			else {
				list->SetCheck(MENU_INPUT_JOYENABLE, true);
				setting->SetJoyEnable(true);
			}
		}
		break;

	// joystick to keyboard
	case MENU_INPUT_JOYKEY:
		if (down == false) {
			enable = list->GetCheck(MENU_INPUT_JOYKEY);
			if (enable == true) {
				list->SetCheck(MENU_INPUT_JOYKEY, false);
				setting->SetJoyKey(false);
			}
			else {
				list->SetCheck(MENU_INPUT_JOYKEY, true);
				setting->SetJoyKey(true);
			}
		}
		break;

	// joystick button swap
	case MENU_INPUT_JOYSWAP:
		if (down == false) {
			swap = list->GetCheck(MENU_INPUT_JOYSWAP);
			if (swap == true) {
				list->SetCheck(MENU_INPUT_JOYSWAP, false);
				setting->SetJoySwap(false);
			}
			else {
				list->SetCheck(MENU_INPUT_JOYSWAP, true);
				setting->SetJoySwap(true);
			}
		}
		break;

#ifndef __ANDROID__
	// mouse timeout
	case MENU_INPUT_MOUSETIME:
		setting->SetMouseTime((Uint32)list->GetSlider(MENU_INPUT_MOUSETIME));
		break;
#endif // !__ANDROID__

	default:
		break;
	}
}

//
// CmdReset()
// command (reset)
//
void Menu::CmdReset(int id)
{
	switch (id) {
	// yes
	case MENU_RESET_YES:
		app->Reset();
		app->LeaveMenu();
		break;

	// no
	case MENU_RESET_NO:
		EnterMain(MENU_MAIN_RESET);
		break;

	default:
		break;
	}
}

//
// CmdQuit()
// command (quit)
//
void Menu::CmdQuit(int id)
{
	switch (id) {
	// yes
	case MENU_QUIT_YES:
		app->Quit();
		app->LeaveMenu();
		break;

	// no
	case MENU_QUIT_NO:
		app->LeaveMenu();
		break;

	default:
		break;
	}
}

//
// CmdSoftKey()
// command (softkey)
//
void Menu::CmdSoftKey(int id)
{
	int set;
	Input *input;

	// get set
	set = softkey_id - MENU_INPUT_SOFTKEY1;

	// get type
	id -= MENU_SOFTKEY_0;

	// setting
	if (setting->SetSoftKeySet(set, id) == true) {
		// inform input of change sofkey set
		input = app->GetInput();
		input->RebuildList();
	}

	// enter input menu
	EnterInput();
}

//
// CmdFile()
// command (file)
//
void Menu::CmdFile(int id)
{
	const char *name;
	bool ret;

	// get file or directory name
	name = list->GetName(id);
	if (name == NULL) {
		return;
	}

	// copy dir to target
	strcpy(file_target, file_dir);

	// directory ?
	if (platform->IsDir(list->GetUser(id)) == true) {
		if (platform->MakePath(file_target, name) == true) {
			strcpy(file_dir, file_target);
			EnterFile();
		}
		return;
	}

	// normal file
	platform->MakePath(file_target, name);

	// tape ?
	if ((file_id == MENU_CMT_PLAY) || (file_id == MENU_CMT_REC)) {
		// eject tape
		tapemgr->Eject();

		// play or rec
		if (file_id == MENU_CMT_PLAY) {
			ret = tapemgr->Play(file_target);
		}
		else {
			ret = tapemgr->Rec(file_target);
		}

		if (ret == true) {
			app->LeaveMenu();
		}
		return;
	}

	// disk
	switch (file_id) {
	case MENU_DRIVE1_OPEN:
	case MENU_DRIVE1_BOTH:
	case MENU_DRIVE2_BOTH:
		// drive 1
		diskmgr[0]->Close();
		ret = diskmgr[0]->Open(file_target, 0);

		// drive 2
		if (file_id != MENU_DRIVE1_OPEN) {
			diskmgr[1]->Close();
			if (ret == true) {
				if (diskmgr[0]->GetBanks() > 1) {
					diskmgr[1]->Open(file_target, 1);
				}
			}
		}

		// leave menu
		if (ret == true) {
			app->LeaveMenu();
		}
		break;

	case MENU_DRIVE2_OPEN:
		// drive 2
		diskmgr[1]->Close();
		ret = diskmgr[1]->Open(file_target, 0);

		// leave menu
		if (ret == true) {
			app->LeaveMenu();
		}
		break;

	default:
		break;
	}
}

//
// Draw()
// draw menu screen
//
void Menu::Draw()
{
	list->Draw();
}

//
// OnKeyDown()
// key down
//
void Menu::OnKeyDown(SDL_Event *e)
{
	list->OnKeyDown(e);
}

//
// OnMouseMotion()
// mouse motion
//
void Menu::OnMouseMotion(SDL_Event *e)
{
	list->OnMouseMotion(e);
}

//
// OnMouseButtonDown()
// mouse button down
//
void Menu::OnMouseButtonDown(SDL_Event *e)
{
	list->OnMouseButtonDown(e);
}

//
// OnMouseButtonUp()
// mouse button up
//
void Menu::OnMouseButtonUp(SDL_Event *e)
{
	list->OnMouseButtonUp(e);
}

//
// OnMouseWheel()
// mouse wheel
//
void Menu::OnMouseWheel(SDL_Event *e)
{
	list->OnMouseWheel(e);
}

//
// OnJoystick()
// joystick
//
void Menu::OnJoystick()
{
	list->OnJoystick();
}

//
// OnFingerDown()
// finger down
//
void Menu::OnFingerDown(SDL_Event *e)
{
	list->OnFingerDown(e);
}

//
// OnFingerUp()
// finger up
//
void Menu::OnFingerUp(SDL_Event *e)
{
	list->OnFingerUp(e);
}

//
// OnFingerMotion()
// finger motion
//
void Menu::OnFingerMotion(SDL_Event *e)
{
	list->OnFingerMotion(e);
}

#endif // SDL
