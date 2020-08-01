// 测试运行条件 mingw C++11

#include<cstdlib>
#include<cstdio>
#include<map>
#include<vector>
#include<string>
#include<stack>
#include<algorithm>
#include<iostream>
#include<cctype>
using namespace std;

// 最大状态深度 ，如果需要设置更大的，最好修改最大栈大小，否则容易 stackoverflow
#define MAX 200

#define PRINT 0
#define IF_COUT \
    if (PRINT)  \
    cout
// 字符集
const string char_set = "abcdefghijklmnopqrstuvwxyzE)*+?";


/*
 * 用途： 接受一个字符串形式的正则表达式，不在字符集的字符会被视为空
 * 返回： 没有返回，状态在一些public属性里面
 * 状态属性： min_dfa参数： least: 状态数
 *						   min_start: 开始状态
 *							final： 接受状态集合
 *							MIN[][]：跳转表
 * 调用方法： re2dfa example;
 *			example.run(test_re);
 */
class re2dfa {
public:
    // nfa/dfa 状态数
    int nfa_state_num,dfa_state_num;

    // 起始状态
    int start;

	// DFA 的接收态
	vector<int> accept;
	// DFA 状态转换表
	int DFA[MAX][27];

    // 关于min_dfa的参数
    int least;
    int min_start;
    // 终结符集
    vector<int> final;
    int MIN[MAX][27];
private:
    // 定义操作栈
	stack<char> op;
	// 定义 NFA 栈
	stack<pair<int, int>> nfa;
	// 正则表达式到 NFA 的转换表
	vector<int> NFA[MAX][27];
	// NFA 到 DFA 的转换表
	map<vector<int>, int> N2Dtran;
private:
    /* 
     * 关于构造NFA的函数
     */
    // 为字母 a 到 z 和 E 构造NFA
	void NFA_letter(int i);

	// 为操作符 | 构造NFA
	void NFA_or();

	// 为操作符 & 构造NFA
	void NFA_cat();

	// 为操作符 * 构造NFA
	void NFA_star();

	// 为操作符 ?, a? = a|e 构造NFA
	void NFA_qestion();

	// 为操作符 '+', aa* (a+) 构造NFA
	void NFA_add();

public:
	// 正则表达式转化成NFA
	void RE2NFA(string s);

    /*
     * NFA to DFA 的函数
     */ 

private:
	// 计算状态T的 e-closure
	vector<int> eClosure(vector<int> T);

	// 计算移动（T，a）
	vector<int> move(vector<int> T, int a);
public:
	// 将 NFA 转换为 DFA
	void NFA2DFA();

    /*
     * 最小化 DFA 
     */ 
    // 重置最小 DFA，将状态分为两组
private:
	// 定义集合变换表
	int groups[MAX][27];

	// 判断状态方向是否确定
	bool included[MAX];
    // DFA 和 最小化 DFA 间的 map 数
	int MAP[MAX];
	// 定义新旧的集合
    vector<vector<int>> old_set, new_set;
	// 最小化 DFA 的转化表

private:
	void init_min();

	// 划分旧集合并构建一个新集合
	void grouping(int n);
public:
	// 将DFA转换为最小DFA
	void DFA2MIN();

    // 正确的话返回0.  函数作用,接受一个字符串, 计算出最小dfa
	int run(string s);
};


/*
 * check2： 判断两个 正则表达式的关系
 * 调用方法: check2 example; ans = example.check(re_string1,re_string2);

 */
class check2{
public:
	// 计算DFA的互补集
	vector<int> reverse(int num_state, vector<int> f)
	{
		vector<int> ans;
		for (int i = 0; i < num_state; i++)
		{
			if (find(f.begin(), f.end(), i) == f.end())
			{
				ans.push_back(i);
			}
		}
		return ans;
	}

	// 判断两个 DFA 是否都被接受
	bool if_end(int x1, int x2, vector<int> f1, vector<int> f2)
	{
		if (find(f1.begin(), f1.end(), x1) != f1.end() && find(f2.begin(), f2.end(), x2) != f2.end())
			return true;
		return false;
	}

	// 判断两个 DFA 是否有交集
	bool if_intersect(int A[][27], int B[][27], int x1, int x2, vector<int> f1, vector<int> f2, int visit[][MAX])
	{
		visit[x1][x2] = true;
		if (if_end(x1, x2, f1, f2))
			return true;
		for (int i = 0; i < 26; i++)
		{
			int next_a = A[x1][i];
			int next_b = B[x2][i];
			if (!visit[next_a][next_b])
			{
				if (if_intersect(A, B, next_a, next_b, f1, f2, visit))
					return true;
				visit[next_a][next_b] = false;
			}
		}
		return false;
	}
    
