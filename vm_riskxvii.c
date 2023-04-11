#include <stdlib.h>
#include <stdio.h>
//#include <string.h>
unsigned registers[32]={0};
int memory[2303];
short int pc=0;


//we use this method to extract  binary of specific length 
int break_binary2(int instruction, int start, int end){
    int bitmask=((1 << (end-start+1))-1) << start;
    int broken_binary= (instruction & bitmask) >> start;
    return broken_binary;
}

//modify the giventh bit
int modifyBit(int binary_number, int position, int bit){
    //
    int bit_mask =1 << position;
    int changed_num=((binary_number & ~bit_mask) | (bit << position));
    return changed_num;
}


void register_dump(){
    printf("PC = 0x%08x;\n", pc);
    for(int i=0;i<32;i++){
        printf("R[%d] = 0x%08x;\n", i, registers[i]);
    }
}

void unknown_instruction(int instruction){
    printf("Instruction Not Implemented: 0x%08x\n", instruction);
    register_dump();
    exit(0);
}

void illegal_operation(int instruction){
    printf("Illegal Operation: 0x%08x\n", instruction);
    register_dump();
    exit(0);
}


//checks which type of instruction it is 
//returns the type of instruction 
int opcode_type_check(int opcode){
    if(opcode==0b0110011){return 1;}
    else if(opcode==0b0010011){return 2;}
    else if(opcode==0b0100011){return 3;}
    else if(opcode==0b1100011){return 4;}
    else if(opcode==0b0110111){return 5;}
    else if(opcode==0b1101111){return 6;}
    else if(opcode==0b1100111){return 2;}
    else if(opcode==0b0000011){return 2;}
    return -1;
}

void heap_banks(){
    illegal_operation(memory[pc/4]);
}

//virtual routines
void virtual_routines(int memory_address, int stored_value){
    if(memory_address==0x800){
        memory[0x800]=stored_value;
        //we use casting to ensure the value that will be printed is of char type
        printf("%c",(char) memory[0x800]);
    }
    else if(memory_address==0x804){
        printf("%d", stored_value);
        
    }
    else if(memory_address==0x808){
        unsigned int value= (unsigned int) stored_value;
        printf("%x", value);
    }
    else if(memory_address==0x080c){
        printf("CPU Halt Requested\n");
        exit(0);
    }
    else if(memory_address==0x812){
        char ch;
        scanf("%c", &ch);
        memory[0x812]=(int) ch;
    }
    else if(memory_address==0x0816){
        int scanned_int;
        scanf("%d", &scanned_int);
        memory[0x816]=scanned_int;
    }
    else if(memory_address==0x820){
        printf("%x", pc);
    }
    else if(memory_address==0x824){
        register_dump();
    }
    else if(memory_address==0x828){
        unsigned int value= (unsigned int) stored_value;
        printf("%x", value);
    }

    else if(memory_address==0x834){
        illegal_operation(memory[pc/4]);
    }
}

//checks if specific memory is accessed to trigger virtual routine
//return 0 if the memory is not accessed
//returns 1, if memory is accessed and carries on virtual routine
int check_virtual_memory_access(int memory_address, int value){
    if((memory_address==0x800 )|| (memory_address==0x804) || (memory_address==0x808) || (memory_address==0x80C) || (memory_address==0x812) || (memory_address==0x816) ||  (memory_address==0x820) || (memory_address==0x824) || (memory_address==0x828) || (memory_address==0x834) ){
        //printf("MEMEORY ACCCESSED!!: %08x\n", memory_address);
        virtual_routines(memory_address, value);
        return 1;
    }
    else{
        return 0;
    }
}

//this function helps to get 8 bits from instruction memory 
//instruction is stored as int in int array
//to access which byte of instruction memory, we use this 
int instruction_memory_access(int x){
    //we calculate which instruction needs to be extracted
    int instruction_no=x/4; //not pc value
    int instruction_pc_no=instruction_no*4;
    //diff bits shows the starting position of bits to be extracted
    int diff= x-instruction_pc_no;
    return (diff*8);
}

//type r starts

void add(int rd, int rs1, int rs2){
    if(rd!=0b00){
        registers[rd]= registers[rs1]+registers[rs2];
    }
    pc=pc+4;
}

void sub(int rd, int rs1, int rs2){
    if(rd!=0b00){
        registers[rd]= registers[rs1]-registers[rs2];
    }
    pc=pc+4;
}

