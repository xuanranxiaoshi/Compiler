//
// Created by csc on 2022/4/20.
//

#include<iostream>
#include<vector>
#include<fstream>
#include<unordered_map>

using namespace std;

const int BUFFER_SIZE = 50;

int row = 1;                        // 当前行号
int wordStart = 0;                  // 当前所识别词法单元第一个字符
int wordEnd = 0;                    // 当前识别词法单元的最后一个字符
int wordsNum = 0;                   // 总计词单元数
int buffer_remain = 0;              // 缓冲区待使用单元数

struct wordUnit{                    // 词单元结构
    string token;
    string lexme;
};


vector<char> BUFFER;	            // 字符缓冲
wordUnit WordsBuffer[BUFFER_SIZE];  // 词单元缓冲


unordered_map<string, string> reserves = {
        { "const", "CONSTTK" }, { "int", "INTTK" }, { "char","CHARTK"},
        {"void", "VOIDTK"}, {"main", "MAINTK"}, {"if", "IFTK"},
        {"else", "ELSETK"},{"switch", "SWITCHTK"},{"case", "CASETK"},
        {"default", "DEFAULTTK"}, {"while", "WHILETK"}, {"for", "FORTK"},
        {"scanf","SCANFTK"}, {"return", "RETURNTK"}, {"printf", "PRINTFTK"}
};


bool isDigit(char c);                       // 判断是否是数字
bool isLetter(char c);                      // 判断是否是字母
wordUnit getWordUnit();                     // 获取一个词单元
string lowerStr(string s);                  // 字符串转小写
string getWord(int start, int end);         // 获取指定区域的字符串
int read_buffer(string source_file);        // 读入源程序，填充BUFFER
wordUnit getFromBuffer();                   // 从缓冲区取出
void back(int step);                        // 退回一个词单元




bool isDigit(char c) {
    return c >= '0' && c <= '9';
}

bool isLetter(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

string lowerStr(string s) {
    string res;
    for(char i : s){
        res += tolower(i);
    }
    return res;
}

string getWord(int start, int end) {
    string word;
    for (int i = start; i < end; i++) {
        word += BUFFER[i];
    }
    return word;
}

int read_buffer(string source_file) {
    ifstream ifs;
    ifs.open(source_file, ios::in);
    if (!ifs.is_open()) {
        return 0;
    }
    char c;
    while ((c = ifs.get()) != EOF) { // EOF end of file
        BUFFER.push_back(c);
    }
    return 1;
}

void back(int step){
    if(buffer_remain + step > BUFFER_SIZE) {
        cout << "WordBuffer full !" << endl;
    }else{
        buffer_remain +=  step;
    }
}

wordUnit getFromBuffer(){
    int index = (wordsNum - buffer_remain) % BUFFER_SIZE;
    buffer_remain --;
    return WordsBuffer[index];
}

wordUnit getWordUnit(){
    if(buffer_remain > 0){
        return getFromBuffer();
    }
    string token;
    string lexeme;

    if(wordStart < BUFFER.size()) {	// 从左至右一次处理缓冲区中的字符)
        // 处理空白、换行、对齐字符
        while (BUFFER[wordStart] == ' ' || BUFFER[wordStart] == '\t' || BUFFER[wordStart] == '\n') {
            if (BUFFER[wordStart] == '\n') {
                row ++;
            }
            wordStart++;
        }
        // 初始化wordEnd
        wordEnd = wordStart + 1;


        // 识别数字
        if (isDigit(BUFFER[wordStart])) {
            while (wordEnd < BUFFER.size() && isDigit(BUFFER[wordEnd])) {
                wordEnd++;
            }
            token = "INTCON";
            lexeme = getWord(wordStart, wordEnd);
        }
            // 识别标识符
        else if (isLetter(BUFFER[wordStart]) || BUFFER[wordStart] == '_') {
            while (wordEnd < BUFFER.size() &&( isDigit(BUFFER[wordEnd]) || isLetter(BUFFER[wordEnd]) || BUFFER[wordEnd] == '_')) {
                wordEnd++;
            }
            token = "IDENFR";
            lexeme = getWord(wordStart, wordEnd);
            // 注意大小写
            string tmp_low = lowerStr(lexeme);
            // 是否为保留字
            if (reserves.find(tmp_low) != reserves.end()) {
                token = reserves[tmp_low];
            }
        }
            // 识别字符串
        else if (BUFFER[wordStart] == '"') {
            while (wordEnd < BUFFER.size() && BUFFER[wordEnd] != '"') {	// todo  没有匹配到右引号
                wordEnd++;
            }
            token = "STRCON";
            lexeme = getWord(wordStart + 1, wordEnd);
            wordEnd += 1;	// 跳过
        }
            // 识别字符常量
        else if (BUFFER[wordStart] == '\'') {		// todo 没有匹配到右引号
            token = "CHARCON";
            if (wordStart + 2 >= BUFFER.size() || BUFFER[wordStart + 2] != '\'') {
                token = "ERROR";
            }
            else {
                lexeme = string(1, BUFFER[wordStart + 1]);
                wordEnd = wordStart + 3;
            }
        }
        else if (BUFFER[wordStart] == '=') {
            if (wordEnd < BUFFER.size() && BUFFER[wordEnd] == '=') {
                token = "EQL";
                lexeme = "==";
                wordEnd += 1;
            }
            else {
                token = "ASSIGN";
                lexeme = "=";
            }
        }
        else if (BUFFER[wordStart] == '>') {
            if (wordEnd < BUFFER.size() && BUFFER[wordEnd] == '=') {
                token = "GEQ";
                lexeme = ">=";
                wordEnd += 1;
            }
            else {
                token = "GRE";
                lexeme = ">";
            }
        }
        else if (BUFFER[wordStart] == '<') {
            if (wordEnd < BUFFER.size() && BUFFER[wordEnd] == '=') {
                token = "LEQ";
                lexeme = "<=";
                wordEnd += 1;
            }
            else {
                token = "LSS";
                lexeme = "<";
            }
        }
        else if (BUFFER[wordStart] == '!') {
            if (wordEnd < BUFFER.size() && BUFFER[wordEnd] == '=') {
                token = "NEQ";
                lexeme = "!=";
                wordEnd += 1;
            }
            else {
                token = "ERROR";
                lexeme = "";
            }
        }
            // 识别运算符和其他单个字符
        else {
            switch (BUFFER[wordStart]) {
                case '(': token = "LPARENT"; break;
                case ')': token = "RPARENT"; break;
                case '{': token = "LBRACE"; break;
                case '}': token = "RBRACE"; break;
                case '+': token = "PLUS"; break;
                case '-': token = "MINU"; break;
                case '*': token = "MULT"; break;
                case '/': token = "DIV"; break;
                case ':': token = "COLON"; break;
                case '[': token = "LBRACK"; break;
                case ']': token = "RBRACK"; break;
                case ';': token = "SEMICN"; break;
                case ',': token = "COMMA"; break;
                default: token = "ERROR"; break;
            }
            lexeme = string(1, BUFFER[wordStart]);
        }

        /*cout << token << " " << lexeme <<" " << ids[token] << endl;*/

        // 更新下一个初始位置
        wordStart = wordEnd;

        wordUnit res = wordUnit{ token, lexeme};

        // 返回识别到的词单元
        WordsBuffer[wordsNum % BUFFER_SIZE] = res;
        wordsNum ++;

        return res;
    }
    else{
        return wordUnit{"END", "-1"};
    }
}


