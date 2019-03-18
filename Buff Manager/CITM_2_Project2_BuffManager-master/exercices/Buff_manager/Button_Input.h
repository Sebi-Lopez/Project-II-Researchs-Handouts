#ifndef _BUTTON_INPUT_H__
#define _BUTTON_INPUT_H__

#include "Module_UI.h"
#include "UI_Object.h"
#include "p2Point.h"
#include "Animation.h"


#define LABEL_PRESSED_OFFSET 4

class Label;

class Button : public UI_Object, public Gui_Listener
{

public:

	Button(const iPoint position, const SDL_Rect rect, SDL_Texture * texture , Gui_Listener * listener);

	virtual ~Button();

	bool Draw();

	bool SetLabel(const iPoint position, const String text, _TTF_Font* font = nullptr, const SDL_Color color = {255,255,255,255});

private:
	//bool Update(float dt);

private:

	SDL_Rect	rect;

public:
	// Components =================================
	Label*			label = nullptr;

private:
	friend Module_UI;
};

#endif // _BUTTON_INPUT_H__