//xor gives one if bits of two operant is opposite
//else it gives 0; (1^1=0)(1^0=1)
void xor(int rd, int rs1, int rs2){
    if(rd!=0b00){
        registers[rd]= registers[rs1]^registers[rs2];
    }
    pc=pc+4;
}

//or operator gives 1 if one of the bits is 1
//else it gives 1
void or(int rd, int rs1, int rs2){
    if(rd!=0b00){
        registers[rd]= registers[rs1] | registers[rs2];
    }
    pc=pc+4;
}

//gives 1 if both bits is 1, else it gives 0
void and(int rd, int rs1, int rs2){
    if(rd!=0b00){
        registers[rd]= registers[rs1] & registers[rs2];
    }
    pc=pc+4;
}

//shifts the bits of rs1 to left by value of rs2 and stores in rd
void sll(int rd, int rs1, int rs2){
    if(rd!=0){
        registers[rd]=registers[rs1] << registers[rs2];
    }
    pc=pc+4;
}

//shifts the bits of rs1 to right by value of rs2 and stores in rd
void srl(int rd, int rs1, int rs2){
    if(rd!=0){
        registers[rd]=registers[rs1] >> registers[rs2];
    }
    pc=pc+4;
}

//shifts the bits of rs1 to right by value of rs2
//bits that are dropped of due to right shift is added in the left most bits
void sra(int rd, int rs1, int rs2){
    if(rd!=0){
        //right most bits that will fall off is stored and shiftited to most significant bits
        int right_most_bits= break_binary2(registers[rs1],0,(registers[rs2]-1));
        right_most_bits= right_most_bits << (32-registers[rs2]);
        registers[rd]=registers[rs1] >> registers[rs2];
        //bits that fell off is added to the msb
        //or operator is used to attach the fallen off bits as the msb of the shifted number are zero
        registers[rd] =registers[rd] | right_most_bits;
    }
    pc=pc+4;
}

//if rs1 < than rs 2, rd is 1, else 0;
void slt(int rd, int rs1, int rs2){
    if(registers[rs1] < registers[rs2]){
        registers[rd]=1;
    }
    else{
        registers[rd]=0;
    }
    pc=pc+4;
}



void type_r( int instruction){
    int rd=break_binary2(instruction,7,11);
    int func3=break_binary2(instruction, 12, 14);
    int rs1=break_binary2(instruction, 15,19);
    int rs2=break_binary2(instruction, 20, 24);
    int func7=break_binary2(instruction, 25,31);

    if( (func3==0b000) && (func7==0b0000000) ){
        add(rd, rs1, rs2);
    }
    else if(  (func3==0b000) && (func7==0b0100000) ){
        sub(rd,rs1,rs2);
    }
    else if( (func3==0b100) && (func7==0b0000000) ){
        xor(rd,rs1,rs2);
    }
    else if( (func3==0b110) && (func7==0b0000000) ){
        or(rd,rs1,rs2);
    }
    else if( (func3==0b111) && (func7==0b0000000) ){
        and(rd,rs1,rs2);
    }
    else if( (func3==001) && (func7==0b0000000) ){
        sll(rd,rs1,rs2);
    }
    else if( (func3==0b101) && (func7==0b0000000) ){
        srl(rd,rs1,rs2);
    }
    else if( (func3==0b101) && (func7==0b0100000) ){
        sra(rd,rs1,rs2);
    }
    else if( (func3==0b010) && (func7==0b0000000) ){
        slt(rd,rs1,rs2);
    }
    else{
        unknown_instruction(instruction);
    }
}


//type I starts 

void addi(int rd, int rs1, int imm){
    if(rd!=0b00000){
        registers[rd]=registers[rs1]+imm;
    }
    pc=pc+4;
}

//same as xor, but done with imm
//returns 1 if bits of 2 operand is opposite
void xori(int rd, int rs1, int imm){
    if(rd!=0b00000){
        registers[rd]=registers[rs1]^imm;
    }
    pc=pc+4;
}

//same as or just with imm
void ori(int rd, int rs1, int imm){
    if(rd!=0b00000){
        registers[rd]=registers[rs1] | imm;
    }
    pc=pc+4;
}

void andi(int rd, int rs1, int imm){
    if(rd!=0b00000){
        registers[rd]=registers[rs1] & imm;
    }
    pc=pc+4;
}

//sets rd to pc+4; pc= rs1+imm
void jalr(int rd, int rs1, int imm){
    if(rd!=0b00000){
        registers[rd]=pc+4;
    }
    pc=registers[rs1]+imm;
}  

