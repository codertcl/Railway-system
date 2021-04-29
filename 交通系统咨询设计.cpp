#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <queue>
#include <unordered_map>
#include <algorithm>
using namespace std;

typedef long long ll;
typedef pair<int, int> p;
//pair将一对值组合成一个值，这一对值可以具有不同的数据类型（T1和T2）
//两个值可以分别用pair的两个公有函数first和second访问
const int inf = 0x3f3f3f3f;
const int maxn = 50;
const int maxm = 5e3;
bool initdone, floyd_done;//第一个表示是否已经初始化
bool vis[maxn], adj[maxn][maxn];//后者存储邻接矩阵
int ecnt, head[maxn], dis[maxn], pre[maxn], sta[maxn], g[maxn][maxn];
//定义边结构体
struct edge
{
	int v, w, nxt; 
} e[maxm];
vector<vector<int>> res;
unordered_map<string, int> id;
string city[maxn];//存储城市名称
ifstream in_adj("adjacency.txt");//读取文件中邻接矩阵信息
ifstream in_dis("distance.txt");
ofstream out("output.txt");

//将边信息添加进结构体类型数组
void addEdge(int u, int v, int w)
{
	//ecnt初值为0
	e[ecnt].v = v;//储存终点城市的信息
	e[ecnt].w = w;//表示距离信息
	e[ecnt].nxt = head[u];//nxt表示下标为ecnt的边 下一条边的对应的编号
	head[u] = ecnt++;//head[u]是下标为u的城市所 具有的边的编号  ecnt是边的编号，每条边被记录两次
}

//初始化读入城市信息
void init()
{
	if (initdone)
	{
		cout << "无需重复初始化" << endl << endl;
		return;
	}
	memset(head, -1, sizeof(head));//将head数组全部初始化为-1
	memset(g, 0x3f, sizeof(g));
	for (int i = 1; i <= 30; i++)
	{
		in_adj >> city[i];//city数组储存城市名称信息
		id[city[i]] = i;//存储城市对应的下标
	}
	for (int i = 1; i <= 30; i++)
	{
		string city_now;
		in_adj >> city_now;//读取城市名称
		int id_now = id[city_now]; //存储城市对应的下标
		for (int j = 1; j <= 30; j++)
		{
			in_adj >> adj[id_now][j];//j为城市对应的行号，j为列号，读取相应城市的邻接矩阵信息
			adj[j][id_now] = adj[id_now][j];//无向图 邻接矩阵对称位置 值相同
		}
	}

	//读取距离信息
	string tmp;
	for (int i = 1; i <= 30; i++)
		in_dis >> tmp;//读取城市名称信息
	for (int i = 1; i <= 30; i++)
	{
		string city_now;
		in_dis >> city_now;//存储城市名称信息
		int id_now = id[city_now];//找到该城市对应的行号
		for (int j = 1; j <= i - 1; j++)
		{
			int w;//表示距离
			in_dis >> w;
			if (!adj[id_now][j])//如果二者不邻接，则跳过距离信息赋值和添加边的过程
				continue;
			g[id_now][j] = g[j][id_now] = w;
			addEdge(id_now, j, w);//j和id_now分别是邻接的城市对应的下标
			addEdge(j, id_now, w);
		}
	}
	cout << "初始化成功" << endl << endl;
	initdone = true;
}


//返回省会城市到武汉中转次数
int bfs(int src, int des)//des为武汉对应的下标
{
	memset(vis, false, sizeof(vis));//初始化标记数组
	queue<p> q;
	vis[src] = true;
	q.push(p(src, 0));//城市编号和0入队
	while (!q.empty())
	{
		int u = q.front().first, s = q.front().second;//u为城市编号 从0开始
		if (u == des)
			return s - 1;
		q.pop();
		for (int i = head[u]; ~i; i = e[i].nxt)//head[u]是下标为u的城市所具有的边的编号
		{//i=-1时终止for循环
			int v = e[i].v;
			if (!vis[v])//如果这条未访问过
			{
				vis[v] = true;
				q.push(p(v, s + 1)); 
			}
		}
	}
	return inf;
}


void print_wuhan()
{
	for (int i = 1; i <= 30; i++)
		if (city[i] != "武汉")
			cout << city[i] << "到武汉至少中转 " << bfs(i, id["武汉"]) << " 次" << endl << endl;
}


void dfs(int now, int des, int step, int sum, vector<int> res_now)
{
	if (step > 10) return;//超过10次
	vis[now] = true;
	res_now.push_back(now);
	if (now == des)
	{
		res_now.push_back(sum);
		res.push_back(res_now);
		return;
	}
	for (int i = head[now]; ~i; i = e[i].nxt)
	{
		int nxt = e[i].v;
		if (vis[nxt]) 
			continue;
		dfs(nxt, des, step + 1, sum + e[i].w, res_now);
		vis[nxt] = false;
	}
}


bool cmp(const vector<int>& va, const vector<int>& vb)
{
	return va[va.size() - 1] < vb[vb.size() - 1];
}