    int check(const string &sr,const string &ss) {
        re2dfa r,s;
        r.run(sr);
        s.run(ss);

        int vis[MAX][MAX];
        vector<int> f1 = r.final;
        vector<int> f2 = s.final;
        // r_min_DFA的互补集
		vector<int> f3 = reverse(r.least, f1);

		// s_min_DFA的互补集
		vector<int> f4 = reverse(s.least, f2);
        memset(vis, 0, sizeof(vis));
        bool ans1= if_intersect(r.MIN, s.MIN, r.min_start, s.min_start, f3, f2, vis);
        memset(vis, 0, sizeof(vis));
        bool ans2= if_intersect(r.MIN, s.MIN, r.min_start, s.min_start, f1, f4, vis);

        /* ! return 0
         * < return 1
         * > return 2
         * = return 3
         */
        if (ans1&&ans2) {
            return 0;
        }
        else if (ans1) {
            return 1;
        }
        else if (ans2) {
            return 2;
        }
        else {
            return 3;
        }
        return -1;
    }
};


/*
样例：
10
((E|a)b*)* (a|b)*
b*a*b?a* b*a*ba*|b*a*
b*a*b?a* (b*|a*)(b|E)a*
(c|d)*c(c|d)(c|d) (c|d)*d(c|d)(c|d)
x+y+z+ x*y*z*
((E|a)b*c)+ abc*
((E|a)b*c)+ abc+
(cd|abd)+abb cdabdabb
((a|E)c)*cdd* (ac)*cd+
ac*cdd* ac*cd+


*/
int main() {
    int n;
    const char out[]="!<>=";
    cin >> n;
    while (n--) {
        string r,s;
        cin >> r>>s;
        check2 c;
        int ans=c.check(r,s);
        if (ans<0) {
            cout<<"error"<<endl;
            break;
        }
        cout <<out[ans] <<endl;
    }
}

//////////////////// 下面是函数的实现

/*
* 关于NFA的函数
*/
// 为字母 a 到 z 和 E 构造NFA

inline void re2dfa::NFA_letter(int i)
{
	pair<int, int> r;
	r.first = nfa_state_num++;
	r.second = nfa_state_num++;
	NFA[r.first][i].push_back(r.second);
	nfa.push(r);
}

// 为操作符 | 构造NFA

inline void re2dfa::NFA_or()
{
	op.pop();
	pair<int, int> r;
	r.first = nfa_state_num++;
	r.second = nfa_state_num++;
	// 获得前两个 nfa 栈顶元素
	pair<int, int> s = nfa.top();
	nfa.pop();
	pair<int, int> t = nfa.top();
	nfa.pop();

	//    / e —— b —— e \
	//   A                   B
	//    \ e —— a —— e /

// 分支
	NFA[r.first][26].push_back(s.first);
	NFA[r.first][26].push_back(t.first);
	// 合并
	NFA[s.second][26].push_back(r.second);
	NFA[t.second][26].push_back(r.second);
	nfa.push(r);
}

// 为操作符 & 构造NFA

inline void re2dfa::NFA_cat()
{
	op.pop();
	// 获取前两个 nfa 栈顶元素
	pair<int, int> t = nfa.top();
	nfa.pop();
	pair<int, int> s = nfa.top();
	nfa.pop();
	pair<int, int> r(s.first, t.second);

	// 链起来
	NFA[s.second][26].push_back(t.first);
	nfa.push(r);
}

// 为操作符 * 构造NFA

inline void re2dfa::NFA_star()
{
	// 构建新状态
	pair<int, int> r;
	r.first = nfa_state_num++;
	r.second = nfa_state_num++;
	// 旧状态
	pair<int, int> s = nfa.top();
	nfa.pop();
	// 来到旧状态的起始位置
	NFA[r.first][26].push_back(s.first);
	// 来到旧状态的结束位置
	NFA[r.first][26].push_back(r.second);
	// 来到旧状态的起始位置
	NFA[s.second][26].push_back(s.first);
	// 来到旧状态的结束位置
	NFA[s.second][26].push_back(r.second);
	nfa.push(r);
}

// 为操作符 ?, a? = a|e 构造NFA

