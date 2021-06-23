#pragma once
#include<map>
#include<string>

#include"Crafter.h"

using std::map;
using std::string;

map<string, skill> skill_table = {
	{ "Empty",{ "Empty", 0, 0, 0, 0, 1, 0, 0 } },
{ "Muscle Memory",{ "Muscle Memory", 300, 0, 6, -10, 54, 0 } },
{ "Reflect",{ "Reflect", 0, 100, 24, -10, 69, 0 } },
{ "Observe",{ "Observe", 0, 0, 7, 0, 13, 0 } },
{ "Basic Synthesis",{ "Basic Synthesis", 120, 0, 0, -10, 1, 0 } },
{ "Careful Synthesis",{ "Careful Synthesis", 150, 0, 7, -10, 62, 0 } },
{ "Focused Synthesis",{ "Focused Synthesis", 200, 0, 5, -10, 67, 0 } },
{ "Focused Synthesis Combo",{ "Focused Synthesis Combo", 200, 0, 12, -10, 67, 0, 2 } },
{ "Groundwork",{ "Groundwork", 300, 0, 18, -20, 72, 0 } },
{ "Brand of the Elements",{ "Brand of the Elements", 100, 0, 6, -10, 37, 0 } },
{ "Basic Touch",{ "Basic Touch", 0, 100, 18, -10, 5, 1 } },
{ "Standard Touch",{ "Standard Touch", 0, 125, 32, -10, 18, 1 } },
{ "Standard Touch (Combo)",{ "Standard Touch (Combo)", 0, 125, 18, -10, 18, 1 } },
{ "Basic Standard Combo",{ "Basic Standard Combo", 0, 225, 36, -20, 18, 2, 2 } },
{ "Focused Touch",{ "Focused Touch", 0, 150, 18, -10, 68, 1 } },
{ "Focused Touch Combo",{ "Focused Touch Combo", 0, 150, 25, -10, 68, 1, 2 } },
{ "Prudent Touch",{ "Prudent Touch", 0, 100, 25, -5, 66, 1 } },
{ "Preparatory Touch",{ "Preparatory Touch", 0, 200, 40, -20, 71, 2 } },
{ "Byregot's Blessing",{ "Byregot's Blessing", 0, 100, 24, -10, 50, 0 } },
{ "Delicate Synthesis",{ "Delicate Synthesis", 100, 100, 32, -10, 76, 1 } },
{ "Master's Mend",{ "Master's Mend", 0, 0, 88, 30, 7, 0 } },
{ "Veneration",{ "Veneration", 0, 0, 18, 0, 15, 0 } },
{ "Name of the Elements",{ "Name of the Elements", 0, 0, 36, 0, 37, 0 } },
{ "Great Strides",{ "Great Strides", 0, 0, 32, 0, 21, 0 } },
{ "Innovation",{ "Innovation", 0, 0, 18, 0, 26, 0 } },
{ "Inner Quiet",{ "Inner Quiet", 0, 0, 18, 0, 11, 0 } },
{ "Waste Not",{ "Waste Not", 0, 0, 56, 0, 15, 0 } },
{ "Waste Not II",{ "Waste Not II", 0, 0, 98, 0, 47, 0 } },
{ "Manipulation",{ "Manipulation", 0, 0, 96, 0, 65, 0 } }
};

map<string, skill> p_table = {
	{ "Empty",{ "Empty", 0, 0, 0, 0, 1, 0, 0 } },
{ "Basic Synthesis",{ "Basic Synthesis", 120, 0, 0, -10, 1, 0 } },
{ "Careful Synthesis",{ "Careful Synthesis", 150, 0, 7, -10, 62, 0 } },
{ "Focused Synthesis Combo",{ "Focused Synthesis Combo", 200, 0, 12, -10, 67, 0, 2 } },
{ "Groundwork",{ "Groundwork", 300, 0, 18, -20, 72, 0 } }
};

map<string, skill> q_table = {
	{ "Empty",{ "Empty", 0, 0, 0, 0, 1, 0, 0 } },
{ "Basic Touch",{ "Basic Touch", 0, 100, 18, -10, 5, 1 } },
{ "Standard Touch",{ "Standard Touch", 0, 125, 32, -10, 18, 1 } },
{ "Basic Standard Combo",{ "Basic Standard Combo", 0, 225, 36, -20, 18, 2, 2 } },
{ "Focused Touch Combo",{ "Focused Touch Combo", 0, 150, 25, -10, 68, 1, 2 } },
{ "Preparatory Touch",{ "Preparatory Touch", 0, 200, 40, -20, 71, 2 } },
{ "Byregot's Blessing",{ "Byregot's Blessing", 0, 100, 24, -10, 50, 0 } }
};

