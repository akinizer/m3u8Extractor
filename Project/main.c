#include <stdio.h>
#include <curl/curl.h>
#include <string.h>
#include <stdbool.h>

///data structure with size and content
struct url_data{
    size_t size;
    char*data;
};

///write data output to the console with memory allocation
size_t write_data(void *ptr, size_t size, size_t nmemb, struct url_data *data) {

    ///variable definitions
    size_t index = data->size;
    size_t n = (size * nmemb);
    char* tmp;

    ///data initial size
    data->size += (size * nmemb);

    #ifdef DEBUG
        fprintf(stderr, "data at %p size=%ld nmemb=%ld\n", ptr, size, nmemb);
    #endif
    tmp = realloc(data->data, data->size + 1); /* +1 for '\0' */

    ///data transfer and memory adjustments
    if(tmp) {
        data->data = tmp;
    } else {
        if(data->data) {
            free(data->data);
        }
        fprintf(stderr, "Failed to allocate memory.\n");
        return 0;
    }

    memcpy((data->data + index), ptr, n);
    data->data[data->size] = '\0';

    ///return the size required for a writing process
    return size * nmemb;
}

///function definitions
void initializer();
void writeToFile(char*str);
void printFirstVariant(char*str);
void showFirstVariant(char*str,char*sub);
void showSegment(char*str,int segTarget);

char*printDoc();
char*getLine(char*str,int line);
char*getSegment(char*str,int segTarget);
char*getSubstr(char*str,int index1, int index2);
char*getFirstVariant(char*str);

int segNum(char*str);
int lineNum(char*str);
int findIndexSubstr(char*str,char*sub);

bool hasSubstr(char*str,char*sub);
bool contains(char*str,char*target);

///main constructor
int main(int argc, char*argv[])
{

    if(argc>=2){
        for(int i=1;i<argc;i++){
            printf("%i) %s\t", i,argv[i]);
        }

    }

    //exit(0);

    initializer(argc, argv);

    printf("terminating the program...\ndo not forget to check the output.ts file in directory\n\nHave a nice and healthy day!\n");

    return 0;
}

void initializer(int argc, char*argv[]){

    printf("the program has been initialized successfully\n\n");

    ///playlist
    char masterplaylist[] = "https://devstreaming-cdn.apple.com/videos/streaming/examples/img_bipbop_adv_example_ts/master.m3u8";
    char variantplaylist[] = "https://devstreaming-cdn.apple.com/videos/streaming/examples/img_bipbop_adv_example_ts/v9/prog_index.m3u8";

    char*output_masterplaylist = printDoc(masterplaylist);
    char*output_variantplaylist = printDoc(variantplaylist);

    printf("the master playlist is loaded\n\n");

    ///visualization

    ///printf("Master:\n\n%s\n----------\n\n",output_masterplaylist); ///whole master document
    ///printf("Master:\n\n%s\n----------\n\n",output_variantplaylist); /// whole first variant document

    ///showFirstVariant(output_masterplaylist,"1920"); ///media with highest resolution

    fileFirstVariant(output_masterplaylist);

    printf("the first variant file is created\n\n");
}

///print a m3u8 playlist from a HTML link
char*printDoc(char*url)
{
    struct url_data data;
    data.size = 0;
    data.data = malloc(4096);
    if(NULL == data.data) {
        fprintf(stderr, "Failed to allocate memory.\n");
        return NULL;
    }

    data.data[0] = '\0';

    CURL *curl;
    CURLcode res;
    curl = curl_easy_init();

    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &data);
        res = curl_easy_perform(curl);

        if(res != CURLE_OK)
              fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));

        curl_easy_cleanup(curl);
    }
    return data.data;
}

///get amount of lines
char*getLine(char*str,int line){
    int size = strlen(str);
    char arr[size];

    ///printf("%c",str[index]); --> chars
    ///printf("%s",str); --> the string

    //copy items of str to arr for the process
    for(int i=0;i<size;arr[i]=str[i],i++);

    char*delimiter = "\n";
    char*tok = strtok(arr,delimiter);

    for(int curLine=0;tok!=NULL;curLine++){
        tok = strtok(NULL,delimiter);
        if(curLine==line)
            return tok;
    }

    return NULL;
}

///return index of a substring in a string
int findIndexSubstr(char*str,char*sub){
    int size=strlen(sub);
    char concat[size];
    for(int i=0;i<strlen(str);i++){
        char*cur=getSubstr(str,i,i+size);
        ///printf("%d) %s VS %s\n",i,cur,sub);
        if(strcmp(cur,sub)==0){
            return i;
        }
    }
    return -1;

}

///return index of a character for a string
int findIndexChar(char*str,char character){
    char*index=strchr(str,character);
    return index ? index - str: -1;
}

///get number of lines
int lineNum(char*str){
    int line=0;
    for(int i=0;i<strlen(str);i++){
        if(str[i]=='\n') line++;
    }
    return line;

}

///get number of segments
int segNum(char*str){
    int seg=0;

    for(int i=0;i<strlen(str);i++){
        if(str[i]=='#') seg++;
    }
    return seg;

}

///retrieves a segment for a specified segment index
char*getSegment(char*str,int segTarget){

    char concat[255];

    int segSize = segNum(str);
    int segCount=0;

    for(int i=0;i<strlen(str);i++){
        if(str[i]=='#') segCount++;
        if(segCount==segTarget){
            char newchar[2]={str[i],'\0'};
            strcat(concat,newchar);

        }
        else if(segCount==segTarget+1){

            return strdup(concat);

        }
    }
    return NULL;

}

///prints a segment for a specified segment index
void showSegment(char*str,int segTarget){


    int segSize = segNum(str);
    int segCount=0;

    for(int i=0;i<strlen(str);i++){
        char concat[255];
        if(str[i]=='#') segCount++;
        if(segCount==segTarget){
            char newchar[2]={str[i],'\0'};
            strcat(concat,newchar);

        }
        else if(segCount==segTarget+1){
            printf("%s",concat);
            return;
        }
    }
    printf("%s","none");

}

///returns yes if substring is found, no otherwise
char*containsMess(char*str,char*target){
    return strstr(str,target)!=NULL?"yes":"no";
}

///checks whether a substring is in a string
bool contains(char*str,char*target){
    return strstr(str,target)!=NULL;
}

///retrieves a subtring among two indices
char*getSubstr(char*str,int index1, int index2){
    int size = index2-index1;
    char*res=(char*)malloc(sizeof(char)*(size+1));
    strncpy(res,(str+index1),size);

    ///if null char '\0' is not set, the termination index or tail slot will be replaced with
    ///a random char
    res[size]='\0';
    return res;
}

///write to file
void writeToFile(char*data){
    remove("out\output.ts");
    FILE*output;
    output=fopen("out\output.ts","a+");
    fprintf(output,data);
    fclose(output);
}

void fileFirstVariant(char*str){
    char concat[255];

    for(int i=0;i<segNum(str);i++){
        char*curseg=getSegment(str,i);
        if(curseg!=NULL && findIndexSubstr(curseg,"1920")!=-1){
            writeToFile(curseg);
        }

    }
    printf("write file is done\n");
}

void showFirstVariant(char*str,char*sub){

    for(int i=0;i<segNum(str);i++){
        char*curseg=getSegment(str,i);
        if(curseg!=NULL && findIndexSubstr(curseg,sub)!=-1){
            printf("%s",curseg);
        }

    }

}