//sets rd to 1 if rs1<imm; else rd is 0
void slti(int rd, int rs1, int imm){
    if(rd!=0){
        if(registers[rs1] < imm){
        registers[rd]=1;
        }
        else{
            registers[rd]=0;
        }
    }
    
    pc=pc+4;
}

//load a 32 bit value from memory into rd
//
void lw(int rd, int rs1, int imm){
    int memory_address=registers[rs1]+imm;
    int value= memory[memory_address];

    if(rd!=0){
        //if memory address is in instruction memory we set the address/4
        //because instruction is set in an integer array
        //memory address acts as index to memory array.
        if( (memory_address>=0x00) && (memory_address<0x3ff) ){
            value=memory[memory_address/4];
        }
        else{
            value=memory[memory_address];
        }

        check_virtual_memory_access(memory_address, value);
        registers[rd]=memory[registers[rs1]+imm];
    }

    //printf("lw finished stored r[%d]= M[r[%0d]+imm(%d)](%08x)=%d\n", rd, rs1, imm, (rs1+imm), registers[rd]);
    pc=pc+4;    
    
}

void lbu(int rd, int rs1, int imm){
    int memory_address=registers[rs1]+imm;
    int value= memory[memory_address];
    //loads a 8bit value from memory into register
    if(rd!=0){
        //we check if it is instruction memory
        //if insturction memory we need to divide memory address by 4 so it acts as index to which instruction
        if( (memory_address>=0x00) && (memory_address<0x7ff) ){
            //which bits need to be extracted is calculates in bit start
            int bit_start=instruction_memory_access(memory_address);
            value=memory[memory_address/4];
            value=break_binary2(value, bit_start, (bit_start+7));
        }
        else{
            value=memory[memory_address];
            value=break_binary2(value, 0, 7);
        }
        
        check_virtual_memory_access(memory_address, value);
        registers[rd]=value;
    }
    pc=pc+4;

}


void type_i( int instruction){
    int opcode=break_binary2(instruction, 0, 6);
    int rd=break_binary2(instruction,7,11);
    int func3=break_binary2(instruction, 12, 14);
    int rs1=break_binary2(instruction, 15,19);
    int imm=break_binary2(instruction, 20, 31);

    if(instruction<0){

        int immediate_num=modifyBit(imm, 12, 1);
        int bitmask=0b0;
        bitmask=~bitmask;
        bitmask=bitmask<<11;
        immediate_num=bitmask | immediate_num;
         //printf("This is imm type_i:n %d\n", immediate_num);
         imm=immediate_num;
    }    

    if( (opcode==0b0010011) && (func3==0b000) ){
        addi(rd,rs1, imm);
    }
    else if( (opcode==0b1100111) && (func3==0b000) ){
        jalr(rd,rs1, imm);
    }
    else if((opcode==0b0000011) && (func3==0b010) ){
        lw(rd, rs1, imm);
    }
    else if((opcode==0b0000011) && (func3==0b100)){
        lbu(rd, rs1, imm);
    }
    else if((opcode==0b0010011) && (func3==0b100)){
        xori(rd, rs1, imm);
    }
    else if((opcode==0b0010011) && (func3==0b110)){
        ori(rd, rs1, imm);
    }
    else if((opcode==0b0010011) && (func3==0b111)){
        andi(rd, rs1, imm);
    }
    else if((opcode==0b0010011) && (func3==0b010)){
        slti(rd, rs1, imm);
    }
    else{
        unknown_instruction(instruction);
    }

}


//S type starts

//store a 8 bit value to memory from a register
void sb(int rs1, int rs2, int imm){
    int value= registers[rs2];
    int memory_address= registers[rs1]+imm;
    //check if it tries to write instruction memory
    if( !( (memory_address>=0x00) && (memory_address<0x3ff) )){
        //value should be before virtual memory access
        //check_virtual_memory_access(memory_address, value);
        value=break_binary2(registers[rs2], 0,7);
        check_virtual_memory_access(memory_address, value);
        memory[registers[rs1]+imm]=value;
    }
    //no else statement cuz we dont write to instruction memory.
    
    pc=pc+4;
}
//store a 16 bit value to memory from a register
void sh(int rs1, int rs2, int imm){
    int value= registers[rs2];
    int memory_address= registers[rs1]+imm;
    
    if( !( (memory_address>=0x00) && (memory_address<0x3ff) )){
        value=break_binary2(registers[rs2], 0,15);
        check_virtual_memory_access(memory_address, value);
        memory[registers[rs1]+imm]=value;
    }
    //no else statement cuz we dont write to instruction memory.
    
    pc=pc+4;
}

