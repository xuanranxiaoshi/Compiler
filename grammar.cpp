//
// Created by csc on 2022/4/20.
//

#include <iostream>
#include <set>
#include <fstream>
using namespace std;

struct wordUnit{                                   // 词单元结构
    string token;
    string lexme;
};


wordUnit word_now;
ofstream out;
set<string> functions_with_return;
set<string> functions_no_return;



extern int row;
extern wordUnit getWordUnit();                     // 获取一个词单元
extern void back(int step);


void getString();
void getProgram(const string& outputFile = "");
void getConstsDefine();
void getConstStatement();
void getUnsignedInt();
void getInt();
void getStatementHeader();
void getConst();
void getVarStatement();
void getVarDefine();
void getVarDefine_noInitialize();
void getVarDefine_withInitialize();
void getFunctionDefine_withReturn();
void getFunctionDefine_noReturn();
void getCompoundStatement();
void getParameterTable();
void getMainFunction();
void getExpression();
void getItem();
void getFactor();
void getStatement();
void getAssignStatement();
void getConditionStatement();
void getCondition();
void getCirculationStatement();
void getStep();
void getSwitchStatement();
void getSwitchTable();
void getSwitchSubStatement();
void getDefaultStatement();
void invokingFunction_withReturn();
void invokingFunction_noReturn();
void valueParameterTable();
void getStatementBlock();
void getReadStatement();
void getWriteStatement();
void returnStatement();


void getFunctionDefine_common();



void error(const string &token, const string& target);
void printWord(const wordUnit& word);
bool match(const wordUnit& word, const string &target);
void output(const string& s);

void error(const string &token, const string& target){
    cout << "[ERROR]: expect \"" + target + "\" but get \"" + token +"\", line: " ;
    cout << row << endl;
}
void printWord(const wordUnit& word){
    if(out.is_open()){
        out << word.token << " " << word.lexme << endl;
    }else{
        cout << word.token << " " << word.lexme << endl;
    }

}
bool match(const wordUnit& word, const string &target){
    if(word.token == target){
        printWord(word);
        return true;
    }
    else{
        error(word.token, target);
        return false;
    }
}

void output(const string& s){
    if(out.is_open()){
        out << s << endl;
    }else{
        cout << s << endl;
    }
}

void getProgram(const string& outputFile){
    // 指定输出文件
    if(!outputFile.empty()){
        out.open(outputFile);
        if(!out.is_open()){
            cout << "输出文件打开失败" << endl;
        }
    }

    word_now = getWordUnit();

    // 常量说明
    if(word_now.token == "CONSTTK"){
        back(1);
        getConstStatement();        // 带循环
        word_now = getWordUnit();   // 更新词单元
    }

    // 变量说明或者又返回值函数说明（再向前看2个）；
    while(word_now.token == "INTTK" || word_now.token == "CHARTK" || word_now.token == "VOIDTK"){
        wordUnit name, sign;
        name = getWordUnit(), sign = getWordUnit();
        back(2); //退回两个词法单元

        // 主函数
        if(word_now.token == "VOIDTK" && name.token == "MAINTK"){
            back(1);
            break;
        }
        // 无返回值函数定义
        else if(word_now.token == "VOIDTK"){
            back(1);
            getFunctionDefine_noReturn();
        }
        //  有返回值的函数定义
        else if(sign.token == "LPARENT"){
            back(1);
            getFunctionDefine_withReturn();
        }
        //变量说明
        else{
            back(1);
            getVarStatement();
        }

        word_now = getWordUnit();
    }

    // 主函数
    getMainFunction();

    // 结果输出处理；
    output("<程序>");

    if(out.is_open()){
        out.close();
    }
}

void getConstStatement(){
    word_now = getWordUnit();
    while(word_now.token == "CONSTTK"){
        printWord(word_now);        // const

        getConstsDefine();          // 常量定义

        word_now = getWordUnit();   // 分号
        match(word_now, "SEMICN");

        word_now = getWordUnit();   // 读入一个词
    }
    back(1);
    // 结果处理
    output("<常量说明>");
}

