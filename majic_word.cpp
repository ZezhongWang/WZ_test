#include<iostream>
#include<cstdio>
#include<algorithm>
#include<vector>
#include<fstream>
#include<stdlib.h>
#include<cassert>
#include<map>
using namespace std;


#define MAX_SPELL 200
#define MAX_TEST 6
#define MAX_N 32
#define SILENCE false
/*
	假设value都是正值，且和在int范围内
	假设全部可消target*/

// hash查找， str -> value


struct TraceNode{
	int pos;
	string pattern;
	TraceNode(int pos = 0, string pattern = ""){
		this->pos = pos;
		this->pattern = pattern;
	}
	void print(){
		cout<<"Pos = "<<pos<<" Pattern = "<<pattern<<endl;
	}
};

struct NodeInfo{
	int height;
	int value;
	NodeInfo(int height = 0, int value = 0){
		this->height = height;
		this->value = value;
	}
	void print(){
		cout<<"Height = "<<height<<" Value = "<<value<<endl;
	}
};

struct TargetStepAnswer
{
	string callPattern;
	int callPosition;
	int maxValue;
	TargetStepAnswer(const string callPattern = "", 
		 int callPosition = 0, int maxValue = 0){
		this->callPattern = callPattern;
		this->callPosition = callPosition;
		this->maxValue = maxValue;
	}
	void print(){
		cout<<"call pattern "<<callPattern<<" at position "
		<<callPosition<<" Max Value is "<<maxValue<<endl;
	}
};

struct OUT
{
	std::vector<int> pos;
	std::vector<string> str;
	int sum;
	int traceNodeNum;
	OUT(int sum = 0, int traceNodeNum = 0){
		this->sum = sum;
		this->traceNodeNum = traceNodeNum;
	}
	void addTrace(TraceNode traceNode){
		pos.push_back(traceNode.pos);
		str.push_back(traceNode.pattern);
		++traceNodeNum;
	}
};

struct IN
{
	int N, K;
	string S;
	string ms[MAX_SPELL];
	int val[MAX_SPELL];
	void print(){
		cout<<"N = "<<N<<" K = "<<K<<endl;
		cout<<"S = "<<S<<endl;
		for (int i = 0; i < K; ++i)
		{
			cout<<ms[i]<<" "<<val[i]<<endl;
		}
		cout<<endl;
	}
};

std::map<string, int> strMap;
std::map<string, TargetStepAnswer> targetToAnswer;
int dp[MAX_N][MAX_N];	


void test();
IN LoadSingleTestData(ifstream &);
OUT DPMethod(IN& );
void WriteSingleAnswerToFile(OUT &);
void valid_out(OUT , OUT );
void WriteMultiAnswerToFile(std::vector<OUT> , string );
OUT LoadSingleAnswer(int index);
NodeInfo dfsHelp(string& , int , TraceNode *);
OUT DFSMethod(IN &input);

void test(){
	ifstream in("test_in.txt", ios::in);
	ifstream out("test_out.txt", ios::in);
	assert(in.is_open());
	assert(out.is_open());
	int N, K;
	string S;
	int test_num = 0;
	while(in.peek() != EOF && test_num++ < MAX_TEST){
		// load test data
		cout<<"Turn "<<test_num<<endl;
		IN single_test = LoadSingleTestData(in);
		// solve the probWriteAnswerToFile and get result
		// OUT ans = DPMethod(single_test);
		OUT ans = DFSMethod(single_test);
		// write the result to the file
		WriteSingleAnswerToFile(ans);
		// validate result

	}
}

IN LoadSingleTestData(ifstream &in){
	IN temp;
	in>>temp.N>>temp.K;
	in>>temp.S;
	assert( temp.S.length() < MAX_N );
	for (int i = 0; i < temp.K; ++i)
	{
		in>>temp.ms[i];
		in>>temp.val[i];
	}
	return temp;
}

void WriteSingleAnswerToFile(OUT &ans){
	ofstream out("out.txt", ios::app);
	assert(out.is_open());
	std::vector<int> pos = ans.pos;
	std::vector<string> str = ans.str;
	for (int i = 0; i < pos.size(); ++i)
	{
		out<<pos[i]<<" "<<str[i]<<endl;		
	}
	out<<ans.sum<<endl;
}

void WriteMultiAnswerToFile(std::vector<OUT> &ans, string file_name = "out.txt"){
	ofstream out(file_name.c_str());
	assert(out.is_open());
	for (std::vector<OUT>::iterator iter = ans.begin(); iter != ans.end(); ++iter)
	{
		WriteSingleAnswerToFile(*iter);
	}
}

