#include <iostream>
#include <fstream>
#include "pin.H"

FILE *out_file;

// Handle command-line arguments
KNOB<std::string> knob_out_file(KNOB_MODE_WRITEONCE, "pintool", "o", "memtrace.out", "specify output file name");


void record_instruction(void *ip) {
    fprintf(out_file, "I,%p\n", ip);
}

void record_read(void *addr) {
    fprintf(out_file, "R,%p\n", addr);
}

void record_write(void *addr) {
    fprintf(out_file, "W,%p\n", addr);
}

void instruction(INS ins, void *v) {
    // IPOINT_BEFORE: call provided function before ins gets called.
    // AFUNPTR: pointer to void function.
    // Optional args of format <type>, <argument>
    // (e.g. IARG_INST_PTR macro, equivalent to IARG_ADDRINT, INS_Address(ins)).
    // IARG_END: Instruction() takes ..., specify end with IARG_END.
    INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)record_instruction, IARG_INST_PTR, IARG_END);

    uint32_t n_mem_ops = INS_MemoryOperandCount(ins);

    for (uint32_t mem_op = 0; mem_op < n_mem_ops; ++mem_op) {
        if (INS_MemoryOperandIsRead(ins, mem_op)) {
            INS_InsertPredicatedCall(ins, IPOINT_BEFORE, (AFUNPTR)record_read, IARG_MEMORYOP_EA, mem_op, IARG_END);
        }
        if (INS_MemoryOperandIsWritten(ins, mem_op))
        {
            INS_InsertPredicatedCall(ins, IPOINT_BEFORE, (AFUNPTR)record_write, IARG_MEMORYOP_EA, mem_op, IARG_END);
        }
    }
}


void fini(int32_t code, void *v) {
    fclose(out_file);
}

// Specify help message.
int32_t usage() {
    std::cerr << "Count the number of dynamic instructions executed" << std::endl;
    std::cerr << std::endl << KNOB_BASE::StringKnobSummary() << std::endl;
    return -1;
}

int main(int argc, char *argv[]) {
    if (PIN_Init(argc, argv)) return usage();

    out_file = fopen(knob_out_file.Value().c_str(), "w");

    fprintf(out_file, "type,addr\n");

    INS_AddInstrumentFunction(instruction, NULL);

    PIN_AddFiniFunction(fini, NULL);

    PIN_StartProgram();
}