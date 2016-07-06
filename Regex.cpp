//正则表达式解析器
//北京石油化工学院 2015级 王万霖 孙兰昌
//2016-7
#include <iostream>
#include <vector>
#include <stack>
#include <string>
#include <algorithm>
#include <cmath>

using namespace std;

class nfaMachine
{
	public:
		struct Edge;
		struct Status
		{
			vector<Edge*> InEdges;
			vector<Edge*> OutEdges;
		};
		struct Edge
		{
			Status* FromStatus;
			Status* ToStatus;
			string Regex;
		};
		vector<Status*> StatusList;
		vector<Edge*> EdgeList;
		Status* startStatus;
		vector<Status*> acceptStatus;
		nfaMachine();
		nfaMachine(vector<string>);
		~nfaMachine();
		void convert();
		bool match(string, char);
		bool nfa(string, Status*);
		void print();
};

vector<string> splitRegex(string);
bool verrify(string);
bool kuoHao(string);
bool lianZiFu(string);
bool jiaHao(string);
string removeSpace(string);
string removeXieGang(string);
bool singleChar(string, char);
bool doubleChar(string, char);
bool mulChar(string, char);

nfaMachine::nfaMachine()
{
	return;
}

nfaMachine::nfaMachine(vector<string> splited_Regex)
{
	Status* startStatus = new nfaMachine::Status;
	this->startStatus = startStatus;
	StatusList.push_back(startStatus);
	Status* previousStatus = startStatus;
	for (int element_count = 0;
		element_count < splited_Regex.size();
		element_count++)
	{
		Status* newStatus = new nfaMachine::Status;
		Edge* newEdge = new nfaMachine::Edge;
		newEdge->FromStatus = previousStatus;
		newEdge->ToStatus = newStatus;
		newEdge->Regex = splited_Regex[element_count];
		previousStatus->OutEdges.push_back(newEdge);
		newStatus->InEdges.push_back(newEdge);
		StatusList.push_back(newStatus);
		EdgeList.push_back(newEdge);
		previousStatus = newStatus;
	}
	this->acceptStatus.push_back(previousStatus);
	return;
}

nfaMachine::~nfaMachine()
{
	for (vector<Status*>::iterator status_i = this->StatusList.begin();
		status_i < this->StatusList.end();
		status_i++)
	{
		delete *status_i;
	}
	for (vector<Edge*>::iterator edge_i = this->EdgeList.begin();
		edge_i < this->EdgeList.end();
		edge_i++)
	{
		delete *edge_i;
	}
}

