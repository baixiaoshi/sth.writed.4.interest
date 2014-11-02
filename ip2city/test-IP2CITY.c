/* *************************************************************************
 *
 * File Name: ip.c
 * Author: wushuiyong
 * mail: wushuiyong@huamanshu.com
 * Created Time: Tue 21 Oct 2014 04:49:51 PM
 * ************************************************************************/
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<inttypes.h>
#define BYTE3INT(X)  (    ( X[0] & 0x000000FF ) \
                      | ( ( X[1] & 0x000000FF ) <<  8 ) \
                      | ( ( X[2] & 0x000000FF ) << 16 )  )

#define BYTE4INT(X)  (    ( X[0] & 0x000000FF ) \
                      | ( ( X[1] & 0x000000FF ) <<  8 ) \
                      | ( ( X[2] & 0x000000FF ) << 16 ) \
                      | ( ( X[3] & 0x000000FF ) << 24 )  )
#define BYTE1INT(X)  (    ( X[0] & 0x000000FF ) )

typedef struct indexIP {
    unsigned char ip[4];
    unsigned char local[3];
} IpStruct;


#define ipsize  (int)sizeof(IpStruct)
typedef unsigned int uint;

int searchIndex(unsigned int iplong, FILE * db, int startPos, int mount) {
    int i = 0, j = mount;
    int m, c;
    IpStruct tmp;

    while (i < j - 1) {
        m = (int) (i + j) / 2;
        fseek(db, startPos+m*ipsize, SEEK_SET);
        fread(&tmp, ipsize, 1, db);
        c = compare(tmp.ip, iplong);
        //printf("compare out: %d %d\n", c, m);
        if (c < 0) {
            i = m;
        } else if (c > 0) {
            j = m;
        } else {
            i = j = m;
        }
    }
    return i;
    //int pos = BYTE3INT(xip.local);
    //printf("pos: %d %d\n", startPos+i*ipsize, pos);

    return 0;
}

int compare(unsigned char a[4], unsigned int tb) {
    unsigned int ta = BYTE4INT(a);
    //printf("%d %d\n", ta, tb);
    if (ta > tb) {
        return 1;
    } else if (ta < tb) {
        return -1;
    } else {
        return 0;
    }
}

void GetData(unsigned char* str, FILE* pFile, int max) {
    int i = 0;
    unsigned char byte[1] = {0};;
    int c = 0;
    while ( (c = fgetc(pFile)) != '\0') {
        *(str+i)=c;
        //byte[0] = fgetc(pFile);
        i++;
    }
    str[i] = 0;
}

int searchLocal(IpStruct tmp, unsigned int iplong, FILE * db, char * local) {
    char buf[80] = {0};
    int first = 0;
    int tmpCount;
    int pos = BYTE3INT(tmp.local);
    int offset = pos;
    printf("local pos: %d\n", pos);
    fseek(db, pos, SEEK_SET);
    fread(buf, 4, 1, db);
    int c = compare(buf, iplong);
    //printf("is compare: %d\n", c);

    fread(buf, 1, 1, db);
    if (buf[0] == 0x01) {
        fread(buf, 3, 1, db);
        pos = BYTE3INT(buf);
        fseek(db, pos, SEEK_SET);
        fread(buf, 1, 1, db);
        first = 1;
        printf("\n0x01\n");
    }

    if (buf[0] == 0x02) {
        fread(buf, 3, 1, db);
        tmpCount = ftell(db);
        pos = BYTE3INT(buf);
        fseek(db, pos, SEEK_SET);
        fread(buf, 1, 1, db);
    }

    if ( buf[0] ) {
        GetData(buf+1, db, 50);
    }
    //printf("local: %s\n", local);
    strcat(local, (char*)buf);
    strcat(local, " ");
    //printf("local: %s\n", local);

    // 获取地区
    if ( tmpCount && first ==0) {
        printf("\ntmpCount\n");
        fseek(db, tmpCount, SEEK_SET);
        //fseek(db, offset, SEEK_SET);
    }
    fread(buf, 1, 1, db);
    while ( buf[0] == 0x02 ) {
        // 获取地区偏移
        fread(buf, 3, 1, db);
        pos = BYTE3INT(buf);
        fseek(db, pos, SEEK_SET);
        fread(buf, 1, 1, db);
    }
    if ( buf[0] == 0x01 || buf[0] == 0x02 ) {
        strcat(local, "未知");
        return 0;
    }
    if ( buf[0] ) {
        GetData(buf+1, db, 40);
    }
    strcat(local, (char*)buf);
    printf("local:%s  \n", local);
    //printf("local: %s\n", local);

    return 0;
}


int main() {

    FILE * in;
    in = fopen("/home/paopao/Data/soft/php-5.5.1/ext/huamanshu/UTFWry.dat", "rb");
    char local[255] = {0};
    unsigned int indexHead = 0;
    unsigned int indexTail = 0;
    fread(&indexHead, sizeof(indexHead), 1, in);
    fread(&indexTail, sizeof(indexTail), 1, in);

    //IpStruct xip = {143,24,199,121};
    IpStruct xip = {211,157,79,117};
    unsigned int iplong = 1968151829;
    IpStruct tmp;
    int pos = searchIndex(iplong, in, indexHead, indexTail);

    fseek(in, indexHead+pos*ipsize, SEEK_SET);
    printf("====\n");
    printf("xip %d\n", BYTE4INT(xip.ip));
    fread(&tmp, ipsize, 1, in);
    printf("xip %d\n", BYTE4INT(tmp.ip));
    searchLocal(tmp, iplong, in, local);
    //printf("xip %d\n", BYTE4INT(xip.ip));
    //printf("zip %d\n", BYTE4INT(zip.ip));
    return 0;
}

