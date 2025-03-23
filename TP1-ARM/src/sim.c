#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>
#include "shell.h"

void decode_instruction();
void decode_adds_extended(uint32_t instruction);
void decode_adds_immediate(uint32_t instruction);
void decode_subs_extended(uint32_t instruction);
void decode_subs_immediate(uint32_t instruction);
void decode_halt(uint32_t instruction);
void decode_cmp_immediate(uint32_t instruction);
void decode_cmp_extended(uint32_t instruction);
void decode_ands(uint32_t instruction);
void decode_eor(uint32_t instruction);
void decode_orr(uint32_t instruction);

/* conditional 214
*/

void process_instruction()
{
    /* execute one instruction here. You should use CURRENT_STATE and modify
     * values in NEXT_STATE. You can call mem_read_32() and mem_write_32() to
     * access memory. 
     * */
    printf("Processing instruction\n");
    decode_instruction();
}


typedef struct instruction_information
{
    /* data */
    uint32_t opcode;
    void* function;
} inst_info; 

// FALTAN MAS INSTRUCCIONES

inst_info INSTRUCTION_SET[] = {
    {0b10101011000, &decode_adds_extended},
    {0b10110001, &decode_adds_immediate},
    {0b11101011000, &decode_subs_extended},
    {0b11110001, &decode_subs_immediate},
    {0b11010100010, &decode_halt},
    {0b11110001, &decode_cmp_immediate},
    {0b11101011001, &decode_cmp_extended}, // ULTIMO DIGITO 0 ==> IGUAL A SUBS
    {0b11101010000, &decode_ands},
    {0b11001010000, &decode_eor},
    {0b10101010000, &decode_orr},
    {0b01010100, &decode_b_cond},
    {0b110100101, &decode_movz},
    {0b100010, &decode_add_immediate}, 
    {0b010110, &decode_add_extended_register}, 


};

#define INSTRUCTION_SET_SIZE (sizeof(INSTRUCTION_SET) / sizeof(INSTRUCTION_SET[0]))

void decode_adds_extended(uint32_t instruction){
    // Get the register numbers
    uint32_t rd = (instruction >> 0) & 0b11111;
    uint32_t rn = (instruction >> 5) & 0b11111;
    uint32_t rm = (instruction >> 16) & 0b11111;

    // Get imm3
    uint32_t imm3 = (instruction >> 10) & 0b111;

    // Get option
    uint32_t option = (instruction >> 13) & 0b111;

    // Add the values
    uint64_t result = CURRENT_STATE.REGS[rn] + CURRENT_STATE.REGS[rm];

    // Update the flags
    NEXT_STATE.FLAG_N = (result >> 63) & 0b1;
    NEXT_STATE.FLAG_Z = (result == 0) ? 1 : 0;

    // Update the register
    NEXT_STATE.REGS[rd] = result;

    // Update the PC
    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
}

void decode_subs_extended(uint32_t instruction){
    // Get the register numbers
    uint32_t rd = (instruction >> 0) & 0b11111;
    uint32_t rn = (instruction >> 5) & 0b11111;
    uint32_t rm = (instruction >> 16) & 0b11111;

    // Get imm3
    uint32_t imm3 = (instruction >> 10) & 0b111;

    // Get option
    uint32_t option = (instruction >> 13) & 0b111;

    // Subtract the values
    uint64_t result = CURRENT_STATE.REGS[rn] - CURRENT_STATE.REGS[rm];

    // Update the flags
    NEXT_STATE.FLAG_N = (result >> 63) & 0b1;
    NEXT_STATE.FLAG_Z = (result == 0) ? 1 : 0;

    // Update the register
    NEXT_STATE.REGS[rd] = result;

    // Update the PC
    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
}

