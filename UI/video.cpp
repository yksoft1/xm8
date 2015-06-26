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

#include "os.h"
#include "common.h"
#include "vm.h"
#include "app.h"
#include "emu_sdl.h"
#include "setting.h"
#include "font.h"
#include "diskmgr.h"
#include "video.h"

//
// defines
//
#define COLOR_BLACK			RGB_COLOR(0, 0, 0)
										// drive status (black)
#define COLOR_WHITE			RGB_COLOR(255, 255, 255)
										// drive status (white)
#define COLOR_NODISK		RGB_COLOR(31, 31, 31)
										// drive status (no disk)
#define COLOR_NOACCESS		RGB_COLOR(95, 95, 95)
										// drive status (no access)
#define COLOR_2D			RGB_COLOR(255, 48, 48)
										// drive status (2D access)
#define COLOR_2HD			RGB_COLOR(58, 239, 50)
										// drive status (2HD access)
#define MINIMUM_HEIGHT		2
										// minimum height
#define STATUS_HEIGHT		16
										// status area height
#define DRIVE_WIDTH			(28 * 8)
										// drive area width
#define FRAME_RATE_X		56
										// x position of frame rate (per ank)
#define FULL_SPEED_X		63
										// x position of full speed (per ank)
#define SYSTEM_INFO_X		71
										// x position of system info (per ank)
#define SOFTKEY_STEP		16
										// softkey mod step

//
// Video()
// constructor
//
Video::Video(App *a)
{
	int loop;

	// save parameter
	app = a;

	// object
	setting = NULL;
	font = NULL;
	diskmgr = NULL;
	window = NULL;
	renderer = NULL;
	draw_texture = NULL;
	menu_texture = NULL;
	softkey_texture = NULL;
	status_texture = NULL;
	frame_buf = NULL;
	menu_buf = NULL;
	softkey_buf = NULL;

	// parameter
	menu_mode = false;
	horizontal = false;
	window_width = 0;
	window_height = 0;
	brightness = 0xff;
	softkey_mode = false;
	softkey_mod = 0xff;
	video_height = (SCREEN_HEIGHT + MINIMUM_HEIGHT + STATUS_HEIGHT);
	status_alpha = 0;

	// rect
	SDL_zero(draw_rect);
	SDL_zero(status_rect);
	memset(clear_rect, 0, sizeof(clear_rect));

	// drive status
	for (loop=0; loop<MAX_DRIVE; loop++) {
		drive_status[loop].ready = false;
		drive_status[loop].readonly = false;
		drive_status[loop].access = ACCESS_MAX;
		drive_status[loop].name[0] = '\0';
	}

	// frame rate
	frame_rate[0] = 0;
	frame_rate[1] = 0x10000;

	// system information
	system_info[0] = 0;
	system_info[1] = 0xffff;

	// full speed
	full_speed[0] = false;
	full_speed[1] = true;

	// power down
	power_down = false;
}

//
// ~Video()
// destructor
//
Video::~Video()
{
	Deinit();
}

