#include <stdio.h> 
#include <time.h> 
  
int main() 
{ 
        struct tm* local; 
        time_t t = time(NULL); 
        local = localtime(&t);

        char current_time[50];
        strcpy(current_time, asctime(local));

        printf("%s", current_time);

    // printf("Local time and date: %s\n", 
    //        asctime(local)); 
  
    return 0; 
} 