void nfaMachine::convert()
{
	vector<Edge*> tmpEdgeList;
	for (vector<Edge*>::iterator edge_i = this->EdgeList.begin();
		edge_i < this->EdgeList.end();
		edge_i++)
	{
		tmpEdgeList.push_back(*edge_i);
	}
	for (vector<Edge*>::iterator edge_i = tmpEdgeList.begin();
		edge_i < tmpEdgeList.end();
		edge_i++)
	{
		char firstChar = (**edge_i).Regex[0];
		char lastChar = (**edge_i).Regex[(**edge_i).Regex.size()-1];
		string remainChar = "";
		for (string::iterator regex_i = (**edge_i).Regex.begin();
			regex_i < (**edge_i).Regex.end();
			regex_i++)
		{
			if (regex_i < (**edge_i).Regex.end()-1)
			{
				remainChar += *regex_i;
			}
		}
		if (lastChar == '+' || lastChar == '*')
		{
			Status* leftStatus = (**edge_i).FromStatus;
			Status* rightStatus = (**edge_i).ToStatus;
			if (lastChar == '*')
			{
				Edge* new_edge = new nfaMachine::Edge;
				(**edge_i).Regex = "";
				this->EdgeList.push_back(new_edge);
				new_edge->Regex = remainChar;
				new_edge->FromStatus = leftStatus;
				new_edge->ToStatus = leftStatus;
				leftStatus->InEdges.push_back(new_edge);
				leftStatus->OutEdges.push_back(new_edge);
			}
			else if (lastChar == '+')
			{
				(**edge_i).Regex = remainChar;
				Status* new_status = new nfaMachine::Status;
				this->StatusList.push_back(new_status);
				(**edge_i).ToStatus = new_status;
				new_status->InEdges.push_back(*edge_i);
				vector<Edge*>::iterator position =
					find(rightStatus->InEdges.begin(),
						rightStatus->InEdges.end(),
						*edge_i);
				rightStatus->InEdges.erase(position);
				Edge* new_edge = new nfaMachine::Edge;
				this->EdgeList.push_back(new_edge);
				new_edge->Regex = "";
				new_edge->FromStatus = new_status;
				new_edge->ToStatus = rightStatus;
				new_status->OutEdges.push_back(new_edge);
				rightStatus->InEdges.push_back(new_edge);
				Edge* cycle_edge = new nfaMachine::Edge;
				this->EdgeList.push_back(cycle_edge);
				cycle_edge->Regex = remainChar;
				cycle_edge->FromStatus = new_status;
				cycle_edge->ToStatus = new_status;
				new_status->InEdges.push_back(cycle_edge);
				new_status->OutEdges.push_back(cycle_edge);
			}
		}
	}
	tmpEdgeList.clear();
	for (vector<Edge*>::iterator edge_i = this->EdgeList.begin();
		edge_i < this->EdgeList.end();
		edge_i++)
	{
		tmpEdgeList.push_back(*edge_i);
	}
	for (vector<Edge*>::iterator edge_i = tmpEdgeList.begin();
		edge_i < tmpEdgeList.end();
		edge_i++)
	{
		if (find((**edge_i).Regex.begin(), (**edge_i).Regex.end(), '(') !=
			(**edge_i).Regex.end())
		{
			string newString = "";
			for (string::iterator str_i = (**edge_i).Regex.begin()+1;
				str_i < (**edge_i).Regex.end()-1;
				str_i++)
			{
				newString += *str_i;
			}
			vector<string> splited_Regex = splitRegex(newString);
			Status* fromStatus = (**edge_i).FromStatus;
			Status* toStatus = (**edge_i).ToStatus;
			vector<Edge*>::iterator position_F =
				find(fromStatus->OutEdges.begin(),
					fromStatus->OutEdges.end(),
					*edge_i);
			fromStatus->OutEdges.erase(position_F);
			vector<Edge*>::iterator position_T = 
				find(toStatus->InEdges.begin(),
					toStatus->InEdges.end(),
					*edge_i);
			toStatus->InEdges.erase(position_T);
			if (splited_Regex.size() == 1)
			{
				break;
			}
			for (int regex_i=0; regex_i<splited_Regex.size(); regex_i++)
			{
				Status* previousStatus = fromStatus;
				Edge* new_edge = new nfaMachine::Edge;
				this->EdgeList.push_back(new_edge);
				previousStatus->OutEdges.push_back(new_edge);
				new_edge->FromStatus = previousStatus;
				new_edge->Regex = splited_Regex[regex_i];
				if (regex_i != splited_Regex.size()-1)
				{
					Status* new_status = new nfaMachine::Status;
					new_edge->ToStatus = new_status;
					new_status->InEdges.push_back(new_edge);
					this->StatusList.push_back(new_status);
					previousStatus = new_status;
				}
				else
				{
					new_edge->ToStatus = toStatus;
					toStatus->InEdges.push_back(new_edge);
				}
			}
		}
	}
	bool KuoHaoStatus = false;
	for (vector<Edge*>::iterator edge_i = this->EdgeList.begin();
		edge_i < this->EdgeList.end();
		edge_i++)
	{
		if (find((**edge_i).Regex.begin(), (**edge_i).Regex.end(), '(') !=
			(**edge_i).Regex.end())
		{
			KuoHaoStatus = true;
			break;
		}
	}
	if (KuoHaoStatus)
	{
		this->convert();
	}
}

bool nfaMachine::match(string ex, char userChar)
{
	if (ex.size() == 1)
	{
		bool singleCharEx = singleChar(ex, userChar);
		return singleCharEx;
	}
	else if (ex.size() == 2)
	{
		bool doubleCharEx = doubleChar(ex, userChar);
		return doubleCharEx;
	}
	else if (ex.size() >= 3)
	{
		bool mulCharEx = mulChar(ex, userChar);
		return mulCharEx;
	}
	else
	{
		return false;
	}
}

