/*
 * MIT License
 *
 * Copyright (c) 2026 INHA_OSAP_003_3
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * Author: <Park Juseong, Kim Insoo, Kim Yeonwoo>
 * Date: <2026-11>
 */

#include <stdio.h>
#include <algorithm>
#include <iostream>
#include <string>

using namespace std;
class node {
 private:
  friend class BSTMap;

  // key-value? key x와 노드 x가 서로 일치하게 설계하는 것 아님?
  int key;
  int height;
  int sub_size;

  node* parent;
  node* left;
  node* right;

 public:
  node(int key, int height, node* parent)
      : key(key),
        height(height),
        sub_size(1),
        parent(parent),
        left(nullptr),
        right(nullptr) {}
};

class BSTMap {
 private:
  int n;
  node* root_node;

  void clear(node* Node);
  void update_height(node* Node);  // 높이 갱신
  node* find_node(int key);
  node* get_successor(node* Node);
  int height(node* Node);
  int subtree_size(node* Node);

  // 261031 by jsPark: 새로 추가한 AVL 회전 함수
  void rotate_right(node* z);  // z를 오른쪽으로 돌린다 (z의 왼쪽 자식이 위로)
  void rotate_left(node* z);   // z를 왼쪽으로 돌린다 (z의 오른쪽 자식이 위로)
  node* restructure(node* z, node* y, node* x);  // LL/LR/RR/RL 판단 후 회전
  void rebalance(node* Node);
  void erase(node* Node);

 public:
  BSTMap();
  ~BSTMap();

  void height(int key);
  void subtree_size(int key);
  int height_(node* Node);
  int size_(node* Node);
  void insert(int key);
  int check(int key);  // = find()
  int size();          // O(1)
  bool empty();        // O(1)
  void find(int x);
  void rank(int x);

  void erase(int x);
  void print();  // 중위 순회 출력 O(n)
  void inorder(node* Node);
  int get_depth(node* Node); /*깊이함수 추가*/
  // 아래 max, min 함수는 출력을 겸함
  void find_Max(int x);  // 최대 key 찾기
  void find_Min(int x);  // 최소 key 찾기
};

void BSTMap::rank(int x) {
  node* target = find_node(x);
  if (target == nullptr) {
    cout << "-1" << endl;
    return;
  }

  node* v = root_node;
  int smaller = 0;
  while (v != nullptr) {
    if (target->key < v->key) {
      v = v->left;
    } else if (target->key > v->key) {
      smaller += size_(v->left) + 1; /*그시점의 v까지 포함*/
      v = v->right;
    } else {
      smaller += size_(v->left);
      cout << get_depth(v) << " " << smaller + 1 << endl;
      return;
    }
  }
  cout << "-1" << endl;
}
void BSTMap::find(int x) {
  node* target = find_node(x);
  if (target == nullptr) {
    cout << "-1" << endl;
    return;
  }
  cout << get_depth(target) << endl;
}
int BSTMap::get_depth(node* Node) {
  int i = 0;
  while (Node->parent != nullptr) {
    Node = Node->parent;
    ++i;
  }
  return i;
}
BSTMap::BSTMap() : n(0), root_node(nullptr) {}
BSTMap::~BSTMap() {
  if (!empty()) {
    clear(root_node);
    n = 0;  // 일단?
  }
}

void BSTMap::clear(node* Node) {
  if (Node == nullptr) {
    return;
  }
  if (Node->left != nullptr) {
    clear(Node->left);
  }
  if (Node->right != nullptr) {
    clear(Node->right);
  }
  delete Node;
}

node* BSTMap::find_node(int key) {
  node* trv = root_node;

  while (trv != nullptr) {
    if (key == trv->key) {
      return trv;
    }

    if (key < trv->key) {
      trv = trv->left;
    } else {
      trv = trv->right;
    }
  }

  return nullptr;
}

void BSTMap::erase(int x) {
  node* target = find_node(x);
  erase(target);
}
void BSTMap::erase(node* Node) {
  if (Node == nullptr) {
    cout << "-1" << endl;
    return;
  }
  n--;
  cout << get_depth(Node) << endl;
  // 후행자 정보 복사
  if (Node->left != nullptr && Node->right != nullptr) {
    node* successor = get_successor(Node);
    Node->key = successor->key;
    Node = successor;
  }

  // 유효한 자식으로 초기화
  node* child;  // node* child = (Node->left ? Node->left : Node->right);
  if (Node->left != nullptr) {
    child = Node->left;
  } else {
    child = Node->right;
  }

  // 261031 by jsPark: AVL Rotation을 위해 삭제할 노드의 부모를 else문 밖에서
  // 미리 저장
  node* par = Node->parent;

  // 삭제할 노드가 루트노드라면 ?
  if (Node == root_node) {
    root_node = child;
    if (child != nullptr) {
      child->parent = nullptr;
    }
  }
  // 일반 노드라면 ?
  else {
    if (child != nullptr) {
      child->parent = par;
    }
    if (Node == par->left) {
      par->left = child;
    } else {
      par->right = child;
    }
  }
  delete Node;

  // 261031 by jsPark: 노드 삭제 후 높이 갱신 + 필요하면 회전 (par가 nullptr이면
  // 아무 일도 안 함)
  rebalance(par);
}
node* BSTMap::get_successor(node* Node) {
  if (Node->right) {
    node* trv = Node->right;
    while (trv->left) {
      trv = trv->left;
    }
    return trv;
  } else {
    node* trv = Node;
    node* Par = Node->parent;
    while (Par && trv == Par->right) {
      Par = Par->parent;
      trv = trv->parent;
    }
    return Par;
  }
  /*코드블락이지만 로직 일관성을 위해 작성 <- ?? 이거도 물어보기 */
}

