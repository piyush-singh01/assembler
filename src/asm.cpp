/*
Name: Piyush Singh
Roll Number: 2001CS51
*/

#include<iostream>
#include<fstream>
#include<string>
#include<sstream>
#include<vector>
#include<map>
#include<set>
#include<utility>

#define INVALID -1
#define VALUE_TYPE 1
#define DATA_TYPE 2
#define SET_TYPE 3
#define OFFSET_TYPE 4
const std::string emptyString = "";
const std::string WHITESPACES = " \n\r\t\f\v";
bool hasError = false;


typedef struct symbol{
    std::string symbolLabel;
    int address;
    bool set;
    int set_val;
}symbol;

typedef struct literal{
    int literalName;
    int address;
}literal;

std::vector<symbol> symbolTable;
std::vector<literal> literalTable;

std::map<std::string,std::pair<int, int>> mnemonics;
std::vector<std::string> errorList(20);
std::set<std::pair<int,std::string>> errors;

bool findInSymbolTable(const std::string &label){
    for(const auto &it:symbolTable){
        if(it.symbolLabel == label){
            return true;
        }
    }
    return false;
}


int getAddressFromSymbolTable(const std::string &param){
    for(const auto &it: symbolTable){
        if(it.symbolLabel == param){
            return it.address;
        }
    }
    return -1;
}

void initMnem(){
    mnemonics.insert({ "ldc",    {0,    1}});
    mnemonics.insert({ "adc",    {1,    1}});
    mnemonics.insert({ "ldl",    {2,    4}});
    mnemonics.insert({ "stl",    {3,    4}});
    mnemonics.insert({ "ldnl",   {4,    4}});
    mnemonics.insert({ "stnl",   {5,    4}});
    mnemonics.insert({ "add",    {6,    0}});
    mnemonics.insert({ "sub",    {7,    0}});
    mnemonics.insert({ "shl",    {8,    0}});
    mnemonics.insert({ "shr",    {9,    0}});
    mnemonics.insert({ "adj",    {10,   1}});
    mnemonics.insert({ "a2sp",   {11,   0}});
    mnemonics.insert({ "sp2a",   {12,   0}});
    mnemonics.insert({ "call",   {13,   4}});
    mnemonics.insert({ "return", {14,   0}});
    mnemonics.insert({ "brz",    {15,   4}});
    mnemonics.insert({ "brlz",   {16,   4}});
    mnemonics.insert({ "br",     {17,   4}});
    mnemonics.insert({ "HALT",   {18,   0}});
    mnemonics.insert({ "data",   {NULL, 2}});
    mnemonics.insert({ "SET",    {NULL, 3}});
}

void initError(){
    errorList[0] = "Empty Label"; 
    errorList[1] = "Duplicate Label Name"; 
    errorList[2] = "Invalid Label Name";
    errorList[3] = "Invalid Mnemonic";
    errorList[4] = "Operand Required but Missing";
    errorList[5] = "Unexpected operand present";
    errorList[6] = "Too many operands";
    errorList[7] = "Not a Valid Number";
    errorList[8] = "Has Unassigned Labels";
    errorList[9] = "No such label exists";
}



/************************* FIRST PASS UTILITIES *************************/
std::string trimLeft(const std::string &s){
    size_t startPos = s.find_first_not_of(WHITESPACES);
    if(startPos == std::string::npos){
        return "";
    } else {
        return s.substr(startPos);
    }
}

std::string trimRight(const std::string &s){
    size_t endPos = s.find_last_not_of(WHITESPACES);
    if(endPos == std::string::npos){
        return "";
    } else {
        return s.substr(0, endPos + 1);
    }
}

std::string trim(const std::string &s){
    return trimRight(trimLeft(s));
}

std::string removeComment(std::string &line){
    std::string newLine;
    int len = (int)line.size();
    for(int i = 0;i<len;i++){
        if(line[i] == ';'){
            break;
        }
        newLine.push_back(line[i]);
    }
    return newLine;
}

int checkLabel(const std::string &line){
    int len = (int)line.size();
    for(int i = 0;i<len;i++){
        if(line[i] == ':'){
            return i;
        }
    }
    return -1;
}

int checkParam(const std::string &instruction){
    int len = (int)instruction.size();
    for(int i = 0;i<len;i++){
        if(instruction[i] == ' '){
            return i;
        }
    }
    return -1;
}