//
// Init()
// initialize
//
bool Video::Init(SDL_Window *win)
{
	// save window
	window = win;

	// get object
	setting = app->GetSetting();

	// frame buffer
	frame_buf = (uint32*)SDL_malloc(SCREEN_WIDTH * (SCREEN_HEIGHT + MINIMUM_HEIGHT + STATUS_HEIGHT) * sizeof(uint32));
	if (frame_buf == NULL) {
		Deinit();
		return false;
	}
	memset(frame_buf, 0, SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(uint32));

	// menu buffer
	menu_buf = (uint32*)SDL_malloc(SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(uint32));
	if (menu_buf == NULL) {
		Deinit();
		return false;
	}
	memset(menu_buf, 0, SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(uint32));

	// softkey buffer
	softkey_buf = (uint32*)SDL_malloc(SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(uint32));
	if (softkey_buf == NULL) {
		Deinit();
		return false;
	}
	memset(softkey_buf, 0, SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(uint32));

	// support RGB888 format only (see common.h)
#ifndef _RGB888
#error invalid rgb format
#endif // _RGB888

	// renderer
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if (renderer == NULL) {
		Deinit();
		return false;
	}

	// drawing texture
	draw_texture = SDL_CreateTexture(   renderer,
										SDL_PIXELFORMAT_RGB888,
										SDL_TEXTUREACCESS_STREAMING,
										SCREEN_WIDTH,
										SCREEN_HEIGHT);
	if (draw_texture == NULL) {
		Deinit();
		return false;
	}

	// menu texture
	menu_texture = SDL_CreateTexture(   renderer,
										SDL_PIXELFORMAT_ARGB8888,
										SDL_TEXTUREACCESS_STREAMING,
										SCREEN_WIDTH,
										SCREEN_HEIGHT);
	if (menu_texture == NULL) {
		Deinit();
		return false;
	}

	// softkey texture
	softkey_texture = SDL_CreateTexture(renderer,
										SDL_PIXELFORMAT_ARGB8888,
										SDL_TEXTUREACCESS_STREAMING,
										SCREEN_WIDTH,
										SCREEN_HEIGHT);
	if (softkey_texture == NULL) {
		Deinit();
		return false;
	}

	// status texture
	status_texture = SDL_CreateTexture( renderer,
										SDL_PIXELFORMAT_ARGB8888,
										SDL_TEXTUREACCESS_STREAMING,
										SCREEN_WIDTH,
										MINIMUM_HEIGHT + STATUS_HEIGHT);
	if (status_texture == NULL) {
		Deinit();
		return false;
	}

	// brightness
	brightness = setting->GetBrightness();
	SDL_SetTextureColorMod(draw_texture, brightness, brightness, brightness);

	// blend mode
	SDL_SetTextureBlendMode(draw_texture, SDL_BLENDMODE_NONE);
	SDL_SetTextureBlendMode(menu_texture, SDL_BLENDMODE_BLEND);
	SDL_SetTextureBlendMode(softkey_texture, SDL_BLENDMODE_BLEND);
	SDL_SetTextureBlendMode(status_texture, SDL_BLENDMODE_NONE);
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);

	return true;
}

//
// Deinit()
// deinitialize
//
void Video::Deinit()
{
	// status texture
	if (status_texture != NULL) {
		SDL_DestroyTexture(status_texture);
		status_texture = NULL;
	}

	// softkey texture
	if (softkey_texture != NULL) {
		SDL_DestroyTexture(softkey_texture);
		softkey_texture = NULL;
	}

	// menu texture
	if (menu_texture != NULL) {
		SDL_DestroyTexture(menu_texture);
		menu_texture = NULL;
	}

	// drawing texture
	if (draw_texture != NULL) {
		SDL_DestroyTexture(draw_texture);
		draw_texture = NULL;
	}

	// renderer
	if (renderer != NULL) {
		SDL_DestroyRenderer(renderer);
		renderer = NULL;
	}

	// sofkey buffer
	if (softkey_buf != NULL) {
		SDL_free(softkey_buf);
		softkey_buf = NULL;
	}

	// menu buffer
	if (menu_buf != NULL) {
		SDL_free(menu_buf);
		menu_buf = NULL;
	}

	// frame buffer
	if (frame_buf != NULL) {
		SDL_free(frame_buf);
		frame_buf = NULL;
	}
}

