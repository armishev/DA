#include <iostream>
#include <vector>
#include <stack>
#include <queue>
using namespace std;


using item=pair<long long,int>;
const long long INF = 1e18;

struct wedge{
    int u,v;
    long long w;
};
using wgraph = vector<vector<wedge> >;

vector<long long> dks(int start, const wgraph & g){
    int n = g.size();
    vector<long long> d(n,INF);
    vector<bool> visited(n);
    priority_queue<item> pq;
    d[start]=0;
    pq.push({-d[start],start});
    while(!pq.empty()){
        item cur = pq.top();
        pq.pop();
        int u = cur.second;
        if(visited[u]){
            continue;
        }
        visited[u]=true;
        for(wedge elem:g[u]){
            int v = elem.v;
            long long w =elem.w;
            if(d[u] + w <d[v]){
                d[v]=d[u]+w;
                pq.push({-d[v],v});
            }
        }
    }
    return d;
}

int main () {
    ios::sync_with_stdio(false);
    cin.tie(0);

    int n,m,start,finish;
    cin>>n>>m>>start>>finish;
    finish--;
    start--;
    wgraph g(n);
    for(int i=0; i<m; i++){
        int u,v;
        long long w;
        cin>>u>>v>>w;
        --u;
        --v;
        g[u].push_back({u,v,w});
        g[v].push_back({v,u,w});
    }
    vector<long long> d= dks(start,g);

    if(d[finish]!=0 && d[finish]!=INF)
        cout << d[finish];
    else
        cout << "No solution";

}