void getConstsDefine(){
    bool integer = true;
    word_now = getWordUnit();

    // 处理多个定义
    while(word_now.token == "INTTK" || word_now.token == "CHARTK"){
        if(word_now.token != "INTTK") integer = false;
        // 处理一次定义多个变量
        do{
            printWord(word_now);        // int/char 终结符

            word_now = getWordUnit();   // 标识符
            match(word_now, "IDENFR");

            word_now = getWordUnit();   // =
            match(word_now, "ASSIGN");

            if(integer) getInt();       // 整数
            else{                       // 字符
                word_now = getWordUnit();
                match(word_now, "CHARCON");
            }

            word_now = getWordUnit();
        } while (word_now.token == "COMMA");
        back(1);
    }
    // 结果处理
    output("<常量定义>");
}

void getInt(){
    word_now = getWordUnit();
    if(word_now.token == "PLUS" || word_now.token == "MINU"){
        printWord(word_now);    // 正负号
    }else{
        back(1);
    }
    getUnsignedInt();           // 无符号整数；
    // 结果处理
    output("<整数>");
}

void getConst(){
    word_now = getWordUnit();
    if(word_now.token == "CHARCON") printWord(word_now);
    else{
        back(1);
        getInt();
    }

    output("<常量>");
}

void getUnsignedInt(){
    // 无符号整数，对应词法分析中的INTTK
    word_now = getWordUnit();
    match(word_now, "INTCON");
    // 结果处理
    output("<无符号整数>");
}

void getVarStatement(){
    bool hasVar = true;
    do {
        getVarDefine();             // 变量定义

        word_now = getWordUnit();   // ；
        match(word_now, "SEMICN");

        // 判断是否还有变量
        word_now = getWordUnit();
        if(word_now.token == "VOIDTK"){
            hasVar = false;
            back(1);
        }

        else if(word_now.token == "INTTK" || word_now.token == "CHARTK"){
            getWordUnit();
            word_now = getWordUnit();
            if(word_now.token == "LPARENT"){
                hasVar = false;
                back(3);
            }else{
                back(3);
            }
        }
        else{
            hasVar = false;
            back(1);
        }
    }while(hasVar);

    output("<变量说明>");
}

void getVarDefine(){
    wordUnit words[9];
    for(int i = 0; i < 9; i++) words[i] = getWordUnit();
//    for(auto & word : words) word = getWordUnit();  // 向前看9个单元

    back(9);

    if(words[2].token == "ASSIGN" ||
        words[4].token == "RBRACK" && words[5].token == "ASSIGN" ||
        words[7].token == "RBRACK" && words[8].token == "ASSIGN"){

        getVarDefine_withInitialize();
    }else{
        getVarDefine_noInitialize();
    }



//    if(words[2].token == "ASSIGN" || words[5].token == "ASSIGN" || words[8].token == "ASSIGN") {
//        getVarDefine_withInitialize();
//    }else {
//        getVarDefine_noInitialize();
//    }

    // 结果处理
    output("<变量定义>");
}

void getVarDefine_noInitialize(){
    // 只考虑识别一行就行， 在变量说明中考虑多行的情况

    word_now = getWordUnit();

    if(word_now.token == "INTTK" || word_now.token == "CHARTK"){    // 类型标识符
        printWord(word_now);
    }else{
        error(word_now.token, "INTTK or CHARTK");
    }

    do{
        word_now = getWordUnit();
        match(word_now, "IDENFR");            // 标识符

        word_now = getWordUnit();
        // 一维数组
        if(word_now.token == "LBRACK"){
            printWord(word_now);                    // [

            getUnsignedInt();                       // 无符号整数

            word_now = getWordUnit();
            match(word_now, "RBRACK");       // ]

            // 二维数组
            word_now = getWordUnit();               // [
            if(word_now.token == "LBRACK"){
                printWord(word_now);

                getUnsignedInt();                   // 无符号整数

                word_now = getWordUnit();
                match(word_now, "RBRACK");   // ]

                word_now = getWordUnit();
            }
        }
        if(word_now.token == "COMMA") printWord(word_now);
    } while (word_now.token == "COMMA"); // 逗号；

    back(1);

    // 结果处理
    output("<变量定义无初始化>");
}

