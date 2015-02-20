#ifndef RB_TREE_H
#define RB_TREE_H

#include <iostream>
#include <functional>

template <typename T>
class RBTree {
public:
    enum class Color {RED, BLACK};
    enum class Order {PRE, IN, POST};

    typedef struct Node {
        T value;
        Node* left;
        Node* right;
        Node* parent;
        Color color;
        Node(T val) : value(val), left(nullptr), right(nullptr), parent(nullptr), color(Color::RED) {}
        Node(T val, Color c) : value(val), left(nullptr), right(nullptr), parent(nullptr), color(c) {}
        Node(T val, Node* p) : value(val), left(nullptr), right(nullptr), parent(p), color(Color::RED) {}
        Node(T val, Node* p, Color c) : value(val), left(nullptr), right(nullptr), parent(p), color(c) {}
        Node(T val, Node* l, Node* r, Node* p, Color c) : value(val), left(l), right(r), parent(p), color(c) {
            if (left != nullptr) {
                left->parent = this;
            }
            if (right != nullptr) {
                right->parent = this;
            }
        }
    } Node;

    Node* root;

    RBTree() : root(nullptr) {}

    void copyTree(Node* from, Node*& to, Node* parent) {
        if (from == nullptr) {
            return;
        }

        to = new Node(from->value, parent, from->color);
        copyTree(from->left, to->left, to);
        copyTree(from->right, to->right, to);
    }

    RBTree(const RBTree& rhs) {
        copyTree(rhs.root, root, nullptr);
    }

    RBTree& operator=(const RBTree& rhs) {
        if (this == &rhs) {
            return *this;
        }

        empty(root);
        copyTree(rhs.root, root, nullptr);

        return *this;
    }

    void empty(Node*& node) {
        if (node != nullptr) {
            empty(node->left);
            empty(node->right);
            delete node;
        }
        node = nullptr;
    }

    ~RBTree() {
        empty(this->root);
    }

    Node* grand_parent(Node* node) {
        if (node == nullptr || node->parent == nullptr) {
            return nullptr;
        }
        return node->parent->parent;
    }

    Node* uncle(Node* node) {
        Node* g = grand_parent(node);
        if (g == nullptr) {
            return NULL;
        } else if (g->left == node->parent) {
            return g->right;
        } else {
            return g->left;
        }
    }

    void insert(T val) {
        real_insert(root, nullptr, val);
    }

    void real_insert(Node*& node, Node* parent, T val) {
        if (node == nullptr) {
            node = new Node(val, parent);
            // Rebalance tree.
            insert_case1(node);
        }
        else if (val < node->value) {
            real_insert(node->left, node, val);
        }
        else {
            real_insert(node->right, node, val);
        }
    }

    Color node_color(Node* n) {
        return n == nullptr ? Color::BLACK : n->color;
    }

    void insert_case1(Node* n) {
        if (n->parent == nullptr) {
            n->color = Color::BLACK;
        } else {
            insert_case2(n);
        }
    }

    void insert_case2(Node* n) {
        if (node_color(n->parent) == Color::BLACK) {
            return;
        } else {
            insert_case3(n);
        }
    }

    void insert_case3(Node* n) {
        Node* u = uncle(n);
        if (u != nullptr && node_color(u) == Color::RED) {
            n->parent->color = Color::BLACK;
            u->color = Color::BLACK;
            Node* g = grand_parent(n);
            g->color = Color::RED;
            insert_case1(g);
        } else {
            insert_case4(n);
        }
    }

    void replace_node(Node* from, Node* to) {
        if (from->parent == nullptr) {
            root = to;
        } else {
            if (from->parent->left == from) {
                from->parent->left = to;
            } else {
                from->parent->right = to;
            }
        }
        if (to != nullptr) {
            to->parent = from->parent;
        }
    }

    void rotate_left(Node* n) {
        Node* r = n->right;
        replace_node(n, r);
        n->right = r->left;
        if (r->left != nullptr) {
            r->left->parent = n;
        }
        r->left = n;
        n->parent = r;
    }