void decode_adds_immediate(uint32_t instruction){
    // Get the register numbers
    uint32_t rd = (instruction >> 0) & 0b11111;
    uint32_t rn = (instruction >> 5) & 0b11111;
    
    // Get the immediate value
    uint32_t imm12 = (instruction >> 10) & 0b111111111111;

    // Check for shift
    uint32_t shift = (instruction >> 22) & 0b11;
    switch (shift) {
        case 0x0: {
            // Handle case shift 00
            
            // Add the values
            uint64_t result = CURRENT_STATE.REGS[rn] + imm12;

            // Update the flags
            NEXT_STATE.FLAG_N = (result >> 63) & 0b1;
            NEXT_STATE.FLAG_Z = (result == 0) ? 1 : 0;

            // Update the register
            NEXT_STATE.REGS[rd] = result;

            // Update the PC
            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
    
            break;
        }
        case 0x1: {
            // Handle case 01
            
            // Shift the immediate value
            uint64_t shifted_imm12 = imm12 << 12;

            // Add the values
            uint64_t result = CURRENT_STATE.REGS[rn] + shifted_imm12;

            // Update the flags
            NEXT_STATE.FLAG_N = (result >> 63) & 0b1;
            NEXT_STATE.FLAG_Z = (result == 0) ? 1 : 0;

            // Update the register
            NEXT_STATE.REGS[rd] = result;

            // Update the PC
            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            break;
        }

        default: {
            // Handle unexpected cases
            printf("Unexpected shift value\n");
            break;
        }
    }
}


void decode_subs_immediate(uint32_t instruction){
    // Get the register numbers
    uint32_t rd = (instruction >> 0) & 0b11111;
    uint32_t rn = (instruction >> 5) & 0b11111;
    
    // Get the immediate value
    uint32_t imm12 = (instruction >> 10) & 0b111111111111;

    // Check for shift
    uint32_t shift = (instruction >> 22) & 0b11;
    switch (shift) {
        case 0x0: {
            // Handle case shift 00
            
            // Substract the values
            uint64_t result = CURRENT_STATE.REGS[rn] - imm12;

            // Update the flags
            NEXT_STATE.FLAG_N = (result >> 63) & 0b1;
            NEXT_STATE.FLAG_Z = (result == 0) ? 1 : 0;

            // Update the register
            NEXT_STATE.REGS[rd] = result;

            // Update the PC
            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
    
            break;
        }
        case 0x1: {
            // Handle case 01
            
            // Shift the immediate value
            uint64_t shifted_imm12 = imm12 << 12;

            // Substract the values
            uint64_t result = CURRENT_STATE.REGS[rn] - shifted_imm12;

            // Update the flags
            NEXT_STATE.FLAG_N = (result >> 63) & 0b1;
            NEXT_STATE.FLAG_Z = (result == 0) ? 1 : 0;

            // Update the register
            NEXT_STATE.REGS[rd] = result;

            // Update the PC
            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            break;
        }
    }
}

void decode_halt(uint32_t instruction){
    // Set the run bit to zero
    RUN_BIT = 0;
}   

void decode_cmp_immediate(uint32_t instruction){
    // Get the register numbers
    uint32_t rn = (instruction >> 5) & 0b11111;
    
    // Get the immediate value
    uint32_t imm12 = (instruction >> 10) & 0b111111111111;

    // Check for shift
    uint32_t shift = (instruction >> 22) & 0b11;
    switch (shift) {
        case 0x0: {
            // Handle case shift 00
            
            // Substract the values
            uint64_t result = CURRENT_STATE.REGS[rn] - imm12;

            // Update the flags
            NEXT_STATE.FLAG_N = (result >> 63) & 0b1;
            NEXT_STATE.FLAG_Z = (result == 0) ? 1 : 0;

            // Update the PC
            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
    
            break;
        }

        case 0x1: {
            // Handle case 01
            
            // Shift the immediate value
            uint64_t shifted_imm12 = imm12 << 12;

            // Substract the values
            uint64_t result = CURRENT_STATE.REGS[rn] - shifted_imm12;

            // Update the flags
            NEXT_STATE.FLAG_N = (result >> 63) & 0b1;
            NEXT_STATE.FLAG_Z = (result == 0) ? 1 : 0;

            // Update the PC
            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            break;
        }

        default: {
            // Handle unexpected cases
            printf("Unexpected shift value\n");
            break;
        }
    }
}

void decode_cmp_extended(uint32_t instruction){
    // Get the register numbers
    uint32_t rn = (instruction >> 5) & 0b11111;
    uint32_t rm = (instruction >> 16) & 0b11111;

    // Get imm3
    uint32_t imm3 = (instruction >> 10) & 0b111;

    // Get option
    uint32_t option = (instruction >> 13) & 0b111;

    // Subtract the values
    uint64_t result = CURRENT_STATE.REGS[rn] - CURRENT_STATE.REGS[rm];

    // Update the flags
    NEXT_STATE.FLAG_N = (result >> 63) & 0b1;
    NEXT_STATE.FLAG_Z = (result == 0) ? 1 : 0;

    // Update the PC
    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
}

