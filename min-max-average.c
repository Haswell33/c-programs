#include <stdio.h>

int main(){
  int x=0;
  printf("ilosc liczb: ");
  scanf("%d", &x);
  int tab[x];
  for(size_t i=1; i<=x;i++){
    scanf("%d", &tab[i]);     
  }
  int max=tab[1];
  int min=tab[1];
  float average=0;              
  for(size_t i=1; i<=x; i++){
    if(tab[i]>max)
    max=tab[i];
  }
  for(size_t i=1; i<=x; i++){
    if(tab[i]<min)
    min=tab[i];
  }
  for(size_t i=1; i<=x; i++){
    average=average+tab[i];
  }
  printf("\nmin:%d max:%d average:%2.1f", min, max, average/x);
  return 0;
}