int HQ_table[101]{
	1, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3,
	3, 3, 4, 4, 4, 4, 5, 5, 5, 5,
	6, 6, 6, 6, 7, 7, 7, 7, 8, 8,
	8, 9, 9, 9, 10, 10, 10, 11, 11, 11,
	12, 12, 12, 13, 13, 13, 14, 14, 14, 15,
	15, 15, 16, 16, 17, 17, 17, 18, 18, 18,
	19, 19, 20, 20, 21, 22, 23, 24, 26, 28,
	31, 34, 38, 42, 47, 52, 58, 64, 68, 71,
	74, 76, 78, 80, 81, 82, 83, 84, 85, 86,
	87, 88, 89, 90, 91, 92, 94, 96, 98, 100
};

int level_table[80] = {
	1, 2, 3, 4, 5, 6, 7, 8, 9, 10,  //1~10
	11, 12, 13, 14, 15, 16, 17, 18, 19, 20,  //11~20
	21, 22, 23, 24, 25, 26, 27, 28, 29, 30,  //21~30
	31, 32, 33, 34, 35, 36, 37, 38, 39, 40,  //31~40
	41, 42, 43, 44, 45, 46, 47, 48, 49, 50,  //41~50
	120, 125, 130, 133, 136, 139, 142, 145, 148, 150,  //51~60
	260, 265, 270, 273, 276, 279, 282, 285, 288, 290,  //51~70
	390, 395, 400, 403, 406, 409, 412, 415, 418, 420 //71~80
};

pair<int, int> level_factor_table[80] = {
	{ 80, 60 }, //-30
{ 82, 64 },
{ 84, 68 },
{ 86, 72 },
{ 88, 76 },
{ 90, 80 },
{ 92, 84 },
{ 94, 88 },
{ 96, 92 },
{ 98, 96 },
{ 100, 100 }, //-20
{ 100, 100 },
{ 100, 100 },
{ 100, 100 },
{ 100, 100 },
{ 100, 100 },
{ 100, 100 },
{ 100, 100 },
{ 100, 100 },
{ 100, 100 },
{ 100, 100 }, //-10
{ 100, 100 },
{ 100, 100 },
{ 100, 100 },
{ 100, 100 },
{ 100, 100 },
{ 100, 100 },
{ 100, 100 },
{ 100, 100 },
{ 100, 100 },
{ 100, 100 }, //0
{ 105, 100 },
{ 110, 100 },
{ 115, 100 },
{ 120, 100 },
{ 125, 100 },
{ 127, 100 },
{ 129, 100 },
{ 131, 100 },
{ 133, 100 },
{ 135, 100 }, //10
{ 137, 100 },
{ 139, 100 },
{ 141, 100 },
{ 143, 100 },
{ 145, 100 },
{ 147, 100 },
{ 147, 100 },
{ 148, 100 },
{ 149, 100 },
{ 150, 100 }, //20
{ 150, 100 },
{ 150, 100 },
{ 150, 100 },
{ 150, 100 },
{ 150, 100 },
{ 150, 100 },
{ 150, 100 },
{ 150, 100 },
{ 150, 100 },
{ 150, 100 }, //30
{ 150, 100 },
{ 150, 100 },
{ 150, 100 },
{ 150, 100 },
{ 150, 100 },
{ 150, 100 },
{ 150, 100 },
{ 150, 100 },
{ 150, 100 },
{ 150, 100 }, //40
{ 150, 100 },
{ 150, 100 },
{ 150, 100 },
{ 150, 100 },
{ 150, 100 },
{ 150, 100 },
{ 150, 100 },
{ 150, 100 },
{ 150, 100 } //49
};

map<string, string> skill_name_cn = {
	{ "Muscle Memory","坚信" },
{ "Reflect","娴静" },
{ "Observe","观察" },
{ "Basic Synthesis","制作" },
{ "Careful Synthesis","模范制作" },
{ "Focused Synthesis","注视制作" },
{ "Focused Synthesis Combo","注视制作连击" },
{ "Groundwork","坯料制作" },
{ "Brand of the Elements","元素之印记" },
{ "Basic Touch","加工" },
{ "Standard Touch","中级加工" },
{ "Standard Touch (Combo)","中级加工（连击）" },
{ "Basic Standard Combo","加工+中级加工连击" },
{ "Focused Touch","注视加工" },
{ "Focused Touch Combo","注视加工连击" },
{ "Prudent Touch","俭约加工" },
{ "Preparatory Touch","坯料加工" },
{ "Byregot's Blessing","比尔格的祝福" },
{ "Delicate Synthesis","精密制作" },
{ "Master's Mend","精修" },
{ "Veneration","崇敬" },
{ "Name of the Elements","元素之美名" },
{ "Great Strides","阔步" },
{ "Innovation","改革" },
{ "Inner Quiet","内静" },
{ "Waste Not","俭约" },
{ "Waste Not II","长期俭约" },
{ "Manipulation","掌握" }
};