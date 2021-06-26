#include<iostream>
#include<iomanip>
#include<limits>
#include<list>
#include<functional>

#include"Crafter.h"
#include"Tables.h"

using std::pair;
using std::make_pair;
using std::list;
using std::function;
using std::cin;
using std::cout;
using std::setw;
using std::endl;

int skill::get_progress(const Crafter & c, bool has_MM, bool has_Ven) {
	double buff = 0.0;
	if (has_Ven)
		buff += 0.5;
	if (has_MM)
		buff += 1;
	return static_cast<int>(c.get_unit_progress() * rp*(1 + buff) / 100); //after rounding
}

int skill::get_quality(const Crafter & c, int current_IQ, bool has_Inn, bool has_GS) {
	double buff = 0.0;
	if (has_Inn)
		buff += 0.5;
	if (has_GS)
		buff += 1;
	int current_rq = rq;
	if (this->name == "Basic Standard Combo")
		current_rq = 100; //Basic Touch
	if (this->name == "Byregot's Blessing")
		current_rq += 20 * (current_IQ > 1 ? current_IQ - 1 : 0);
	int res = static_cast<int>(c.get_unit_quality(current_IQ) * current_rq*(1 + buff) / 100);
	if (this->name == "Basic Standard Combo") {
		if (has_GS)
			buff -= 1;
		current_rq = 125; //Standard Touch
		res += static_cast<int>(c.get_unit_quality(current_IQ + 1) * current_rq*(1 + buff) / 100);
	}
	return res; //after rounding
}


void status::step(const Crafter& c, const string& s) {
	skill = s;
	if (skill == "NULL") {
		CP_consumed = INT_MAX;
		return;
	}
	progress += skill_table[s].get_progress(c, MM > 0, Ven > 0);
	quality += skill_table[s].get_quality(c, IQ, Inn > 0, GS > 0);

	CP_consumed += skill_table[s].CP;

	if (s != "Master's Mend")
		dur += skill_table[s].dur / (WN > 0 ? 2 : 1);
	else //Master's Mend
		dur += skill_table[s].dur;
	if (Man > 0)
		dur += 5 * skill_table[s].step;
	if (dur >= Dmax)
		dur = Dmax;

	//Muscle Memory
	if (MM > 0) {
		if (skill_table[s].type_P)
			MM = 0;
		else
			MM -= skill_table[s].step;
	}
	if (s == "Muscle Memory")
		MM = 5;
	//Veneration
	if (Ven > 0)
		Ven -= skill_table[s].step;
	if (s == "Veneration")
		Ven = 4;
	//Innovation
	if (Inn > 0)
		Inn -= skill_table[s].step;
	if (s == "Innovation")
		Inn = 4;
	//Great Strides
	if (GS > 0) {
		if (skill_table[s].type_Q)
			GS = 0;
		else
			GS -= skill_table[s].step;
	}
	if (s == "Great Strides")
		GS = 3;
	//Inner Quiet
	if (s == "Inner Quiet")
		IQ = 1;
	if (s == "Reflect")
		IQ = 3;
	if (s == "Byregot's Blessing")
		IQ = 0;
	if (IQ > 0)
		IQ += skill_table[s].IQ_stack_granted;
	//Waste Not
	if (WN > 0)
		WN -= skill_table[s].step;
	if (s == "Waste Not")
		WN = 4;
	if (s == "Waste Not II")
		WN = 8;
	//Manipulation
	if (Man > 0)
		Man -= skill_table[s].step;
	if (s == "Manipulation")
		Man = 8;
}

int Crafter::get_level_progress_factor() {
	int level_diff = level_table[character_level - 1] - recipe_level;
	if (level_diff < -30)
		level_diff = -30;
	if (level_diff > 49)
		level_diff = 49;
	return level_factor_table[level_diff + 30].first;
}

int Crafter::get_level_quality_factor() {
	int level_diff = level_table[character_level - 1] - recipe_level;
	if (level_diff < -30)
		level_diff = -30;
	if (level_diff > 49)
		level_diff = 49;
	return level_factor_table[level_diff + 30].second;
}

