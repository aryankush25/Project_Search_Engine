#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#define HASH_SIZE 100

#define URL_LENGTH 1000

#define SEED_URL "https://www.chitkara.edu.in"

#define MAX_URL_PER_PAGE 1000

#define INTERVAL_TIME 10

#define CHECK_MALLOC(s) if ((s) == NULL) { \
printf("memory allocation failed in %s: line%d", _FILE, __LINE_); \
perror(":"); \
exit(-1); \
}

#define SET_MEM(start,size) memset(start,0,size)

struct url {
    char c[URL_LENGTH];
    int depth;
};

struct node {
    struct url u;
    int visited;
    int key;
    struct node *next;
};

struct hash {
    struct node *_head;
    struct node *_tail;
    int count;
};

void testURL(char *url)
{
    if (strcmp(url,SEED_URL) != 0 ) {
        printf("----------------------\n");
        printf("User Enter Invalid URL\n");
        printf("----------------------\n");
    }
    else {
        char ch[100] = "wget --spider ";
        strcat(ch, url);
        
        if (!system(ch)) {
            printf("------------------------------------------\n");
            printf("%s is a Valid URL\n", url);
            printf("------------------------------------------\n");
        }
        else {
            printf("---------------------\n");
            printf("%s is a not Valid URL\n", url);
            printf("---------------------\n");
        }
    }
}

void testDepth(char *depth)
{
    if(strcmp(depth, "1") != 0 ) {
        printf("------------------------\n");
        printf("User Enter Invalid Depth\n");
        printf("------------------------\n");
    }
}

void testDir(char *dir)
{
    struct stat statbuf;
    if (stat(dir, &statbuf) == -1) {
        fprintf(stderr, "-----------------\n");
        fprintf(stderr, "Invalid directory\n");
        fprintf(stderr, "-----------------\n");
    }

    if ( !S_ISDIR(statbuf.st_mode) ) {
        fprintf(stderr, "-----------------------------------------------------\n");
        fprintf(stderr, "Invalid directory entry. Your input isn't a directory\n");
        fprintf(stderr, "-----------------------------------------------------\n");
    }

    if ( (statbuf.st_mode & S_IWUSR) != S_IWUSR ) {
        fprintf(stderr, "------------------------------------------\n");
        fprintf(stderr, "Invalid directory entry. It isn't writable\n");
        fprintf(stderr, "------------------------------------------\n");
    }
}

int testArguments(int argc, char *argv[])
{
    
    if (argc != 4) {
        printf("-------------------------------------\n");
        printf("User Enter Invalid Number Of Arguments\n");
        printf("-------------------------------------\n");
        return 0;
    }
    
    testURL(argv[1]);
    testDepth(argv[2]);
    testDir(argv[3]);
    
    return 1;
}

void getpage(char *url)
{
    char urlbuffer[1000] = {0};
    
    strcat(urlbuffer, "wget -O ");
    strcat(urlbuffer, "/Users/aryankush25/Desktop/Project_Search_Engine/page/temp.txt ");
    strcat(urlbuffer, url);
    system(urlbuffer);
}

void printArguments(int argc, char *argv[])
{
    printf("----------------------------------------------------------------------------------------------\n");
    
    for (int i = 1; i < argc; i++) {
        for(int j = 0; *(*(argv + i)+j) != '\0'; j++) {
            printf("%c", *(*(argv + i)+j) );
        }
        printf("\n");
    }
    
    printf("----------------------------------------------------------------------------------------------\n");
}

char *getPageContent()
{
    char c;
    int i = 0;
    FILE *fptr = fopen("./page/temp.txt","r");
    
    while (!feof(fptr))
    {
        c = getc(fptr);
        i++;
    }
    
    char *str = (char *) malloc(sizeof(char) * i);
    i = 0;
    
    rewind(fptr);
    
    while (!feof(fptr))
    {
        c = getc(fptr);
        str[i++] = c ;
    }
    str[i-1] = '\0';
    
    return str;
}

