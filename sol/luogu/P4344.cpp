#include <bits/stdc++.h>
using namespace std;

using i64 = long long;
using u64 = unsigned long long;
using u32 = unsigned;
using u128 = unsigned __int128;

template <class Info, class Tag>
struct LazySegmentTree {
    int n;
    std::vector<Info> info;
    std::vector<Tag> tag;

    LazySegmentTree() = delete;
    LazySegmentTree(int n_, const Info &v_ = {}) { init(std::vector<Info>(n_, v_)); }
    LazySegmentTree(const std::vector<Info> &data) { init(data); }

    void init(const std::vector<Info> &data) {
        n = data.size();
        info.assign(4 << std::__lg(n), {});
        tag.assign(4 << std::__lg(n), {});

        auto build = [&](auto self, int p, int l, int r) -> void {
            if (r - l == 1) {
                info[p] = data[l];
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
        tag[p] = {};
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
            return {};
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
    int fill(int p, int l, int r, int x, int y, int rest) {
        if (l >= y or r <= x) {
            return rest;
        }
        if (l >= x and r <= y and info[p].act - info[p].sum <= rest) {
            int ori = info[p].act - info[p].sum;
            apply(p, Tag{2});
            return rest - ori;
        }
        int m = (l + r) / 2;
        push(p);
        rest = fill(2 * p, l, m, x, y, rest);
        if (rest) {
            rest = fill(2 * p + 1, m, r, x, y, rest);
        }
        pull(p);
        return rest;
    }
    int fill(int l, int r, int x) {
        return fill(1, 0, n, l, r, x);
    }
};

constexpr i64 inf = 1e18;

struct Tag {
    int type = 0;
    void apply(const Tag &t) {
        if (t.type == 0) {
            return;
        }
        type = t.type;
    }
};

struct Info {
    int act = 0;
    int sum = 0;
    int ans = 0;
    int l = 0;
    int r = 0;

    Info() {}
    Info(bool x) : act(1), sum(x), ans(1 - x), l(1 - x), r(1 - x) {}

    void apply(const Tag &t) {
        if (t.type == 1) {
            ans = l = r = act;
            sum = 0;
        } else if (t.type == 2) {
            sum = act;
            ans = l = r = 0;
        }
    }
};

Info operator+(const Info &a, const Info &b) {
    Info c;
    c.act = a.act + b.act;
    c.sum = a.sum + b.sum;

    if (a.l == a.act) {
        c.l = a.l + b.l;
    } else {
        c.l = a.l;
    }
    if (b.r == b.act) {
        c.r = a.r + b.r;
    } else {
        c.r = b.r;
    }

    c.ans = max({a.ans, a.r + b.l, b.ans});
    return c;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n, m;
    cin >> n >> m;

    LazySegmentTree<Info, Tag> seg(n, Info(1));

    while (m--) {
        int o, l, r;
        cin >> o >> l >> r;
        l--;

        if (o == 0) {
            seg.rangeApply(l, r, Tag{1});
        } else if (o == 1) {
            int s = seg.rangeQuery(l, r).sum;
            seg.rangeApply(l, r, Tag{1});

            int x, y;
            cin >> x >> y;
            x--;

            seg.fill(x, y, s);
        } else {
            cout << seg.rangeQuery(l, r).ans << "\n";
        }
    }

    return 0;
}