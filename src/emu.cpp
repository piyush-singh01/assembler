/*
Name:Piyush Singh
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
#define HALT 18

int SP,PC,A,B;
int memory[1<<24];

std::map<std::string,std::pair<int, int>> mnemonics;
std::map<int, std::string> opCodeToMnem;

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

    /**/
    opCodeToMnem.insert({0, "ldc"});
    opCodeToMnem.insert({1, "adc"});
    opCodeToMnem.insert({2, "ldl"});
    opCodeToMnem.insert({3, "stl"});
    opCodeToMnem.insert({4, "ldnl"});
    opCodeToMnem.insert({5, "stnl"});
    opCodeToMnem.insert({6, "add"});
    opCodeToMnem.insert({7, "sub"});
    opCodeToMnem.insert({8, "shl"});
    opCodeToMnem.insert({9, "shr"});
    opCodeToMnem.insert({10,"adj"});
    opCodeToMnem.insert({11,"a2sp"});
    opCodeToMnem.insert({12,"sp2a"});
    opCodeToMnem.insert({13,"call"});
    opCodeToMnem.insert({14,"return"});
    opCodeToMnem.insert({15,"brz"});
    opCodeToMnem.insert({16,"brlz"});
    opCodeToMnem.insert({17,"br"});
    opCodeToMnem.insert({18,"HALT"});
}


/****************************** UTILITIES ******************************/
void helpWithFormat(){
    std::cout<<"Correct Format: ./emu [options] filename.o"<<std::endl;
    std::cout<<"-trace: show instruction trace"<<std::endl;
    std::cout<<"-before: memory dump before program execution"<<std::endl;
    std::cout<<"-after: memory dump after program execution"<<std::endl;
    std::cout<<"-isa: Display the instruction set architecture";
}

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

int convertBinToNum(const std::string &machineCode){
    int res = 0;
    for(int i = 0;i<32;i++){
        res *=2;
        if(machineCode[i] == '1'){
            res++;
        }
    }
    return res;
}

void printMemDumpFile(int lineNum, std::ofstream &outputFile){
    for(int i = 0;i<lineNum;i++){
        if(i%4 == 0){
            outputFile << convertToHex(i) << std::endl;
        }
        outputFile << convertToHex(memory[i]) <<" ";
    }
    outputFile << "\n";
    return;
}

void printMemDump(int lineNum, bool isBefore){
    if(isBefore){
        std::cout<<std::endl<<"Memory Dump before the execution pf program:"<<std::endl;
    } else {
        std::cout<<std::endl<<"Memory Dump after the execution of program:"<<std::endl;
    }
    for(int i = 0;i<lineNum;i++){
        std::string inHex = convertToHex(i);
        std::string memInHex = convertToHex(memory[i]);
        if(i%4 == 0){
            std::cout << std::endl <<inHex << "  ";
        }
        std::cout << memInHex << "  ";
    }
    std::cout<<std::endl;
    return;
}

int readMachineCode(std::ifstream &machineCode){
    std::string instruction;
    int lineNum = 0;
    while(getline(machineCode, instruction)){
        int num = convertBinToNum(instruction);
        memory[lineNum] = num;
        lineNum++;
    }
    return lineNum;
}

int mnemType(const std::string &mnem){
    if(mnemonics.find(mnem) == mnemonics.end()){
        return -1;
    } else {
        return mnemonics[mnem].second;
    }
}

std::string printMnemType(const std::string &mnem){
    int mt = mnemType(mnem);
    if(mt == VALUE_TYPE){
        return "Value Operand";
    } else if(mt == OFFSET_TYPE){
        return "Offset Operand";
    } else {
        return "No Operand";
    }
}

void printISA(){
    std::cout<<"OpCode \t Mnemonic \t Operand"<<std::endl;
    for(int i = 0;i<=HALT;i++){
        std::cout<<i<<"\t"<<opCodeToMnem[i]<<"\t"<<mnemType(opCodeToMnem[i]) << "\t" << printMnemType(opCodeToMnem[i])<<std::endl;
    }
    std::cout<<"SET::Value";
}

int fetchMnemCode(int instruct) {
    int val = ((1 << 8) - 1) & instruct;
	return val;
}

int fetchOperand(int instruct) {
	return (instruct >> 8);
}

void showTrace(int opCode, int operand){
    std::cout << "PC = " << convertToHex(PC) << ", SP = " << convertToHex(SP) << ", A = " << convertToHex(A) << ", B = " << convertToHex(B) << " " << opCodeToMnem[opCode] << "\t" << convertToHex(operand) << std::endl;
}