//
// SetWindowSize()
// setup draw_rect, status_rect
//
void Video::SetWindowSize(int width, int height)
{
	bool status;
	int v_height;

	// font and disk manager
	if (font == NULL) {
		font = app->GetFont();
	}
	if (diskmgr == NULL) {
		diskmgr = app->GetDiskManager();
	}

	// save parameter
	window_width = width;
	window_height = height;

	// update video_height
	status = setting->HasStatusLine();
	if (status == true) {
		v_height = SCREEN_HEIGHT + MINIMUM_HEIGHT + STATUS_HEIGHT;
	}
	else {
		v_height = SCREEN_HEIGHT;
	}
	if (v_height != video_height) {
		video_height = v_height;

		// rebuild texture (status only)
		RebuildTexture(true);
	}

	// check aspect
	if ((height * SCREEN_WIDTH) >= (width * video_height)) {
		horizontal = false;
	}
	else {
		horizontal = true;
	}

	if (horizontal == false) {
		// vertical (portrait)
		draw_rect.x = 0;
		draw_rect.y = 0;
		draw_rect.w = width;
		draw_rect.h = (width * video_height) / SCREEN_WIDTH;
		if (draw_rect.h == height) {
			// just window rect = draw rect
			clear_rect[0].w = 0;
			clear_rect[0].h = 0;
			clear_rect[1].w = 0;
			clear_rect[1].h = 0;
		}
		else {
			// centering
			draw_rect.y = (height / 2) - (draw_rect.h / 2);
			clear_rect[0].x = 0;
			clear_rect[0].y = 0;
			clear_rect[0].w = width;
			clear_rect[0].h = draw_rect.y;
			clear_rect[1].x = 0;
			clear_rect[1].y = draw_rect.y + draw_rect.h;
			clear_rect[1].w = width;
			clear_rect[1].h = height - clear_rect[1].y;
		}
	}
	else {
		// horizontal (landscape)
		draw_rect.x = 0;
		draw_rect.y = 0;
		draw_rect.w = (height * SCREEN_WIDTH) / video_height;
		draw_rect.h = height;
		if (draw_rect.w == width) {
			// just window rect = draw rect
			clear_rect[0].w = 0;
			clear_rect[0].h = 0;
			clear_rect[1].w = 0;
			clear_rect[1].h = 0;
		}
		else {
			// centering
			draw_rect.x = (width / 2) - (draw_rect.w / 2);
			clear_rect[0].x = 0;
			clear_rect[0].y = 0;
			clear_rect[0].w = draw_rect.x;
			clear_rect[0].h = height;
			clear_rect[1].x = draw_rect.x + draw_rect.w;
			clear_rect[1].y = 0;
			clear_rect[1].w = width - clear_rect[1].x;
			clear_rect[1].h = height;
		}
	}

	// status rect
	if (status == true) {
		// add status line
		status_rect.h = draw_rect.h;
		draw_rect.h = (draw_rect.w * SCREEN_HEIGHT) / SCREEN_WIDTH;
		status_rect.x = draw_rect.x;
		status_rect.w = draw_rect.w;
		status_rect.y = draw_rect.y + draw_rect.h;
		status_rect.h -= draw_rect.h;
	}
	else {
		// transparent status line
		status_rect.x = draw_rect.x;
		status_rect.y = (draw_rect.w * (SCREEN_HEIGHT - STATUS_HEIGHT)) / SCREEN_WIDTH;
		status_rect.w = draw_rect.w;
		status_rect.h = draw_rect.h - status_rect.y;
		status_rect.y += draw_rect.y;
	}
}

//
// RebuildTexture()
// rebulid texture
//
void Video::RebuildTexture(bool statusonly)
{
	SDL_Texture *texture;

	if (statusonly == false) {
		// set hint
		SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, setting->GetScaleQuality());

		// drawing texture
		texture = SDL_CreateTexture(renderer,
									SDL_PIXELFORMAT_RGB888,
									SDL_TEXTUREACCESS_STREAMING,
									SCREEN_WIDTH,
									SCREEN_HEIGHT);
		if (texture != NULL) {
			SDL_DestroyTexture(draw_texture);
			draw_texture = texture;
			SDL_SetTextureBlendMode(draw_texture, SDL_BLENDMODE_NONE);
		}

		// menu texture
		texture = SDL_CreateTexture(renderer,
									SDL_PIXELFORMAT_ARGB8888,
									SDL_TEXTUREACCESS_STREAMING,
									SCREEN_WIDTH,
									SCREEN_HEIGHT);
		if (texture != NULL) {
			SDL_DestroyTexture(menu_texture);
			menu_texture = texture;
			SDL_SetTextureBlendMode(menu_texture, SDL_BLENDMODE_BLEND);
		}

		// softkey texture
		texture = SDL_CreateTexture(renderer,
									SDL_PIXELFORMAT_ARGB8888,
									SDL_TEXTUREACCESS_STREAMING,
									SCREEN_WIDTH,
									SCREEN_HEIGHT);
		if (texture != NULL) {
			SDL_DestroyTexture(softkey_texture);
			softkey_texture = texture;
			SDL_SetTextureBlendMode(softkey_texture, SDL_BLENDMODE_BLEND);
			CopyFrameBuf(softkey_texture, softkey_buf, SCREEN_HEIGHT);
		}
	}

	if (setting->HasStatusLine() == true) {
		// rebuild status texture
		texture = SDL_CreateTexture(renderer,
									SDL_PIXELFORMAT_ARGB8888,
									SDL_TEXTUREACCESS_STREAMING,
									SCREEN_WIDTH,
									MINIMUM_HEIGHT + STATUS_HEIGHT);
	}
	else {
		texture = SDL_CreateTexture(renderer,
									SDL_PIXELFORMAT_ARGB8888,
									SDL_TEXTUREACCESS_STREAMING,
									SCREEN_WIDTH,
									STATUS_HEIGHT);
	}
	if (texture != NULL) {
		SDL_DestroyTexture(status_texture);
		status_texture = texture;

		// blend mode
		if (setting->HasStatusLine() == true) {
			SDL_SetTextureBlendMode(status_texture, SDL_BLENDMODE_NONE);
		}
		else {
			SDL_SetTextureBlendMode(status_texture, SDL_BLENDMODE_BLEND);
		}

		// reset status area
		ResetStatus();
	}
}