//store a 32 bit value to memory from register 
void sw(int rs1,int rs2, int imm){
    int memory_address=registers[rs1]+imm;
    if(memory_address>=0xb700 && memory_address<=0xd700){
        heap_banks();
    }
    if(memory_address<=0 || memory_address>2303){
        printf("memory address: %d, rs1[%d]=%d imm:%d\n", memory_address,rs1, registers[rs1], imm);
        exit(1);
    }

    if(!( (memory_address>=0x00) && (memory_address<=0x3ff)) ){
        int value= registers[rs2];
        check_virtual_memory_access(memory_address, value);
        memory[memory_address]=value;
    }

    pc=pc+4;
}

int imm_manipulate_S(int imm1, int imm2){
    int immediate_num= imm2 << 5; //| imm1; 
    //imm2[11:5] imm1[4:0]
    immediate_num=immediate_num | imm1;
    return immediate_num;
}

void type_s(int instruction){
    //int opcode=break_binary2(instruction, 0, 6);
    int imm1=break_binary2(instruction, 7,11);
    int func3= break_binary2(instruction,12,14);
    int rs1=break_binary2(instruction, 15,19);
    int rs2=break_binary2(instruction,20,24);
    int imm2=break_binary2(instruction, 25,31);
    int immediate_num=imm_manipulate_S(imm1, imm2);

    if(instruction<0){
        immediate_num=modifyBit(immediate_num, 11, 1);
        int bitmask=0b0;
        //~bit complementl; turns 0 to 1 and 1 to 0
        bitmask=~bitmask;
        //making space for 11 bit immediate number
        bitmask=bitmask<<11;
        //msb will be 1 and after that will be imm value
        immediate_num=bitmask | immediate_num;
    }

    if(func3==0b000){
        sb(rs1, rs2, immediate_num);
    }
    else if(func3==0b010){
        sw(rs1,rs2,immediate_num);
    }
    else if(func3==0b001){
        sh(rs1,rs2,immediate_num);
    }
    else{
        unknown_instruction(instruction);
    }
}


//sb type starts
void beq(int rs1, int rs2,int imm){
    if(registers[rs1]==registers[rs2]){
        pc=pc+imm;
    }
    else{
        pc=pc+4;
    }
}


void bne(int rs1, int rs2,int imm){
    if(registers[rs1]!=registers[rs2]){
        pc=pc+imm;
    }else{
        pc=pc+4;
    }
}

void blt(int rs1, int rs2,int imm){
    if(registers[rs1]<registers[rs2]){
        pc=pc+imm;
    }else{
        pc=pc+4;
    }
}

void bge(int rs1, int rs2,int imm){
    if(registers[rs1]>=registers[rs2]){
        pc=pc+imm;
    }else{
        pc=pc+4;
    }
}

//branch if less than// treat numbers unsigned
void bltu(int rs1, int rs2,int imm){
    //casting the values to treat them as unsigned.
    unsigned r1= (unsigned) (registers[rs1]);
    unsigned r2= (unsigned) (registers[rs2]);

    if(r1<r2){
        pc=pc+imm;
    }else{
        pc=pc+4;
    }
}

void bgeu(int rs1, int rs2, int imm){
    //treat numbers as unsigned
    unsigned r1= (unsigned) (registers[rs1]);
    unsigned r2= (unsigned) (registers[rs2]);

    if(r1>=r2){
        pc=pc+imm;
        
    }else{
        pc=pc+4;
    }
}

//imm of sb is imm1[4:1 |11] imm2[12 | 10:5]
int imm_manipulate_SB(int imm1, int imm2){
    int part11=break_binary2(imm1,0,0);
    int part1_4=break_binary2(imm1, 1,4);
    int part10_5=break_binary2(imm2,0,5);
    int part12=break_binary2(imm2,6, 6);
    int immediate_num= part12 << 1 | part11;                        
    immediate_num= immediate_num << 6 | part10_5;
    immediate_num=immediate_num << 4 | part1_4;
    //we assume imm[0] is 0 so we bit shift it to left to give least significat bit 0;
    immediate_num= immediate_num << 1;
    return immediate_num;
}

