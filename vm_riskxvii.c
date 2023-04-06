#include <stdlib.h>
#include <stdio.h>
#include <string.h>
int registers[32]={0};
int memory[2303];
int pc=0;



//DELETE LATER
//to print binary values
void displayBits(unsigned int value){
    unsigned int displayMask=1 << 31;
    printf("%10u=", value);

    for(unsigned int c=1; c<=32; c++){
        putchar(value & displayMask ? '1': '0' );
        value <<=1;
        if(c% 8==0){
            putchar(' ');
        }
    }
    putchar('\n');
}


//breaks the binary number from start to end
int break_binary( int binary_number,  int start, int end){
    
     int bit_mask=(1<<(end-start))-1;
     int broken_binary=(binary_number>>start) & bit_mask;
    return broken_binary;    
}

int break_binary2(int instruction, int start, int end){
    int bitmask=((1 << (end-start+1))-1) << start;
    int broken_binary= (instruction & bitmask) >> start;
    return broken_binary;
}

//modify the giventh bit
int modifyBit(int binary_number, int position, int bit){
    int bit_mask =1 << position;
    int changed_num=((binary_number & ~bit_mask) | (bit << position));
    return changed_num;
}


//we extract the first 7 bits of the binary number to check the opcode
//we make a bitmask which shifts 1 7 bits to the left, thenw e subtract 1 to it
// we use it to mask the first 7 bits 
int opcode_extract(int instruction){
    int bitmask= (1<<7)-1;
    int to_return= instruction & bitmask;
    return to_return;
    //returns first 7 bits // works
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
    else if(opcode==0b0110011){return 1;}
    return -1;
}



//virtual routines
void virtual_routines(int memory_address, int stored_value){
    if(memory_address==0x800){
        //printf("console write character\n");
        memory[0x800]=stored_value;
        printf("%c",(char) memory[0x800]);
    }
    else if(memory_address==0x804){
        printf("%d", stored_value);
        
    }
    else if(memory_address==0x808){
        printf("%08x", stored_value);
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
        printf("%08x", pc);
    }
    else if(memory_address==0x824){
        printf("memory 824 called\n");
    }
    else if(memory_address==0x828){
        printf("memory 828 called\n");
    }
}

