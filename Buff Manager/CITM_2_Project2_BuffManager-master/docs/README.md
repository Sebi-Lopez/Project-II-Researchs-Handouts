# The Buff Manager

“I am <https://www.linkedin.com/in/jaume-montagut-guix-7389a4166/>(Jaume Montagut), student of the
<https://www.citm.upc.edu/ing/estudis/graus-videojocs/>(Bachelor’s Degree in
Video Games by UPC at CITM). This content is generated for the second year’s
subject Project 2, under supervision of lecturer
<https://es.linkedin.com/in/ricardpillosu>(Ricard Pillosu).”

# The intention

This project aims to provide a flexible, robust and easy to manage solution for dealing with stats in a videogame.
We've prepared c++ code with a few exercices (TO DOs) and an explanation so that you can understand the implementation and adapt it to the needs of your own game.

# The download links

To start, download the project found here:

[https://github.com/JaumeMontagut/CITM_2_Project2_BuffManager]

![](https://lh3.googleusercontent.com/--Fqu5ifWEUs/WDChZLkU1iI/AAAAAAAABV4/2StCv-SrSXk-dzgv8k5xyu2m80fUcvt9wCJkCGAYYCw/w1082-h609-n-rw-no/large.gif)

# The importance

The first thing you may ask is: "Why do we need a system like this?".
The answer is simple, when we have multiple systems that modify stats (equipment, talents, passives, etc.) and hundereds of items in those systems, making them interact becomes complete chaos if we don't have an appropriate system for it.

![](https://github.com/JaumeMontagut/CITM_2_Project2_BuffManager/blob/master/docs/attributes.jpg)

# The problem

So now that we know why it is important to have a system like this we need to understand exactly what it does.
We need to create a system that given buffs from different sources, calculates a stat that we can later use to deal damage, calculate the speed at which we need to move, how many seconds can we be stunned, etc.

![](https://github.com/JaumeMontagut/CITM_2_Project2_BuffManager/blob/master/docs/buff_manager_problem.png)

# The concept

Our approach for this problem has been the following:

* Each character will have a series of stats.
* Each stat, such as attack, will have a base value and a list of all the buffs that have been applied to them.

On the other side we'll have a class named Buff Source which will be the parent of all the systems mentioned previously.
It will hold information about the buffs that can be applied with that source.

![](https://github.com/JaumeMontagut/CITM_2_Project2_BuffManager/blob/master/docs/Buff_manager_approach.png)

## Base Classes

Here we will show more in-depth the members and methods of the classes that create our logic.

### Character Class

```
class Character : public Entity {
public:
	int curr_health;
	int max_health;

private:
	std::map<std::string, Stat*> stats;
	std::map<std::string, Label*> stat_labels;

public:
	Character(pugi::xml_node character_node);
	bool Start() override;
	bool Update(float dt) override;

	void AddBuff(BuffSource * buff_source);
	void RemoveBuff(BuffSource * buff_source);
};
```

### Stat class

```
class Stat {
private:
  float base_value;
	float final_value;
  
	std::vector<Buff*> additive_buffs;
	std::vector<Buff*> multiplicative_buffs
  
public:
	Stat(float base);
  
	void AddBuff(Buff & buff);
	void RemoveBuff(uint source_id);
	void CalculateStat();
	float GetValue();
};
```

### Buff class

```
enum class BUFF_TYPE {
	ADDITIVE, //AKA: Flat, raw
	MULTIPLICATIVE, // AKA: Percent
	MAX
};

class Buff {
private:
	std::string stat = "\0";
  float value = 0.f;
	BUFF_TYPE type = BUFF_TYPE::MAX;
	uint source_id = 0u;

public:
	Buff(BUFF_TYPE type, std::string stat, float value, uint source_id);
	BUFF_TYPE GetType();
	std::string GetStat();
	float GetValue();
	uint GetSource();
	bool IsCausedBySource(uint source_id);
};
```

Each buff can have a type. In our case we've represented the most commmon types, which are flat (additive) and percentatge (multiplicative) buffs, but if your game needs it you can add more types and specify how they are calcualted inside Stat::CalculateStat().

### Buff source class

```
class BuffSource {
private:
	uint source_id;
	std::list<Buff*> buffs;

public:
	BuffSource(pugi::xml_node buff_source_node);
};
```

### Spell class

```
class Spell : public BuffSource {
public:
	bool is_active = false;
  
private:
  void(*function_ptr)(Spell *) = nullptr;

public:
	Spell(pugi::xml_node spell_node);
};
```

For this example we'll only be using a spell class. Since it's the most complex system that can alter the stats of a character, it will be easier to take it as a base for other system implementations.

## Adding buffs

![](https://github.com/JaumeMontagut/CITM_2_Project2_BuffManager/blob/master/docs/1.PNG)

To add buffs we follow this steps:

1. Buffs are defined with its source in the XML.

```
<spell name="rage">
  <function name="add_buff"/>
  <atlas_icon row ="0" column="4"/>
  <buff stat  ="attack" value ="3" type ="additive"/>
  <buff stat ="defense" value="3" type="additive"/>
</spell>
```

2. They are added on the source's buffs vector when it's created

```
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
```

3. Then they must be applied to the character, in our case, when we activate a spell.

```
void AddSpellBuff(Spell * spell) {
	if (!spell->is_active) {
		App->scene->dwarf->AddBuff(spell);
	}
	else {
		App->scene->dwarf->RemoveBuff(spell);
	}
	spell->is_active = !spell->is_active;
}
```

4. We look to which stat is applied and call its AddBuff() function

```
void Stat::AddBuff(Buff & buff)
{
	switch (buff.GetType())
	{
	case BUFF_TYPE::ADDITIVE:
		additive_buffs.push_back(&buff);
		break;
	case BUFF_TYPE::MULTIPLICATIVE:
		multiplicative_buffs.push_back(&buff);
		break;
	}
}
```

Instead of only having one list with all the buffs, we've decided to split it into two to be able to traverse throgh the buffs faster.
The `additive_buffs` list is used for flat buffs, meaning buffs like +3 damage and the `multiplicative_buffs` list is used for percentage buffs, meaning buffs like +30% damage.

Now the buff is stored in one of the stats so it can be used to calculate its value later!

## Removing buffs

![](https://github.com/JaumeMontagut/CITM_2_Project2_BuffManager/blob/master/docs/2.PNG)

To remove buffs we follow this steps:

1. We identiy which `BuffSource` caused the buff.
Each `BuffSource` has a unique identifier, `source_id` which we can use to detect which sources caused a certain buff.
In our case, we are using the same code as with add buff because when the player presses the spell button we get the spell which as a member has its `source_id`.

```
void AddSpellBuff(Spell * spell) {
	if (!spell->is_active) {
		App->scene->dwarf->AddBuff(spell);
	}
	else {
		App->scene->dwarf->RemoveBuff(spell);
	}
	spell->is_active = !spell->is_active;
}
```

2. We detect which stats the buffs alters

```
void Character::RemoveBuff(BuffSource * buff_source)
{
	for (std::list<Buff*>::iterator buff = buff_source->buffs.begin(); buff != buff_source->buffs.end(); ++buff)
	{
		std::string stat_name = (*buff)->GetStat();

		stats[stat_name]->RemoveBuff((*buff)->GetSource());
		stats[stat_name]->CalculateStat();
	}
}
```

3. And call `RemoveBuff(uint source_id)` on them

```
void Stat::RemoveBuff(uint source_id)
{
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
```

Now all the buffs have been removed from the respective stats lists!

## Calculating buffs

![](https://github.com/JaumeMontagut/CITM_2_Project2_BuffManager/blob/master/docs/3.PNG)

Finally, and the most important part is how the buffs are calculated.

```
void Stat::CalculateStat()
{
	final_value = base_value;
  
	for (std::vector<Buff*>::iterator iter = additive_buffs.begin(); iter != additive_buffs.end(); ++iter)
	{
		final_value += (*iter)->GetValue();
	}
  
	float totalMult = 0.f;
	for (std::vector<Buff*>::iterator iter = multiplicative_buffs.begin(); iter != multiplicative_buffs.end(); ++iter)
	{
		totalMult += (*iter)->GetValue();
	}
  
	final_value += totalMult * final_value;
}
```

We could have a single list and just apply buffs above each other, using the order in which they have been added to the list, but this would create a lot of confusion, frustration and difficulty calculating the buffs because changing the order would result in entirely different stat values.

For example, if we have a character with 10 base attack. And we first add "+50% attack" and then "+3 attack" we would end up with 18 attack.
```10 + 10 * 0.5 + 3 = 18```

If instead, we first add "+3 attack and then "+50% attack" we would end up with 19.6 attack!
```10 + 3 = 13 + 13 * 0.5 = 19.6```

That means we need to determine an order in which buffs are applied if we don't want to frustrate our player, in our case we've opted for first adding the flat buffs and then adding the percentage buffs on top of that.
This decision has big consequences on the design of the game, so we recommend spending time testing which types of bonuses your game will use and how are you going to apply them.

## Optimizations

One of the major optimizations you can make in a buff manager is to reduce the time it takes for stats to be calculated.
To achieve this we already have:
* Separated each buff in a vector depending on their type (flat or percent).
* Only recalculated stats each time they are changed, not each time they are needed. To achieve this we've created an additional value called final_value which holds the stat value will all the buffs applied, and is recalculated each time you add or remove a buff.

# The exercices

Get the slides [here](https://github.com/JaumeMontagut/CITM_2_Project2_BuffManager/blob/master/docs/BUFF%20MANAGER.pptx).

# The adaptation

To adapt this solution to your game, you would first need to think about what stats your game is going to use.

For the character, you may want to consider adding:
* Max health
* Max mana
* Movement speed or movement points
* Attack speed
* Critical chance
* Evasion
* Experience gain
* Gold gain

To add a character stat, you just need to add it under a character in the config.xml.

For your spells you may want to add.
* Mana cost
* Cooldown
* Range
* Duration

You may also want to create different classes for each of the sources of buffs you can have.
An example of an equipment class could be:

```
class Equipment : public BuffSource {
	int gold_cost;
	int durability;
};
```

Always make sure your new class inherits from BuffSource. Note that a class can have more than one inheritance, so you don't need to worry if it is already a child class.


# The resources

[https://forum.unity.com/threads/tutorial-character-stats-aka-attributes-system.504095/]

[https://www.youtube.com/watch?v=SH25f3cXBVc]

[https://www.youtube.com/watch?v=e8GmfoaOB4Y]

[http://howtomakeanrpg.com/a/how-to-make-an-rpg-stats.html]

[http://www.gamasutra.com/blogs/LarsDoucet/20111101/90518/A_Status_Effect_Stacking_Algorithm.php]

[https://gamedev.stackexchange.com/questions/29982/whats-a-way-to-implement-a-flexible-buff-debuff-system]

[https://www.youtube.com/watch?v=8Dg_QjDEs2Q&list=PLDlDppj_BAZDlDDefRh1jrxxqypvtuaCD&index=12]

[https://www.youtube.com/watch?v=8gOudI_g8jM&list=PLDlDppj_BAZDlDDefRh1jrxxqypvtuaCD&index=15]

[https://www.youtube.com/watch?v=li6ha2d8Arw&list=PLVbdRvy0bA6lt57nPF6r5EXhqquu69wKZ&index=1]

[https://www.gamedev.net/forums/topic/622135-time-related-state-effects-buffsdebuffs-what-are-manageable-code-designs/]

[https://gamedevelopment.tutsplus.com/tutorials/using-the-composite-design-pattern-for-an-rpg-attributes-system--gamedev-243]


# The contact

[<img width="32" height="32" src="https://cdn.iconscout.com/icon/free/png-256/github-84-436555.png">](https://github.com/JaumeMontagut) [<img width="32" height="32" src="https://cdn.icon-icons.com/icons2/122/PNG/128/twitter_socialnetwork_20007.png">](https://twitter.com/_wadoren)
[<img width="32" height="32" src="https://cdn3.iconfinder.com/data/icons/capsocial-round/500/linkedin-128.png">](https://www.linkedin.com/in/jaume-montagut-guix-7389a4166/)

# The spam ¯\\_(ツ)_/¯

We recently participated in the 3HMA contest!
Help us by voting [here](http://upcvideogames.com/juegos/), we are SWAP!