bool isAlphabet(const char &ch){
    if((ch >= 'a' && ch<= 'z') || (ch>='A' && ch<='Z')){
        return true;
    } else {
        return false;
    }
}

bool isNum(const char &ch){
    if(ch >= '0' && ch <= '9'){
        return true;
    } else {
        return false;
    }
}

bool isValidLabelName(const std::string &label){
    int len = (int)label.size();
    if(len == 0){
        return false;
    }
    if(!isAlphabet(label[0]) && label[0] != '_'){
        return false;
    }
    for(int i = 0;i<len;i++){
        if(!isNum(label[i]) && !isAlphabet(label[i]) && label[i] != '_'){
            return false;
        }
    }
    return true;
}

int mnemType(const std::string &mnem){
    if(mnemonics.find(mnem) == mnemonics.end()){
        return -1;
    } else {
        return mnemonics[mnem].second;
    }
}

bool operandRequired(int mnemType){
    if(mnemType >= 1){
        return true;
    } else {
        return false;
    }
}

bool isValidParamName(const std::string &param){
    int len = (int)param.size();
    if(len == 0){
        return false;
    }
    if(!isAlphabet(param[0]) && param[0] != '_'){
        return false;
    }
    for(int i = 0;i<len;i++){
        if(!isNum(param[i]) && !isAlphabet(param[i]) && param[i] != '_'){
            return false;
        }
    }
    return true;
}

bool extraWords(const std::string &param){
    std::stringstream ss(param);
    std::string tempWord;
    int count = 0;
    while(ss >> tempWord){
        count++;
    }
    if(count > 1){
        return true;
    } else {
        return false;
    }
}

bool isDec(const std::string &param){
    int len = (int)param.size();
    for(int i = 0;i<len;i++){
        if(param[i] == '-' || param[i] == '+'){
            continue;
        }
        if(param[i] > '9' || param[i] < '0'){
            return false;
        }
    } 
    return true;
}

bool isHex(const std::string &param){
    int len = (int)param.size();
    std::string prefix = param.substr(0,2);
    if(prefix != "0x"){
        return false;
    }
    for(int i = 2;i<len;i++){
        if(param[i] > '9' || param[i] < '0'){
            return false;
        }
    }
    return true;
}

bool isOct(const std::string &param){
    int len = (int)param.size();
    std::string prefix = param.substr(0,1);
    if(prefix != "0"){
        return false;
    }
    for(int i = 1;i<len;i++){
        if(param[i] > '9' || param[i] < '0'){
            return false;
        }
    }
    return true;
}

bool isValidNum(const std::string &param){
    if(isDec(param) || isHex(param) || isOct(param)){
        return true;
    } else {
        return false;
    }
}

bool hasUnassignedLabels(){
    for(const auto &it:symbolTable){
        if(it.address == -1){
            return true;
        }
    }
    return false;
}

int convertToNum(const std::string &param){
    if(isDec(param)){
        int num = std::stoi(param);
        return num;
    } else if(isHex(param)){
        unsigned int num;
        std::stringstream ss;
        ss << std::hex << param;
        ss >> num;
        return num;
    } else if(isOct(param)){
        unsigned int num;
        std::stringstream ss;
        ss << std::oct << param;
        ss >> num;
        return num;
    } else {
        return INVALID;
    }
}

void throwError(int errorCode, int lineNumber, std::ofstream &log){
    std::string error = errorList[errorCode];
    log<<"Error at Line "<<lineNumber<<" : "<<error<<std::endl;
    hasError = true;
    return;
}

void noErrorPassOne(std::ofstream &log){
    log<<"No error in Pass One"<<std::endl;
}

void noErrorPassTwo(std::ofstream &log){
    log<<"No error in Pass Two"<<std::endl;
}