//
// SetFrameRate()
// set frame rate from app
//
void Video::SetFrameRate(Uint32 rate)
{
	frame_rate[0] = rate;
}

//
// SetSystemInfo()
// set system information from app
//
void Video::SetSystemInfo(Uint32 info)
{
	system_info[0] = info;
}

//
// SetFullSpeed()
// set running sppeed
//
void Video::SetFullSpeed(bool full)
{
	full_speed[0] = full;
}

//
// SetPowerDown()
// set power down
//
void Video::SetPowerDown(bool down)
{
	power_down = down;
}

//
// SetSoftKey()
// enable displaying softkey
//
void Video::SetSoftKey(bool visible, bool direct)
{
	if (softkey_mode != visible) {
		softkey_mode = visible;

		if ((visible == false) && (direct == true)) {
			softkey_mod = 0x00;
		}
		else {
			softkey_mod = 0xff;
		}

		// reset
		if (softkey_mode == true) {
			SDL_SetTextureAlphaMod(softkey_texture, softkey_mod);
		}
	}
}

//
// UpdateSoftKey()
// update softkey texture from frame buffer
//
void Video::UpdateSoftKey()
{
	CopyFrameBuf(softkey_texture, (Uint32*)softkey_buf, SCREEN_HEIGHT);
}

//
// ConvertPoint()
// convert point from window to texture
//
bool Video::ConvertPoint(int *x, int *y)
{
	int draw_x;
	int draw_y;

	// get
	draw_x = *x;
	draw_y = *y;

	// offset
	draw_x -= draw_rect.x;
	draw_y -= draw_rect.y;

	// convert
	draw_x = (draw_x * SCREEN_WIDTH) / draw_rect.w;
	draw_y = (draw_y * video_height) / draw_rect.h;

	// over check
	if ((draw_x < 0) || (draw_x >= SCREEN_WIDTH)) {
		*x = 0;
		*y = 0;
		return false;
	}

	if ((draw_y < 0) || (draw_y >= video_height)) {
		*x = 0;
		*y = 0;
		return false;
	}

	*x = draw_x;
	*y = draw_y;
	return true;
}

//
// ConvertFinger()
// convert finger point from window to texture
//
bool Video::ConvertFinger(float tx, float ty, int *x, int *y)
{
	// multiple with window rect
	tx *= (float)window_width;
	ty *= (float)window_height;

	// float to int
	*x = (int)tx;
	*y = (int)ty;

	// convert point
	return ConvertPoint(x, y);
}

