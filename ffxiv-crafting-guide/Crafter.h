#pragma once
#include<iostream>
#include<iomanip>
#include<utility>
#include<string>
#include<map>
#include<list>
#include<functional>

using std::pair;
using std::make_pair;
using std::string;
using std::map;
using std::list;
using std::function;

#define START_WITH_MUSCLE_MEMORY 0
#define START_WITH_REFLECT 1
#define START_WITH_INNER_QUIET 2

struct skill {
	string name;
	int rp;
	int rq;
	int CP;
	int dur;
	int acquired_level;
	int IQ_stack_granted;
	int step;
	bool type_P;
	bool type_Q;

	skill() {};
	skill(const string name, int relative_progress, int relative_quality, int CP, int dur, int acquired_level, int IQ_stack_granted, int step = 1) :
		name(name), rp(relative_progress), rq(relative_quality), CP(CP), dur(dur), acquired_level(acquired_level), IQ_stack_granted(IQ_stack_granted), step(step) {
		type_P = (relative_progress > 0);
		type_Q = (relative_quality > 0);
	};
	//Progress and Quality are stored by percents

	int get_progress(double unit_progress, double buff) {
		return static_cast<int>(unit_progress * rp*(1 + buff) / 100); //after rounding
	}

	int get_quality(double unit_quality, int current_IQ, double buff) {
		int current_rq = rq;
		if (this->name == "Byregot's Blessing")
			current_rq += 20 * (current_IQ > 1 ? current_IQ - 1 : 0);
		return static_cast<int>(unit_quality * current_rq*(1 + buff) / 100); //after rounding
	}
};

struct Crafter;
struct status {
	string skill;
	int progress = 0, quality = 0, CP_consumed = 0, dur = 0, Dmax = 0;
	int MM = 0, Ven = 0;
	int Inn = 0, GS = 0, IQ = 0;
	int WN = 0, Man = 0;
	int forward = 0;
	bool backward_binding = false;

	status() {};
	status(int Dmax) :dur(Dmax), Dmax(Dmax) {};
	void step(const Crafter& c, const string& s);
};

struct Crafter {
	int language;
	int character_level; //Character properties
	double CP, craftsmanship, control;
	int recipe_level; //Recipe properties
	double  suggested_craftsmanship, suggested_control;
	int Pmax, Qmax, Dmax;
	int level_progress_factor, level_quality_factor;
	int start_with;
	double alpha_10 = 2.4, alpha_20 = 1.2; //1(Dur)=alpha(CP)
	list<status> p_list, q_list, res_list;

	Crafter(int language) :language(language) {
		read_paras();
		level_progress_factor = get_level_progress_factor();
		level_quality_factor = get_level_quality_factor();
	}

	int get_level_progress_factor();
	int get_level_quality_factor();
	double get_unit_progress() const;
	double get_unit_quality(int IQ) const;

	void pool_progress();
	void pool_quality();
	void combo_expand(list<status>& l);
	list<status> dur_arrange(list<status>& pre);
	void merge();

	void solve_me();

	int get_HQ_percent();
	void read_paras();
	void read_paras_en();
	void read_paras_cn();
	void print();
	void print_en();
	void print_cn();
};