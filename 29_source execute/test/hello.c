#include <stdio.h>

void func_c(void){
    int c = 0;
    c++;
    printf("func_c\n");
}
void func_b(void){
    int b = 0;
    b++;
    printf("func_b\n");
    func_c();
}
void func_a(void){
    int a = 0;
    a++;
    printf("func_a\n");
    func_b();
}

int main(){
    printf("hello world\n");

    func_a();
    return 0;
}