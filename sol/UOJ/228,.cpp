// https://uoj.ac/problem/228
#include <bits/stdc++.h>
using namespace std;

using i64 = long long;
using u64 = unsigned long long;
using u32 = unsigned;
using u128 = unsigned __int128;

template<class Info, class Tag>
struct LazySegmentTree {
    int n;
    vector<Info> info;
    vector<Tag> tag;

    LazySegmentTree() : n(0) {}
    LazySegmentTree(int n_, Info v_ = {}) { init(n_, v_); }
    LazySegmentTree(vector<Info> a) { init(a); }

    void init(int n_, Info v_) {
        init(vector<Info>(n_, v_));
    }
    void init(vector<Info> init_) {
        n = init_.size();
        info.assign(4 << __lg(n), {});
        tag.assign(4 << __lg(n), {});

        auto build = [&](auto self, int p, int l, int r) -> void {
            if (r - l == 1) {
                info[p] = init_[l];
                return;
            }
            int m = (l + r) / 2;
            self(self, 2 * p, l, m);
            self(self, 2 * p + 1, m, r);
            pull(p);
        };
        build(build, 1, 0, n);
    }

    void pull(int p) {
        info[p] = info[2 * p] + info[2 * p + 1];
    }
    void apply(int p, const Tag &v) {
        info[p].apply(v);
        tag[p].apply(v);
    }
    void push(int p) {
        apply(2 * p, tag[p]);
        apply(2 * p + 1, tag[p]);
        tag[p] = Tag();
    }
    void modify(int p, int l, int r, int x, const Info &v) {
        if (r - l == 1) {
            info[p] = v;
            return;
        }
        int m = (l + r) / 2;
        push(p);
        if (x < m) {
            modify(2 * p, l, m, x, v);
        } else {
            modify(2 * p + 1, m, r, x, v);
        }
        pull(p);
    }
    void modify(int p, const Info &v) {
        modify(1, 0, n, p, v);
    }
    Info rangeQuery(int p, int l, int r, int x, int y) {
        if (l >= y || r <= x) {
            return Info();
        }
        if (l >= x && r <= y) {
            return info[p];
        }
        int m = (l + r) / 2;
        push(p);
        return rangeQuery(2 * p, l, m, x, y) + rangeQuery(2 * p + 1, m, r, x, y);
    }
    Info rangeQuery(int l, int r) {
        return rangeQuery(1, 0, n, l, r);
    }
    void rangeApply(int p, int l, int r, int x, int y, const Tag &v) {
        if (l >= y || r <= x) {
            return;
        }
        if (l >= x && r <= y) {
            apply(p, v);
            return;
        }
        int m = (l + r) / 2;
        push(p);
        rangeApply(2 * p, l, m, x, y, v);
        rangeApply(2 * p + 1, m, r, x, y, v);
        pull(p);
    }
    void rangeApply(int l, int r, const Tag &v) {
        return rangeApply(1, 0, n, l, r, v);
    }
    void half(int p, int l, int r, int x, int y) {
        if (l >= y || r <= x) {
            return;
        }
        if (l >= x && r <= y) {
            int sqr1 = __builtin_sqrt(info[p].max);
            int sqr2 = __builtin_sqrt(info[p].min);
            if (info[p].max - sqr1 == info[p].min - sqr2) {
                apply(p, {sqr1 - info[p].max});
                return;
            }
        }
        int m = (l + r) / 2;
        push(p);
        half(2 * p, l, m, x, y);
        half(2 * p + 1, m, r, x, y);
        pull(p);
    }
    void half(int l, int r) {
        return half(1, 0, n, l, r);
    }
};

constexpr i64 inf = 1e18;
struct Tag {
    i64 add = 0;
    void apply(const Tag &t) {
        add += t.add;
    }
};
struct Info {
    i64 min = inf;
    i64 max = -inf;
    i64 sum = 0;
    i64 act = 0;

    void apply(const Tag &t) {
        min += t.add;
        max += t.add;
        sum += act * t.add;
    }
};
Info operator+(const Info &a, const Info &b) {
    Info c;
    c.min = std::min(a.min, b.min);
    c.max = std::max(a.max, b.max);
    c.sum = a.sum + b.sum;
    c.act = a.act + b.act;
    return c;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n, m;
    cin >> n >> m;

    vector<int> a(n);
    for (int i = 0; i < n; i++) {
        cin >> a[i];
    }
    vector<Info> init(n);
    for (int i = 0; i < n; i++) {
        init[i].act = 1;
        init[i].max = init[i].min = init[i].sum = a[i];
    }
    LazySegmentTree<Info, Tag> seg(init);

    for (int i = 0; i < m; i++) {
        int o, x, y;
        cin >> o >> x >> y;
        x--;
        if (o == 1) {
            int v;
            cin >> v;
            seg.rangeApply(x, y, {v});
        } else if (o == 2) {
            seg.half(x, y);
        } else {
            cout << seg.rangeQuery(x, y).sum << "\n";
        }
    }

    return 0;
}