void getPath()
{
	res.clear();//清空vector
	memset(vis, false, sizeof(vis));//重置vis为false
	cout << "请分别输入起点名称和终点名称: " << endl;
	string city_src, city_des;
	cin >> city_src >> city_des;
	if (id.find(city_src) == id.end() || id.find(city_des) == id.end())//未找到起点或者重点
	{//find 返回迭代器指向当前查找元素的位置否则返回map::end()位置		
		cout << "地点不存在" << endl << endl;
		return;
	}
	int id_src = id[city_src], id_des = id[city_des];
	dfs(id_src, id_des, 0, 0, {});
	sort(res.begin(), res.end(), cmp);
	int cnt = 0;
	for (auto vec : res)
	{
		out << "第 " << ++cnt << " 条路径" << endl;
		for (int i = 0; i < vec.size() - 2; i++)
			out << "从 " << city[vec[i]] << " 到 " << city[vec[i + 1]] << endl;
		out << "总路程为 " << vec[vec.size() - 1] << endl << endl;
	}
	cout << "所有 10 步以内的路径已输出到文件" << endl << endl;
}


void dijkstra(bool flag)
{
	memset(dis, 0x3f, sizeof(dis));
	memset(vis, false, sizeof(vis));
	cout << "请分别输入起点名称和终点名称: ";
	string city_src, city_des;
	cin >> city_src >> city_des;
	if (id.find(city_src) == id.end() || id.find(city_des) == id.end())
	{
		cout << "地点不存在" << endl << endl;
		return;
	}
	int id_site = 0;
	if (flag)
	{
		cout << "请输入要跳过的城市名: ";
		string city_site;
		cin >> city_site;
		if (id.find(city_site) == id.end())
		{
			cout << "地点不存在" << endl << endl;
			return;
		}
		id_site = id[city_site];
	}
	//优先级队列具有最高级先出
	priority_queue<p, vector<p>, greater<p> > q;
	int id_src = id[city_src], id_des = id[city_des];//获取起点和终点城市编号
	q.push(p(dis[id_src] = 0, id_src));
	while (!q.empty())
	{
		int u = q.top().second;
		q.pop();
		if (vis[u]) continue;
		vis[u] = true;
		for (int i = head[u]; ~i; i = e[i].nxt)
		{
			int v = e[i].v, w = e[i].w;
			if (v != id_site && dis[v] > dis[u] + w)
			{
				pre[v] = u;
				dis[v] = dis[u] + w;
				q.push(p(dis[v], v));
			}
		}
	}
	cout << endl << city_src << " 到 " << city_des << " 的最短路径如下: " << endl;
	int top = 0;
	for (int i = id_des; i != id_src; i = pre[i])
		sta[top++] = i;
	for (int i = top - 1, last = id_src; i >= 0; i--)
	{
		int u = last, v = sta[i];
		cout << "从 " << city[u] << " 到 " << city[v] << endl;
		last = v;
	}
	cout << "总路程为 " << dis[id_des] << endl << endl;
}


//单源最短路径floyd
void floyd_single(int n)
{
	if (!floyd_done)
		for (int k = 1; k <= n; k++)
			for (int i = 1; i <= n; i++)
				for (int j = 1; j <= n; j++)
					g[i][j] = min(g[i][j], g[i][k] + g[k][j]);
	floyd_done = true;
	cout << "请分别输入起点名称和终点名称: ";
	string city_src, city_des;
	cin >> city_src >> city_des;
	if (id.find(city_src) == id.end() || id.find(city_des) == id.end())
	{
		cout << "地点不存在" << endl << endl;
		return;
	}
	int id_src = id[city_src], id_des = id[city_des];
	cout << endl << city_src << " 到 " << city_des << " 的最短路径长为: " << g[id_src][id_des] << endl << endl;
}


//floyd三重for循环求所有点对之间最短路径
void floyd_all(int n)
{
	if (!floyd_done)
		for (int k = 1; k <= n; k++)
			for (int i = 1; i <= n; i++)
				for (int j = 1; j <= n; j++)
					g[i][j] = min(g[i][j], g[i][k] + g[k][j]);
	floyd_done = true;
	for (int i = 1; i <= n; i++)
		for (int j = i + 1; j <= n; j++)
			cout << city[i] << " 到 " << city[j] << " 的最短路径长为 " << g[i][j] << endl << endl;
	cout << endl;
}


bool chooseFunc()
{
	int choice;
	cin >> choice;
	switch (choice)
	{
	case 1: 
		init();
		break;
	case 2: 
		print_wuhan();
		break;
	case 3:
		getPath();
		break;
	case 4:
		dijkstra(false);
		break;
	case 5:
		dijkstra(true); 
		break;
	case 6: 
		floyd_single(30); 
		break;
	case 7:
		floyd_all(30); 
		break;
	case 8: 
		return false;
	default: 
		cout << "输入有误，请重新输入" << endl;
	}
	return true;
}


void showMenu()
{
	cout << "请输入您需要进行的操作标号" << endl;
	cout << endl;
	cout << "1:初始化交通网络图" << endl << endl;
	cout << "2:获取各省会/直辖市/特区到武汉中转次数" << endl << endl;
	cout << "3:获取指定起点到终点的所有路径" << endl << endl;
	cout << "4:获取单源最短路径 (Dijkstra)" << endl << endl;
	cout << "5:获取绕开指定地点的单源最短路径 (Dijkstra)" << endl << endl;
	cout << "6:获取单源最短路径 (Floyd)" << endl << endl;
	cout << "7:获取多源最短路径" << endl << endl;
	cout << "8:退出程序" << endl << endl;
	cout << endl;
}


int main()
{
	do showMenu();
	while (chooseFunc());
	return 0;
}