void BSTMap::insert(int key) {
  if (empty()) {
    root_node = new node(key, 1, nullptr);
    cout << "0" << endl;
    n++;
    return;
  }

  // 삽입할 자리 탐색
  node* trv = root_node;
  node* par = nullptr;
  while (trv != nullptr) {
    if (trv->key == key) {
      cout << "-1" << endl;
      return;
    }
    par = trv;
    if (key < trv->key) {
      trv = trv->left;

    } else {
      trv = trv->right;
    }
  }

  node* new_node = new node(key, 1, par);
  if (new_node->key < par->key) {
    par->left = new_node;
  } else {
    par->right = new_node;
  }

  ++n;

  // 261031 by jsPark: 삽입 노드의 부모부터 위로 올라가며 높이 갱신 + 필요하면
  // 회전
  rebalance(par);

  cout << get_depth(new_node) << endl;
}

int BSTMap::height_(node* Node) { return Node ? Node->height : 0; }
int BSTMap::size_(node* Node) { return Node ? Node->sub_size : 0; }

// 261031 by jsPark: AVL 오른쪽 회전 함수
void BSTMap::rotate_right(node* z) {
  node* y = z->left;    // 위로 올라갈 노드
  node* B = y->right;   // 이사갈 서브트리 (nullptr일 수도 있음)
  node* p = z->parent;  // z의 원래 부모

  // 1. y를 z의 자리로 옮김
  y->parent = p;
  if (p == nullptr) {  // z가 루트였다면 이제 y가 루트
    root_node = y;
  } else if (p->left == z) {  // z가 부모의 왼쪽 아이였다면
    p->left = y;
  } else {  // z가 부모의 오른쪽 아이였다면
    p->right = y;
  }

  // 2. z를 y의 오른쪽 자식으로 내림
  y->right = z;
  z->parent = y;

  // 3. B를 z의 왼쪽 자식으로 (nullptr이 아니면 무조건 z의 왼쪽 자식이 됨)
  z->left = B;
  if (B != nullptr) {
    B->parent = z;
  }

  // 높이 다시 계산할 때 반드시 아래(z)에서 위(y)로 순서
  z->height = max(height_(z->left), height_(z->right)) + 1;
  z->sub_size = size_(z->left) + size_(z->right) + 1;
  y->height = max(height_(y->left), height_(y->right)) + 1;
  y->sub_size = size_(y->left) + size_(y->right) + 1;
}
// 261031 by jsPark: AVL 왼쪽 회전 함수
void BSTMap::rotate_left(node* z) {
  node* y = z->right;  // 위로 올라갈 노드
  node* B = y->left;   // 이사갈 서브트리 (nullptr일 수도 있음)
  node* p = z->parent;

  // 1. y를 z의 자리로 옮김
  y->parent = p;
  if (p == nullptr) {
    root_node = y;
  } else if (p->left == z) {
    p->left = y;
  } else {
    p->right = y;
  }

  // 2. z를 y의 왼쪽 자식으로
  y->left = z;
  z->parent = y;

  // 3. B를 z의 오른쪽 자식으로 (nullptr이 아니면 무조건 z의 오른쪽 자식이 됨)
  z->right = B;
  if (B != nullptr) {
    B->parent = z;
  }

  // 높이 다시 계산할 때 반드시 아래(z)에서 위(y)로 순서
  z->height = max(height_(z->left), height_(z->right)) + 1;
  z->sub_size = size_(z->left) + size_(z->right) + 1;
  y->height = max(height_(y->left), height_(y->right)) + 1;
  y->sub_size = size_(y->left) + size_(y->right) + 1;
}
// z : 균형이 깨진 노드,  y : z의 자식 중 서브트리 높이가 더 큰 자식,  x : y의
// 자식 중 서브트리 높이가 더 큰 자식 회전이 끝난 뒤 그 자리의 , LL/RR은 y가
// 위로, LR/RL은 x 리턴
node* BSTMap::restructure(node* z, node* y, node* x) {
  if (y == z->left) {
    if (x == y->left) {  // LL : 일자로 왼쪽으로 쏠림 -> 오른쪽 회전 1번
      rotate_right(z);
      return y;
    } else {  // LR : "<" 모양 -> y를 왼쪽 회전, z를 오른쪽 회전
      rotate_left(y);
      rotate_right(z);
      return x;
    }
  } else {
    if (x == y->right) {  // RR : 일자로 오른쪽으로 쏠림 -> 왼쪽 회전 1번
      rotate_left(z);
      return y;
    } else {  // RL : ">" 모양 -> y를 오른쪽 회전, z를 왼쪽 회전
      rotate_right(y);
      rotate_left(z);
      return x;
    }
  }
}
void BSTMap::update_height(node* Node) {
  int lefth = (Node->left ? Node->left->height : 0);
  int righth = (Node->right ? Node->right->height : 0);

  if (lefth > righth) {
    Node->height = lefth + 1;
  } else {
    Node->height = righth + 1;
  }

  Node->sub_size = size_(Node->left) + size_(Node->right) + 1;
}

