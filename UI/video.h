//
// eXcellent Multi-platform emulator type 8 - 'XM8'
// based on ePC-8801MA
//
// Author (ePC-8801MA) : Takeda.Toshiya
// Author (XM8) : Tanaka.Yasushi
//
// [ video driver ]
//

#ifdef SDL

#ifndef VIDEO_H
#define VIDEO_H

//
// video driver
//
class Video
{
public:
	Video(App *a);
										// constructor
	virtual ~Video();
										// destructor
	bool Init(SDL_Window *win);
										// initialize
	void Deinit();
										// deinitialize
	void SetWindowSize(int width, int height);
										// set window sizse
	void Draw();
										// rendering

	// virtual machine
	uint32* GetFrameBuf(uint32 y);
										// get frame buffer for ePC-8801MA

	// menu
	void SetMenuMode(bool mode);
										// set menu mode
	Uint32* GetMenuFrame();
										// get frame buffer for menu

	// input
	Uint32* GetSoftKeyFrame();
										// get frame buffer for softkey
	void SetSoftKey(bool enable, bool direct = false);
										// enable displaying softkey
	void UpdateSoftKey();
										// update softkey texture from frame buffer
	bool ConvertPoint(int *x, int *y);
										// convert point from window to texturure
	bool ConvertFinger(float tx, float ty, int *x, int *y);
										// convert finger point from window to texture

	// main scheduler
	void SetFrameRate(Uint32 rate);
										// set frame rate(x10)
	void SetSystemInfo(Uint32 info);
										// set system info
	void SetFullSpeed(bool full);
										// set running speed
	void SetPowerDown(bool down);
										// set power down

private:
	// floppy drive information
	typedef struct _drive_info {
		bool ready;
										// media is ready
		bool readonly;
										// media is write protected
		int access;
										// access status
		char name[16 + 1];
										// media name
	} drive_info;

	void DrawAccess();
										// draw access status
	void DrawFrameRate();
										// draw frame rate
	void DrawFullSpeed();
										// draw full speed
	void DrawSystemInfo();
										// draw system info
	void DrawPowerDown();
										// draw power down
	void DrawMenu();
										// draw menu
	void CopyFrameBuf(SDL_Texture *texture, Uint32 *src);
										// copy frame buffer to texture

	// object
	App *app;
										// application
	Setting *setting;
										// setting
	Font *font;
										// font
	DiskManager **diskmgr;
										// disk manager array

	// frame buffer
	uint32 *frame_buf;
										// frame buffer for ePC-8801MA
	uint32 *menu_buf;
										// frame buffer for menu
	uint32 *softkey_buf;
										// frame buffer for softkey

	// flag and parameter
	bool horizontal;
										// screen direction
	bool menu_mode;
										// menu mode
	int window_width;
										// window width
	int window_height;
										// window height
	SDL_Rect draw_rect;
										// drawing rect
	SDL_Rect status_rect[2];
										// status rect
	Uint8 brightness;
										// brightness
	bool softkey_mode;
										// softkey mode
	Uint8 softkey_mod;
										// softkey mod

	// drive status
	drive_info drive_status[MAX_DRIVE];
										// drive status

	// frame rate
	Uint32 frame_rate[2];
										// frame rate (now & prev)

	// system info
	Uint32 system_info[2];
										// system information (now & prev)

	// full speed
	bool full_speed[2];
										// full speed (now & prev)

	// power down
	bool power_down;
										// power down

	// object (SDL)
	SDL_Window *window;
										// window
	SDL_Renderer *renderer;
										// renderer
	SDL_Texture *draw_texture;
										// drawing texture
	SDL_Texture *menu_texture;
										// menu texture
	SDL_Texture *softkey_texture;
										// softkey texture
};

#endif // VIDEO_H

#endif // SDL