void getVarDefine_withInitialize(){
    word_now = getWordUnit();

    int dimension = 0;

    if(word_now.token == "INTTK" || word_now.token == "CHARTK"){    // 类型标识符
        printWord(word_now);
    }else{
        error(word_now.token, "INTTK or CHARTK");
    }

    word_now = getWordUnit();
    match(word_now, "IDENFR");        // 标识符

    // 变量初始化
    word_now = getWordUnit();
    if(word_now.token == "ASSIGN"){         // =
        printWord(word_now);
        getConst();                         // 常量
    }
    else{
        // 一维数组
        dimension = 1;

        match(word_now, "LBRACK");                    // [
        getUnsignedInt();                                   // 无符号整数
        word_now = getWordUnit();
        match(word_now, "RBRACK");                    // ]

        word_now = getWordUnit();
        // 二维数组
        if(word_now.token == "LBRACK"){
            dimension = 2;

            match(word_now, "LBRACK");                // [
            getUnsignedInt();                               // 无符号整数
            word_now = getWordUnit();
            match(word_now, "RBRACK");                // ]

            word_now = getWordUnit();
        }

        match(word_now, "ASSIGN");                    // =

        word_now = getWordUnit();
        match(word_now, "LBRACE");                    // {

        if(dimension == 1){
            do{
                getConst();                     // 常量

                word_now = getWordUnit();                   // ,
                if(word_now.token == "COMMA")
                    printWord(word_now);
            }while(word_now.token == "COMMA");

            match(word_now, "RBRACE");                // }
        }else{
            do{
                word_now = getWordUnit();                   // {
                match(word_now, "LBRACE");

                do{
                    getConst();                             // 常量
                    word_now = getWordUnit();
                    if(word_now.token == "COMMA")
                        printWord(word_now);                // ,
                }while(word_now.token == "COMMA");

                match(word_now, "RBRACE");           // }

                word_now = getWordUnit();
                if(word_now.token == "COMMA")
                    printWord(word_now);                    // ,

            }while(word_now.token == "COMMA");

            match(word_now, "RBRACE");               // }
        }
    }

    // 结果处理
    output("<变量定义及初始化>");
}

void getFunctionDefine_common(){
    word_now = getWordUnit();
    match(word_now, "LPARENT");      //（

    getParameterTable();                   // 参数表

    word_now = getWordUnit();
    match(word_now, "RPARENT");     // ）

    word_now = getWordUnit();
    match(word_now, "LBRACE");      // {

    getCompoundStatement();                // 复合语句

    word_now = getWordUnit();
    match(word_now, "RBRACE");      // }
}

void getFunctionDefine_withReturn(){
    getStatementHeader();                  // 声明头部
    getFunctionDefine_common();

    output("<有返回值函数定义>");
}

void getFunctionDefine_noReturn(){
    word_now = getWordUnit();
    match(word_now, "VOIDTK");      // void

    word_now = getWordUnit();
    match(word_now, "IDENFR");      // 标识符

    // 记录无返回值的函数
    functions_no_return.insert(word_now.lexme);

    getFunctionDefine_common();

    output("<无返回值函数定义>");
}

void getStatementHeader(){
    word_now = getWordUnit();
    if(word_now.token == "INTTK" || word_now.token == "CHARTK"){
        printWord(word_now);                                // int | char
    }else{
        error(word_now.token, "int or char");
    }

    word_now = getWordUnit();
    match(word_now, "IDENFR");                      // 标识符

    // 记录有返回值的函数
    functions_with_return.insert(word_now.lexme);

    // 结果处理
    output("<声明头部>");
}

void getParameterTable(){
    word_now = getWordUnit();
    // 为空的情况
    if(word_now.token == "RPARENT"){
        back(1);
    }else{
        back(1);
        do{
            word_now = getWordUnit();
            if(word_now.token == "INTTK" || word_now.token == "CHARTK"){
                printWord(word_now);                            // int | char
            }else{
                error(word_now.token, "int or char");
            }

            word_now = getWordUnit();                          // id
            match(word_now, "IDENFR");

            word_now = getWordUnit();
            if(word_now.token == "COMMA") printWord(word_now);
        } while (word_now.token == "COMMA");

        back(1);
    }

    output("<参数表>");
}

void getCompoundStatement(){
    word_now = getWordUnit();

    // 常量说明
    if(word_now.token == "CONSTTK"){
        back(1);
        getConstStatement();
        word_now = getWordUnit();
    }
    // 变量说明
    if(word_now.token == "INTTK" || word_now.token == "CHARTK"){
        back(1);
        getVarStatement();
        word_now = getWordUnit();
    }

    back(1);
    getStatementBlock();

    output("<复合语句>");
}

void getStatementBlock(){
    word_now = getWordUnit();
    while (word_now.token != "RBRACE"){
        back(1);
        getStatement();

        word_now = getWordUnit();
    }
    back(1);

    output("<语句列>");
}

