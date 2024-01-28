#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>

void hash_to_string(unsigned long hash, char *str)
{
    // Implementation of hash_to_string function
    // ...

    // Reverse the string
    int i = 0;
    int j = strlen(str) - 1;
    while (i < j)
    {
        char temp = str[i];
        str[i] = str[j];
        str[j] = temp;
        i++;
        j--;
    }
}

unsigned long djb2_hash(const char *str)
{
    unsigned long hash = 5381;
    int c;

    while ((c = *str++))
    {
        hash = ((hash << 5) + hash) + c; // djb2 algorithm
    }

    return hash;
}

int main()
{
    unsigned long hash;
    char str[100];

    printf("Enter a hash code: ");
    scanf("%lu", &hash);

    hash_to_string(hash, str);
    printf("Converted string: %s\n", str);

    return 0;
}
