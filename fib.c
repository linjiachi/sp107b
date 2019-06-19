#include<stdio.h>

int main(){
    int f1=1;    //@1, D=A, @f1, M=D
    int f2=0;    //@0, D=A, @f2, M=D
    int f, t;       
    int n=8;     //@8, D=A, @n, M=D
    int i=2;     //@2, D=A, @i, M=D

wbegin:
    if(i>n) goto wend;    //@i, D=M, @n, D=D-M, D;JGT
    f = f1 + f2;          //@f1, D=M, @f2, D=D+M, @f, M=D
    f2 = f1;              //@f1, D=M, @f2, M=D;
    f1 = f;               //@f, D=M, @f1, M=D;
    i = i + 1;            //@1, D=A, @i, D=D+1, @i, M=D
                          //@i, M=M+1 
    printf("f= %d\n", f);
    goto wbegin;
wend:                     //@wend
    return;               //0;JMP
}