/****************************** FIRST PASS ******************************/
int analysisPass(std::ifstream &src, std::ofstream &log){
    int PC = 0;
    int lineNum = 0;
    std::string line;
    while(getline(src,line)){
        lineNum++;

        line = trim(line);
        line = removeComment(line);
        if(line == emptyString){
            continue;
        }
        std::string instruction = "";
        std::string label = "";
        instruction = line;

        int labelIndex = checkLabel(line);
        if(labelIndex != -1){
            label = line.substr(0,labelIndex);
            instruction = line.substr(labelIndex + 1);
        }
        instruction = trim(instruction);
        
        // now we have the label in string label and the instruction after it in string instruction
        std::string mnem = instruction;
        std::string param = "";
        int paramIndex = checkParam(instruction);
        if(paramIndex != -1){
            mnem = instruction.substr(0,paramIndex);
            param = instruction.substr(paramIndex + 1);
        }
        mnem = trim(mnem);
        param = trim(param);
        
        /*Now we have label, mnem, param*/
        /*Check if label name is correct and not duplicate*/
        if(extraWords(param)){
            throwError(6, lineNum, log);
            continue;
        }
        if(labelIndex != -1){
            if((int)label.size() == 0){
                throwError(0, lineNum, log); //Empty label
                continue;
            }
            if(isValidLabelName(label)){
                if(findInSymbolTable(label)){
                    throwError(1, lineNum, log); //Duplicate label name
                    continue;
                } else {
                    symbol symNew;
                    symNew.symbolLabel = label;
                    symNew.address = PC;
                    symNew.set = false;
                    symNew.set_val = -1;
                    symbolTable.push_back(symNew);
                }
            } else {
                throwError(2, lineNum, log); //Invalid label name
                continue;
            }
        }

        if(mnem == emptyString){
            continue;
        }

        /*Check if mnemonic name is valid ...*/
        int mnemonicType = mnemType(mnem);
        if(mnemonicType == -1){
            throwError(3, lineNum, log);
            continue;
        }

        if(operandRequired(mnemonicType)){
            if(param == emptyString){
                throwError(4, lineNum, log);
                continue;
            } 
            if(!isValidParamName(param) && !isValidNum(param)){
                throwError(7, lineNum, log);
                continue;
            }
            if(isValidParamName(param)){

            } else if(isValidNum(param)){
                if(mnemonicType == SET_TYPE){
                    int num = convertToNum(param);
                    symbol symNew = {label, -1, true, num};
                    continue;
                }
            }

        } else {
            if(param != emptyString){
                throwError(5, lineNum, log);
                continue;
            }
        }
        PC++;
    }
    if(hasUnassignedLabels()){
        throwError(8,lineNum,log);
    }

    if(hasError == true){
        return -1;
    } else {
        noErrorPassOne(log);
        return 0;
    }
}



/************************* SECOND PASS UTLITIES *************************/
std::string convertToHex(const int &address){
    std::stringstream ss;
    std::string word;
    ss << std::hex << address;
    std::string hexVal = ss.str();
    int len = (int)hexVal.size();
    std::string result;
    int len0 = 8 - len;
    for(int i = 0;i<len0;i++){
        result.push_back('0');
    }
    result += hexVal;
    return result;
}

char digit[] = {'0', '1'};
std::string con32bitString(int n){
    std::string res;
    for(int i = 31;i>=0;i--){
        int setVal = (n>>i) & 1;
        char bit = digit[setVal];
        res.push_back(bit);
    }
    return res;
}

std::string con24bitString(int n){
    std::string res;
    for(int i = 23;i>=0;i--){
        int setVal = (n>>i) & 1;
        char bit = digit[setVal];
        res.push_back(bit);
    }
    return res;
}

std::string con8bitString(int n){
    std::string res;
    for(int i = 7;i>=0;i--){
        int setVal = (n>>i) & 1;
        char bit = digit[setVal];
        res.push_back(bit);
    }
    return res;
}

std::string fetchOPcode(const std::string &mnem){
    std::string opCode;
    opCode = mnemonics[mnem].first;
    return opCode;
}

bool isOffsetType(const std::string &mnem){
    if(mnemonics[mnem].second == OFFSET_TYPE){
        return true;
    } else {
        return false;
    }
}

bool assignOperandCode(std::string &operand, const std::string &param, const std::string &mnem, const int mnemonicType, int PC){
    int num = 0;
    if(mnemonicType == DATA_TYPE){
        operand = con32bitString(convertToNum(param));
        return true;
    }
    if(isValidNum(param)){
        num = convertToNum(param);
    } else if(isValidLabelName(param) && param != emptyString){
        num = getAddressFromSymbolTable(param);
    } else {
        return false;
    }

    if(isOffsetType(mnem) && isValidLabelName(param)){
        num = num - (PC + 1);
    }
    operand = con24bitString(num);
    return true;
}

unsigned int binToNum(const std::string &machineCode){
    unsigned int res = 0;
    for(long i = 31;i>=0;i--){
        if(machineCode[31 - i] == '1'){
                res += 1LL<<i;
        }
    }
    return res;
}

