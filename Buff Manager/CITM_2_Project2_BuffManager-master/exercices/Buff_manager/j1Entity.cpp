#include "j1Entity.h"
#include <vector>
#include "PugiXml/src/pugiconfig.hpp"
#include "PugiXml/src/pugixml.hpp"
#include "p2Log.h"
#include "Module_Buff.h"
#include "j1App.h"
#include "j1Scene.h"

Entity::Entity(int x, int y) : x(x), y(y) {
}

bool Entity::Start()
{
	return true;
}

bool Entity::Update(float dt)
{
	return true;
}

j1Entity::j1Entity()
{
	name.assign("entity");
}

bool j1Entity::Awake(pugi::xml_node & node)
{
	entities_node = node;
	return true;
}

bool j1Entity::Start()
{
	Entity * new_entity = nullptr;
	for (pugi::xml_node entity_node = entities_node.child("entity"); entity_node != nullptr; entity_node = entity_node.next_sibling("entity"))
	{
		if (std::strcmp(entity_node.attribute("type").as_string(), "character") == 0)
		{
			new_entity = new Character(entity_node);
			if (std::strcmp(entity_node.child("name").attribute("value").as_string(), "dwarf") == 0)
			{
				App->scene->dwarf = (Character*)new_entity;
			}
			else if (std::strcmp(entity_node.child("name").attribute("value").as_string(), "goblin") == 0)
			{
				App->scene->goblin = (Character*)new_entity;
			}
			entities.push_back(new_entity);
		}
		else {
			LOG("Entity type not found.");
		}
	}

	for (std::vector<Entity*>::iterator iter = entities.begin(); iter != entities.end(); ++iter)
	{
		(*iter)->Start();
	}
	return true;
}

bool j1Entity::Update(float dt) {
	for (std::vector<Entity*>::iterator iter = entities.begin(); iter != entities.end(); ++iter)
	{
		(*iter)->Update(dt);
	}

	return true;
}
