#include <iostream>
#include<fstream>
#include<vector>

using namespace std;

/*
 * 中心控制程序，协调词法和语法程序；
 * */

struct wordUnit{                                    // 词单元结构
    string token;
    string lexme;
};

extern int read_buffer(string source_file);        // 读入源程序，填充BUFFER
extern wordUnit getWordUnit();                     // 获取一个词单元
extern void back();
extern wordUnit getFromBuffer();                   // 从缓冲区取出
extern void getProgram(const string& outputFile = "");




int main(){
    string source_file = "testfile.txt";
    string output_file = "output.txt";

    // todo 缓冲区优化
    if(!read_buffer(source_file)){
        cout << "file not open!" << endl;
    }

    getProgram(output_file);
}