//
// Draw()
// rendering
//
void Video::Draw()
{
	int ret;
	Uint8 bri;
	Uint8 step;
	Uint32 alpha;

	// brightness
	bri = setting->GetBrightness();
	if (brightness != bri) {
		brightness = bri;
		SDL_SetTextureColorMod(draw_texture, brightness, brightness, brightness);
	}

	// status line alpha level
	alpha = 0;
	if (setting->HasStatusLine() == false) {
		alpha = (Uint32)setting->GetStatusAlpha();
	}
	alpha <<= 24;
	if (status_alpha != alpha) {
		ResetStatus();
		status_alpha = alpha;
	}

	// status line
	DrawAccess();
	DrawFrameRate();
	DrawFullSpeed();
	DrawSystemInfo();

	// warning message
	DrawPowerDown();

	// buffer to texture
	CopyFrameBuf(draw_texture, (Uint32*)frame_buf, SCREEN_HEIGHT);
	if (setting->HasStatusLine() == true) {
		CopyFrameBuf(status_texture, (Uint32*)&frame_buf[SCREEN_WIDTH * SCREEN_HEIGHT], MINIMUM_HEIGHT + STATUS_HEIGHT);
	}
	else {
		CopyFrameBuf(status_texture, (Uint32*)&frame_buf[SCREEN_WIDTH * SCREEN_HEIGHT], STATUS_HEIGHT);
	}

	// menu
	if (menu_mode == true) {
		DrawMenu();
		return;
	}

	// clear if required
	if ((clear_rect[0].w != 0) || (clear_rect[0].h != 0)) {
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
		SDL_RenderClear(renderer);
	}

	// draw_texture & status_texture
	ret = SDL_RenderCopy(renderer, draw_texture, NULL, &draw_rect);
	if (ret == 0) {
		ret = SDL_RenderCopy(renderer, status_texture, NULL, &status_rect);
	}

	if (softkey_mode == true) {
		// softkey = enable
		if (ret == 0) {
			ret = SDL_RenderCopy(renderer, softkey_texture, NULL, &draw_rect);
		}
	}
	else {
		// sofkey = disable
		if ((ret == 0) && (softkey_mod > 0)) {
			// draw softkey with mod
			SDL_SetTextureAlphaMod(softkey_texture, softkey_mod);
			ret = SDL_RenderCopy(renderer, softkey_texture, NULL, &draw_rect);

			// get step
			step = setting->GetSoftKeyAlpha() / SOFTKEY_STEP;
			if (step == 0) {
				step++;
			}

			// step down
			if (softkey_mod < step) {
				softkey_mod = 0;
			}
			else {
				softkey_mod -= step;
			}
		}
	}

	// present
	if (ret == 0) {
		SDL_RenderPresent(renderer);
	}
}

//
// DrawAccess()
// draw access status
//
void Video::DrawAccess()
{
	int drive;
	bool equal;
	Uint32 *buf;
	SDL_Rect rect;
	Uint32 fore;

	// access bit
	diskmgr[0]->GetSignal(diskmgr[1]);

	// drive loop
	for (drive=0; drive<MAX_DRIVE; drive++) {
		// compare
		equal = false;
		if (diskmgr[drive]->IsOpen() == drive_status[drive].ready) {
			if (diskmgr[drive]->IsProtect() == drive_status[drive].readonly) {
				if (diskmgr[drive]->GetAccess() == drive_status[drive].access) {
					equal = true;
				}
			}
		}

		// diskmgr[drive]->GetName() returns NULL if drive is not ready
		if ((equal == true) && (drive_status[drive].ready == true)) {
			if (strcmp(diskmgr[drive]->GetName(), drive_status[drive].name) != 0) {
				strcpy(drive_status[drive].name, diskmgr[drive]->GetName());
				equal = false;
			}
		}

		// as same as previous state ?
		if (equal == true) {
			continue;
		}

		// backup
		drive_status[drive].ready = diskmgr[drive]->IsOpen();
		drive_status[drive].readonly = diskmgr[drive]->IsProtect();
		drive_status[drive].access = diskmgr[drive]->GetAccess();

		// frame buffer pointer
		buf = &frame_buf[SCREEN_WIDTH * SCREEN_HEIGHT];
		if (drive == 0) {
			buf += DRIVE_WIDTH;
		}

		// rectanbgle
		rect.x = 0;
		rect.y = 0;
		rect.w = DRIVE_WIDTH - 1;
		if (setting->HasStatusLine() == true) {
			rect.h = MINIMUM_HEIGHT + STATUS_HEIGHT;
		}
		else {
			rect.h = STATUS_HEIGHT;
		}

		// set fill rect color
		fore = COLOR_NODISK;
		if (drive_status[drive].ready == true) {
			switch (drive_status[drive].access) {
			// no access
			case ACCESS_NONE:
				fore = COLOR_NOACCESS;
				break;

			// 2D access
			case ACCESS_2D:
				fore = COLOR_2D;
				break;

			case ACCESS_2HD:
				fore = COLOR_2HD;
				break;

			default:
				fore = COLOR_NOACCESS;
				break;
			}
		}

		// fill rect (full or half)
		if ((drive_status[drive].ready == true) && (drive_status[drive].readonly == true)) {
			font->DrawHalfRect(buf, &rect, fore | status_alpha, COLOR_BLACK | status_alpha);
		}
		else {
			font->DrawFillRect(buf, &rect, fore | status_alpha);
		}

		// drive number
		font->DrawAnkQuarter(buf, (char)(drive + '1'), COLOR_WHITE | status_alpha, COLOR_BLACK | status_alpha);

		// disk name
		if (drive_status[drive].ready == true) {
			font->DrawSjisCenterOr(buf, &rect, drive_status[drive].name, COLOR_WHITE | status_alpha);
		}
	}
}

