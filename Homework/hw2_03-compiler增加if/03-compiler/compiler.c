#include <assert.h>
#include "compiler.h"

int E();
void STMT();
void IF();
void WHILE();
void BLOCK();

/*HackCPU */
void F_HACKCPU(int, char*);
void E_HACKCPU(int, int, char*, int);
void ASSING_HACKCPU(char*, int);
void IFNOT_HACKCPU(int, int);
void GOTO_HACKCPU(int);

int tempIdx = 0, labelIdx = 0;

#define nextTemp() (tempIdx++)
#define nextLabel() (labelIdx++)
#define emit printf

int isNext(char *set) {
  char eset[SMAX], etoken[SMAX];
  sprintf(eset, " %s ", set);
  sprintf(etoken, " %s ", tokens[tokenIdx]);
  return (tokenIdx < tokenTop && strstr(eset, etoken) != NULL);
}

int isEnd() {
  return tokenIdx >= tokenTop;
}

char *next() {
  // printf("token[%d]=%s\n", tokenIdx, tokens[tokenIdx]);
  return tokens[tokenIdx++];
}

char *skip(char *set) {
  if (isNext(set)) {
    return next();
  } else {
    printf("skip(%s) got %s fail!\n", set, next());
    assert(0);
  }
}

// F = (E) | Number | Id
int F() {
  int f;
  if (isNext("(")) { // '(' E ')'
    next(); // (
    f = E();
    next(); // )
  } else { // Number | Id
    f = nextTemp();
    char *item = next();
    //emit("t%d = %s\n", f, item);
    F_HACKCPU(f, item);
  }
  return f;
}

// E = F (op E)*
int E() {
  int i1 = F();
  while (isNext("+ - * / & | ! < > =")) {
    char *op = next();
    int i2 = E();
    int i = nextTemp();
    //emit("t%d = t%d %s t%d\n", i, i1, op, i2);
    E_HACKCPU(i, i1, op, i2);
    i1 = i;
  }
  return i1;
}

// ASSIGN = id '=' E;
void ASSIGN() {
  char *id = next();
  skip("=");
  int e = E();
  skip(";");
  //emit("%s = t%d\n", id, e);
  ASSING_HACKCPU(id, e);
}

// IF = if (E) STMT (else STMT)?
void IF() {
  int elseLabel = nextLabel();
  int endifLabel = nextLabel();
  skip("if");
  skip("(");
  int e = E();
  skip(")");
  //emit("if not t%d goto L%d\n", e, elseLabel);
  IFNOT_HACKCPU(e,elseLabel);
  STMT();
  //emit("goto L%d\n", endifLabel);
  GOTO_HACKCPU(endifLabel);
  if (isNext("else")){
    emit("(L%d)\n", elseLabel);
    skip("else");
    STMT();
  }
  emit("(L%d)\n", endifLabel);
}

// while (E) STMT
void WHILE() {
  int whileBegin = nextLabel();
  int whileEnd = nextLabel();
  emit("(L%d)\n", whileBegin);
  skip("while");
  skip("(");
  int e = E();
  //emit("if not T%d goto L%d\n", e, whileEnd);
  IFNOT_HACKCPU(e, whileEnd);
  skip(")");
  STMT();
  //emit("goto L%d\n", whileBegin);
  GOTO_HACKCPU(whileBegin);
  emit("(L%d)\n", whileEnd);
}

void STMT() {
  if (isNext("while"))
    return WHILE();
  else if (isNext("if"))
    IF();
  else if (isNext("{"))
    BLOCK();
  else
    ASSIGN();
}

void STMTS() {
  while (!isEnd() && !isNext("}")) {
    STMT();
  }
}

// { STMT* }
void BLOCK() {
  skip("{");
  STMTS();
  skip("}");
}

void PROG() {
  STMTS();
}

void parse() {
  printf("============ parse =============\n");
  tokenIdx = 0;
  PROG();
}

void F_HACKCPU(int f, char *item) {
  if(isAlpha(item[0])) {
    emit("@%s\t#t%d = %s\n", item, f, item);
    emit("D = M\n");
  }
  else {
    emit("@%s\t#t%d = %s\n", item, f, item);
    emit("D = A\n");
  }
  emit("@t%d\n", f);
  emit("M = D\n");
}

void E_HACKCPU(int i, int i1, char *op, int i2) {
  emit("@t%d\t#t%d = t%d %s t%d\n", i1, i, i1, op, i2);
  emit("D = M\n");
  emit("@t%d\n", i2);
  emit("D = D %s M\n", op);
  emit("@t%d\n", i);
  emit("M = D\n");
}
void ASSING_HACKCPU(char *id, int e) {
  emit("@t%d\t#%s = t%d\n", e, id, e);
  emit("D = M\n");
  emit("@%s\n", id);
  emit("M = D\n");
}
void IFNOT_HACKCPU(int e, int jump) {
  emit("@t%d\t#if not t%d goto L%d\n", e, e, jump);
  emit("D = M\n");
  emit("@L%d\n", jump);
  emit("D;JEQ\n");
}
void GOTO_HACKCPU(int end) {
  emit("@L%d\t#goto L%d\n", end);
  emit("0;JMP\n");
}