inline void re2dfa::NFA_qestion()
{
	// 新状态
	pair<int, int> r;
	r.first = nfa_state_num++;
	r.second = nfa_state_num++;
	pair<int, int> s = nfa.top();
	nfa.pop();
	// 分支
	NFA[r.first][26].push_back(s.first);
	NFA[r.first][26].push_back(r.second);
	// 合并
	NFA[s.second][26].push_back(r.second);
	nfa.push(r);
}

// 为操作符 '+', aa* (a+) 构造NFA

inline void re2dfa::NFA_add()
{
	// 新状态
	pair<int, int> r;
	r.first = nfa_state_num++;
	r.second = nfa_state_num++;
	// 旧状态
	pair<int, int> s = nfa.top();
	nfa.pop();
	// 至少发生一次操作
	NFA[r.first][26].push_back(s.first);
	// 来到旧状态的起始位置
	NFA[s.second][26].push_back(s.first);
	// 来到旧状态的结束位置
	NFA[s.second][26].push_back(r.second);
	nfa.push(r);
}



// 正则表达式转化成NFA

inline void re2dfa::RE2NFA(string s)
{
	nfa_state_num = 0;
	for (int i = 0; i < s.size(); i++)
	{
		// 如果字符是字母，则构造新的子NFA
		if (islower(s[i]))
		{
			// 如果堆栈的前两个子NFA需要连接
			if (i != 0 && char_set.find(s[i - 1]) != string::npos)
			{
				if (!op.empty() && op.top() == '&')
					NFA_cat();
				op.push('&');
			}
			if (s[i] != 'E')
				NFA_letter(s[i] - 'a');
			else
				NFA_letter(26);
		}
		// 如果字符是“|”，清除其前面的所有“|”和“&;”，直到出现“（”
		else if (s[i] == '|')
		{
			while (!op.empty() && op.top() != '(')
			{
				if (op.top() == '|')
					NFA_or();
				else if (op.top() == '&')
					NFA_cat();
			}
			op.push('|');
		}
		else if (s[i] == '*')
			NFA_star();
		else if (s[i] == '?')
			NFA_qestion();
		else if (s[i] == '+')
			NFA_add();
		// 如果字符是“（”，这意味着它后面的表达式是一个整体的
		else if (s[i] == '(')
		{
			// 如果这两种状态需要联系起来
			if (i != 0 && char_set.find(s[i - 1]) != string::npos)
			{
				if (!op.empty() && op.top() == '&')
					NFA_cat();
				op.push('&');
			}
			op.push('(');
		}
		// 如果字符是“|”，清除其前面的所有“|”和“&;”，直到出现“（”
		else if (s[i] == ')')
		{
			while (op.top() != '(')
			{
				if (op.top() == '|')
					NFA_or();
				else if (op.top() == '&')
					NFA_cat();
			}
			op.pop();
		}
	}
	// 如果运算符堆栈不为空，将其清除
	while (!op.empty())
	{
		if (op.top() == '|')
			NFA_or();
		else if (op.top() == '&')
			NFA_cat();
	}
}

// 计算状态T的 e-closure

inline vector<int> re2dfa::eClosure(vector<int> T)
{
	stack<int> s;
	vector<int>::iterator it;
	for (it = T.begin(); it != T.end(); it++)
	{
		s.push(*it);
	}
	while (!s.empty())
	{
		int t = s.top();
		s.pop();
		for (it = NFA[t][26].begin(); it != NFA[t][26].end(); it++)
		{
			if (find(T.begin(), T.end(), *it) == T.end())
			{
				T.push_back(*it);
				s.push(*it);
			}
		}
	}
	sort(T.begin(), T.end());
	return T;
}

// 计算移动（T，a）

inline vector<int> re2dfa::move(vector<int> T, int a)
{
	vector<int>::iterator it1, it2;
	vector<int> next;
	for (it1 = T.begin(); it1 != T.end(); it1++)
	{
		for (it2 = NFA[*it1][a].begin(); it2 != NFA[*it1][a].end(); it2++)
		{
			if (find(next.begin(), next.end(), *it2) == next.end())
			{
				next.push_back(*it2);
			}
		}
	}
	return next;
}

// 将 NFA 转换为 DFA