std::string convertToHexforBin(const int &machineCodeInt, bool require8bits){
    std::stringstream ss;
    std::string word;
    ss << std::hex << machineCodeInt;
    std::string hexVal = ss.str();
    int len = (int)hexVal.size();
    std::string result = "";
    std::string result2 = "";
    int len0 = 8 - len;
    for(int i = 0;i<len0;i++){
        result.push_back('0');
    }
    result += hexVal;
    result2 += hexVal;
    if(require8bits){
        return result;
    } else {
        return result2;
    }
}

std::string convertBinToHex(const std::string &machineCode, bool require8bits){
    int val = binToNum(machineCode);
    return convertToHexforBin(val, require8bits);
}


/****************************** SECOND PASS ******************************/
void synthesisPass(std::ifstream &src, std::ofstream &log, std::ofstream &object, std::ofstream &listing){
    int PC = 0;
    int lineNum = 0;
    std::string line;
    while(getline(src, line)){
        line = trim(line);
        line = removeComment(line);
        lineNum++;
        std::string label = "";
        std::string instruction = "";
        std::string mnem = "";
        std::string param = "";
        std::string opcode = "";
        std::string operand = "";
        std::string machineCode = "";
        if(line == emptyString){
            continue;
        }
        instruction = line;
        int labelIndex = checkLabel(line);
        if(labelIndex != -1){
            label = line.substr(0,labelIndex);
            instruction = line.substr(labelIndex + 1);
            std::string hexPC = convertToHex(PC);
            listing << hexPC << "\t\t\t\t" << label <<":\n";
        }
        instruction = trim(instruction);
        mnem = instruction;
        int paramIndex = checkParam(instruction);
        if(paramIndex != -1){
            mnem = instruction.substr(0,paramIndex);
            param = instruction.substr(paramIndex + 1);
        }
        mnem = trim(mnem);
        if(mnem == emptyString){
            continue;
        }
        param = trim(param);
        int mnemonicType = mnemType(mnem);
        opcode = fetchOPcode(mnem);

        if(mnemonicType == SET_TYPE){
            continue;
        }
        if(operandRequired(mnemonicType)){
            assignOperandCode(operand, param, mnem, mnemonicType, PC);
        } else {
            std::string zero = "0";
            assignOperandCode(operand, zero, mnem, mnemonicType, PC);
        }
        machineCode = machineCode + operand;
        if(mnemonicType != DATA_TYPE){
            opcode = con8bitString(mnemonics[mnem].first);
            machineCode = machineCode + opcode;
        }
        object << machineCode<< std::endl;
        std::string hexPC = convertToHex(PC);
        std::string hexMachine = convertBinToHex(machineCode, true);
        listing << hexPC << "\t" << convertBinToHex(machineCode, true) <<"\t" << mnem << "\t" << convertBinToHex(operand, false) << std::endl;
        PC++; 
    }
    noErrorPassTwo(log);
    return;
}



/****************************** MAIN ******************************/
int main(int argc, char* argv[])
{
    std::ifstream src;
    std::ifstream src2;
    std::ofstream log;
    std::ofstream object;
    std::ofstream listing;

    std::string correctFormat = "Correct Format: ./asm file_name.asm";
    if(argc != 2){
        std::cout<<"Error: Terminating Program, Invalid Input Format"<<std::endl;
        std::cout<<"Specify exactly one file name"<<std::endl;
        std::cout<<correctFormat;
        return 0;
    }
    std::string file = argv[1];

    std::string fileName;
    for(int i = 0;i<(int)file.size();i++){
        if(argv[1][i] == '.'){
            break;
        }
        fileName.push_back(argv[1][i]);
    }

    std::string logName = fileName + ".log";
    std::string objName = fileName + ".o";
    std::string listName = fileName + ".lst";
    

    src.open(file, std::ios::in);
    src2.open(file, std::ios::in);
    log.open(logName, std::ios::out);
    object.open(objName, std::ios::out);
    listing.open(listName, std::ios::out);
    
    if(!src.is_open()){
        std::cout<<"Error: Can not find the file: "<<file<<std::endl;
        std::cout<<"Program Terminated"<<std::endl;
        return 0;
    }

    initMnem();
    initError();
    
    int firstPass = analysisPass(src,log);
    if(firstPass == -1){
        return 0;
    }
    synthesisPass(src2,log,object,listing);
    
    src.close();
    log.close();
    object.close();
    listing.close();

    return 0;
}