OUT LoadSingleAnswer(int index){
	OUT temp;
	ifstream in("test_out.txt", ios::in);
	int n_num = 0;
	while(index > n_num){
		string buff;
		getline(in, buff);
		if(buff.find(':') != -1){
			++n_num;
		}
	}
	if (index == n_num)
	{
		string buff;
		getline(in, buff);
		while (buff.find(' ') != -1)
		{
			int p = atoi(buff.substr(0, buff.find(' ')).c_str());
			string t = buff.substr(buff.find(' '), buff.length());
			temp.pos.push_back(p);
			temp.str.push_back(t);
		}
		int s = atoi(buff.c_str());
		temp.sum = s;
	}
	return temp;
}

void InitMap(IN &input){
	for (int i = 0; i < MAX_N; ++i)
	{
		for (int j = 0; j < MAX_N; ++j)
		{
			dp[i][j] = 0;
		}
	}
	// strMap.clear();
	// for (int i = 0; i < input.K; ++i)
	// {
	// 	// 可优化，去除包含之类
	// 	strMap.insert(pair<string, int>(input.ms[i], input.val[i]));
	// }
	targetToAnswer.clear();
	for (int patternIndex = 0; patternIndex < input.K; ++patternIndex)
	{
		TargetStepAnswer patternAnswer(input.ms[patternIndex], 0, input.val[patternIndex]);
		// 可去重，或者增多
		// !!!一定要注意去除多余的非最优解，这里假设都是最优的！！！！！
		// assert(dfsHelp(target))
		targetToAnswer.insert(pair<string, TargetStepAnswer>(input.ms[patternIndex], patternAnswer));
	}
}

int getHashValue(std::map<string, int> &m, string tar){
	std::map<string, int>::iterator	 mapIter;
	mapIter = strMap.find(tar);
	bool hasFound =	(mapIter != strMap.end());
	if(hasFound){
		return mapIter->second;
	}
	return -1;
}

string get_str(string s, int begin, int end){
	return s.substr(begin, end - begin + 1);
}

TargetStepAnswer getAnswer(std::map<string, TargetStepAnswer> &Map,  string target){
	std::map<string, TargetStepAnswer>::iterator mapIter;
	mapIter = Map.find(target);
	bool hasFound =	(mapIter != Map.end());
	if(hasFound == false){
		cout<<"Target "<<target<<" not found"<<endl;
	}
	assert(hasFound==true);
	TargetStepAnswer ans = mapIter->second;
	return ans;
}

void printMap(std::map<string, TargetStepAnswer> Map){
	cout<<"********************************"<<endl;
	cout<<"Print Map:"<<endl;
	cout<<"Value"<<'\t'<<"Pattern"<<'\t'
		<<"callPos"<<'\t'<<"callPattern"<<endl;
	for (std::map<string, TargetStepAnswer>::iterator mapIter = Map.begin();
	 		mapIter != Map.end() ; ++mapIter)
	{
		string pattern = mapIter->first;
		TargetStepAnswer ans = mapIter->second;
		cout<<ans.maxValue<<'\t'<<pattern
			<<'\t'<<ans.callPosition<<'\t'
			<<ans.callPattern<<endl;
	}
	cout<<"********************************"<<endl;
}

OUT DFSMethod(IN &input){
	InitMap(input);
	string target = input.S;
	TraceNode traceList[MAX_N];
	int depth = 0;
	NodeInfo rootInfo = dfsHelp(target, depth, traceList);
	if (SILENCE == false)
	{
		printMap(targetToAnswer);
	}
	int maxSum = rootInfo.value;
	int maxHeight = rootInfo.height;
	OUT solution(maxSum, maxHeight);
	// for (int traceIndex = 0; traceIndex < maxHeight; ++traceIndex)
	// {
	// 	// traceList[traceIndex].print();
	// 	solution.addTrace(traceList[traceIndex]);
	// }
	TargetStepAnswer targetAnswer = getAnswer(targetToAnswer, target);
	// while !(target为空 或 target已经没有可以继续匹配的时候){
	// 	target = target 在Answer.position去掉 Answer.pattern
	// 	将Answer.pos 与 Answer.pattern 写入OUT
	// 	Answer = target所对应的最优解
	// }
	while(!targetAnswer.callPattern.empty()){
		string pattern = targetAnswer.callPattern;
		int pos = targetAnswer.callPosition;
		target = target.replace(pos, pattern.length(), "");
		solution.addTrace(TraceNode(pos, pattern));
		if (target.empty())
		{
			break;
		}
		targetAnswer = getAnswer(targetToAnswer, target);
	}
	return solution;
}


int getAnswerValue(std::map<string, TargetStepAnswer> &Map, const string &target){
	std::map<string, TargetStepAnswer>::iterator mapIter;
	mapIter = Map.find(target);
	bool hasFound =	(mapIter != Map.end());
	if(hasFound){
		TargetStepAnswer ans = mapIter->second;
		return ans.maxValue;
	}
	return -1;
}