void decode_ands(uint32_t instruction){
    // Get the register numbers
    uint32_t rd = (instruction >> 0) & 0b11111;
    uint32_t rn = (instruction >> 5) & 0b11111;
    uint32_t rm = (instruction >> 16) & 0b11111;

    // Get imm6
    uint32_t imm6 = (instruction >> 10) & 0b111111;

    // And the values
    uint64_t result = CURRENT_STATE.REGS[rn] & CURRENT_STATE.REGS[rm];

    // Update the flags
    NEXT_STATE.FLAG_N = (result >> 63) & 0b1;
    NEXT_STATE.FLAG_Z = (result == 0) ? 1 : 0;

    // Update the register
    NEXT_STATE.REGS[rd] = result;

    // Update the PC
    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
}

void decode_eor(uint32_t instruction){

    // Get the register numbers
    uint32_t rd = (instruction >> 0) & 0b11111;
    uint32_t rn = (instruction >> 5) & 0b11111;
    uint32_t rm = (instruction >> 16) & 0b11111;

    // Get imm6
    uint32_t imm6 = (instruction >> 10) & 0b111111;

    // Eor the values
    uint64_t result = CURRENT_STATE.REGS[rn] ^ CURRENT_STATE.REGS[rm];

    // Update the flags
    NEXT_STATE.FLAG_N = (result >> 63) & 0b1;
    NEXT_STATE.FLAG_Z = (result == 0) ? 1 : 0;

    // Update the register
    NEXT_STATE.REGS[rd] = result;

    // Update the PC
    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
}

void decode_orr(uint32_t instruction){
    // Get the register numbers
    uint32_t rd = (instruction >> 0) & 0b11111;
    uint32_t rn = (instruction >> 5) & 0b11111;
    uint32_t rm = (instruction >> 16) & 0b11111;

    // Get imm6
    uint32_t imm6 = (instruction >> 10) & 0b111111;

    // Or the values
    uint64_t result = CURRENT_STATE.REGS[rn] | CURRENT_STATE.REGS[rm];

    // Update the flags
    NEXT_STATE.FLAG_N = (result >> 63) & 0b1;
    NEXT_STATE.FLAG_Z = (result == 0) ? 1 : 0;

    // Update the register
    NEXT_STATE.REGS[rd] = result;

    // Update the PC
    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
}

void decode_b(uint32_t instruction){ //opcode = 000101
    // Extraer el offset de 26 bits (bits 25:0)
    int32_t imm26 = instruction & 0x03FFFFFF; // 26 bits de offset

    // Extender el offset a 28 bits agregando '00' al final
    int32_t offset_28 = imm26 << 2; // Desplazar 2 bits a la izquierda (equivalente a multiplicar por 4)

    // Aplicar el signo al offset (si el bit más significativo de imm26 es 1, el offset es negativo)
    if (imm26 & 0x02000000) { // Verificar el bit 25 de imm26
        offset_28 |= 0xFC000000; // Extender el signo a los 32 bits
    }

    // Calcular la dirección de destino: PC + offset
    uint64_t target_address = CURRENT_STATE.PC + offset_28;

    // Actualizar el PC en NEXT_STATE
    NEXT_STATE.PC = target_address;

    }

void decode_br(uint32_t instruction) { //opcode = 1101011

    // Extraer el registro Rn (bits 20:16)
    uint8_t Rn = (instruction >> 16) & 0x1F;

    // Obtener la dirección de destino desde el registro Rn
    uint64_t target_address = CURRENT_STATE.REGS[Rn];

    // Actualizar el PC en NEXT_STATE
    NEXT_STATE.PC = target_address;

}

