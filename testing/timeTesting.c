#include <stdio.h> 
#include <time.h> 
  
int main() 
{ 
  
    struct tm* local; 
    time_t t = time(NULL); 
  
    // Get the localtime 
    local = localtime(&t); 
  
    printf("Local time and date: %s\n", 
           asctime(local)); 
  
    return 0; 
} 