//
// DrawFrameRate()
// draw frame rate
//
void Video::DrawFrameRate()
{
	Uint32 *buf;
	char string[32];

	// compare and copy
	if (frame_rate[0] == frame_rate[1]) {
		return;
	}
	frame_rate[1] = frame_rate[0];

	// format
	if (frame_rate[0] > 999) {
		strcpy(string, "--.-fps");
	}
	else {
		sprintf(string, "%2d.%1dfps", frame_rate[0] / 10, frame_rate[0] % 10);
	}

	// draw
	buf = &frame_buf[SCREEN_WIDTH * SCREEN_HEIGHT];
	buf += (FRAME_RATE_X * 8);
	if (setting->HasStatusLine() == true) {
		buf += SCREEN_WIDTH;
	}
	font->DrawAnkHalf(buf, string, COLOR_WHITE | status_alpha, COLOR_BLACK | status_alpha);
}

//
// DrawFullSpeed()
// draw full speed
//
void Video::DrawFullSpeed()
{
	Uint32 *buf;

	// compare and copy
	if (full_speed[0] == full_speed[1]) {
		return;
	}
	full_speed[1] = full_speed[0];

	// draw
	buf = &frame_buf[SCREEN_WIDTH * SCREEN_HEIGHT];
	buf += (FULL_SPEED_X * 8);
	if (setting->HasStatusLine() == true) {
		buf += SCREEN_WIDTH;
	}

	if (full_speed[0] == true) {
		font->DrawAnkHalf(buf, " NOWAIT ", COLOR_WHITE | status_alpha, COLOR_BLACK | status_alpha);
	}
	else {
		font->DrawAnkHalf(buf, "        ", COLOR_WHITE | status_alpha, COLOR_BLACK | status_alpha);
	}
}

//
// DrawSystemInfo()
// draw system info
//
void Video::DrawSystemInfo()
{
	Uint32 info;
	Uint32 cpu;
	Uint32 *buf;
	char string[32];

	// equal ?
	if (system_info[0] == system_info[1]) {
		return;
	}

	// copy
	info = system_info[0];
	system_info[1] = info;

	// cpu clock
	cpu = (info >> 8) & 0xff;
	if (cpu != 0) {
		strcpy(&string[4], "4MHz ");
	}
	else {
		if (setting->Is8HMode() == true) {
			strcpy(&string[4], "8MHzH");
		}
		else {
			strcpy(&string[4], "8MHz ");
		}
	}

	// mode
	switch (info & 0xff) {
	case MODE_PC88_V1S:
		memcpy(string, "V1S-", 4);
		break;

	case MODE_PC88_V1H:
		memcpy(string, "V1H-", 4);
		break;

	case MODE_PC88_V2:
		memcpy(string, " V2-", 4);
		break;

	case MODE_PC88_N:
		memcpy(string, "  N-", 4);
		break;
	}

	// draw
	buf = &frame_buf[SCREEN_WIDTH * SCREEN_HEIGHT];
	buf += (SYSTEM_INFO_X * 8);
	if (setting->HasStatusLine() == true) {
		buf += SCREEN_WIDTH;
	}
	font->DrawAnkHalf(buf, string, COLOR_WHITE | status_alpha, COLOR_BLACK | status_alpha);
}