void decode_b_cond(uint32_t instruction) {
    // Extraer la condición (bits 31:28)
    uint8_t cond = (instruction >> 28) & 0xF;

    // Extraer el offset de 19 bits (bits 23:5)
    int32_t imm19 = (instruction >> 5) & 0x7FFFF; // 19 bits de offset

    // Aplicar el desplazamiento (el offset está en múltiplos de 4)
    int32_t offset = imm19 << 2; // Desplazar 2 bits a la izquierda (equivalente a multiplicar por 4)

    // Aplicar el signo al offset (si el bit más significativo de imm19 es 1, el offset es negativo)
    if (imm19 & 0x40000) { // Verificar el bit 18 de imm19
        offset |= 0xFFF80000; // Extender el signo a los 32 bits
    }

    // Variable para almacenar la dirección de destino
    uint64_t target_address;

    // Manejar cada condición según el valor de 'cond'
    switch (cond) {
        case 0x0: // BEQ (Branch if Equal)
            if (CURRENT_STATE.FLAG_Z) {
                target_address = CURRENT_STATE.PC + offset;
                NEXT_STATE.PC = target_address;
            } else {
                NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            }
            break;

        case 0x1: // BNE (Branch if Not Equal)
            if (!CURRENT_STATE.FLAG_Z) {
                target_address = CURRENT_STATE.PC + offset;
                NEXT_STATE.PC = target_address;
            } else {
                NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            }
            break;

        case 0xC: // BGT (Branch if Greater Than)
            if (CURRENT_STATE.FLAG_Z == 0 && CURRENT_STATE.FLAG_N == 0) {
                target_address = CURRENT_STATE.PC + offset;
                NEXT_STATE.PC = target_address;
            } else {
                NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            }
            break;

        case 0xB: // BLT (Branch if Less Than)
            if (CURRENT_STATE.FLAG_N == 1) {
                target_address = CURRENT_STATE.PC + offset;
                NEXT_STATE.PC = target_address;
            } else {
                NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            }
            break;

        case 0xA: // BGE (Branch if Greater Than or Equal)
            if (CURRENT_STATE.FLAG_N == 0) {
                target_address = CURRENT_STATE.PC + offset;
                NEXT_STATE.PC = target_address;
            } else {
                NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            }
            break;

        case 0xD: // BLE (Branch if Less Than or Equal)
            if (CURRENT_STATE.FLAG_Z == 1 || CURRENT_STATE.FLAG_N == 1) {
                target_address = CURRENT_STATE.PC + offset;
                NEXT_STATE.PC = target_address;
            } else {
                NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            }
            break;

        default:
            // Si la condición no es reconocida, simplemente avanzar al siguiente instrucción (PC + 4)
            printf("Error: Condición no reconocida (cond = 0x%X)\n", cond);
            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            break;
    }

}


void decode_movz(uint32_t instruction) {
    uint16_t imm16 = (instruction >> 5) & 0xFFFF; // Bits 20:5
    uint8_t Rd = (instruction >> 0) & 0x1F;       // Bits 4:0


    // Verificar que hw = 0 (sin desplazamiento)

    // Cargar el valor inmediato en el registro destino
    uint64_t value = imm16;

    // Si sf = 1 (64 bits), asegurarse de que los bits superiores sean 0
    NEXT_STATE.REGS[Rd] = value;
    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
}