NodeInfo dfsHelp(string &target, int depth, TraceNode *traceList){
	// for (int pattern_index = 0; pattern_index < K; ++pattern_index)
	if (SILENCE == false)
	{
		cout<<"Visit target = "<<target<<" Depth = "<<depth<<endl;
	}
	if (target.empty())
	{
		return NodeInfo(0, 0);
	}
	int answerValue = getAnswerValue(targetToAnswer, target);
	bool hasAnswerAlready = (answerValue!=-1);
	if ( hasAnswerAlready )
	{
		if (SILENCE==false)
		{
			cout<<"Call target: "<<target<<" with Value "
			<<answerValue<<endl;
		}
		return NodeInfo(0, answerValue);
	}
	// 设置多余的来验证一下代码
	NodeInfo targetInfo(0, 0);
	TargetStepAnswer targetAnswer;

	for (std::map<string, TargetStepAnswer>::iterator iter = targetToAnswer.begin();
			iter != targetToAnswer.end(); ++iter)
	{
		// 	if target string has pattern:			
		// 		depth++;
		// 		int RestTargetValue = dfshelp( target - pattern )
		// 		if RestTargetValue + patternValue > maxTargetValue:
		// 			maxTargetValue = RestTargetValue + patternValue
		// 			trace[depth] = pattern position
		// 		depth--;
		// 	return maxTargetValue;	
		string pattern = iter->first;
		// 有多个pattern怎么办？
		// 待修改
		int patternPos = target.find(pattern);
		bool hasPattern = (patternPos != -1);
		while (hasPattern)
		{
			string restTarget = target;
			restTarget = restTarget.replace(patternPos, pattern.length(), "");
			++depth;
			NodeInfo restTargetInfo = dfsHelp(restTarget, depth, traceList);
			// int patternValue = iter->second;
			TargetStepAnswer patternAnswer = iter->second;
			int patternValue = patternAnswer.maxValue;
			if (restTargetInfo.value + patternValue > targetInfo.value)
			{
				if (SILENCE==false)
				{
					cout<<"Update target: "<<target<<" Value from "<<targetInfo.value
					<<" to "<<restTargetInfo.value + patternValue<<endl;
					cout<<"Call pattern: "<<pattern<<endl;
				}
				targetInfo.value = restTargetInfo.value + patternValue;
				targetInfo.height = restTargetInfo.height + 1;
				// 下面这段最优路径记录方法有问题，之后的搜索会将之前的搜索覆盖掉
				// TraceNode bestStep(patternPos, pattern);
				// traceList[depth-1] = bestStep;
				// 更新最优解
				targetAnswer.callPosition = patternPos;
				targetAnswer.callPattern = pattern;
				targetAnswer.maxValue = targetInfo.value;			
			}
			--depth;
			// 如果还有剩余的pattern，则继续寻找匹配
			patternPos = target.find(pattern, patternPos+1);
			hasPattern = (patternPos != -1);
		}
	}
	targetToAnswer.insert(pair<string, TargetStepAnswer>(target, targetAnswer));
	return targetInfo;
}

OUT DPMethod(IN &input){
	InitMap(input);
	string ss = input.S;
	// 状态转移方程 dp[i][j] = max(dp[l][r] + w[k], dp[i][j])
	// k为长度范围
	for (int k = 0; k < ss.size(); ++k)
	{
		for (int i = 0; i + k < ss.size(); ++i)
		{
			int j = i + k;
			// 查找是否有直接匹配字段
			int val = getHashValue(strMap, get_str(ss, i, j));
			cout<<"ss.substr("<<i<<", "<<j<<") = "<<
				get_str(ss, i, j)
				<<" value = "<<val<<endl;
			if (val != -1)
			{
				dp[i][j] = val;
			}
			for (int l = i; l <= j; ++l)
			{
				for (int r = l; r <= j; ++r)
				{
					string two_side = get_str(ss, i, l-1) + get_str(ss, r+1, j);
					int value = getHashValue(strMap, two_side);
					dp[i][j] = max(dp[i][j], dp[l][r] + value);
				}
			}
		}
	}
	if (SILENCE == false)
	{
		cout<<"dp matrix:"<<endl;
		for (int i = 0; i < ss.size(); ++i)
		{
			for (int j = 0; j < ss.size(); ++j)
			{
				cout<<dp[i][j]<<'\t';
			}
			cout<<endl;
		}
	}
	OUT temp;
	temp.pos.push_back(0);
	temp.str.push_back("AA");
	temp.sum = 10;
	return temp;
}


int main(){
	test();
}