double Crafter::get_unit_progress() const {
	return (craftsmanship + 10000) / (suggested_craftsmanship + 10000)*((craftsmanship * 21) / 100 + 2)*level_progress_factor / 100;  //corresponding to 100%
}

double Crafter::get_unit_quality(int IQ) const {
	double current_control = control + (control / 5) * (IQ > 1 ? IQ - 1 : 0);
	return (current_control + 10000) / (suggested_control + 10000)*((current_control * 35) / 100 + 35)*level_quality_factor / 100;  //corresponding to 100%
}

void Crafter::pool_progress() {
	struct dpnode {
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

	if (start_with == START_WITH_MUSCLE_MEMORY)
		Pmax -= skill_table["Muscle Memory"].get_progress(*this, false, false); //Muscle Memory
	if (Pmax < 0)
		Pmax = 0; //In case a single Muscle Memory fills all the progress
	dpnode* dp = new dpnode[Pmax + 1];
	for (int P = 0; P <= Pmax; ++P) {
		if (P == 0)
			dp[P].set(0.0, P, "Empty", false);
		else {
			dp[P].set(DBL_MAX, -1, "NULL", false); //by induction, all the impossible nodes are in this form
			for (auto& s : p_table) {
				auto dp_update = [&](bool has_MM, bool has_Ven) {
					double buff_cost = 0.0;
					if (has_Ven)
						buff_cost += 4.5 * s.second.step;
					int P_dad = (P - s.second.get_progress(*this, has_MM, has_Ven) > 0) ? (P - s.second.get_progress(*this, has_MM, has_Ven)) : 0;
					if (has_MM && P_dad > 0)
						return;
					double alpha;
					if (s.second.dur == -20 && s.second.step == 1)
						alpha = alpha_20;
					else
						alpha = alpha_10;
					double curr_cost = dp[P_dad].cost + s.second.CP + buff_cost - alpha * s.second.dur;
					if (curr_cost < dp[P].cost) //use lazy update to ensure a relative descent order
						dp[P].set(curr_cost, P_dad, s.first, has_Ven);
				};

				if (s.first == "Empty")
					continue;
				dp_update(false, false); //without Veneration
				dp_update(false, true); //with Veneration
				if (start_with == START_WITH_MUSCLE_MEMORY) {
					dp_update(true, false); //without Veneration
					dp_update(true, true); //with Veneration
				}
			}
		}
	}
	dpnode* curr = &dp[Pmax];
	if (curr->stat.skill != "NULL") {
		while (curr->stat.skill != "Empty") {
			p_list.emplace_front(curr->stat);
			curr = &dp[curr->P_dad];
		}
	}
	if (p_list.empty()) { //in case a single Muscle Memory fill all the progress
		p_list.emplace_front(curr->stat);
		p_list.begin()->skill = "Basic Synthesis";
	}
	delete[] dp;
}

void Crafter::pool_quality() {
	struct dpnode {
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

	int min_IQ;
	if (start_with == START_WITH_MUSCLE_MEMORY)
		min_IQ = 1;
	else if (start_with == START_WITH_REFLECT) {
		min_IQ = 3;
		Qmax -= skill_table["Reflect"].get_quality(*this, 0, false, false); //Reflect
	}
	else
		min_IQ = 1;
	if (Qmax <= 0)
		return;
	dpnode** dp = new dpnode*[12];
	for (int k = 0; k <= 11; ++k)
		dp[k] = new dpnode[Qmax + 1];
	for (int k = min_IQ; k <= 11; ++k) {
		for (int Q = 0; Q <= Qmax; ++Q) {
			if (k == min_IQ && Q == 0)
				dp[k][Q].set(0.0, k, Q, "Empty", false, false);
			else {
				dp[k][Q].set(DBL_MAX, -1, -1, "NULL", false, false); //by induction, all the impossible nodes are in this form
				for (auto& s : q_table) {
					auto _dp_update = [&](int k_dad, bool has_Inn, bool has_GS) {
						double buff_cost = 0.0;
						if (has_Inn)
							buff_cost += 4.5 * s.second.step;
						if (has_GS)
							buff_cost += 32;
						int Q_dad = (Q - s.second.get_quality(*this, k_dad, has_Inn, has_GS) > 0) ? (Q - s.second.get_quality(*this, k_dad, has_Inn, has_GS)) : 0;
						double alpha;
						if (s.second.dur == -20 && s.second.step == 1)
							alpha = alpha_20;
						else
							alpha = alpha_10;
						double curr_cost = dp[k_dad][Q_dad].cost + s.second.CP + buff_cost - alpha * s.second.dur;
						if (curr_cost <= dp[k][Q].cost) //use gready update to ensure more powerful skills at later steps
							dp[k][Q].set(curr_cost, k_dad, Q_dad, s.first, has_Inn, has_GS);
					};

					if (s.first == "Empty")
						continue;
					if (s.first == "Byregot's Blessing" && Q != Qmax) //check Byregot's Blessing only at dp[k][Q_max]
						continue;
					//IQ stack granted
					int k_dad = k - s.second.IQ_stack_granted;
					if (k_dad < min_IQ)
						continue;
					_dp_update(k_dad, false, false);
					_dp_update(k_dad, false, true);
					_dp_update(k_dad, true, false);
					_dp_update(k_dad, true, true);
					//there can be no IQ stack granted since IQ stack cap is 11
					if (k == 11) {
						k_dad = k;
						_dp_update(k_dad, false, false);
						_dp_update(k_dad, false, true);
						_dp_update(k_dad, true, false);
						_dp_update(k_dad, true, true);
					}
				} //skill s
			}
		} //Q
	} //k
	double cost = DBL_MAX;
	int final_IQ = min_IQ;
	for (int k = min_IQ; k <= 11; ++k) {
		if (dp[k][Qmax].cost < cost) {
			final_IQ = k;
			cost = dp[k][Qmax].cost;
		}
	}
	dpnode* curr = &dp[final_IQ][Qmax];
	if (curr->stat.skill != "NULL") {
		while (curr->stat.skill != "Empty") {
			q_list.emplace_front(curr->stat);
			curr = &dp[curr->k_dad][curr->Q_dad];
		}
	}
	for (int k = 0; k <= 11; ++k)
		delete[] dp[k];
	delete[] dp;
}

void Crafter::combo_expand(list<status>& l) {
	for (auto s = l.begin(); s != l.end(); ++s) {
		if (s->skill == "Focused Synthesis Combo") {
			auto obs = l.emplace(s, *s);
			obs->skill = "Observe";
			obs->forward = 1;
			auto focus = l.emplace(s, *s);
			focus->skill = "Focused Synthesis";
			focus->backward_binding = true;
			l.erase(s);
			s = focus;
		}
		if (s->skill == "Focused Touch Combo") {
			auto obs = l.emplace(s, *s);
			obs->skill = "Observe";
			obs->forward = 1;
			auto focus = l.emplace(s, *s);
			focus->skill = "Focused Touch";
			focus->backward_binding = true;
			l.erase(s);
			s = focus;
		}
		if (s->skill == "Basic Standard Combo") {
			auto bsc = l.emplace(s, *s);
			bsc->skill = "Basic Touch";
			bsc->forward = 1;
			auto stdd = l.emplace(s, *s);
			stdd->skill = "Standard Touch (Combo)";
			stdd->backward_binding = true;
			l.erase(s);
			s = stdd;
		}
	}
}

list<status> Crafter::dur_arrange(list<status>& pre) {
	list<int> suffix_dur;
	int sd = 0;
	for (auto p = pre.rbegin(); p != pre.rend(); ++p) {
		if (p == pre.rbegin()) {
			if (p->skill == "Groundwork")
				sd = 20;
			else
				sd = 1;
		}
		else
			sd -= skill_table[p->skill].dur;
		suffix_dur.push_front(sd);
	}

	function<list<status>(list<status>::iterator, list<int>::iterator, status)> _arrange =
		[&_arrange, &pre, &suffix_dur, this](list<status>::iterator pre_start, list<int>::iterator suffix_dur_start, status stat) -> list<status> {
		list<status> r;
		status curr_stat = stat;
		int cost = INT_MAX;

		auto branch = [&](const string& s, bool pre_executed) {
			list<status> f;
			curr_stat = stat;
			curr_stat.step(*this, s);
			auto p = pre_start;
			auto sd = suffix_dur_start;
			if (pre_executed == false)
				f = _arrange(p, sd, curr_stat);
			else
				f = _arrange(++p, ++sd, curr_stat);
			if (f.back().CP_consumed < cost) {
				cost = f.back().CP_consumed;
				r.clear();
				r.push_back(curr_stat);
				r.splice(r.end(), f);
			}
		};

		if (pre_start == pre.end()) { //finished
			curr_stat.step(*this, "Empty");
			r.push_back(curr_stat);
			return r;
		}
		curr_stat.step(*this, "NULL");
		r.push_back(curr_stat); //in case this branch is impossible for later arrangement, i.e. every future is failed
		if (stat.dur <= 0) //boom
			return r;
		if (stat.CP_consumed > CP) //out of CP
			return r;
		if (pre_start->backward_binding == true) {
			branch(pre_start->skill, true);
			return r;
		}
		if (stat.MM == 1) {
			if (skill_table[pre_start->skill].type_P)
				branch(pre_start->skill, true);
			return r; //we ignore the situation that fails to make use of the Muscle Memory buff: if your recipe has low prog and high quality, you should try Reflect to start with
		}
		if (pre_start->GS > 0 && stat.GS < skill_table[pre_start->skill].step + pre_start->forward)
			branch("Great Strides", false);
		else if (pre_start->Ven > 0 && stat.Ven < skill_table[pre_start->skill].step + pre_start->forward)
			branch("Veneration", false);
		else if (pre_start->Inn > 0 && stat.Inn < skill_table[pre_start->skill].step + pre_start->forward)
			branch("Innovation", false);
		else {
			if (stat.dur < *suffix_dur_start) { //durability is not enough for the rest of skills
				if (stat.dur < stat.Dmax)
					branch("Master's Mend", false);
				if (stat.Man == 0)
					branch("Manipulation", false);
				if (stat.WN == 0) {
					branch("Waste Not", false);
					branch("Waste Not II", false);
				}
			}
			if (pre_start->skill == "Basic Touch" && pre_start->forward == 0 && stat.WN == 0)
				branch("Prudent Touch", true);
			if (pre_start->skill != "Groundwork" || stat.dur >= 20 || stat.WN > 0 && stat.dur >= 10)
				branch(pre_start->skill, true);
		}
		return r;
	};

	list<status> res;
	status stat(Dmax);
	if (start_with == START_WITH_MUSCLE_MEMORY) {
		stat.step(*this, "Muscle Memory");
		res.emplace_back(stat);
		stat.step(*this, "Inner Quiet");
		res.emplace_back(stat);
	}
	else if (start_with == START_WITH_REFLECT) {
		stat.step(*this, "Reflect");
		res.emplace_back(stat);
	}
	else {
		stat.step(*this, "Inner Quiet");
		res.emplace_back(stat);
	}
	res.splice(res.end(), _arrange(pre.begin(), suffix_dur.begin(), stat));
	return res;
}

void Crafter::merge() {
	list<status> pre = p_list, tmp = q_list;
	if (tmp.empty()) { //in case q_list is empty
		tmp = pre;
		combo_expand(tmp);
		tmp = dur_arrange(tmp);
		res_list.swap(tmp);
		return;
	}

	auto p_end = pre.end();
	--p_end; //p_list is always non-empty
	pre.splice(pre.end(), tmp);
	auto p2_start = pre.end();

	int cost = INT_MAX;
	while (p_end != pre.rend().base()) {
		p2_start = pre.emplace(p2_start, *p_end);
		auto t = p_end;
		--p_end;
		pre.erase(t);
		/* merge two adjacent Basic Synthesis and Basic Touch into Delicate Synthesis:
		Delicate Synthesis is more cost-effective than Basic Synthesis + Basic Touch (alpha=1.4);
		Prudent Touch is equally cost-effective as Basic Synthesis, and it saves one step by using Delicate instead.
		*/
		for (auto p = pre.begin(); p != pre.end(); ++p) {
			if (!tmp.empty() && tmp.back().skill == "Basic Synthesis" && p->skill == "Basic Touch") {
				tmp.back().skill = "Delicate Synthesis";
				tmp.back().GS = p->GS;
				tmp.back().Inn = p->Inn;
			}
			else
				tmp.emplace_back(*p);
		}
		combo_expand(tmp);
		tmp = dur_arrange(tmp);
		if (tmp.back().CP_consumed < cost) {
			cost = tmp.back().CP_consumed;
			res_list.swap(tmp);
		}
		tmp.clear();
	}
}

void Crafter::solve_me() {
	switch (language) {
	case 1: //English
		cout << "Optimizing progress skills......" << endl << endl; break;
	case 2: //Chinese
		cout << "优化作业技能中……" << endl << endl; break;
	default:
		cout << "Optimizing progress skills......" << endl << endl; break;
	}
	pool_progress();

	switch (language) {
	case 1: //English
		cout << "Optimizing quality skills......" << endl << endl; break;
	case 2: //Chinese
		cout << "优化加工技能中……" << endl << endl; break;
	default:
		cout << "Optimizing quality skills......" << endl << endl; break;
	}
	pool_quality();

	switch (language) {
	case 1: //English
		cout << "Generating rotations......" << endl << endl; break;
	case 2: //Chinese
		cout << "生成工序中……" << endl << endl; break;
	default:
		cout << "Generating rotations......" << endl << endl; break;
	}
	merge();
	cout << endl;
}

int Crafter::get_HQ_percent() {
	int q_percent = (int)(100 * res_list.back().quality / Qmax);
	return (q_percent > 100 ? HQ_table[100] : HQ_table[q_percent]);
}

void Crafter::read_paras() {
	switch (language)
	{
	case 1: //English
		read_paras_en(); break;
	case 2: //Chinese
		read_paras_cn(); break;
	default:
		read_paras_en();
	}
	cout << endl;
}

void Crafter::read_paras_en() {
	cout << "Please input the following info, separating numbers with \'space\': " << endl;
	cout << "Character status (level, craftsmanship, control and CP): " << endl;
	cin >> character_level >> craftsmanship >> control >> CP;
	cout << endl << "Recipe (level, suggested craftsmanship, suggested control, progress, quality, durability): " << endl;
	cin >> recipe_level >> suggested_craftsmanship >> suggested_control >> Pmax >> Qmax >> Dmax;
	cout << endl << "Which skill would you like to start with? Muscle Memory(M), Reflect(R), Inner Quiet(I): " << endl;
	char s;
	cin >> s;
	cout << endl << "The following parameter measures how much 1 durability charges for CP under Waste Not, tuning up it will decrease the effect of Groundwork/Preparatory Touch. " << endl
		<< "It ranges from 1.2 to 2.4, you may try 1.2, 1.5, 1.8, 2.0, 2.35 for significant changes. " << endl
		<< "My default setting is 1.2. If you are not confident with this, just input 0 to leave it be." << endl;
	double a20;
	cin >> a20;
	if (a20 > 0)
		alpha_20 = a20;
	if (s == 'M')
		start_with = START_WITH_MUSCLE_MEMORY;
	else if (s == 'R')
		start_with = START_WITH_REFLECT;
	else if (s == 'I')
		start_with = START_WITH_INNER_QUIET;
}

void Crafter::read_paras_cn() {
	cout << "请您输入下列信息，用“空格”分隔数字：" << endl;
	cout << "角色信息（等级、作业精度、加工精度、制作力）： " << endl;
	cin >> character_level >> craftsmanship >> control >> CP;
	cout << endl << "配方信息（配方等级、作业精度要求、加工精度要求、进度、品质、耐久）： " << endl;
	cin >> recipe_level >> suggested_craftsmanship >> suggested_control >> Pmax >> Qmax >> Dmax;
	cout << endl << "您希望第一个技能使用什么？坚信 (M)、闲静 (R)、内静 (I)：" << endl;
	char s;
	cin >> s;
	cout << endl << "下面这个参数衡量了在俭约状态下1点耐久对应的制作力，调高这个参数将会降低坯料制作/坯料加工的效果。" << endl
		<< "这个参数的范围从1.2到2.4，您可以尝试1.2、1.5、1.8、2.0、2.35这几个数字。" << endl
		<< "我将它默认设置为1.2。如果您对此不自信，请输入0以使用默认设置。" << endl;
	double a20;
	cin >> a20;
	if (a20 > 0)
		alpha_20 = a20;
	if (s == 'M')
		start_with = START_WITH_MUSCLE_MEMORY;
	else if (s == 'R')
		start_with = START_WITH_REFLECT;
	else if (s == 'I')
		start_with = START_WITH_INNER_QUIET;
}

void Crafter::print() {
	switch (language)
	{
	case 1: //English
		print_en(); break;
	case 2: //Chinese
		print_cn(); break;
	default:
		print_en();
	}
	cout << endl;
}

void Crafter::print_en() {
	cout << "/**********************Optimization Result**********************/" << endl;
	if (res_list.empty()) {
		cout << "Fail to craft under current input :(" << endl
			<< "Here are some tips for you: " << endl
			<< "\t 1. Enhance your character with more craftsmanship/control/CP;" << endl
			<< "\t 2. Maybe it'll work if you try another skill to start with." << endl;
		return;
	}
	cout << "Turn" << setw(25) << "Skill" << setw(12) << "Progress" << setw(12) << "Quality" << setw(12) << "Durability" << endl;
	int t = 0;
	for (auto p = res_list.begin(); p != res_list.end(), p->skill != "Empty"; ++p)
		cout << setw(4) << ++t << setw(25) << skill_table[p->skill].name << setw(12) << p->progress << setw(12) << p->quality << setw(12) << p->dur << endl;
	cout << "Total CP: " << res_list.back().CP_consumed << "\t\t" << "HQ percent: " << get_HQ_percent() << endl;

	//macro
	cout << endl << "In-game macro: " << endl;
	for (auto p = res_list.begin(); p != res_list.end(), p->skill != "Empty"; ++p)
		cout << "/ac \"" << skill_table[p->skill].name << "\"" << endl;
}

void Crafter::print_cn() {
	cout << "/***************************优化结果***************************/" << endl;
	if (res_list.empty()) {
		cout << "制作失败 :(" << endl
			<< "给您的一些建议如下：" << endl
			<< "\t 1. 强化您的角色，使其拥有更高的作业精度/加工精度/制作力；" << endl
			<< "\t 2. 尝试更换其他的起手技能。" << endl;
		return;
	}
	cout << "工次" << setw(25) << "技能" << setw(12) << "进度" << setw(12) << "品质" << setw(12) << "耐久" << endl;
	int t = 0;
	for (auto p = res_list.begin(); p != res_list.end(), p->skill != "Empty"; ++p)
		cout << setw(4) << ++t << setw(25) << skill_name_cn[p->skill] << setw(12) << p->progress << setw(12) << p->quality << setw(12) << p->dur << endl;
	cout << "制作力总计：" << res_list.back().CP_consumed << "\t\t" << "HQ概率：" << get_HQ_percent() << endl;

	//macro
	cout << endl << "游戏宏：" << endl;
	for (auto p = res_list.begin(); p != res_list.end(), p->skill != "Empty"; ++p)
		cout << "/ac " << skill_name_cn[p->skill] << endl;
}