// Node에서 시작해 루트까지 올라가면서 [높이 갱신 -> 균형 검사 -> 필요하면 회전]
// 반복 insert,erase 가 끝난 뒤 둘 다 이 함수를 호출
void BSTMap::rebalance(node* Node) {
  while (Node != nullptr) {
    update_height(Node);  // 1. 내 높이부터 새로 계산

    int lefth = height_(Node->left);
    int righth = height_(Node->right);

    // 2. 높이 차 판단 로직 (-1, 0, 1이 아닐 경우)
    int sub = lefth - righth;
    if (sub < 0) {
      sub *= -1; /*절댓값 작업*/
    }
    if (sub >= 2) {
      node* z = Node;
      node* y = nullptr;
      node* x = nullptr;

      if (height_(z->left) > height_(z->right)) {
        y = z->left;
      } else {
        y = z->right;
      }

      if (height_(y->left) > height_(y->right)) {
        x = y->left;
      } else if (height_(y->left) < height_(y->right)) {
        x = y->right;
      } else { /*x노드 높이가 동률일때 LL이나 RR케이스로 처리하도록*/
        if (y == z->left) {
          x = y->left;
        } else {
          x = y->right;
        }
      }

      Node = restructure(z, y, x);  // 회전 수행
    }
    Node = Node->parent;
  }
}

int BSTMap::check(
    int key) {  // find() : value 반환 중 + 10/26 다시 key로 바꿔봤어요
  node* target = find_node(key);
  if (target == nullptr) {
    return 0;
  }
  return target->key;
}
int BSTMap::size() {  // size()
  return n;
}
bool BSTMap::empty() {
  return n == 0;  // O(1)
}

// MAXIMUM x : key와 깊이를 같이 출력
void BSTMap::find_Max(int x) {
  node* Node = find_node(x);
  if (!Node) {
    return;
  }
  while (Node->right) {
    Node = Node->right;
  }
  cout << Node->key << ' ' << get_depth(Node) << '\n';
}
// MINIMUM x : key와 깊이를 같이 출력
void BSTMap::find_Min(int x) {
  node* Node = find_node(x);
  if (!Node) {
    return;
  }
  while (Node->left) {
    Node = Node->left;
  }
  cout << Node->key << ' ' << get_depth(Node) << '\n';
}

void BSTMap::inorder(node* Node) {
  if (Node == nullptr) {
    return;
  }
  inorder(Node->left);       // 1. 왼쪽 (작은 값)
  cout << Node->key << " ";  // 2. 자기 자신
  inorder(Node->right);      // 3. 오른쪽 (큰 값)
}
void BSTMap::print() {
  inorder(root_node);
  cout << endl;
}

int main() {
  int N; /*테스트케이스 개수*/
  int a; /*명령어 개수*/
  string name;
  cin >> N;
  while (N--) {
    cin >> a;
    BSTMap s;
    while (a--) {
      cin >> name;
      if (name == "Find") {
        int q;
        cin >> q;
        s.find(q);
      } else if (name == "Insert") {
        int q;
        cin >> q;
        s.insert(q);
      } else if (name == "Empty") {
        cout << s.empty() << endl;
      } else if (name == "Size") {
        cout << s.size() << endl;
      } else if (name == "Maximum") {
        int q;
        cin >> q;
        s.find_Max(q);
      } else if (name == "Minimum") {
        int q;
        cin >> q;
        s.find_Min(q);
      } else if (name == "Erase") {
        int q;
        cin >> q;
        s.erase(q);
      } else if (name == "Rank") {
        int q;
        cin >> q;
        s.rank(q);
      }
    }
  }
}