void getStatement(){

    word_now = getWordUnit();
    if(word_now.token == "WHILETK" || word_now.token == "FORTK"){
        back(1);
        getCirculationStatement();              // 循环语句
    }
    else if(word_now.token == "IFTK"){
        back(1);
        getConditionStatement();                // 条件语句
    }
    else if(word_now.token == "SCANFTK"){
        back(1);
        getReadStatement();                     // 读语句

        word_now = getWordUnit();
        match(word_now, "SEMICN");        // ;
    }
    else if(word_now.token == "PRINTFTK"){
        back(1);
        getWriteStatement();                    // 写语句

        word_now = getWordUnit();
        match(word_now, "SEMICN");        // ;
    }
    else if(word_now.token == "SWITCHTK"){
        back(1);
        getSwitchStatement();                   // 情况语句
    }
    else if(word_now.token == "RETURNTK"){
        back(1);
        returnStatement();                      // 返回语句
        word_now = getWordUnit();
        match(word_now, "SEMICN");        // ;
    }
    else if(word_now.token == "LBRACE"){
        printWord(word_now);                    // {
        getStatementBlock();                    // 语句列

        word_now = getWordUnit();
        match(word_now, "RBRACE");        // }
    }
    else if(word_now.token == "SEMICN"){
        printWord(word_now);                    // 空;
    }
    else if(word_now.token == "IDENFR"){
        wordUnit sign = getWordUnit();

        if(sign.token == "ASSIGN" || sign.token == "LBRACK"){
            back(2);
            getAssignStatement();               // 赋值语句
            word_now = getWordUnit();
            match(word_now, "SEMICN");   // ;
        }
        // 有返回值函数和无返回值函数调用语句
        else if(sign.token == "LPARENT"){
            back(2);
            if(functions_no_return.count(word_now.lexme) != 0){
                invokingFunction_noReturn();    // 无返回值调用
            }
            else if(functions_with_return.count(word_now.lexme) != 0){
                invokingFunction_withReturn();  // 有返回值调用
            }
            else{
                error(word_now.token, "function");
            }
            word_now = getWordUnit();
            match(word_now, "SEMICN");   // ;
        }
        else{
            error(word_now.token, "assign or invoking function");
        }
    }
    else{
        error(word_now.token, "statement");
    }

    output("<语句>");
}

void getCirculationStatement(){
    word_now = getWordUnit();
    if(word_now.token == "WHILETK"){
        printWord(word_now);                    // while

        word_now = getWordUnit();
        match(word_now, "LPARENT");      // (

        getCondition();                         // 条件

        word_now = getWordUnit();
        match(word_now, "RPARENT");       // )

        getStatement();                         // 语句

    }else if(word_now.token == "FORTK"){
        printWord(word_now);                    // for

        word_now = getWordUnit();
        match(word_now, "LPARENT");      // (
        word_now = getWordUnit();
        match(word_now, "IDENFR");       // id
        word_now = getWordUnit();
        match(word_now, "ASSIGN");       // =
        getExpression();                        // 表达式
        word_now = getWordUnit();
        match(word_now, "SEMICN");        //;

        getCondition();                         // 条件
        word_now = getWordUnit();
        match(word_now, "SEMICN");        // ;

        word_now = getWordUnit();
        match(word_now, "IDENFR");       // id
        word_now = getWordUnit();
        match(word_now, "ASSIGN");       // =
        word_now = getWordUnit();
        match(word_now, "IDENFR");       // id
        word_now = getWordUnit();               // + | -
        if(word_now.token == "PLUS" || word_now.token == "MINU"){
            printWord(word_now);
        }else{
            error(word_now.token, "+ or -");
        }
        getStep();                              // 步长
        word_now = getWordUnit();
        match(word_now, "RPARENT");      // )

        getStatement();                         // 语句

    }else{
        error(word_now.token, "while or for");
    }

    output("<循环语句>");
}

void getCondition(){
    getExpression();                // 表达式

    word_now = getWordUnit();       // 关系运算符
    if(word_now.token == "LSS" || word_now.token == "LEQ" || word_now.token == "GRE" ||
        word_now.token == "GEQ" || word_now.token == "NEQ" || word_now.token == "EQL"){
        printWord(word_now);
    }else{
        error(word_now.token, "relation operations");
    }

    getExpression();                // 表达式

    output("<条件>");
}

