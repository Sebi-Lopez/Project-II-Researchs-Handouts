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
#include "Entity_Character.h"
#include "p2Defs.h"
#include "p2Log.h"
#include "Module_UI.h"
#include "Label.h"
#include "j1Fonts.h"

Buff::Buff(BUFF_TYPE type, std::string stat, float value, uint source_id) :
	type(type),
	stat(stat),
	value(value),
	source_id(source_id)
{
}

BUFF_TYPE Buff::GetType()
{
	return type;
}

std::string Buff::GetStat() {
	return stat;
}

float Buff::GetValue()
{
	return value;
}

uint Buff::GetSource()
{
	return source_id;
}

bool Buff::IsCausedBySource(uint source_id) {
	return this->source_id == source_id;
}

Stat::Stat(float base) :
	base_value(base),
	final_value(base)
{
}

void Stat::AddBuff(Buff & buff)
{
	switch (buff.GetType())
	{
	case BUFF_TYPE::ADDITIVE:
		App->buff->AddOutPutText("Added +" + std::to_string((int)buff.GetValue()) + " " + buff.GetStat());
		additive_buffs.push_back(&buff);
		break;
	case BUFF_TYPE::MULTIPLICATIVE:
		App->buff->AddOutPutText("Added +" + std::to_string((int)(buff.GetValue()*100)) + "% " + buff.GetStat());
		multiplicative_buffs.push_back(&buff);
		break;
	default:
		LOG("Buff type not detected.");
		break;
	}
}

//Searches through all the buffs and removes the ones caused by the source
void Stat::RemoveBuff(uint source_id)
{
	App->buff->AddOutPutText("Removed buff.");

	additive_buffs.erase(std::remove_if(
		additive_buffs.begin(),
		additive_buffs.end(),
		[source_id](Buff * buff) { return buff->IsCausedBySource(source_id); }),
		additive_buffs.end());

	multiplicative_buffs.erase(std::remove_if(
		multiplicative_buffs.begin(),
		multiplicative_buffs.end(),
		[source_id](Buff * buff) { return buff->IsCausedBySource(source_id); }),
		multiplicative_buffs.end());
}

void Stat::CalculateStat()
{
	final_value = base_value;
	//TODO 1: Calculate the final stat value as explained
	//   - First add all flat (additive) buffs
	//   - Then add all percent (multiplicative) buffs
	
	for (std::vector<Buff*>::iterator buff = additive_buffs.begin(); buff != additive_buffs.end(); ++buff)
	{
		final_value +=(*buff)->GetValue();
	}
	
	for (std::vector<Buff*>::iterator buff = multiplicative_buffs.begin(); buff != multiplicative_buffs.end(); ++buff)
	{
		final_value += final_value * (*buff)->GetValue();
	}
	
}

float Stat::GetValue()
{
	return final_value;
}

Character::Character(pugi::xml_node character_node) :
	Entity(character_node.attribute("x").as_int(), character_node.attribute("y").as_int())
{
	int i = 0, space = 25, scale_factor = 4, initial_height =-50;
	for (pugi::xml_node iter = character_node.child("stats").child("stat"); iter; iter = iter.next_sibling("stat"))
	{
		std::string stat_name = iter.attribute("stat").as_string();

		//Create the stat
		stats.insert(std::pair<std::string, Stat*>(
			stat_name,
			new Stat(iter.attribute("value").as_int())));

		//Create a label for the stat
		stat_labels.insert(std::pair<std::string, Label*>(
			stat_name,
			App->ui->CreateLabel(
				{ x * scale_factor, y - initial_height + space * i },
				stat_name + ": " + std::to_string((int)stats[stat_name]->GetValue()),
				App->ui->pixel_font,
				nullptr,
				{0,0,0})));
		++i;
	}
	character_name = character_node.child("name").attribute("value").as_string();
	max_health = curr_health = character_node.child("health").attribute("value").as_int();
	tex_path = character_node.child("spritesheet").attribute("path").as_string();
	frame = { character_node.child("animation").child("frame").attribute("x").as_int(),
				character_node.child("animation").child("frame").attribute("y").as_int(),
				character_node.child("animation").child("frame").attribute("w").as_int(),
				character_node.child("animation").child("frame").attribute("h").as_int() };
}

bool Character::Start()
{
	tex = App->tex->Load(tex_path.c_str());
	return false;
}

bool Character::Update(float dt)
{
	App->render->Blit(tex, x - frame.w * 0.5f, y - frame.h, &frame);
	//Health bar background
	App->render->DrawQuad({ x - 5, y - 22, 10, 2 }, 0, 0, 0);
	//Health bar fill
	App->render->DrawQuad({ x - 5, y - 22, (int)(10.f * ((float)curr_health / (float)max_health)), 2 }, 158, 41, 59);
	return true;
}

void Character::AddBuff(BuffSource * buff_source)
{
	for (std::list<Buff*>::iterator buff = buff_source->buffs.begin(); buff != buff_source->buffs.end(); ++buff)
	{
		std::string stat_name = (*buff)->GetStat();

		stats[stat_name]->AddBuff(*(*buff));
		stats[stat_name]->CalculateStat();
		stat_labels[stat_name]->SetText(stat_name + ": " + std::to_string((int)stats[stat_name]->GetValue()));
	}
}

void Character::RemoveBuff(BuffSource * buff_source)
{
	for (std::list<Buff*>::iterator buff = buff_source->buffs.begin(); buff != buff_source->buffs.end(); ++buff)
	{
		std::string stat_name = (*buff)->GetStat();

		stats[stat_name]->RemoveBuff((*buff)->GetSource());
		stats[stat_name]->CalculateStat();
		stat_labels[stat_name]->SetText(stat_name + ": " + std::to_string((int)stats[stat_name]->GetValue()));
	}
}

void Character::UpdateStatLabels()
{
	for (std::map<std::string, Label*>::iterator iter = stat_labels.begin(); iter != stat_labels.end(); ++iter)
	{
		(*iter).second->SetText((*iter).first + ": " + std::to_string(stats[(*iter).first]->GetValue()));
	}
}

BuffSource::BuffSource(pugi::xml_node source_node)
{
	source_id = App->buff->GetNewSourceID();
	for (pugi::xml_node iter = source_node.child("buff"); iter; iter = iter.next_sibling("buff"))
	{
		buffs.push_back(new Buff(
			App->buff->GetBuffType(iter.attribute("type").as_string()),
			iter.attribute("stat").as_string(),
			iter.attribute("value").as_float(),
			source_id));
	}
}

Spell::Spell(pugi::xml_node spell_node) : BuffSource(spell_node)
{
	function_ptr = App->buff->GetFunctionPointer(spell_node.child("function").attribute("name").as_string());
}