    void rotate_right(Node* n) {
        Node* l = n->left;
        replace_node(n, l);
        n->left = l->right;
        if (l->right != nullptr) {
            l->right->parent = n;
        }
        l->right = n;
        n->parent = l;
    }

    void insert_case4(Node* n) {
        Node* g = grand_parent(n);
        if (n == n->parent->right && n->parent == g->left) {
            rotate_left(n->parent);
            n = n->left;
        } else if (n == n->parent->left && n->parent == g->right) {
            rotate_right(n->parent);
            n = n->right;
        }
        insert_case5(n);
    }

    void insert_case5(Node* n) {
        Node* g = grand_parent(n);
        n->parent->color = Color::BLACK;
        g->color = Color::RED;
        if (n == n->parent->left) {
            rotate_right(g);
        } else {
            rotate_left(g);
        }
    }

    Node* search(T val) {
        return real_search(root, val);
    }

    Node* real_search (Node* node, T val) {
        while (node != nullptr) {
            if (node->value == val) {
                return node;
            }
            else if (val < node->value) {
                node = node->left;
            }
            else {
                node = node->right;
            }
        }
        return nullptr;
    }

    void remove(T val) {
        remove_real(root, val);
    }

    void remove_real(Node* node, T val) {
        if (node == nullptr) {
            return;
        }

        // Found the value we were looking for.
        if (node->value == val) {
            // Case 1 No children, just remove the node.
            if (node->left == nullptr && node->right == nullptr) {
                if (node->parent) {
                    // Clean up the parent's child pointer.
                    if (node->parent->left == node) {
                        node->parent->left = nullptr;
                    } else {
                        node->parent->right = nullptr;
                    }
                    delete node;
                } else {
                    // This is the root node we are deleting.
                    delete this->root;
                    this->root = nullptr;
                }
            }
            // Case 2 Only one child, remove the node, and replace it with the child.
            else if (node->left == nullptr || node->right == nullptr) {
                Node* child = node->left;
                if (node->right) {
                    child = node->right;
                }
                if (node->parent) {
                    // Clean up the parent's child pointer.
                    if (node->parent->left == node) {
                        node->parent->left = child;
                    } else {
                        node->parent->right = child;
                    }
                    delete node;
                } else {
                    // This is the root node we are deleting, just replace the child
                    this->root = child;
                    delete node;
                }
            }
            // Case 3 both children are present, choose successor, and replace current node with successor, then remove current node.
            else if (node->left != nullptr && node->right != nullptr) {
                Node* successor = node->right;
                while (successor->left) {
                    successor = successor->left;
                }
                T temp = node->value;
                node->value = successor->value;
                successor->value = temp;
                remove_real(node->right, temp);
            }
        } else {
            // Didn't find the value yet, recurse into subtrees to find the value.
            remove_real(node->left, val);
            remove_real(node->right, val);
        }
    }

    template <typename Func>
    auto traverse(Node* node, Func f, Order order = Order::PRE) -> decltype(f(node)) {
        if (node == nullptr) {
            return;
        }

        if (order == Order::PRE) {
            f(node);
        }

        if (node->left) {
            traverse(node->left, f, order);
        }

        if (order == Order::IN) {
            f(node);
        }

        if (node->right) {
            traverse(node->right, f, order);
        }

        if (order == Order::POST) {
            f(node);
        }
    }

    template <typename Enumeration>
    auto as_integer(Enumeration const value)
        -> typename std::underlying_type<Enumeration>::type
    {
        return static_cast<typename std::underlying_type<Enumeration>::type>(value);
    }

    void print(Order order = Order::PRE) {
        return traverse(root, [](Node* node) {
            int color = static_cast<typename std::underlying_type<Color>::type>(node->color);
            char color_name = ' ';
            if (color == 0) {
                color_name = 'R';
            } else {
                color_name = 'B';
            }
            std::cout << node->value << ":" << color_name << "\n";
        }, order);
    }

};


#endif // RB_TREE_H