void getExpression(){
    word_now = getWordUnit();       // [+ | -]
    if(word_now.token == "PLUS" || word_now.token == "MINU"){
        printWord(word_now);
        word_now = getWordUnit();
    }

    back(1);
    getItem();                      // 项

    word_now = getWordUnit();
    while (word_now.token == "PLUS" || word_now.token == "MINU"){
        printWord(word_now);        // 加法运算符

        getItem();                  // 项

        word_now = getWordUnit();
    }

    back(1);

    output("<表达式>");
}

void getItem(){
    getFactor();                    // 因子

    word_now = getWordUnit();
    while(word_now.token == "MULT" || word_now.token == "DIV"){
        printWord(word_now);        // 乘法符号

        getFactor();                // 因子

        word_now = getWordUnit();
    }

    back(1);

    output("<项>");
}

void getFactor(){
    word_now = getWordUnit();
    if(word_now.token == "CHARCON"){             // 字符
        printWord(word_now);
    }
    else if(word_now.token == "PLUS" || word_now.token == "MINU" || word_now.token == "INTCON"){
        back(1);
        getInt();                               // 整数
    }
    else if(word_now.token == "LPARENT"){
        printWord(word_now);                    // (
        getExpression();                        // 表达式

        word_now = getWordUnit();               // )
        match(word_now, "RPARENT");
    }
    else if(word_now.token == "IDENFR"){
        wordUnit temp = word_now;

        word_now = getWordUnit();
        if(word_now.token == "LBRACK"){
            printWord(temp);                    // 标识符
            printWord(word_now);                // [
            getExpression();                    // 表达式
            word_now = getWordUnit();           // ]
            match(word_now, "RBRACK");

            word_now = getWordUnit();
            if(word_now.token == "LBRACK"){
                printWord(word_now);            // [
                getExpression();                // 表达式
                word_now = getWordUnit();       // ]
                match(word_now, "RBRACK");
            }else{
                back(1);
            }
        }
        else if (word_now.token == "LPARENT"){  // look (
            back(2);
            invokingFunction_withReturn();      // 有返回值函数调用
        }else{
            printWord(temp);                // 标识符
            back(1);
        }
    }
    else{
        error(word_now.token, "factor");
    }

    output("<因子>");
}

void invokingFunction_withReturn(){
    word_now = getWordUnit();
    match(word_now, "IDENFR");   // 标识符
    word_now = getWordUnit();
    match(word_now, "LPARENT");  // (
    valueParameterTable();             // 值参数表
    word_now = getWordUnit();
    match(word_now, "RPARENT");  // )

    output("<有返回值函数调用语句>");
}

void valueParameterTable(){
    word_now = getWordUnit();
    if(word_now.token == "RPARENT"){
        back(1);                // 空
    }else{
        back(1);
        getExpression();             // 表达式
        word_now = getWordUnit();
        while(word_now.token == "COMMA"){
            printWord(word_now);    // ,

            getExpression();        // 表达式

            word_now = getWordUnit();
        }
        back(1);
    }

    output("<值参数表>");
}

void getStep(){
    word_now = getWordUnit();
    match(word_now, "INTCON");

    output("<步长>");
}

void getConditionStatement(){
    word_now = getWordUnit();
    match(word_now, "IFTK");    // if
    word_now = getWordUnit();
    match(word_now, "LPARENT"); // (
    getCondition();                    // 条件
    word_now = getWordUnit();
    match(word_now, "RPARENT"); // )
    getStatement();                    // 语句

    word_now = getWordUnit();
    while(word_now.token == "ELSETK"){
        printWord(word_now);          // else
        getStatement();               // 语句
        word_now = getWordUnit();
    }
    back(1);

    output("<条件语句>");
}

void invokingFunction_noReturn(){
    word_now = getWordUnit();
    match(word_now, "IDENFR");   // 标识符
    word_now = getWordUnit();
    match(word_now, "LPARENT");  // (
    valueParameterTable();             // 值参数表
    word_now = getWordUnit();
    match(word_now, "RPARENT");  // )

    output("<无返回值函数调用语句>");
}

