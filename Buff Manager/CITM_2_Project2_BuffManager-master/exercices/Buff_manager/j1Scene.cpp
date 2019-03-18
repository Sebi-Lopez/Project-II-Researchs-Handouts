#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Input.h"
#include "j1Textures.h"
#include "j1Audio.h"
#include "j1Render.h"
#include "j1Window.h"
#include "j1Scene.h"
#include "Module_Buff.h"
#include "j1Textures.h"
#include "Module_UI.h"
#include "Button_Input.h"
#include "j1Fonts.h"

j1Scene::j1Scene() : j1Module()
{
	name.assign("scene");
}

// Destructor
j1Scene::~j1Scene()
{}

// Called before render is available
bool j1Scene::Awake(pugi::xml_node& config)
{
	return true;
}

// Called before the first frame
bool j1Scene::Start()
{	
	return true;
}

// Called each loop iteration
bool j1Scene::PreUpdate()
{
	return true;
}

// Called each loop iteration
bool j1Scene::Update(float dt)
{
	//Draw background
	uint w, h;
	App->win->GetWindowSize(w, h);
	App->render->DrawQuad({ 0, 0, (int)w, (int)h }, 237, 190, 82);
	App->render->DrawQuad({ 10, 119, 120, 50 }, 74, 74, 74);
	return true;
}

// Called each loop iteration
bool j1Scene::PostUpdate()
{
	bool ret = true;

	if(App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;

	return ret;
}

// Called before quitting
bool j1Scene::CleanUp()
{
	LOG("Freeing scene");
	return true;
}