bool nfaMachine::nfa(string remain_string, Status* status_now = 0)
{
	if (find(this->acceptStatus.begin(), this->acceptStatus.end(), status_now) !=
		this->acceptStatus.end())
	{
		return true;
	}
	if (status_now == 0)
	{
		status_now = this->startStatus;
	}
	if (remain_string.size() <= 0)
	{
		bool status = false;
		for (vector<Edge*>::iterator edge_i = status_now->OutEdges.begin();
			edge_i < status_now->OutEdges.end();
			edge_i++)
		{
			if ((**edge_i).Regex == "")
			{
				if (this->nfa(remain_string, (**edge_i).ToStatus))
				{
					status = true;
				}
			}
		}
		if (status)
		{
			return true;
		}
		if (find(this->acceptStatus.begin(),
			this->acceptStatus.end(), status_now)
			!= this->acceptStatus.end())
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		string new_string = "";
		for (string::iterator str_i = remain_string.begin()+1;
			str_i < remain_string.end();
			str_i++)
		{
			new_string += *str_i;
		}
		vector<Status*> possible_status;
		vector<Status*> possible_eposilo_status;
		char this_char = remain_string[0];
		for (vector<Edge*>::iterator edge_i = status_now->OutEdges.begin();
			edge_i < status_now->OutEdges.end();
			edge_i++)
		{
			if ((**edge_i).Regex == "")
			{
				possible_eposilo_status.push_back((**edge_i).ToStatus);
			}
			else if (this->match((**edge_i).Regex, this_char))
			{
				possible_status.push_back((**edge_i).ToStatus);
			}
		}
		bool accept_status = false;
		for (vector<Status*>::iterator status_i = possible_status.begin();
			status_i < possible_status.end();
			status_i++)
		{
			if (nfa(new_string, *status_i) == true)
			{
				accept_status = true;
			}
		}
		for (vector<Status*>::iterator status_i = possible_eposilo_status.begin();
			status_i < possible_eposilo_status.end();
			status_i++)
		{
			if (nfa(remain_string, *status_i) == true)
			{
				accept_status = true;
			}
		}
		return accept_status;
	}
}

void nfaMachine::print()
{
	int times = 1;
	for (vector<Status*>::iterator status_i = StatusList.begin();
		status_i < StatusList.end();
		status_i++)
	{
		cout<<"[id:"<<*status_i<<"]";
		if (find(this->acceptStatus.begin(), this->acceptStatus.end(), *status_i) !=
			this->acceptStatus.end())
		{
			cout<<"【接受状态】";
		}
		else if (*status_i == this->startStatus)
		{
			cout<<"【起始状态】";
		}
		cout<<"第 "<<times<<" 个状态，共有"<<(**status_i).InEdges.size()<<"个导入状态";
		cout<<"   有"<<(**status_i).OutEdges.size()<<"个导出状态"<<endl;
		int edge_count = 1;
		for (vector<Edge*>::iterator edge_i = (**status_i).OutEdges.begin();
			edge_i < (**status_i).OutEdges.end();
			edge_i++)
		{
			cout<<"[指向:"<<(**edge_i).ToStatus<<"]"<<"    "<<edge_count<<" "<<(**edge_i).Regex;
			if ((**edge_i).FromStatus == (**edge_i).ToStatus)
			{
				cout<<" (指向同一个状态)";
			}
			cout<<endl;
			edge_count++;
		}
		edge_count = 1;
		times++;
	}
	return;
}

vector<string> splitRegex(string ex)
{
	vector<string> res;
	stack<char> symbolStack;
	string tmpString = "";
	bool status = false;
	for (string::iterator str_i = ex.begin();
		str_i != ex.end();
		str_i++)
	{
		if (*str_i == '[' || *str_i == '(')
		{
			status = true;
		}
		if (status == true)
		{
			tmpString += *str_i;
			if (*str_i == '[' || *str_i == '(')
			{
				symbolStack.push(*str_i);
			}
			else if (*str_i == ']' || *str_i == ')')
			{
				symbolStack.pop();
				if (symbolStack.size() == 0)
				{
					status = false;
					res.push_back(tmpString);
					tmpString = "";
				}
			}
		}
		else
		{
			string tmp = "";
			tmp += *str_i;
			res.push_back(tmp);
		}
	}
	vector<string> result;
	for (int i = 0; i < res.size(); i++)
	{
		string tmp = "";
		if (res[i] == "\\")
		{
			tmp += res[i];
			tmp += res[i+1];
			result.push_back(tmp);
			tmp = "";
			i++;
			continue;
		}
		if (res[i] == "*" || res[i] == "+")
		{
			tmp = res[i];
			*(result.end()-1) += tmp;
			tmp = "";
		}
		else
		{
			tmp = res[i];
			result.push_back(tmp);
		}
	}
	return result;
}

string removeSpace(string ex)
{
	string str = "";
	for (int i = 0; i < ex.size(); i++)
	{
		if (ex[i] != ' ')
		{
			str += ex[i];
		}
	}
	return str;
}

