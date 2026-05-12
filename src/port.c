#include "quakedef.h"

#ifdef USE_MILSKO
static MwWidget win = NULL;
static MwWidget opengl;

static int MilskoToQuakeKey(int key){
	if(key == MwKEY_ENTER) key = K_ENTER;
	if(key == MwKEY_ESCAPE) key = K_ESCAPE;
	if(key == MwKEY_BACKSPACE) key = K_BACKSPACE;
	if(key == MwKEY_LEFT) key = K_LEFTARROW;
	if(key == MwKEY_RIGHT) key = K_RIGHTARROW;
	if(key == MwKEY_UP) key = K_UPARROW;
	if(key == MwKEY_DOWN) key = K_DOWNARROW;
	if(key == MwKEY_LEFTSHIFT) key = K_SHIFT;
	if(key == MwKEY_RIGHTSHIFT) key = K_SHIFT;
	if(key == MwKEY_ALT) key = K_ALT;
	if(key == MwKEY_CONTROL) key = K_CTRL;

	if('A' <= key && key <= 'Z') return tolower(key);

	return key;
}

static int MilskoToQuakeMouse(int btn){
	int b = 0;
	if(btn == MwMOUSE_LEFT) b = 0;
	if(btn == MwMOUSE_MIDDLE) b = 2;
	if(btn == MwMOUSE_RIGHT) b = 1;
	if(btn == MwMOUSE_WHEELUP) return K_MWHEELUP;
	if(btn == MwMOUSE_WHEELDOWN) return K_MWHEELDOWN;

	return K_MOUSE1 + b;
}

static void MWAPI mousemove(MwWidget handle, void* user, void* call) {
	MwPoint* p = call;
	int vecX = p->x;
	int vecY = p->y;

	if (mouse_active) {
		mx += vecX * 2;
		my += vecY * 2;
	}
}

static void MWAPI key(MwWidget handle, void* user, void* call){
	int k = *(int*)call;
	k = MilskoToQuakeKey(k);
	if(k == -0xdead) return;
	if(k & MwKEY_FLAG) return;
	Key_Event(k, 1);
}

static void MWAPI keyrelease(MwWidget handle, void* user, void* call){
	int k = *(int*)call;
	k = MilskoToQuakeKey(k);
	if(k == -0xdead) return;
	if(k & MwKEY_FLAG) return;
	Key_Event(k, 0);
}

static void MWAPI mousedown(MwWidget handle, void* user, void* call){
	MwMouse* m = call;
	Key_Event(MilskoToQuakeMouse(m->button), 1);
}

static void MWAPI mouseup(MwWidget handle, void* user, void* call){
	MwMouse* m = call;
	Key_Event(MilskoToQuakeMouse(m->button), 0);
}

static void MWAPI focusin(MwWidget handle, void* user, void* call){
	if(mouse_active){
		MwGrabPointer(opengl, 0);
		MwGrabPointer(opengl, 1);
	}
}

static void MWAPI focusout(MwWidget handle, void* user, void* call){
	MwGrabPointer(opengl, 0);
}


void Port_Init(int width, int height){
	MwSizeHints sh;
	MwWidget label;

	MwLibraryInit();

	win = MwVaCreateWidget(MwWindowClass, "Quake", NULL, MwDEFAULT, MwDEFAULT, width, height + 24,
		MwNtitle, "Quake",
	NULL);
	label = MwVaCreateWidget(MwLabelClass, "Label", win, 0, height, width, 24,
		MwNtext, "Milsko Quake!",
	NULL);
	opengl = MwCreateWidget(MwOpenGLClass, "OpenGL", win, 0, 0, width, height);

	sh.min_width = sh.max_width = width;
	sh.min_height = sh.max_height = height + 24;
	MwVaApply(win,
		MwNsizeHints, &sh,
	NULL);

	MwOpenGLMakeCurrent(opengl);

	MwAddUserHandler(opengl, MwNmouseMoveHandler, mousemove, NULL);
	MwAddUserHandler(opengl, MwNkeyHandler, key, NULL);
	MwAddUserHandler(opengl, MwNkeyReleaseHandler, keyrelease, NULL);
	MwAddUserHandler(opengl, MwNmouseDownHandler, mousedown, NULL);
	MwAddUserHandler(opengl, MwNmouseUpHandler, mouseup, NULL);
	MwAddUserHandler(opengl, MwNfocusInHandler, focusin, NULL);
	MwAddUserHandler(opengl, MwNfocusOutHandler, focusout, NULL);

	if(fullscreen){
		/* todo */
	}
}