void decode_add_immediate(uint32_t instruction) {
    uint8_t shift = (instruction >> 22) & 0x3;    // Bits 23:22
    uint16_t imm12 = (instruction >> 10) & 0xFFF; // Bits 21:10
    uint8_t Rn = (instruction >> 5) & 0x1F;       // Bits 9:5
    uint8_t Rd = (instruction >> 0) & 0x1F;       // Bits 4:0

    // Aplicar el desplazamiento al valor inmediato
    uint64_t immediate_value = imm12;
    if (shift == 0b01) {
        immediate_value <<= 12; // Desplazar 12 bits a la izquierda

    // Obtener el valor del registro fuente
    uint64_t value = CURRENT_STATE.REGS[Rn];

    // Realizar la suma
    uint64_t result = value + immediate_value;

    // Guardar el resultado en el registro destino
    NEXT_STATE.REGS[Rd] = result;
    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
}

void decode_add_extended_register(uint32_t instruction) {
    // Extraer los campos de la instrucción
    uint8_t Rm = (instruction >> 16) & 0x1F;      // Bits 20:16
    uint8_t Rn = (instruction >> 5) & 0x1F;       // Bits 9:5
    uint8_t Rd = (instruction >> 0) & 0x1F;       // Bits 4:0

    // Obtener los valores de los registros fuente
    uint64_t value1 = CURRENT_STATE.REGS[Rn];
    uint64_t value2 = CURRENT_STATE.REGS[Rm];

    // Realizar la suma
    uint64_t result = value1 + value2;

    // Guardar el resultado en el registro destino
    NEXT_STATE.REGS[Rd] = result;
    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
}

void decode_cbz(uint32_t instruction) {
    // Extraer los campos de la instrucción
    uint8_t op = (instruction >> 24) & 0x1;       // Bit 24
    uint8_t Rt = (instruction >> 5) & 0x1F;       // Bits 4:0
    int32_t imm19 = (instruction >> 5) & 0x7FFFF; // Bits 23:5

    int32_t offset = imm19 << 2; // Desplazar 2 bits a la izquierda (equivalente a multiplicar por 4)

    if (imm19 & 0x40000) { // Verificar el bit 18 de imm19
        offset |= 0xFFF80000; // Extender el signo a los 32 bits
    }

    // Verificar si el registro Rt es cero
    if (CURRENT_STATE.REGS[Rt] == 0) {
        // Calcular la dirección de destino: PC + offset
        uint64_t target_address = CURRENT_STATE.PC + offset;

        // Actualizar el PC en NEXT_STATE
        NEXT_STATE.PC = target_address;

    } else {
        // Si no se cumple la condición, simplemente avanzar al siguiente instrucción (PC + 4)
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;

    }

void decode_cbnz(uint32_t instruction) {
    // Extraer los campos de la instrucción
    uint8_t Rt = (instruction >> 5) & 0x1F;       // Bits 4:0
    int32_t imm19 = (instruction >> 5) & 0x7FFFF; // Bits 23:5

    int32_t offset = imm19 << 2; // Desplazar 2 bits a la izquierda (equivalente a multiplicar por 4)

    if (imm19 & 0x40000) { // Verificar el bit 18 de imm19
        offset |= 0xFFF80000; // Extender el signo a los 32 bits
    }

    // Verificar si el registro Rt es cero
    if (CURRENT_STATE.REGS[Rt] != 0) {
        // Calcular la dirección de destino: PC + offset
        uint64_t target_address = CURRENT_STATE.PC + offset;

        // Actualizar el PC en NEXT_STATE
        NEXT_STATE.PC = target_address;

    } else {
        // Si no se cumple la condición, simplemente avanzar al siguiente instrucción (PC + 4)
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;

    }

void decode_stur(uint32_t instruction) {
    // Extraer los campos de la instrucción
    int16_t imm9 = (instruction >> 12) & 0x1FF;  // Bits 20:12 (offset de 9 bits con signo)
    uint8_t Rn = (instruction >> 5) & 0x1F;      // Bits 9:5 (registro base)
    uint8_t Rt = (instruction >> 0) & 0x1F;      // Bits 4:0 (registro fuente)


    // Aplicar el signo al offset (si el bit más significativo de imm9 es 1, el offset es negativo)
    if (imm9 & 0x100) { // Verificar el bit 8 de imm9
        imm9 |= 0xFE00; // Extender el signo a los 16 bits
    }

    // Calcular la dirección de memoria: X2 + offset
    uint64_t address = CURRENT_STATE.REGS[Rn] + imm9;

    // Verificar que la dirección de memoria esté dentro del rango válido (0x10000000 en adelante)
    if (address < 0x10000000 || address >= 0x10100000) {
        printf("Error: Dirección de memoria fuera del rango válido (0x10000000 a 0x10100000).\n");
        return;
    }

    // Obtener el valor del registro fuente
    uint64_t value = CURRENT_STATE.REGS[Rt];

    mem_write_64(address, value);
    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
}

}



void decode_instruction(){

    printf("Decoding instruction\n");
    uint32_t instruction = mem_read_32(CURRENT_STATE.PC);
    printf("Instruction: 0x%X\n", instruction);

    // Posibles opcodes con diferentes tamaños según el formato de instrucción
    uint32_t opcode_11 = (instruction >> 21) & 0x7FF;  // 11 bits (R, I, D, IW)
    uint32_t opcode_8  = (instruction >> 24) & 0xFF;   // 8 bits (CB)
    uint32_t opcode_6  = (instruction >> 26) & 0x3F;   // 6 bits (B)

    printf("Opcodes: 11-bit: 0x%X, 8-bit: 0x%X, 6-bit: 0x%X\n", opcode_11, opcode_8, opcode_6);

    // Buscar el opcode en el conjunto de instrucciones
    for (int i = 0; i < INSTRUCTION_SET_SIZE; i++) {
        if (INSTRUCTION_SET[i].opcode == opcode_11 || 
            INSTRUCTION_SET[i].opcode == opcode_8  || 
            INSTRUCTION_SET[i].opcode == opcode_6) {

            printf("Match found\n");
            void (*decode_function)(uint32_t) = INSTRUCTION_SET[i].function;
            decode_function(instruction);
            return;
        }
    }
}
