#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "kernel/stat.h"
#include "kernel/fs.h"
#include "user/user.h"

//将路径格式化为文件名
char* fmt_name(char *path){
    static char buf[DIRSIZ+1];
    char *p;
    // 寻找最后一个斜杠后的第一个字符
    for(p=path+strlen(path); p >= path && *p != '/'; p--);
    p++;
    // 将文件名复制到缓冲区中
    memmove(buf, p, strlen(p)+1);
    return buf;
}

//在指定路径中查找指定的文件
void find(char *path, char *findName){
    int fd;
    struct stat st;
    if((fd = open(path, O_RDONLY)) < 0){
        printf("find: cannot open %s\n", path);
        return;
        //以只读模式打开指定路径的文件,文件打开失败则输出失败信息
    }
    if(fstat(fd, &st) < 0){
        printf("find: cannot stat %s\n", path);
        close(fd);
        return;
        //获取文件描述符fd所指向文件的状态信息,获取失败则输出提示
    }
    char buf[512], *p;
    //用于存储路径和处理字符串操作
    struct dirent de;
    //存储目录项的信息
    switch(st.type){
        case T_FILE:
        //是文件则判断是否相同
            if(strcmp(fmt_name(path),findName)==0){
                printf("%s\n", path);
            }
            break;
        case T_DIR:
        //是目录
            if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
                //拼接后的路径长度(路径长度加上目录项的大小以及斜杠和结束符的长度)是否超过了缓冲区buf的大小
                printf("find: path too long\n");
                break;
            }
            strcpy(buf, path);
            p = buf+strlen(buf);
            *p++ = '/';
            while(read(fd, &de, sizeof(de)) == sizeof(de)){
                if(de.inum == 0 || de.inum == 1 || strcmp(de.name, ".")==0 || strcmp(de.name, "..")==0)
                    continue;
                // 将目录项的名称复制到缓冲区中
                memmove(p, de.name, strlen(de.name));
                p[strlen(de.name)] = 0;
                // 递归调用 find() 函数，继续在子目录中查找
                find(buf, findName);
            }
            break;
    }
    close(fd);
}

int main(int argc, char *argv[]){
    if(argc < 3){
        printf("find: find <path> <fileName>\n");
        exit(0);
    }
    // 调用 find() 函数开始在指定路径下查找指定文件
    find(argv[1], argv[2]);
    exit(0);
}