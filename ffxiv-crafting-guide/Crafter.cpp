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

Crafter::Crafter(int language) :language(language) {
	read_paras();
	level_progress_factor = get_level_progress_factor();
	level_quality_factor = get_level_quality_factor();

	if (start_with == START_WITH_MUSCLE_MEMORY) {
		tPmax -= skill_table["Muscle Memory"].get_progress(*this, false, false); //Muscle Memory
		min_IQ = 1;
	}
	else if (start_with == START_WITH_REFLECT) {
		min_IQ = 3;
		tQmax -= skill_table["Reflect"].get_quality(*this, 0, false, false); //Reflect
	}
	else
		min_IQ = 1;
	if (tPmax < 0)
		tPmax = 0; //In case a single Muscle Memory fills all the progress
	if (tQmax < 0)
		tQmax = 0;

	pdp = new p_dpnode[tPmax + 1];
	qdp = new q_dpnode*[12];
	for (int k = 0; k <= 11; ++k)
		qdp[k] = new q_dpnode[tQmax + 1];
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

double Crafter::_pdp_curr_cost(int P, string s, int P_dad, bool has_MM, bool has_Ven) {
	double buff_cost = 0.0;
	if (has_Ven)
		buff_cost += 4.5 * p_table[s].step;
	double alpha;
	if (p_table[s].dur == -20 && p_table[s].step == 1)
		alpha = alpha_20;
	else
		alpha = alpha_10;
	return pdp[P_dad].cost + p_table[s].CP + buff_cost - alpha * p_table[s].dur;
}

void Crafter::_pdp_update(int P, string s) {
	for (bool has_MM : {false, true}) {
		if (start_with != START_WITH_MUSCLE_MEMORY && has_MM == true)
			continue;
		for (bool has_Ven : {false, true}) {
			int P_dad = (P - p_table[s].get_progress(*this, has_MM, has_Ven) > 0) ? (P - p_table[s].get_progress(*this, has_MM, has_Ven)) : 0;
			if (has_MM && P_dad > 0)
				continue;
			double curr_cost = _pdp_curr_cost(P, s, P_dad, has_MM, has_Ven);
			if (curr_cost < pdp[P].cost) //use lazy update to ensure a relative descent order
				pdp[P].set(curr_cost, P_dad, s, has_Ven);
		}
	}
}

void Crafter::dp_progress() {
	for (int P = 0; P <= tPmax; ++P) {
		if (P == 0)
			pdp[P].set(0.0, P, "Empty", false);
		else {
			pdp[P].set(DBL_MAX, -1, "NULL", false); //by induction, all the impossible nodes are in this form
			for (auto& s : p_table) {
				if (s.first == "Empty")
					continue;
				_pdp_update(P, s.first);
			}
		}
	}
	p_dpnode* curr = &pdp[tPmax];
	if (curr->stat.skill != "NULL") {
		while (curr->stat.skill != "Empty") {
			p_list.emplace_front(curr->stat);
			curr = &pdp[curr->P_dad];
		}
	}
	if (p_list.empty()) { //in case a single Muscle Memory fill all the progress
		p_list.emplace_front(curr->stat);
		p_list.begin()->skill = "Basic Synthesis";
	}
}

double Crafter::_qdp_curr_cost(int k, int Q, string s, int k_dad, int Q_dad, bool has_Inn, bool has_GS) {
	double buff_cost = 0.0;
	if (has_Inn)
		buff_cost += 4.5 * q_table[s].step;
	if (has_GS)
		buff_cost += 32;
	double alpha;
	if (q_table[s].dur == -20 && q_table[s].step == 1)
		alpha = alpha_20;
	else
		alpha = alpha_10;
	return qdp[k_dad][Q_dad].cost + q_table[s].CP + buff_cost - alpha * q_table[s].dur;
}

void Crafter::_qdp_update(int k, int Q, string s) {
	for (int k_dad : {k, k - q_table[s].IQ_stack_granted}) {
		if (k != 11 && k_dad == k || k_dad < min_IQ) //there can be no IQ stack granted since IQ stack cap is 11
			continue;
		for (bool has_Inn : {false, true}) {
			for (bool has_GS : {false, true}) {
				int Q_dad = (Q - q_table[s].get_quality(*this, k_dad, has_Inn, has_GS) > 0) ? (Q - q_table[s].get_quality(*this, k_dad, has_Inn, has_GS)) : 0;
				double curr_cost = _qdp_curr_cost(k, Q, s, k_dad, Q_dad, has_Inn, has_GS);
				if (curr_cost <= qdp[k][Q].cost) //use gready update to ensure more powerful skills at later steps
					qdp[k][Q].set(curr_cost, k_dad, Q_dad, s, has_Inn, has_GS);
			}
		}
	}
};

void Crafter::dp_quality() {
	if (tQmax <= 0)
		return;
	for (int k = min_IQ; k <= 11; ++k) {
		for (int Q = 0; Q <= tQmax; ++Q) {
			if (k == min_IQ && Q == 0)
				qdp[k][Q].set(0.0, k, Q, "Empty", false, false);
			else {
				qdp[k][Q].set(DBL_MAX, -1, -1, "NULL", false, false); //by induction, all the impossible nodes are in this form
				for (auto& s : q_table) {
					if (s.first == "Empty" || s.first == "Byregot's Blessing")
						continue;
					_qdp_update(k, Q, s.first);
				} //skill s
			}
		} //Q
	} //k
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

bool Crafter::test_Q(int Q) {
	q_list.clear();
	res_list.clear();
	double cost = DBL_MAX;
	q_dpnode final_node;
	final_node.set(DBL_MAX, -1, -1, "NULL", false, false);
	for (int k = min_IQ; k <= 11; ++k) {
		cost = qdp[k][Q].cost;
		for (bool has_Inn : {false, true}) {
			for (bool has_GS : {false, true}) {
				int Q_dad = (Q - q_table["Byregot's Blessing"].get_quality(*this, k, has_Inn, has_GS) > 0) ? (Q - q_table["Byregot's Blessing"].get_quality(*this, k, has_Inn, has_GS)) : 0;
				double curr_cost = _qdp_curr_cost(k, Q, "Byregot's Blessing", k, Q_dad, has_Inn, has_GS);
				if (curr_cost <= cost)
					final_node.set(curr_cost, k, Q_dad, "Byregot's Blessing", has_Inn, has_GS);
			}
		}
	}
	q_dpnode* curr = &final_node;
	if (curr->stat.skill != "NULL") {
		while (curr->stat.skill != "Empty") {
			q_list.emplace_front(curr->stat);
			curr = &qdp[curr->k_dad][curr->Q_dad];
		}
	}
	merge();
	if (!res_list.empty() && res_list.back().CP_consumed <= CP)
		return true;
	else
		return false;
}

void Crafter::solve_me() {
	switch (language) {
	case 1: //English
		cout << "Optimizing progress skills......" << endl << endl; break;
	case 2: //Chinese
		cout << "�Ż���ҵ�����С���" << endl << endl; break;
	default:
		cout << "Optimizing progress skills......" << endl << endl; break;
	}
	dp_progress();

	switch (language) {
	case 1: //English
		cout << "Optimizing quality skills......" << endl << endl; break;
	case 2: //Chinese
		cout << "�Ż��ӹ������С���" << endl << endl; break;
	default:
		cout << "Optimizing quality skills......" << endl << endl; break;
	}
	dp_quality();

	switch (language) {
	case 1: //English
		cout << "Generating rotations......" << endl << endl; break;
	case 2: //Chinese
		cout << "���ɹ����С���" << endl << endl; break;
	default:
		cout << "Generating rotations......" << endl << endl; break;
	}

	if (!test_Q(tQmax)) {
		switch (language) {
		case 1: //English
			cout << "Your current character is not strong enough to reach 100% HQ on this recipe." << endl
				<< "Would you like to search for a rotation which reaches quality as high as possible? Yes(Y)/No(N): " << endl; break;
		case 2: //Chinese
			cout << "���Ľ�ɫ���Բ�����������䷽�ϴﵽ100%HQ��" << endl
				<< "��Ҫ�����ﵽ�����ܸ�Ʒ�ʵĹ�������(Y)/��(N): " << endl; break;
		default:
			cout << "Your current character is not strong enough to reach 100% HQ on this recipe." << endl
				<< "Would you like to search for a rotation which reaches quality as high as possible? Yes(Y)/No(N): " << endl; break;
		}
		char ch;
		cin >> ch;
		if (ch == 'Y') {
			switch (language) {
			case 1: //English
				cout << endl << "Searching on the space of quality: " << endl
					<< setw(10) << "Quality" << setw(25) << "Reachability" << endl; break;
			case 2: //Chinese
				cout << endl << "��������Ʒ�ʿռ䣺" << endl
					<< setw(10) << "Ʒ��" << setw(25) << "�ɴ���" << endl; break;
			default:
				cout << endl << "Searching on the space of quality: " << endl
					<< setw(10) << "Quality" << setw(25) << "Reachability" << endl; break;
			}
			int low = 0, high = tQmax - 1;
			while (low != high) {
				if (high - low == 1) {
					if (!test_Q(high))
						test_Q(low);
					low = high;
				}
				int mid = (low + high) / 2;
				cout << setw(10) << mid + Qmax - tQmax;
				if (test_Q(mid)) {
					cout << setw(25) << "Yes" << endl;
					low = mid;
				}
				else {
					cout << setw(25) << "No" << endl;
					high = mid - 1;
				}
			}
		}
	}
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
	tPmax = Pmax;
	tQmax = Qmax;
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
	cout << "��������������Ϣ���á��ո񡱷ָ����֣�" << endl;
	cout << "��ɫ��Ϣ���ȼ�����ҵ���ȡ��ӹ����ȡ����������� " << endl;
	cin >> character_level >> craftsmanship >> control >> CP;
	cout << endl << "�䷽��Ϣ���䷽�ȼ�����ҵ����Ҫ�󡢼ӹ�����Ҫ�󡢽��ȡ�Ʒ�ʡ��;ã��� " << endl;
	cin >> recipe_level >> suggested_craftsmanship >> suggested_control >> Pmax >> Qmax >> Dmax;
	tPmax = Pmax;
	tQmax = Qmax;
	cout << endl << "��ϣ����һ������ʹ��ʲô������ (M)���о� (R)���ھ� (I)��" << endl;
	char s;
	cin >> s;
	cout << endl << "������������������ڼ�Լ״̬��1���;ö�Ӧ������������������������ή����������/���ϼӹ���Ч����" << endl
		<< "��������ķ�Χ��1.2��2.4�������Գ���1.2��1.5��1.8��2.0��2.35�⼸�����֡�" << endl
		<< "�ҽ���Ĭ������Ϊ1.2��������Դ˲����ţ�������0��ʹ��Ĭ�����á�" << endl;
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
	cout << "/**************************Optimization Result**************************/" << endl;
	if (res_list.empty()) {
		cout << "Fail to craft under current input :(" << endl
			<< "Here are some tips for you: " << endl
			<< "\t 1. Enhance your character with more craftsmanship/control/CP;" << endl
			<< "\t 2. Maybe it'll work if you try another skill to start with." << endl;
		return;
	}
	cout << setw(10) << "Turn" << setw(26) << "Skill" << setw(12) << "Progress" << setw(12) << "Quality" << setw(12) << "Durability" << endl;
	int t = 0;
	for (auto p = res_list.begin(); p != res_list.end(), p->skill != "Empty"; ++p)
		cout << setw(10) << ++t << setw(26) << skill_table[p->skill].name << setw(12) << p->progress << setw(12) << p->quality << setw(12) << p->dur << endl;
	cout << "Total CP: " << res_list.back().CP_consumed << "\t\t" << "HQ percent: " << get_HQ_percent() << endl;

	//macro
	cout << endl << "In-game macro: " << endl;
	for (auto p = res_list.begin(); p != res_list.end(), p->skill != "Empty"; ++p)
		cout << "/ac \"" << skill_table[p->skill].name << "\"" << endl;
}

void Crafter::print_cn() {
	cout << "/*******************************�Ż����*******************************/" << endl;
	if (res_list.empty()) {
		cout << "����ʧ�� :(" << endl
			<< "������һЩ�������£�" << endl
			<< "\t 1. ǿ�����Ľ�ɫ��ʹ��ӵ�и��ߵ���ҵ����/�ӹ�����/��������" << endl
			<< "\t 2. ���Ը������������ּ��ܡ�" << endl;
		return;
	}
	cout << setw(10) << "����" << setw(26) << "����" << setw(12) << "����" << setw(12) << "Ʒ��" << setw(12) << "�;�" << endl;
	int t = 0;
	for (auto p = res_list.begin(); p != res_list.end(), p->skill != "Empty"; ++p)
		cout << setw(10) << ++t << setw(26) << skill_name_cn[p->skill] << setw(12) << p->progress << setw(12) << p->quality << setw(12) << p->dur << endl;
	cout << "�������ܼƣ�" << res_list.back().CP_consumed << "\t\t" << "HQ���ʣ�" << get_HQ_percent() << endl;

	//macro
	cout << endl << "��Ϸ�꣺" << endl;
	for (auto p = res_list.begin(); p != res_list.end(), p->skill != "Empty"; ++p)
		cout << "/ac " << skill_name_cn[p->skill] << endl;
}

Crafter::~Crafter() {
	delete[] pdp;
	for (int k = 0; k <= 11; ++k)
		delete[] qdp[k];
	delete[] qdp;
}