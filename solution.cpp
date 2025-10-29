#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <cstdlib>

using namespace std;

const int MAXN = 300005;
const int LOGN = 20;
const long long NEGATIVE_INF = -1e18;

int N, M;
vector<pair<int, long long>> adj[MAXN];  // adjacency list: (neighbor, weight)
int euler[2 * MAXN];  // Euler tour
int first_occurrence[MAXN];  // first occurrence in Euler tour
int depth_at_euler[2 * MAXN];  // depth at each position in Euler tour
long long dist_from_root[MAXN];  // distance from root
int euler_size = 0;

// Sparse table for RMQ on Euler tour (for LCA)
int sparse_table[2 * MAXN][LOGN];
int log_table[2 * MAXN];

// Forward declarations
int lca(int u, int v);
long long get_distance(int u, int v);

// Segment tree for maintaining diameter endpoints
struct SegmentTree {
    struct Node {
        int a, b;  // diameter endpoints (-1 if empty)
    };
    
    vector<Node> tree;
    int size;
    
    void init(int n) {
        size = 1;
        while (size < n) size *= 2;
        tree.assign(2 * size, {-1, -1});
    }
    
    long long get_dist(int u, int v) {
        if (u == -1 || v == -1) return 0;
        int l = lca(u, v);
        return dist_from_root[u] + dist_from_root[v] - 2 * dist_from_root[l];
    }
    
    Node merge(const Node& left, const Node& right) {
        vector<int> candidates;
        if (left.a != -1) candidates.push_back(left.a);
        if (left.b != -1) candidates.push_back(left.b);
        if (right.a != -1) candidates.push_back(right.a);
        if (right.b != -1) candidates.push_back(right.b);
        
        if (candidates.empty()) return {-1, -1};
        if (candidates.size() == 1) return {candidates[0], candidates[0]};
        
        int best_a = -1, best_b = -1;
        long long max_dist = NEGATIVE_INF;
        
        for (size_t i = 0; i < candidates.size(); i++) {
            for (size_t j = i + 1; j < candidates.size(); j++) {
                long long d = get_dist(candidates[i], candidates[j]);
                if (d > max_dist) {
                    max_dist = d;
                    best_a = candidates[i];
                    best_b = candidates[j];
                }
            }
        }
        
        return {best_a, best_b};
    }
    
    void set(int pos, int val, int x, int lx, int rx) {
        if (rx - lx == 1) {
            if (val == -1) {
                tree[x] = {-1, -1};
            } else {
                tree[x] = {val, val};
            }
            return;
        }
        int mid = (lx + rx) / 2;
        if (pos < mid) {
            set(pos, val, 2 * x + 1, lx, mid);
        } else {
            set(pos, val, 2 * x + 2, mid, rx);
        }
        tree[x] = merge(tree[2 * x + 1], tree[2 * x + 2]);
    }
    
    void set(int pos, int val) {
        set(pos, val, 0, 0, size);
    }
    
    Node query(int l, int r, int x, int lx, int rx) {
        if (lx >= r || rx <= l) return {-1, -1};
        if (lx >= l && rx <= r) return tree[x];
        int mid = (lx + rx) / 2;
        return merge(query(l, r, 2 * x + 1, lx, mid), 
                    query(l, r, 2 * x + 2, mid, rx));
    }
    
    Node query(int l, int r) {
        return query(l, r + 1, 0, 0, size);
    }
} seg_tree;

void dfs_euler(int u, int parent, int d, long long dist) {
    first_occurrence[u] = euler_size;
    euler[euler_size] = u;
    depth_at_euler[euler_size] = d;
    dist_from_root[u] = dist;
    euler_size++;
    
    for (auto [v, w] : adj[u]) {
        if (v == parent) continue;
        dfs_euler(v, u, d + 1, dist + w);
        euler[euler_size] = u;
        depth_at_euler[euler_size] = d;
        euler_size++;
    }
}

void build_sparse_table() {
    // Initialize log table (only up to euler_size)
    log_table[1] = 0;
    for (int i = 2; i <= euler_size; i++) {
        log_table[i] = log_table[i / 2] + 1;
    }
    
    // Build sparse table for RMQ on depth
    for (int i = 0; i < euler_size; i++) {
        sparse_table[i][0] = i;
    }
    
    for (int j = 1; j < LOGN; j++) {
        for (int i = 0; i + (1 << j) <= euler_size; i++) {
            int left = sparse_table[i][j - 1];
            int right = sparse_table[i + (1 << (j - 1))][j - 1];
            if (depth_at_euler[left] < depth_at_euler[right]) {
                sparse_table[i][j] = left;
            } else {
                sparse_table[i][j] = right;
            }
        }
    }
}

int lca(int u, int v) {
    int l = first_occurrence[u];
    int r = first_occurrence[v];
    if (l > r) swap(l, r);
    
    int len = r - l + 1;
    int k = log_table[len];
    
    int left_min = sparse_table[l][k];
    int right_min = sparse_table[r - (1 << k) + 1][k];
    
    if (depth_at_euler[left_min] < depth_at_euler[right_min]) {
        return euler[left_min];
    } else {
        return euler[right_min];
    }
}

long long get_distance(int u, int v) {
    int l = lca(u, v);
    return dist_from_root[u] + dist_from_root[v] - 2 * dist_from_root[l];
}

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);
    
    cin >> N;
    
    for (int i = 0; i < N - 1; i++) {
        int u, v;
        long long w;
        cin >> u >> v >> w;
        adj[u].push_back({v, w});
        adj[v].push_back({u, w});
    }
    
    // Build Euler tour and distance arrays
    dfs_euler(1, -1, 0, 0);
    build_sparse_table();
    
    cin >> M;
    
    vector<int> A;  // dynamic list
    seg_tree.init(M);
    long long last_ans = 0;
    
    for (int q = 0; q < M; q++) {
        int type;
        cin >> type;
        
        if (type == 1) {
            int x;
            cin >> x;
            // XOR always produces non-negative result, so modulo is safe
            x = (x ^ abs(last_ans)) % N + 1;
            
            A.push_back(x);
            seg_tree.set(A.size() - 1, x);
            
        } else if (type == 2) {
            if (!A.empty()) {
                seg_tree.set(A.size() - 1, -1);
                A.pop_back();
            }
            
        } else if (type == 3) {
            int l, r, x;
            cin >> l >> r >> x;
            
            if (A.empty()) {
                cout << 0 << "\n";
                last_ans = 0;
                continue;
            }
            
            x = (x ^ abs(last_ans)) % N + 1;
            l = (l ^ abs(last_ans)) % A.size() + 1;
            r = (r ^ abs(last_ans)) % A.size() + 1;
            
            if (l > r) swap(l, r);
            
            // Convert to 0-indexed
            l--;
            r--;
            
            auto node = seg_tree.query(l, r);
            
            long long ans = 0;
            if (node.a != -1) {
                ans = max(ans, get_distance(x, node.a));
            }
            if (node.b != -1) {
                ans = max(ans, get_distance(x, node.b));
            }
            
            cout << ans << "\n";
            last_ans = ans;
        }
    }
    
    return 0;
}