//checks if specific memory is accessed to trigger virtual routine
//return 0 if the memory is not accessed
//returns 1, if memory is accessed and carries on virtual routine
int check_virtual_memory_access(int memory_address, int value){
    if((memory_address==0x800 )| (memory_address==0x804) | (memory_address==0x808) | (memory_address==0x80C) | (memory_address==0x812) | (memory_address==0x816) |  (memory_address==0x820) | (memory_address==0x824) | (memory_address==0x828) ){
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
    int instruction_no=x/4; //not pc value
    int instruction_pc_no=instruction_no*4;
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

void xor(int rd, int rs1, int rs2){
    if(rd!=0b00){
        registers[rd]= registers[rs1]^registers[rs2];
    }
    pc=pc+4;
}

void or(int rd, int rs1, int rs2){
    if(rd!=0b00){
        registers[rd]= registers[rs1] | registers[rs2];
    }
    pc=pc+4;
}

void and(int rd, int rs1, int rs2){
    if(rd!=0b00){
        registers[rd]= registers[rs1] & registers[rs2];
    }
    pc=pc+4;
}


void sll(int rd, int rs1, int rs2){
    if(rd!=0){
        registers[rd]=registers[rs1] << registers[rs2];
    }
    pc=pc+4;
}

void srl(int rd, int rs1, int rs2){
    if(rd!=0){
        registers[rd]=registers[rs1] >> registers[rs2];
    }
    pc=pc+4;
}

// void sra(int rd, int rs1, int rs2){
//     if(rd!=0){
//         registers[rd]=registers[rs1] >> registers[rs2];
//     }
//     pc=pc+4;
// }


void type_r( int instruction){
    int opcode=break_binary2(instruction, 0,6);
    int rd=break_binary(instruction,7,11);
    int func3=break_binary(instruction, 12, 14);
    int rs1=break_binary(instruction, 15,19);
    int rs2=break_binary(instruction, 20, 24);
    int func7=break_binary(instruction, 25,31);

    if( (func3==0b000) & (func7==0b0000000) ){
        add(rd, rs1, rs2);
    }
    else if( (opcode==0b0110011) & (func3==0b000) & (func7=0b0100000) ){
        sub(rd,rs1,rs2);
    }
    else if( (opcode==0b0110011) & (func3==0b100) & (func7=0b0000000) ){
        xor(rd,rs1,rs2);
    }
    else if( (opcode==0b0110011) & (func3==0b110) & (func7=0b0000000) ){
        or(rd,rs1,rs2);
    }
    else if( (opcode==0b0110011) & (func3==0b111) & (func7=0b0000000) ){
        and(rd,rs1,rs2);
    }
    else if( (func3==001) & (func7==0b0000000) & (opcode==0b0110011)){
        sll(rd,rs1,rs2);
    }
    else if( (func3==101) & (func7==0b0000000) & (opcode==0b0110011)){
        srl(rd,rs1,rs2);
    }
    
}


//type I starts 

void addi(int rd, int rs1, int imm){
    if(rd!=0b00000){
        registers[rd]=registers[rs1]+imm;
    }
    pc=pc+4;
}

void xori(int rd, int rs1, int imm){
    if(rd!=0b00000){
        registers[rd]=registers[rs1]^imm;
    }
    pc=pc+4;
}

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

void jalr(int rd, int rs1, int imm){
    if(rd!=0b00000){
        registers[rd]=pc+4;
    }
    //printf("jalr rd(%08x)=%08x, rs1(%08x)=%08x, (pc+4)=%08x\n", rd,registers[rd], rs1,registers[rs1], pc+4);
    pc=registers[rs1]+imm;
    //printf("After pc:%08x\n", pc);
}  

void lw(int rd, int rs1, int imm){
    int memory_address=registers[rs1]+imm;
    int value= memory[memory_address];
    //printf("LW Memoery address: %08x\n",memory_address);
    //come back
    check_virtual_memory_access(memory_address, value);
    if(rd!=0){
        registers[rd]=memory[registers[rs1]+imm];
    }

    //printf("lw finished stored r[%d]= M[r[%0d]+imm(%d)](%08x)=%d\n", rd, rs1, imm, (rs1+imm), registers[rd]);
    pc=pc+4;    
    
}

void lbu(int rd, int rs1, int imm){
    int memory_address=registers[rs1]+imm;
    int value= memory[memory_address];
    //printf("before memory address lbu %08x\n", memory_address);
    check_virtual_memory_access(memory_address, value);
    if(rd!=0){
        if(memory_address>=0x00 && memory_address<0x3ff){
            int bit_start=instruction_memory_access(memory_address);
            value=memory[memory_address/4];
            value=break_binary2(value, bit_start, (bit_start+7));
        }
        else{
            value=memory[memory_address];
            value=break_binary2(value, 0, 7);
        }
        
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

    //printf("This is imm_i %d\n\n", imm);
    if(instruction<0){
        //printf("negative type_i\n");
        //displayBits(imm);
        int immediate_num=modifyBit(imm, 12, 1);
        //displayBits(immediate_num);
        int bitmask=0b0;
        bitmask=~bitmask;
        bitmask=bitmask<<11;
        immediate_num=bitmask | immediate_num;
         //printf("This is imm type_i:n %d\n", immediate_num);
         imm=immediate_num;
    }    

    if( (opcode==0b0010011) & (func3==0b000) ){
        addi(rd,rs1, imm);
    }
    else if( (opcode==0b1100111) & (func3==0b000) ){
        jalr(rd,rs1, imm);
    }
    else if((opcode==0b0000011) & (func3==0b010) ){
        lw(rd, rs1, imm);
    }
    else if((opcode==0b0000011) & (func3==0b100)){
        lbu(rd, rs1, imm);
    }
    else if((opcode==0b0010011) & (func3==0b100)){
        xori(rd, rs1, imm);
    }
    else if((opcode==0b0010011) & (func3==0b110)){
        ori(rd, rs1, imm);
    }
    else if((opcode==0b0010011) & (func3==0b111)){
        andi(rd, rs1, imm);
    }

}


//S type starts

//store a 8 bit value to memory from a register
void sb(int rs1, int rs2, int imm){
    int value= registers[rs2];
    value=break_binary(value, 0, 8);

    //to be continued from here
    int memory_address= registers[rs1]+imm;
    //printf("This is memory: %d & rs2: %08x, rs1: %d and value: %d\n", memory_address,  registers[rs2], registers[rs1], value);
    //previously used and it works for printing h;
    // if(memory_address>=0x800 & memory_address<=0x8ff){
        
    //     virtual_routines(memory_address, value);
    // }

    check_virtual_memory_access(memory_address, value);
    //if(check==0){
        value=break_binary(registers[rs2], 0,7);
        memory[registers[rs1]+imm]=value;
    //}

    pc=pc+4;
}

void sw(int rs1,int rs2, int imm){
    int memory_address=registers[rs1]+imm;
    //printf("Memeory addres sw: %d\n", memory_address);
    int value= registers[rs2];
    check_virtual_memory_access(memory_address, value);
    //if(check==0){
        memory[memory_address]=value;
    //}

    pc=pc+4;
}

int imm_manipulate_S(int imm1, int imm2){
    int immediate_num= imm2 << 5; //| imm1; 
    //displayBits(immediate_num);
    immediate_num=immediate_num | imm1;
    //printf("this is stype imm:%08x\n", immediate_num);
    //displayBits(immediate_num);
    return immediate_num;
}

void type_s(int instruction){
    //int opcode=break_binary2(instruction, 0, 6);
    int imm1=break_binary2(instruction, 7,11);
    int func3= break_binary2(instruction,12,14);
    int rs1=break_binary2(instruction, 15,19);
    int rs2=break_binary2(instruction,20,24);
    int imm2=break_binary2(instruction, 25,31);
    //displayBits(imm1);
    //displayBits(imm2);
    int immediate_num=imm_manipulate_S(imm1, imm2);

    if(instruction<0){
        //printf("negative\n");
        immediate_num=modifyBit(immediate_num, 11, 1);
        int bitmask=0b0;
        bitmask=~bitmask;
        bitmask=bitmask<<11;
        immediate_num=bitmask | immediate_num;
    }
    //printf("This is imm type S:n %d\n", immediate_num);

    if(func3==0b000){
        sb(rs1, rs2, immediate_num);
    }
    else if(func3==0b010){
        sw(rs1,rs2,immediate_num);
    }
}


//sb type starts
void beq(int rs1, int rs2,int imm){
    if(registers[rs1]==registers[rs2]){
        pc=pc+imm;
        //printf("triggered\n");
    }
    else{
        pc=pc+4;
    }
    //printf("beq is done andn pc:%08x\n", pc);
}

void bgeu(int rs1, int rs2, int imm){
    if(registers[rs1]>=registers[rs2]){
        pc=pc+imm;
        
    }else{
        pc=pc+4;
    }
    //printf("bgeu is done andn pc:%08x\n", pc);
}

void bne(int rs1, int rs2,int imm){
    if(registers[rs1]!=registers[rs2]){
        pc=pc+imm;
    }else{
        pc=pc+4;
    }
    //printf("bne is done andn pc:%08x\n", pc);
}

void blt(int rs1, int rs2,int imm){
    if(registers[rs1]<registers[rs2]){
        pc=pc+imm;
    }else{
        pc=pc+4;
    }
    //printf("blt is done andn pc:%08x\n", pc);
}

void bge(int rs1, int rs2,int imm){
    if(registers[rs1]>=registers[rs2]){
        pc=pc+imm;
    }else{
        pc=pc+4;
    }
    //printf("bne is done andn pc:%08x\n", pc);
}

int imm_manipulate_SB(int imm1, int imm2){
    int part11=break_binary2(imm1,0,0);
    //displayBits(part11);
    int part1_4=break_binary2(imm1, 1,4);
    //displayBits(part1_4);
    int part10_5=break_binary2(imm2,0,5);
    //displayBits(part10_5);
    int part12=break_binary2(imm2,6, 6);
    //displayBits(part12);
    int immediate_num= part12 << 1 | part11;                        
    immediate_num= immediate_num << 6 | part10_5;
    immediate_num=immediate_num << 4 | part1_4;
    //we assume imm[0] is 0 so we bit shift it to left to give least significat bit 0;
    immediate_num= immediate_num << 1;
    //displayBits(immediate_num);
    return immediate_num;
}

void type_sb(int instruction){
     //displayBits(instruction);
    // int opcode=opcode_extract(instruction); //break_binary(instruction, 0, 6);
    // int imm1= break_binary(instruction,7,11);
    // int func3=break_binary(instruction,12,14);
    // int rs1=break_binary(instruction,15,19);
    // int rs2=break_binary(instruction, 20,24);
    // int imm2= break_binary(instruction,25, 31);
    //int opcode=break_binary2(instruction, 0, 6);
    int imm1=break_binary2(instruction, 7,11);
    int func3= break_binary2(instruction,12,14);
    int rs1=break_binary2(instruction, 15,19);
    int rs2=break_binary2(instruction,20,24);
    int imm2=break_binary2(instruction, 25,31);
    //printf("SB imm1:%d and SB imm2:%d\n", imm1, imm2);

    // displayBits(opcode);
    // displayBits(imm1);
    // displayBits(func3);
    // displayBits(rs1);
    // displayBits(rs2);
    // displayBits(imm2);
    int immediate_num=imm_manipulate_SB(imm1, imm2);
    if(instruction<0){
        immediate_num=modifyBit(immediate_num, 12, 1);
        int bitmask=0b0;
        bitmask=~bitmask;
        bitmask=bitmask<<11;
        immediate_num=bitmask | immediate_num;
    }
    //printf("This is imm of SB: %d\n", immediate_num);
    
    //displayBits(immediate_num);

    if(func3==0b000){
        beq(rs1,rs2,immediate_num);
    }
    else if(func3==0b111){
        bgeu(rs1,rs2,immediate_num);
    }
    else if(func3==0b001){
        bne(rs1,rs2,immediate_num);
    }
    else if(func3==0b001){
        blt(rs1,rs2,immediate_num);
    }
    else if(func3==0b101){
        bge(rs1, rs2, immediate_num);
    }

}

//u type starts

void lui(int rd, int imm){
    if(rd!=0b00000){
        int value=imm;
        value=value << 12;
        registers[rd]= value;        
    }
    pc=pc+4;
}


void type_u( int instruction){
    int rd=break_binary(instruction,7,11);
    int imm=break_binary(instruction, 12, 31);
    
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
    registers[rd]=pc+4;
    //printf("%d: rd(%d)= %d+4; ", pc, rd, pc);
    pc=pc+(imm);
    //printf("  after Pc=%d \n", pc);
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
    int opcode= opcode_extract(instruction);
    int type=opcode_type_check(opcode);
    //printf("This is instruction: %08x,of type: %d\n", instruction, type);

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
    fread(memory, 4, 256, fle);
    fclose(fle);

    // for(int i=0; i<256; i++){
    //    // instruction_memory[i]=instruction[i];
    //     memory[i]=instruction[i];
    // }
    

    while(1){
        registers[0]=0;
        carry_instruction(memory[pc/4]);
        registers[0]=0;        
    }
    
    return 0;
}

