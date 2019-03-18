#ifndef __MODULE_BUFF_MANAGER_H__
#define __MODULE_BUFF_MANAGER_H__

#include "j1Module.h"
#include "PugiXml/src/pugiconfig.hpp"
#include "PugiXml/src/pugixml.hpp"
#include "Module_UI.h"
#include "Entity_Character.h"

class UI_Object;
class Button;

class Module_Buff : public j1Module, public Gui_Listener {
public:
	Module_Buff();
	bool Awake(pugi::xml_node & buf_node) override;
	bool Start() override;

	//Next turn

	void AddOutPutText(std::string);

	uint GetNewSourceID();
	BUFF_TYPE GetBuffType(std::string);

	//Function pointers
	void FillFunctionsMap();
	void(*(GetFunctionPointer)(std::string function_name))(Spell *);

	virtual bool OnClick(UI_Object* object);

private:
	uint last_source_id = 0u;
	pugi::xml_node buff_node;

	std::vector<Spell*> spells;
	std::vector<Button *> spell_buttons;
	std::list<Label*> output_Texts;

	std::map<std::string, void(*)(Spell*)> spell_functions;
};

//Spell functions
void Cut(Spell * spell);
void AddSpellBuff(Spell * spell);

#endif