void Port_Step(void){
	if(!win) return;

	if(MwPending(win)) MwStep(win);
}

void Port_GrabPointer(int toggle){
	if(!win) return;

	if(toggle){
		MwHideCursor(opengl);
		MwGrabPointer(opengl, 1);
	}else{
		MwGrabPointer(opengl, 0);
	}
}

void Port_Shutdown(void){
	if (win) {
		MwDestroyWidget(win);
	}
	win = NULL;
}

void Port_SwapBuffer(void){
	MwOpenGLSwapBuffer(opengl);
}
#endif

#ifdef USE_SDL2
static SDL_Window* win;
static SDL_GLContext context;

void Port_Init(int width, int height){
	SDL_Init(SDL_INIT_VIDEO);

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	win = SDL_CreateWindow("Quake", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_OPENGL);
	context = SDL_GL_CreateContext(win);
}

void Port_Step(void){
	if(!win) return;

	SDL_Event ev;

	while(SDL_PollEvent(&ev)){
		if(ev.type == SDL_MOUSEMOTION){
			if (mouse_active) {
				mx += ev.motion.xrel * 2;
				my += ev.motion.yrel * 2;
			}
		}else if(ev.type == SDL_MOUSEBUTTONDOWN || ev.type == SDL_MOUSEBUTTONUP){
			int b = 0;
			int btn = ev.button.button;

			if(btn == SDL_BUTTON_LEFT) b = 0;
			if(btn == SDL_BUTTON_MIDDLE) b = 2;
			if(btn == SDL_BUTTON_RIGHT) b = 1;

			Key_Event(K_MOUSE1 + b, ev.type == SDL_MOUSEBUTTONDOWN);
		}else if(ev.type == SDL_MOUSEWHEEL){
			int k;

			if(ev.wheel.y > 0){
				k = K_MWHEELUP;
			}else if(ev.wheel.y < 0){
				k = K_MWHEELDOWN;
			}
			Key_Event(k, 1);
			Key_Event(k, 0);
		}else if(ev.type == SDL_KEYDOWN || ev.type == SDL_KEYUP){
			int key = ev.key.keysym.sym;

			if(key == SDLK_RETURN) key = K_ENTER;
			if(key == SDLK_ESCAPE) key = K_ESCAPE;
			if(key == SDLK_BACKSPACE) key = K_BACKSPACE;
			if(key == SDLK_LEFT) key = K_LEFTARROW;
			if(key == SDLK_RIGHT) key = K_RIGHTARROW;
			if(key == SDLK_UP) key = K_UPARROW;
			if(key == SDLK_DOWN) key = K_DOWNARROW;
			if(key == SDLK_LSHIFT) key = K_SHIFT;
			if(key == SDLK_RSHIFT) key = K_SHIFT;
			if(key == SDLK_LALT) key = K_ALT;
			if(key == SDLK_RALT) key = K_ALT;
			if(key == SDLK_LCTRL) key = K_CTRL;
			if(key == SDLK_RCTRL) key = K_CTRL;
			if(key == SDLK_LGUI) key = K_SUPER;
			if(key == SDLK_RGUI) key = K_SUPER;
			if(SDLK_a <= key && key <= SDLK_z) key = key - SDLK_a + 'a';
			if(SDLK_0 <= key && key <= SDLK_9) key = key - SDLK_0 + '0';

			Key_Event(key, ev.type == SDL_KEYDOWN);
		}
	}
}

void Port_GrabPointer(int toggle){
	if(toggle){
		SDL_ShowCursor(0);
	}
	SDL_SetWindowMouseGrab(win, toggle);
	SDL_SetWindowKeyboardGrab(win, toggle);

	if(toggle){
		SDL_SetRelativeMouseMode(SDL_TRUE);
		SDL_SetHint(SDL_HINT_MOUSE_RELATIVE_MODE_WARP, "0");
	}
}


void Port_Shutdown(void){
	SDL_Quit();
}

void Port_SwapBuffer(void){
	SDL_GL_SwapWindow(win);
}
#endif
