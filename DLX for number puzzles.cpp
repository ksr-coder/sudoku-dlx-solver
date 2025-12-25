#include <cctype>
#include <cstring>
#include <iostream>
#include <stack>
constexpr int N = 1e6 + 10;
int ans[10][10];

struct DLX {
  static constexpr int MAXSIZE = 4000;//大的内存预留，至少要到达3240
  int n, m, tot, first[MAXSIZE + 10], siz[MAXSIZE + 10];//tot为计数器，为每个节点分配唯一的ID
  int L[MAXSIZE + 10], R[MAXSIZE + 10], U[MAXSIZE + 10], D[MAXSIZE + 10];//用数组模拟交叉十字指针
  int col[MAXSIZE + 10], row[MAXSIZE + 10];//内容中的节点指向的行标和列表
  std::stack<int>stk;//栈优化

  void build(const int &r, const int &c) {  // 构建十字链表
    n = r, m = c;
    for (int i = 0; i <= c; ++i) {//构建列表头
      L[i] = i - 1, R[i] = i + 1;//构建左指向和右指向链表
      U[i] = D[i] = i;//上下指向的指针不改变
    }
    L[0] = c, R[c] = 0, tot = c;//列端元素构建循环链表
    memset(first, 0, sizeof(first));
    memset(siz, 0, sizeof(siz));
  }

  void insert(const int &r, const int &c) {  // 进行insert的后续，为元素设置图中的指针
    col[++tot] = c, row[tot] = r, ++siz[c];//增加元素数量的同时，设置列指针和行指针，同时增加列数量
    D[tot] = D[c], U[D[c]] = tot, U[tot] = c, D[c] = tot;//将该元素插入到c正下方
    if (!first[r])//行不存在
      first[r] = L[tot] = R[tot] = tot;
    else {//行已经存在
      R[tot] = R[first[r]], L[R[first[r]]] = tot;
      L[tot] = first[r], R[first[r]] = tot;
    }
  }

  void remove(const int &c) {  // 删除列，同时删除与列相交的所有行
    L[R[c]] = L[c], R[L[c]] = R[c];
    for (int i = D[c]; i != c; i = D[i])
      for (int j = R[i]; j != i; j = R[j])
        U[D[j]] = U[j], D[U[j]] = D[j], --siz[col[j]];
  }

  void recover(const int &c) {  // 对remove操作的回溯，从行到列
    int i, j;
    for (i = U[c]; i != c; i = U[i])
      for (j = L[i]; j != i; j = L[j]) U[D[j]] = D[U[j]] = j, ++siz[col[j]];
    L[R[c]] = R[L[c]] = c;
  }

  bool dance() {  // 核心的递归函数
    int c = R[0];
    if (!R[0]) {//递归终止条件，当图为空时，表示找到了答案，此时ans数组存入答案
      while (!stk.empty()){
        int cur=stk.top();
        stk.pop();//不断从栈中取出数据
        int x = (cur- 1) / 9 / 9 + 1;//根据元素ID计算行标
        int y = (cur- 1) / 9 % 9 + 1;//根据元素ID计算列标
        int v = (cur- 1) % 9 + 1;//根据元素ID计算数值
        ans[x][y] = v;
      }
      return true;
    }
    for (int i = R[0]; i != 0; i = R[i])
      if (siz[i] < siz[c]) c = i;//线性查找包含元素最少的列标
    remove(c);//开始“选择”，先删除这一列试试看
    for (int i = D[c]; i != c; i = D[i]) {//依次枚举
      stk.push(row[i]);//枚举选择的行，压入答案栈中
      for (int j = R[i]; j != i; j = R[j]) remove(col[j]);//依次删除所有相关的列
      if (dance()) return true;
      for (int j = L[i]; j != i; j = L[j]) recover(col[j]);//回溯，反向恢复所有相关的列
      stk.pop();//枚举失败，当次答案出栈
    }
    recover(c);//表示“回溯”，在这种选择没有成功时，恢复到原来状态。
    return false;
  }
} solver;

void Insert(int row, int col, int num) {//插入已经确定的节点
  int dx = (row - 1) / 3 + 1;//计算宫的行标
  int dy = (col - 1) / 3 + 1;//计算宫的列标
  int room = (dx - 1) * 3 + dy;//得到对应的宫的ID
  int id = (row - 1) * 9 * 9 + (col - 1) * 9 + num;;//生成元素ID，即元素在图中的行标
  int f1 = (row - 1) * 9 + num;            // 表示数独中行标
  int f2 = 81 + (col - 1) * 9 + num;       // 表示数独中的列标
  int f3 = 81 * 2 + (room - 1) * 9 + num;  // 表示数独中的宫
  int f4 = 81 * 3 + (row - 1) * 9 + col;   // 表示数独中这个点已经有元素了
  solver.insert(id, f1);
  solver.insert(id, f2);
  solver.insert(id, f3);
  solver.insert(id, f4);
}

using std::cin;
using std::cout;

int main() {
  cin.tie(nullptr)->sync_with_stdio(false);
  solver.build(729, 324);//建立图
  for (int i = 1; i <= 9; ++i)
    for (int j = 1; j <= 9; ++j) {
      cin >> ans[i][j];
      for (int v = 1; v <= 9; ++v) {
        if (ans[i][j] && ans[i][j] != v) continue;//关键：在输入为空时，依次插入1-9；在输入非空时，只插入输入的数字
        Insert(i, j, v);//筛选出输入的1-9的已经确定的数字
      }
    }
  solver.dance();
  for (int i = 1; i <= 9; ++i, cout << '\n')
    for (int j = 1; j <= 9; ++j, cout << ' ') cout << ans[i][j];
  return 0;
}