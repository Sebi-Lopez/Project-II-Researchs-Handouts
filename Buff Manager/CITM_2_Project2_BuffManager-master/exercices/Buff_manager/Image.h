#ifndef _IMAGE_H__
#define _IMAGE_H__

#include "p2Point.h"
#include "Animation.h"
#include "UI_Object.h"
#include "SDL/include/SDL_rect.h"

class Image: public UI_Object
{

public:
	Image(const iPoint position, const SDL_Rect draw_rect, SDL_Texture * texture, Gui_Listener* listener);
	bool Draw();

private:
	SDL_Rect rect;
};

#endif // _IMAGE_H__

