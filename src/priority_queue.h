#pragma once
#include <memory>
#include <algorithm>
#include <random>
#include <iostream>

template <typename T, typename Compare = std::less<T>>
class PriorityQueue {
public:
    PriorityQueue() : root_(nullptr) {
    }
    int Size() const {
        return Treap::GetSize(root_);
    }
    void Push(T new_item) {
        auto [a, b] = Treap::SplitByValue(root_, new_item);
        root_ = Treap::Merge(Treap::Merge(a, std::shared_ptr<Treap>(new Treap(new_item))), b);
    }
    T Top() {
        auto [a, b] = Treap::SplitBySize(root_, 1);
        T res = a->x_;
        root_ = Treap::Merge(a, b);
        return res;
    }
    void Pop() {
        auto [a, b] = Treap::SplitBySize(root_, 1);
        root_ = b;
    }

private:
    static int GetRand() {
        return rand();
    }
    struct Treap {
        std::shared_ptr<Treap> l_;
        std::shared_ptr<Treap> r_;
        T x_;
        int y_;
        int size_;
        explicit Treap(const T& x) : l_(nullptr), r_(nullptr), x_(x), y_(GetRand()), size_(1) {
        }
        static int GetSize(std::shared_ptr<Treap> t) {
            if (!t) {
                return 0;
            }
            return t->size_;
        }
        static void Update(std::shared_ptr<Treap> t) {
            if (!t) {
                return;
            }
            t->size_ = GetSize(t->l_) + 1 + GetSize(t->r_);
        }
        static std::pair<std::shared_ptr<Treap>, std::shared_ptr<Treap>> SplitByValue(std::shared_ptr<Treap> t, T key) {
            if (!t) {
                return {0, 0};
            }
            if (!Compare{}(key, t->x_)) {
                auto [a, b] = SplitByValue(t->r_, key);
                t->r_ = a;
                Update(t);
                return {t, b};
            } else {
                auto [a, b] = SplitByValue(t->l_, key);
                t->l_ = b;
                Update(t);
                return {a, t};
            }
        }
        static std::pair<std::shared_ptr<Treap>, std::shared_ptr<Treap>> SplitBySize(std::shared_ptr<Treap> t, int k) {
            if (!t) {
                return {0, 0};
            }
            if (GetSize(t->l_) < k) {
                auto [a, b] = SplitBySize(t->r_, k - GetSize(t->l_) - 1);
                t->r_ = a;
                Update(t);
                return {t, b};
            } else {
                auto [a, b] = SplitBySize(t->l_, k);
                t->l_ = b;
                Update(t);
                return {a, t};
            }
        }
        static std::shared_ptr<Treap> Merge(std::shared_ptr<Treap> a, std::shared_ptr<Treap> b) {
            if (!a) {
                return b;
            }
            if (!b) {
                return a;
            }
            if (a->y_ <= b->y_) {
                a->r_ = Merge(a->r_, b);
                Update(a);
                return a;
            } else {
                b->l_ = Merge(a, b->l_);
                Update(b);
                return b;
            }
        }
    };
    std::shared_ptr<Treap> root_;
};
