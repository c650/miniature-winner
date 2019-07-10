#include <bits/stdc++.h>

typedef long long ll;

template <typename T>
struct skip_list {
	struct node {
		const T val;
		node *u, *d, *l, *r;

		node(const T& _val) : val(_val), u(nullptr), d(nullptr),
		             l(nullptr), r(nullptr) {}

		node* link(node* u, node* d, node* l, node* r) {
			if (u) {
				u->d = this;
				u->d->u = u;
			}
			if (d) {
				d->u = this;
				d->u->d = d;
			}
			if (l) {
				this->r = l->r;
				l->r = this;
				l->r->l = l;
				if (this->r)
					this->r->l = this;
			}
			if (r) {
				this->l = r->l;
				r->l = this;
				r->l->r = r;
				if (this->l)
					this->l->r = this;
			}

			return this;
		}

		node* unlink(void) {
			if (this->u)
				this->u->d = nullptr;
			if (this->d)
				this->d->u = nullptr;
			if (this->l)
				this->l->r = this->r;
			if (this->r)
				this->r->l = this->l;
			this->u = this->d = this->l = this->r = nullptr;

			return this;
		}
	};

	node *head, *tail;
	ll size, height;
	std::mt19937_64 gen;

	skip_list(void) : size(0), height(1) {
		gen.seed(std::chrono::system_clock::now().time_since_epoch().count());
		head = new node(std::numeric_limits<T>::min());
		tail = new node(std::numeric_limits<T>::max());
		head->link(nullptr, nullptr, nullptr, tail);
	}

	~skip_list(void) {
		node *cur = head, *next_down;
		while (cur) {
			next_down = cur->d;
			delete_row(cur);
			cur = next_down;
		}
	}

	void delete_row(node* cur) {
		for (node* next_right; cur; ) {
			next_right = cur->r;
			delete cur->unlink();
			cur = next_right;
		}
	}

	node* create_row() {
		node* h = new node(head->val);
		node* t = new node(tail->val);
		h->link(nullptr, head, nullptr, t);
		// dont link head twice, cause cycle?
		t->link(nullptr, tail, nullptr, nullptr);
		for (node *cur = head->r, *lst = h; cur != tail; cur = cur->r)
			if (gen() % 2)
				lst = (new node(cur->val))->link(nullptr, cur, lst, nullptr);

		head = h;
		tail = t;
		return h;
	}

	std::vector<node*> get_lb(const T& val) {
		std::vector<node*> trav(height);
		trav[0] = head;
		for (ll i = 0; i < height; ++i) {
			while (trav[i] && trav[i]->r && trav[i]->r->val < val)
				trav[i] = trav[i]->r;
			if (i + 1 < height)
				trav[i+1] = trav[i]->d;
		}
		return trav;
	}

	bool insert(const T& val) {
		if (contains(val))
			return false;

		++size;

		ll new_height = 0;
		while ((1LL << new_height) <= size)
			++new_height;

		for (; height < new_height; ++height)
			create_row();

		auto trav = get_lb(val);

		node* old_ins = nullptr;
		for (ll i = height-1; i >= 0; --i) {
			node* ins = (new node(val))->link(nullptr, old_ins, trav[i], nullptr);
			old_ins = ins;
			if (gen() % 2)
				break;
		}

		return true;
	}

	bool remove(const T& val) {
		if (!contains(val))
			return false;

		auto trav = get_lb(val);
		for (ll i = height-1; i >= 0; --i) {
			if (trav[i] && trav[i]->r && trav[i]->r->val == val) {
				node* tmp = trav[i]->r;
				delete tmp->unlink();
			}
		}

		--size;

		ll new_height = 0;
		while ((1LL << new_height) <= size)
			++new_height;

		for (; height > new_height && height > 1; --height) {
			node *h = head->d, *t =tail->d;
			delete_row(head);
			head = h; tail = t;
		}

		return true;
	}

	bool contains(const T& val) {
		auto trav = get_lb(val);
		for (const node* e : trav)
			if (e && e->r && e->r->val == val)
				return true;
		return false;
	}

	friend std::ostream& operator<<(std::ostream& out, const skip_list<T>& skippy) {
		for (skip_list::node* hh = skippy.head; hh; hh = hh->d, out << "\n")
			for (skip_list::node* cur = hh->r; cur->r; cur = cur->r)
				out << cur->val << " ";
		return out;
	}
};

int main(void) {

	std::mt19937_64 gen(std::chrono::system_clock::now().time_since_epoch().count());

	skip_list<ll> skippy;

	for (ll i = 0; i < 1000000; ++i)
		skippy.insert(i);

	// skippy.insert(1);

	// for (ll tmes = 20; tmes--;) {
	// 	const ll val = gen() % 100;
	// 	skippy.insert(val);
	// }

	// std::cout << skippy;

	return 0;
}