void noRepeat(char **urls)
{
    int n =0;
    while (urls[n][0] != '\0') {
        int i = 0;
        while (urls[n][i] != '\0') {
            i++;
        }
        if (urls[n][i - 1] == '/') {
            urls[n][i - 1] = '\0';
        }
        n++;
    }
    
    for (int i = 0; i < n; i++) {
        if (strcmp(urls[i], SEED_URL) == 0) {
            for (int k = i; urls[k][0] != '\0'; k++) {
                strcpy(urls[k], urls[k + 1]);
            }
            i--;
            continue;
        }
        
        for (int j = i + 1; urls[j][0] != '\0'; j++) {
            if (strcmp(urls[i], urls[j]) == 0) {
                for (int k = j; urls[k][0] != '\0'; k++) {
                    strcpy(urls[k], urls[k + 1]);
                }
                j--;
            }
        }
    }
}
void baseSame(char **urls)
{
    for (int i = 0; urls[i][0] != '\0'; i++) {
        if (strstr(urls[i], SEED_URL) == NULL) {
            for (int k = i; urls[k][0] != '\0'; k++) {
                strcpy(urls[k], urls[k + 1]);
            }
            i--;
        }
    }
}
char **getURLs(char *html)
{
    int i = 0, j = 0, k = 0, l = 0;
    char search[] = "a href=";
    char *str = (char *) malloc(sizeof(char) * strlen(html));
    char **urls = (char**) calloc(MAX_URL_PER_PAGE, sizeof(char *));
    
    for(int i = 0; i < MAX_URL_PER_PAGE; i++) {
        * (urls + i) = (char*) calloc(URL_LENGTH, sizeof(char));
    }
    
    while (html[i] != '\0') {
        if ( html[i] == search[j] ) {
            if ( j == 6 && html[i + 1] == '"' ) {
                i = i + 2;
                
                if( html[i] != 'h' ) {
                    j = 0;
                    i++;
                    continue;
                }
                
                while (html[i] != '"') {
                    str[k++] = html[i++];
                }
                
                str[k++] = '\n';
                j = 0;
            }
            else {
                j++;
            }
        }
        else {
            j = 0;
        }
        i++;
    }
    str[--k] = '\0';
    i = 0;
    j = 0;
    l = 0;

    while (i <= k) {
        if(str[i] == '\n' || str[i] == '\0') {
            urls[j][l] = '\0';
            l = 0;
            j++;
            if(j == MAX_URL_PER_PAGE)
                break;
        }
        else {
            urls[j][l] = str[i];
            l++;
            if(l == URL_LENGTH) {
                l = 0;
                while (str[i] != '\n') {
                    i++;
                }
            }
        }
        i++;
    }
    
    free(str);
    noRepeat(urls);
    baseSame(urls);
    return urls;
}

void itoa(int num, char* result)
{
    static  int i=0;
    if(num == 0) {
        i=0;
        return;
    }
    int rem = num%10;
    itoa(num/10,result);
    result[i] = (rem) + 48;
    result[i+1] = '\0';
    i++;
}

void saveURLsInFile(char **urls)
{
    FILE *fptr = fopen("./URLs.txt","w");
    int i = 0;
    while (urls[i][0] != '\0') {
        int j = 0;
        while (urls[i][j] != '\0') {
            putc(urls[i][j], fptr);
            j++;
        }
        putc('\n', fptr);
        i++;
    }
    
    char c[10];
    
    itoa(i, c);
    
    fputs("Total No. of URLs - ", fptr);
    fputs(c, fptr);
    fclose(fptr);
}

void displayURLs(char **urls)
{
    int i =0;
    while (urls[i][0] != '\0') {
        printf("%s\n", urls[i]);
        i++;
    }
    printf("Number of URLs - %d\n", i);
}

void createNode(struct hash *h, int key, char* url)
{
    struct node *temp = (struct node *)malloc(sizeof(struct node));
    strcpy(temp->u.c, url);
    temp->visited = 0;
    temp->key = key;
    temp->next = h[key]._head;
    h[key]._head = temp;
}

int chekNodePresent(struct hash *h, int key, char* url)
{
    struct node *temp = h[key]._head;
    
    while (temp != NULL) {
        if(strcmp(temp->u.c, url) == 0)
            return 1;
        temp = temp->next;
    }
    return 0;
}

void insertIntoHash(struct hash *h, struct node **head, struct node **tail, char **urls)
{
    int i = 0;
    while (urls[i][0] != '\0') {
        int key = 0;
        int j = 0;
        while (urls[i][j] != '\0') {
            key = key + urls[i][j];
            j++;
        }
        key = key % 100;
        h[key].count++;
        if(chekNodePresent(h, key, urls[i]) != 1)
            createNode(h, key, urls[i]);
        i++;
    }
}

void displayHashTable(struct hash *h)
{
    for (int i = 0; i < 100; i++) {
        struct node *temp = h[i]._head;
        while (temp != NULL) {
            printf("%s\n", temp->u.c);
            temp = temp->next;
        }
        printf("\n");
    }
}
int main(int argc, char *argv[]) // ./a.out https://www.chitkara.edu.in 1 page
{
    printArguments(argc, argv);
    if(testArguments(argc, argv) == 0) return 0;
    
    struct hash h[HASH_SIZE];
    for (int i = 0; i < HASH_SIZE; i++) {
        h[i]._head = NULL;
        h[i]._tail = NULL;
        h[i].count = 0;
    }
    
    struct node *head = NULL;
    struct node *tail = NULL;
    
    getpage(argv[1]);
    char *html = getPageContent();
    char **urls = getURLs(html);
    
    insertIntoHash(h, &head, &tail, urls);
    insertIntoHash(h, &head, &tail, urls);
    
    displayHashTable(h);
    saveURLsInFile(urls);
    displayURLs(urls);
    
    return 0;
}