void type_sb(int instruction){
    int imm1=break_binary2(instruction, 7,11);
    int func3= break_binary2(instruction,12,14);
    int rs1=break_binary2(instruction, 15,19);
    int rs2=break_binary2(instruction,20,24);
    int imm2=break_binary2(instruction, 25,31);
 
    int immediate_num=imm_manipulate_SB(imm1, imm2);
    if(instruction<0){
        immediate_num=modifyBit(immediate_num, 12, 1);
        int bitmask=0b0;
        bitmask=~bitmask;
        bitmask=bitmask<<11;
        immediate_num=bitmask | immediate_num;
    }

    if(func3==0b000){
        beq(rs1,rs2,immediate_num);
    }
    else if(func3==0b111){
        bgeu(rs1,rs2,immediate_num);
    }
    else if(func3==0b001){
        bne(rs1,rs2,immediate_num);
    }
    else if(func3==0b100){
        blt(rs1,rs2,immediate_num);
    }
    else if(func3==0b101){
        bge(rs1, rs2, immediate_num);
    }
    else if(func3==0b110){
        bltu(rs1, rs2, immediate_num);
    }
    else{
        unknown_instruction(instruction);
    }

}

//u type starts
//just loads the upper immediate of imm as rd
//since we dont add 0s in 11;0 of imm we left shift imm by 12 and rd=imm
void lui(int rd, int imm){
    if(rd!=0b00000){
        int value=imm;
        value=value << 12;
        registers[rd]= value;        
    }
    pc=pc+4;
}


void type_u( int instruction){
    int rd=break_binary2(instruction,7,11);
    int imm=break_binary2(instruction, 12, 31);
    
    if(instruction<0){
        int immediate_num=modifyBit(imm, 20, 1);
        int bitmask=0b0;
        bitmask=~bitmask;
        bitmask=bitmask<<11;
        immediate_num=bitmask | immediate_num;
        imm=immediate_num;
    }

    lui(rd,imm);
}


//uj starts

void jal(int rd, int imm){
    if(rd!=0){
        registers[rd]=pc+4;
    }    
    pc=pc+(imm);
}

int imm_manipulate_UJ(int imm){
    int part12_19=break_binary2(imm, 0,7);
    int part11=break_binary2(imm, 8,8);
    int part1_10=break_binary2(imm,9,18);
    int part20=break_binary2(imm, 19,19);    
    int immediate_number=part20 << 8 | part12_19;
    immediate_number=immediate_number << 1 | part11;
    immediate_number= immediate_number << 10 | part1_10;
    //asssume least significant bit is 0
    immediate_number=immediate_number << 1;
    //displayBits(immediate_number);
    return immediate_number;
    
}

void type_uj(int instruction){
    //int opcode= break_binary2(instruction, 0, 6);
    int rd=break_binary2(instruction,7,11);
    int imm=break_binary2(instruction, 12, 31);
    imm= imm_manipulate_UJ(imm);
    int immediate_num=imm;

    if(instruction<0){
        //printf("negative uj\n");
        immediate_num=modifyBit(immediate_num, 20, 1);
        int bitmask=0b0;
        bitmask=~bitmask;
        bitmask=bitmask<<11;
        immediate_num=bitmask | immediate_num;
        imm=immediate_num;
    }

    //printf("This is imm uj %d\n", imm);
    // displayBits(opcode);
    // displayBits(rd);
    // displayBits(imm);

    jal(rd,imm);

}





//carries out the instruction
void carry_instruction(int instruction){
    int opcode= break_binary2(instruction, 0,6);
    int type=opcode_type_check(opcode);
    if(type==1){
        type_r(instruction);
    }
    else if(type==2){
        type_i(instruction);
    }
    else if(type==3){
        type_s(instruction);
    }
    else if(type==4){
        type_sb(instruction);
    }
    
    else if(type==5){
        type_u(instruction);
    }
    else if(type==6){
        type_uj(instruction);
    }
    else{
        unknown_instruction(instruction);
    }
}




int main(int argc, char ** argv){

    if(argc<2){
        printf("No files provided\n");
        exit(0);
    }

    
    FILE* fle;
    //fopen can open binary files just by using rb
    fle= fopen(argv[1], "rb");
    if(fle==NULL){
        printf("File not found or Error opening file\n");
        return 3;
    }

    //int instruction[1024];
    fread(memory, 4, 512, fle);
    fclose(fle);


    while(1){
        carry_instruction(memory[pc/4]);
        registers[0]=0;        
    }
    
    return 0;
}