inline void re2dfa::NFA2DFA()
{
	dfa_state_num = 0;
	// 初始化状态
	pair<int, int> begin = nfa.top();
	vector<int> T;
	T.push_back(begin.first);
	vector<int> U = eClosure(T);
	vector<vector<int>> Dstates;
	// 无标记状态
	stack<vector<int>> s;
	Dstates.push_back(U);
	s.push(U);
	N2Dtran[U] = dfa_state_num++;
	if (find(U.begin(), U.end(), begin.second) != U.end())
		accept.push_back(dfa_state_num - 1);
	while (!s.empty())
	{
		T = s.top();
		s.pop();
		for (int a = 0; a < 26; a++)
		{
			U = eClosure(move(T, a));
			// U 不在 D 状态中
			if (find(Dstates.begin(), Dstates.end(), U) == Dstates.end())
			{
				Dstates.push_back(U);
				s.push(U);
				N2Dtran[U] = dfa_state_num++;
				if (find(U.begin(), U.end(), begin.second) != U.end())
					accept.push_back(dfa_state_num - 1);
			}
			DFA[N2Dtran[T]][a] = N2Dtran[U];
		}
	}
}

inline void re2dfa::init_min()
{
	// v1:不接受状态组，v2:接受状态组
	vector<int> v1, v2;
	for (int i = 0; i < dfa_state_num; i++)
	{
		if (find(accept.begin(), accept.end(), i) == accept.end()) {
			v1.push_back(i);
		}
		else {
			v2.push_back(i);
		}
	}
	new_set.push_back(v1);
	new_set.push_back(v2);
}

// 划分旧集合并构建一个新集合

inline void re2dfa::grouping(int n)
{
	int t;
	vector<int> v;
	// 更新组转换表
	for (int i = 0; i < old_set[n].size(); i++)
	{
		for (int j = 0; j < 26; j++)
		{
			t = DFA[old_set[n][i]][j];
			for (int k = 0; k < old_set.size(); k++)
			{
				// 如果找到，则更新组转换表
				if (find(old_set[k].begin(), old_set[k].end(), t) != old_set[k].end())
				{
					groups[old_set[n][i]][j] = k;
					break;
				}
			}
		}
	}
	// 为每个分类再分集合
	for (int i = 0; i < old_set[n].size(); i++)
	{
		if (!included[old_set[n][i]])
		{
			v.clear();
			v.push_back(old_set[n][i]);
			included[old_set[n][i]] = true;
			for (int j = i + 1; j < old_set[n].size(); j++)
			{
				if (!included[old_set[n][j]])
				{
					int k;
					for (k = 0; k < 26; k++)
					{
						if (groups[old_set[n][i]][k] != groups[old_set[n][j]][k])
							break;
					}
					// 如果对于所有字符，两个状态始终具有同一个下一状态，则合并它们
					if (k == 26)
					{
						v.push_back(old_set[n][j]);
						included[old_set[n][j]] = true;
					}
				}
			}
			new_set.push_back(v);
		}
	}
}

// 将DFA转换为最小DFA

inline void re2dfa::DFA2MIN()
{
	init_min();
	memset(groups, 0, sizeof(groups));
	// 将集合分成一个较小的集合
	while (old_set.size() != new_set.size())
	{
		old_set = new_set;
		new_set.clear();
		memset(included, false, sizeof(included));
		for (int i = 0; i < old_set.size(); i++)
			grouping(i);
	}
	// 找到开始状态并构建映射
	least = 0;
	for (int i = 0; i < old_set.size(); i++)
	{
		for (int j = 0; j < old_set[i].size(); j++)
		{
			MAP[old_set[i][j]] = least;
			if (old_set[i][j] == 0)
				min_start = least;
		}
		least++;
	}
	// 找到最终状态
	for (int i = 0; i < accept.size(); i++)
	{
		if (find(final.begin(), final.end(), MAP[accept[i]]) == final.end())
		{
			final.push_back(MAP[accept[i]]);
		}
	}
	sort(final.begin(), final.end());
	// 构造 DFA 和 最小化 DFA 之间的映射
	memset(included, false, sizeof(included));
	for (int i = 0; i < dfa_state_num; i++)
	{
		if (!included[MAP[i]])
		{
			included[MAP[i]] = true;
			for (int j = 0; j < 26; j++)
				MIN[MAP[i]][j] = MAP[DFA[i][j]];
		}
	}
	IF_COUT << "## old:" << dfa_state_num << " new least: " << least << endl;
	//dfa_state_num = L;
}

// 正确的话返回0.  函数作用,接受一个字符串, 计算出最小dfa

inline int re2dfa::run(string s) {
	this->RE2NFA(s);
	IF_COUT << "NFA good\n";
	this->NFA2DFA();
	IF_COUT << "DFA good\n";
	this->DFA2MIN();
	IF_COUT << " MIN good\n";
	// 假定返回的都是正确的,因为前面也没有做错误判断
	return 0;
}