bool kuoHao(string ex)
{
	stack<char> symbolStack;
	for (int i = 0; i < ex.size(); i++)
	{
		if (ex[i] == '[' || ex[i] == '(')
		{
			symbolStack.push(ex[i]);
		}
		if (ex[i] == ']' || ex[i] == ')')
		{
			if (symbolStack.empty() == true )
			{
				return false;
			}
			if ((ex[i] == ']' && symbolStack.top() == '[') || (ex[i] == ')' && symbolStack.top() == '('))
			{
				symbolStack.pop();
			}
			else
			{
				return false;
			}
		}
	}
	if (symbolStack.empty() == true)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool xieGang(string ex)
{
	string::iterator i = ex.end() - 1;
	if (*i != '\\')
	{
		return true;
	}
	else
	{
		return false;
	}
}

string removeXieGang(string ex)
{
	string re = "";
	for (int i = 0; i < ex.size(); i++)
	{
		if (ex[i] == '\\')
		{
			i++;
		}
		else
		{
			re += ex[i];
		}
	}
	return re;
}

bool lianZiFuFind(char cha1, char cha2)
{
	if (cha1 == '0' && cha2 == '9')
	{
		return true;
	}
	else if (cha1 == 'a' && cha2 == 'z')
	{
		return true;
	}
	else if (cha1 == 'A' && cha2 == 'Z')
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool lianZiFu(string ex)
{
	stack<char> charStack;
	if (ex[0] == '-' || ex[ex.size()-1] == '-')
	{
		return false;
	}
	else
	{
		for (int i = 0; i < ex.size(); i++)
		{
			if (ex[i] == '-')
			{
				charStack.push(ex[i]);
				if (lianZiFuFind(ex[i-1], ex[i+1]))
				{
					charStack.pop();
				}
				else
				{
					return false;
				}
			}
		}
		if (charStack.empty() == true)
		{
			return true;
		}
	}
}

bool jiaHao(string ex)
{
	if (ex[0] == '+' || ex[0] == '*')
	{
		return false;
	}
	else
	{
		for (int i = 0; i < ex.size(); i++)
		{
			if (ex[i] == '+' || ex[i] == '*')
			{
				if ((ex[i] == '+' || ex[i] == '*') && (ex[i+1] == '+' || ex[i+1] == '*'))
				{
					return false;
				}
				else
				{
					return true;
				}
			}
		}
		return true;
	}
}

bool verrify(string ex)
{
	ex = removeSpace(ex);
	if (xieGang(ex) == true)
	{
		ex = removeXieGang(ex);
	}
	else
	{
		return false;
	}
	bool tmp1  = kuoHao(ex);
	bool tmp2 = lianZiFu(ex);
	bool tmp3 = jiaHao(ex);
	return tmp1 && tmp2 && tmp3;
}

bool singleChar(string ex, char userChar)
{
	string userString = "";
	userString += userChar;
	if (ex == userString)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool doubleChar(string ex, char userChar)
{
	int ascii = userChar;
	if (ex == "\\w")
	{
		if ((ascii >= 48 && ascii <= 57) ||
		(ascii >= 97 && ascii <= 122) ||
		(ascii >= 65 && ascii <= 90))
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else if (ex == "\\d")
	{
		if (ascii >= 48 && ascii <= 57)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else if (ex == "\\.")
	{
		if (userChar == '.')
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		cout<<"Unknown ex!"<<endl;
	}
}

bool mulChar(string ex, char userChar)
{
	int ascii = userChar;
	if (ascii >= 48 && ascii <= 57)
	{
		unsigned int found = ex.find("0-9");
		if (signed(found) != string::npos)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else if (ascii >= 97 && ascii <=122)
	{
		unsigned int found = ex.find("a-z");
		if (signed(found) != string::npos)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else if (ascii >= 65 && ascii <= 90)
	{
		unsigned int found = ex.find("A-Z");
		if (signed(found) != string::npos)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
}

int main(int argc, char* argv[])
{
	string regex = "(\\w\\.)*";
	string matched_target = "";
	cout<<"准备匹配的目标:"<<matched_target<<endl;
	vector<string> test = splitRegex(regex);
	nfaMachine dgideasNFA(test);
	dgideasNFA.convert();
	dgideasNFA.print();
	if (dgideasNFA.nfa(matched_target))
	{
		cout<<"接受"<<endl;
	}
	else
	{
		cout<<"拒绝"<<endl;
	}
	return 0;
}