void getAssignStatement(){
    word_now = getWordUnit();
    match(word_now, "IDENFR");        // 标识符
    word_now = getWordUnit();
    if(word_now.token == "ASSIGN"){
        printWord(word_now);                // =
        getExpression();                    // 表达式
    }
    else if(word_now.token == "LBRACK"){
        printWord(word_now);                // [
        getExpression();                    // 表达式
        word_now = getWordUnit();
        match(word_now, "RBRACK");   // ]

        word_now = getWordUnit();
        if(word_now.token == "ASSIGN"){
            printWord(word_now);            // =
            getExpression();                // 表达式
        }
        else if(word_now.token == "LBRACK"){
            printWord(word_now);                // [
            getExpression();                    // 表达式
            word_now = getWordUnit();
            match(word_now, "RBRACK");   // ]
            word_now = getWordUnit();
            match(word_now, "ASSIGN");   // =
            getExpression();                    // 表达式
        }
        else{
            error(word_now.token, "= or [");
        }
    }
    else{
        error(word_now.token, "= or [");
    }

    output("<赋值语句>");
}

void getReadStatement(){
    word_now = getWordUnit();
    match(word_now, "SCANFTK");     // scanf
    word_now = getWordUnit();
    match(word_now, "LPARENT");     // (
    word_now = getWordUnit();
    match(word_now, "IDENFR");      // 标识符
    word_now = getWordUnit();
    match(word_now, "RPARENT");     // )

    output("<读语句>");
}

void getWriteStatement(){
    word_now = getWordUnit();
    match(word_now, "PRINTFTK");              // printf
    word_now = getWordUnit();
    match(word_now, "LPARENT");               // (

    word_now = getWordUnit();
    if(word_now.token == "STRCON"){
        back(1);
        getString();                                // 字符串

        word_now = getWordUnit();
        if(word_now.token == "COMMA"){
            printWord(word_now);                    // ,
            getExpression();                        // 表达式
            word_now = getWordUnit();
            match(word_now, "RPARENT");       // )
        }else{
            match(word_now, "RPARENT");
        }
    }else{
        back(1);
        getExpression();                            // 表达式
        word_now = getWordUnit();
        match(word_now, "RPARENT");           // )
    }

    output("<写语句>");
}

void getSwitchStatement(){
    word_now = getWordUnit();
    match(word_now, "SWITCHTK");              // case
    word_now = getWordUnit();
    match(word_now, "LPARENT");               // (
    getExpression();                                // 表达式
    word_now = getWordUnit();
    match(word_now, "RPARENT");               // )
    word_now = getWordUnit();
    match(word_now, "LBRACE");                // {
    getSwitchTable();                               // 情况表
    getDefaultStatement();                          // 缺省
    word_now = getWordUnit();
    match(word_now, "RBRACE");                // }

    output("<情况语句>");
}

void getSwitchTable(){
    word_now = getWordUnit();
    while (word_now.token == "CASETK"){
        back(1);
        getSwitchSubStatement();                    // 情况子语句

        word_now = getWordUnit();
    }
    back(1);

    output("<情况表>");
}

void getSwitchSubStatement(){
    word_now = getWordUnit();
    match(word_now, "CASETK");               // case;
    getConst();                                     // 常量；
    word_now = getWordUnit();
    match(word_now, "COLON");                // :
    getStatement();                                // 语句

    output("<情况子语句>");
}

void getDefaultStatement(){
    word_now = getWordUnit();
    match(word_now, "DEFAULTTK");           // default
    word_now = getWordUnit();
    match(word_now, "COLON");               // :
    getStatement();                               // 语句

    output("<缺省>");
}

void returnStatement(){
    word_now = getWordUnit();
    match(word_now, "RETURNTK");            // return

    word_now = getWordUnit();
    if(word_now.token == "LPARENT"){
        printWord(word_now);                       // (
        getExpression();                           // 表达式
        word_now = getWordUnit();
        match(word_now, "RPARENT");         // )
    }else{
        back(1);
    }

    output("<返回语句>");
}

void getMainFunction(){
    word_now = getWordUnit();
    match(word_now, "VOIDTK");  // void
    word_now = getWordUnit();
    match(word_now, "MAINTK");  // main
    word_now = getWordUnit();
    match(word_now, "LPARENT");  // (
    word_now = getWordUnit();
    match(word_now, "RPARENT");  // )
    word_now = getWordUnit();
    match(word_now, "LBRACE");   // {

    getCompoundStatement();            // 复合语句

    word_now = getWordUnit();
    match(word_now, "RBRACE");  // }


    output("<主函数>");
}

void getString(){
    word_now = getWordUnit();
    match(word_now, "STRCON");

    output("<字符串>");
}
