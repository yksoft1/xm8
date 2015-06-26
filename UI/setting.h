//
// eXcellent Multi-platform emulator type 8 - 'XM8'
// based on ePC-8801MA
//
// Author (ePC-8801MA) : Takeda.Toshiya
// Author (XM8) : Tanaka.Yasushi
//
// [ setting ]
//

#ifdef SDL

#ifndef SETTING_H
#define SETTING_H

//
// defines
//
#define SETTING_V1S_MODE		0
										// BOOT MODE (V1S)
#define SETTING_V1H_MODE		1
										// BOOT MODE (V1H)
#define SETTING_V2_MODE			2
										// BOOT MODE (V2)
#define SETTING_N_MODE			3
										// BOOT MODE (N)

//
// platform dependent
//
class Setting
{
public:
	Setting();
										// constructor
	virtual ~Setting();
										// destructor
	bool Init();
										// initialize
	void Deinit();
										// deinitialize
	bool LoadSetting(FILEIO *fio);
										// load
	void SaveSetting(FILEIO *fio);
										// save
	const char* GetSettingDir();
										// get setting directory

	// system
	int GetSystemMode();
										// get system mode (0-3)
	void SetSystemMode(int mode);
										// set system mode (0-3)
	int GetCPUClock();
										// get cpu clock (4 or 8)
	void SetCPUClock(int clock);
										// set cpu clock (4 or 8)
	bool HasMemoryWait();
										// get memory wait
	void SetMemoryWait(bool wait);
										// set memory wait
	bool Is8HMode();
										// get 8MHzH mode
	void Set8HMode(bool high);
										// set 8MHzH mode
	Uint32 GetSystems();
										// get system information

	// video
	int GetWindowWidth();
										// get window width
	void SetWindowWidth(int width);
										// set window width
	int GetSkipFrame();
										// get skip frame
	void SetSkipFrame(int frame);
										// set skip frame
	Uint8 GetBrightness();
										// get brightness for vm
	void SetBrightness(Uint8 bri);
										// set brightness for vm
	Uint8 GetMenuAlpha();
										// get alpha blending level for menu
	bool IsLowReso();
										// get 15kHz monitor
	void SetLowReso(bool low);
										// set 15kHz monitor
	bool HasScanline();
										// get scan line
	void SetScanline(bool scanline);
										// set scan line

	// audio
	int GetAudioDevice();
										// get audio device
	int GetAudioFreq();
										// get audio frequency
	void SetAudioFreq(int freq);
										// set audio frequency
	int GetAudioPower();
										// get audio samples (for SDL_OpenAudioDevice)
	int GetAudioUnit();
										// get audio samples (for vm->create_sound)
	int GetAudioBuffer();
										// get audio buffer (ms)
	void SetAudioBuffer(int ms);
										// set audio buffer (ms)
	bool HasOPNA();
										// get sound board II
	void SetOPNA(bool opna);
										// set sound board II

	// input
	int GetSoftKeyType();
										// get softkey type
	bool NextSoftKey();
										// next softkey
	bool PrevSoftKey();
										// prev softkey
	int GetSoftKeySet(int set);
										// get softkey set
	bool SetSoftKeySet(int set, int type);
										// set softkey set
	Uint8 GetSoftKeyAlpha();
										// get alpha blending level for softkey
	void SetSoftKeyAlpha(Uint8 alpha);
										// set alpha blending level for softkey
	Uint32 GetSoftKeyTime();
										// get softkey timeout
	void SetSoftKeyTime(Uint32 ms);
										// set softkey timeout
	bool IsJoyEnable();
										// get joystick enable
	void SetJoyEnable(bool enable);
										// set joystick enable
	bool IsJoySwap();
										// get joystick button swap
	void SetJoySwap(bool swap);
										// set joystick button swap
	bool IsJoyKey();
										// get joystick to keyboard
	void SetJoyKey(bool enable);
										// set joystick to keyboard
	Uint32 GetMouseTime();
										// get mouse timeout
	void SetMouseTime(Uint32 ms);
										// set mouse timeout

	// state
	int GetStateNum();
										// get state number
	void SetStateNum(int num);
										// set state number

private:
	void Load();
										// load setting
	void Save();
										// save setting
	char setting_dir[_MAX_PATH * 3];
										// setting directory
	char setting_path[_MAX_PATH * 3];
										// setting path
	int window_width;
										// window width
	int skip_frame;
										// skip frame
	Uint8 brightness;
										// brightness
	int softkey_index;
										// softkey type
	int softkey_set[4];
										// softkey set
	Uint8 softkey_alpha;
										// softkey alpha
	Uint32 softkey_time;
										// softkey timeout
	bool joystick_enable;
										// joystick enable
	bool joystick_swap;
										// joystick button swap
	bool joystick_key;
										// joystick to keyboard
	Uint32 mouse_time;
										// mouse timeout
	int state_num;
										// state number
};

#endif // SETTING_H

#endif // SDL
