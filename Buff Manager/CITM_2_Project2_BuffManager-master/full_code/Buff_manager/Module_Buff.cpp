#include <vector>
#include <algorithm>
#include "p2Defs.h"
#include "p2Log.h"
#include "Module_Buff.h"
#include <iostream> //To be able to use "unique_ptr"
#include "j1Render.h"
#include "j1App.h"
#include "PugiXml/src/pugiconfig.hpp"
#include "PugiXml/src/pugixml.hpp"
#include "j1Textures.h"
#include "SDL/include/SDL_rect.h"
#include "j1Window.h"
#include "j1Entity.h"
#include "j1Module.h"
#include "UI_Object.h"
#include "Module_UI.h"
#include "Button_Input.h"
#include "Label.h"
#include "j1Input.h"
#include "j1Scene.h"
#include "Entity_Character.h"

Module_Buff::Module_Buff() : j1Module()
{
	name.assign("buff");
}

bool Module_Buff::Awake(pugi::xml_node & buff_node)
{
	this->buff_node = buff_node;
	FillFunctionsMap();
	return true;
}

bool Module_Buff::Start() {
	Button * new_button = nullptr;
	int i = 0, j = 0, button_width = 16, scale_factor = 4;

	for (pugi::xml_node spell_node = buff_node.child("spell"); spell_node; spell_node = spell_node.next_sibling("spell"))
	{
		//Create the spell
		spells.push_back(new Spell(spell_node));

		//Create a button for the spell
		if (60 + i * (button_width * scale_factor + 10) + button_width * scale_factor > (10 + 120)*App->win->GetScale())
		{
			++j;
			i = 0;
		}
		new_button = App->ui->CreateButton(
			{ 60 + i * (button_width * scale_factor + 10), 605 + j * (button_width * scale_factor + 20) },
			{ button_width * spell_node.child("atlas_icon").attribute("column").as_int(), 16 * spell_node.child("atlas_icon").attribute("row").as_int(), 16, 16 },
			App->buff);
		new_button->SetLabel(
			{ new_button->position.x, new_button->position.y + new_button->section.h * new_button->scale_factor },
			spell_node.attribute("name").as_string(),
			App->ui->pixel_font_small);
		spell_buttons.push_back(new_button);
		++i;
	}
	return true;
}

uint Module_Buff::GetNewSourceID()
{
	return last_source_id++;
}

BUFF_TYPE Module_Buff::GetBuffType(std::string buff_type)
{
	if (std::strcmp(buff_type.c_str(), "additive") == 0) {
		return BUFF_TYPE::ADDITIVE;
	}
	else if (std::strcmp(buff_type.c_str(), "multiplicative") == 0) {
		return BUFF_TYPE::MULTIPLICATIVE;
	}
	else {
		LOG("Buff type not found.");
	}
}

void Module_Buff::FillFunctionsMap()
{
	spell_functions["cut"] = &Cut;
	spell_functions["add_buff"] = &AddSpellBuff;
	//TODO 6: Link the spell name with the function
	spell_functions["mental"] = &Mental;
}

void(*Module_Buff::GetFunctionPointer(std::string functionName))(Spell *) {
	return spell_functions[functionName];
}

bool Module_Buff::OnClick(UI_Object * object)
{
	for (int i = 0; i < spell_buttons.size(); ++i)
	{
		if (spell_buttons[i] == object)
		{
			spells[i]->function_ptr(spells[i]);
		}
	}
	return true;
}

void Module_Buff::AddOutPutText(std::string text)
{
	for (std::list<Label*>::reverse_iterator iter = output_Texts.rbegin(); iter != output_Texts.rend(); ++iter)
	{
		iPoint newPos = { (*iter)->position.x,(*iter)->position.y + 30 };
		(*iter)->SetPosition(newPos);
	}

	output_Texts.push_back(App->ui->CreateLabel({ 700,0 }, text, App->ui->pixel_font_small, nullptr, { 0,0,0,255 }));

	if (output_Texts.size() > 30)
	{
		App->ui->DeleteObject((UI_Object*)*output_Texts.begin());
		output_Texts.pop_front();
	}
}

void Cut(Spell * spell)
{
	if (App->scene->goblin->alive) {
		int damage = App->scene->dwarf->stats["attack"]->GetValue() - App->scene->goblin->stats["defense"]->GetValue();
		if (damage > 0)
		{
			App->scene->goblin->curr_health -= damage;
			App->buff->AddOutPutText(App->scene->dwarf->character_name + " dealt " + std::to_string(damage) + " damage to " + App->scene->goblin->character_name + ".");
			if (App->scene->goblin->curr_health <= 0 && App->scene->goblin->alive)
			{
				App->scene->goblin->alive = false;
				App->scene->goblin->curr_health = 0;
				App->buff->AddOutPutText(App->scene->goblin->character_name + " died.");
			}
		}
	}
	else {
		App->buff->AddOutPutText("it's already dead...");
	}
}

void AddSpellBuff(Spell * spell) {
	if (!spell->is_active) {
		App->scene->dwarf->AddBuff(spell);
	}
	else {
		App->scene->dwarf->RemoveBuff(spell);
	}
	spell->is_active = !spell->is_active;
}

//TODO 4: Create a function similar to Cut()
void Mental(Spell * spell)
{
	if (App->scene->goblin->alive) {
		int damage = App->scene->dwarf->stats["intelligence"]->GetValue();
		if (damage > 0)
		{
			App->scene->goblin->curr_health -= damage;
			App->buff->AddOutPutText(App->scene->dwarf->character_name + " dealt " + std::to_string(damage) + " damage to " + App->scene->goblin->character_name + ".");
			if (App->scene->goblin->curr_health <= 0 && App->scene->goblin->alive)
			{
				App->scene->goblin->alive = false;
				App->scene->goblin->curr_health = 0;
				App->buff->AddOutPutText(App->scene->goblin->character_name + " died.");
			}
		}
	}
	else {
		App->buff->AddOutPutText("it's already dead...");
	}
}
