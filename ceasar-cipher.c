#include <stdio.h>
#include <string.h>
#include <conio.h>

int main(){
	int y=0;
	int x;
	printf("wielkosc stringow, uprzesuniecie:");
	scanf("%d %d", &y, &x);
	char inputString[y];
	scanf("%s", inputString);
	for(size_t i=0; i<strlen(inputString); i++){	
		if ((inputString[i]>='a') && (inputString[i]<='z')){
			inputString[i]=inputString[i]+x;
		}
		printf("%c", inputString[i]);
	}
	getche();
	return 0;
}
