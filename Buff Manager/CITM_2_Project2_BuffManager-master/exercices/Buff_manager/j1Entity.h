#ifndef __J1_ENTITY_H__
#define __J1_ENTITY_H__

#include "j1Module.h"
#include <vector>
#include "PugiXml/src/pugiconfig.hpp"
#include "PugiXml/src/pugixml.hpp"

class Entity {
public:
	Entity(int x, int y);
	virtual bool Start();
	virtual bool Update(float dt);

protected:
	int x, y;
};


class j1Entity : public j1Module {
public:
	j1Entity();
	bool Awake(pugi::xml_node & entity_node) override;
	bool Start() override;
	bool Update(float dt) override;

public:
	std::vector<Entity*> entities;
	pugi::xml_node entities_node;
};

#endif // !__J1_ENTITY_H__