//
// ResetStatus()
// reset status information
//
void Video::ResetStatus()
{
	int drive;

	// drive
	for (drive=0; drive<MAX_DRIVE; drive++) {
		drive_status[drive].access = ACCESS_MAX;
	}

	// frame rate
	frame_rate[1] = 0x10000;

	// full speed
	if (app->IsFullSpeed() == true) {
		full_speed[1] = false;
	}
	else {
		full_speed[1] = true;
	}

	// system information
	system_info[1] = 0xffff;
}

//
// DrawPowerDown()
// draw power down
//
void Video::DrawPowerDown()
{
	SDL_Rect rect;

	if (power_down == false) {
		return;
	}

	rect.x = (SCREEN_WIDTH / 2) - (240 / 2);
	rect.y = (SCREEN_HEIGHT / 2) - (32 / 2);
	rect.w = 240;
	rect.h = 32;

	font->DrawRect(frame_buf, &rect, RGB_COLOR(255, 255, 255), RGB_COLOR(0, 0, 0));
	font->DrawSjisCenterOr(frame_buf, &rect, "Battery level is too low", RGB_COLOR(255, 255, 255));
}

//
// DrawMenu()
// draw menu
//
void Video::DrawMenu()
{
	int ret;

	// copy menu frame to texture
	CopyFrameBuf(menu_texture, menu_buf, SCREEN_HEIGHT);

	// clear if required
	if ((clear_rect[0].w != 0) || (clear_rect[0].h != 0)) {
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
		SDL_RenderClear(renderer);
	}

	// draw_texture & status_texture
	ret = SDL_RenderCopy(renderer, draw_texture, NULL, &draw_rect);
	if (ret == 0) {
		ret = SDL_RenderCopy(renderer, status_texture, NULL, &status_rect);
	}

	// draw texture
	if (ret == 0) {
		// menu texture
		ret = SDL_RenderCopy(renderer, menu_texture, NULL, &draw_rect);
		if (ret == 0) {
			SDL_RenderPresent(renderer);
		}
	}
}

//
// CopyFrameBuf()
// copy frame buffer to texture
//
void Video::CopyFrameBuf(SDL_Texture *texture, Uint32 *src, int height)
{
	Uint32 *dest;
	int ret;
	void *pixels;
	int pitch;
	int y;

	// lock entire texture
	ret = SDL_LockTexture(texture, NULL, &pixels, &pitch);
	if (ret == 0) {
		dest = (Uint32*)pixels;

		if (pitch == (SCREEN_WIDTH * sizeof(Uint32))) {
			// copy entire frame
			memcpy(dest, src, SCREEN_WIDTH * height * sizeof(uint32));
		}
		else {
			// copy per line
			pitch /= sizeof(Uint32);
			for (y=0; y<height; y++) {
				memcpy(dest, src, SCREEN_WIDTH * sizeof(uint32));
	
				src += SCREEN_WIDTH;
				dest += pitch;
			}
		}

		// unlock texture
		SDL_UnlockTexture(texture);
	}
}

//
// GetFrameBuf()
// get frame buffer pointer for ePC-8801MA
//
uint32* Video::GetFrameBuf(uint32 y)
{
	SDL_assert(y < SCREEN_HEIGHT);

	return &frame_buf[SCREEN_WIDTH * y];
}

//
// SetMenuMode()
// set menu mode
//
void Video::SetMenuMode(bool mode)
{
	menu_mode = mode;
}

//
// GetMenuFrame()
// get frame buffer pointer for menu
//
Uint32* Video::GetMenuFrame()
{
	return (Uint32*)menu_buf;
}

//
// GetSoftKeyFrame()
// get frame buffer pointer for softkey
//
Uint32* Video::GetSoftKeyFrame()
{
	return (Uint32*)softkey_buf;
}

#endif // SDL