/****************************** OPERATIONS ******************************/
void ldc(int operand) {
	B = A;
	A = operand;
}
void adc(int operand) {
	A += operand;
}
void ldl(int offset) {
	B = A;
	A = memory[SP + offset];
}
void stl(int offset) {
	memory[SP + offset] = A;
	A = B;
}
void ldnl(int offset) {
	A = memory[A + offset];
}
void stnl(int offset) {
	memory[A + offset] = B;
}
void add() {
	A += B;
}
void sub() {
	A = B - A;
}
void shl() {
	A = B << A;
}
void shr() {
	A = B >> A;
}
void adj(int value) {
	SP += value;
}
void a2sp() {
	SP = A;
	A = B;
}
void sp2a() {
	B = A;
	A = SP;
}
void call(int offset) {
	B = A;
	A = PC;
	PC += offset;
}
void _return() {
	PC = A;
	A = B;
}
void brz(int offset) {
	if (A == 0) 
    {
        PC += offset;
    }
}
void brlz(int offset) {
	if (A < 0) {
        PC += offset;
    }
}
void br(int offset) {
	PC += offset;
} 



int executeInstructions(int opCode, int operand){
    if (opCode == 0)  ldc(operand);
	if (opCode == 1)  adc(operand);
	if (opCode == 2)  ldl(operand);
	if (opCode == 3)  stl(operand);
	if (opCode == 4)  ldnl(operand);
	if (opCode == 5)  stnl(operand);
	if (opCode == 6)  add();
	if (opCode == 7)  sub();
	if (opCode == 8)  shl();
	if (opCode == 9)  shr();
	if (opCode == 10) adj(operand);
	if (opCode == 11) a2sp();
	if (opCode == 12) sp2a();
	if (opCode == 13) call(operand);
	if (opCode == 14) _return();
	if (opCode == 15) brz(operand);
	if (opCode == 16) brlz(operand);
	if (opCode == 17) br(operand);
    else {
        return 0;
    }
    return 1;
}

int executeTrace(bool isTrace){
    SP = (1<<23) - 1;
    int operand = 0;
    int opcode = 0;
    int instructNum = 0;
    while((opcode = fetchMnemCode(memory[PC])) <= 18){
        operand = fetchOperand(memory[PC]);
        if(isTrace){
            showTrace(opcode, operand);
        }
        if(opcode == HALT){
            break;
        }
        if(opcode > HALT){
            return 1;
        }
        PC++;
        executeInstructions(opcode, operand);
        instructNum++;
    }
    std::cout<<"Number of instructions executed: "<<instructNum;
    return 0;
}



/****************************** MAIN ******************************/
int main(int argc, char* argv[]){
    initMnem();
    std::ifstream machineCode;
    std::ofstream out;

    if(argc < 3){
        std::cout<<"Invalid Input Format\n";
        helpWithFormat();
        return 0;
    }
    
    std::string file;
    file = argv[2];
    std::string fileName;
    for(int i = 0;i<(int)file.size();i++){
        if(argv[2][i] == '.'){
            break;
        }
        fileName.push_back(argv[2][i]);
    }
    std::string memDump;
    memDump = fileName + "_memDump.txt";

    machineCode.open(argv[2], std::ios::in | std::ios::binary);
    out.open(memDump, std::ios::out);
    if(!machineCode.is_open()){
        std::cout<<"Can not find the specified input file. Terminating Program\n";
        helpWithFormat();
        return 0;
    }
    bool isTrace = false;
    bool isBefore = false;
    bool isAfter = false;
    bool isISA = false;
    std::string str = argv[1];
    if(str == "-trace"){
        isTrace = true;
    }
    if(str == "-before"){
        isBefore = true;
    }
    if(str == "-after"){
        isAfter = true;
    }
    if(str == "-isa"){
        isISA = true;
    }
    if(!(isTrace || isBefore || isAfter || isISA)){
        helpWithFormat();
        return 0;
    }
    int instructionCount = readMachineCode(machineCode);
    if(isBefore){
        printMemDump(instructionCount, true);
    }
    if(isISA){
        printISA();
    }
    executeTrace(isTrace);

    if(isAfter){
        printMemDump(instructionCount, false);
    }
    printMemDumpFile(instructionCount, out);

    machineCode.close();
    out.close();
    return 0;
}