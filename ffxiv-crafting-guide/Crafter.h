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

struct Crafter;

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

	int get_progress(const Crafter& c, bool has_MM, bool has_Ven);
	int get_quality(const Crafter& c, int current_IQ, bool has_Inn, bool has_GS);
};

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
	int tPmax, tQmax;
	int level_progress_factor, level_quality_factor;
	int start_with;
	double alpha_10 = 2.4, alpha_20 = 1.2; //1(Dur)=alpha(CP)
	list<status> p_list, q_list, res_list;

	struct p_dpnode {
		double cost = DBL_MAX;
		int P_dad = -1;
		status stat; //simplified rotation, status under which skill is executed
		void set(double cost, int P_dad, string skill, bool has_Ven) {
			this->cost = cost;
			this->P_dad = P_dad;
			this->stat.skill = skill;
			this->stat.Ven = has_Ven;
		}
	};
	p_dpnode* pdp;

	int min_IQ;
	struct q_dpnode {
		double cost = DBL_MAX;
		int k_dad = -1;
		int Q_dad = -1;
		status stat; //simplified rotation, status under which skill is executed
		void set(double cost, int k_dad, int Q_dad, string skill, bool has_Inn, bool has_GS) {
			this->cost = cost;
			this->k_dad = k_dad;
			this->Q_dad = Q_dad;
			this->stat.skill = skill;
			this->stat.Inn = has_Inn;
			this->stat.GS = has_GS;
		}
	};
	q_dpnode** qdp;

	Crafter(int language);

	int get_level_progress_factor();
	int get_level_quality_factor();
	double get_unit_progress() const;
	double get_unit_quality(int IQ) const;

	double _pdp_curr_cost(int P, string s, int P_dad, bool has_MM, bool has_Ven);
	void _pdp_update(int P, string s);
	void dp_progress();

	double _qdp_curr_cost(int k, int Q, string s, int k_dad, int Q_dad, bool has_Inn, bool has_GS);
	void _qdp_update(int k, int Q, string s);
	void dp_quality();

	void combo_expand(list<status>& l);
	list<status> dur_arrange(list<status>& pre);
	void merge();

	bool test_Q(int Q);
	void solve_me();

	int get_HQ_percent();
	void read_paras();
	void read_paras_en();
	void read_paras_cn();
	void print();
	void print_en();
	void print_cn();

	~Crafter();
};