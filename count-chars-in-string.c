#include <stdio.h>
#include <string.h>
#include <conio.h>

int main(){
	char string[999];
	float bufor[123]={};
	scanf("%[^\n]%*c", string);
	
	for(size_t i=0; i<strlen(string); i++){
		if ((string[i]>='A') && (string[i]<='Z')){	
			string[i]=string[i]+32;
		}
	}
    for(size_t j=0; j<strlen(string; j++){
    	bufor[string[j]]++;
	}
	
    for(size_t i=0; i<123; i++){
    	if(bufor[i]>0){
			  printf("\n%c", i);
			  printf(" %2.0f", bufor[i]);
			//pokazuje ile razy wystapilo, jesli potrzeba czesc(np 0.2%) to trzeba bufor[i]/strlen(string
		} 		
	}
	getchar();
	return 0;
}
