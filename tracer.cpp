#include <iostream>
#include <fstream>
#include "pin.H"

using std::ofstream;

ofstream out_file;

// Handle command-line arguments
KNOB<std::string> knob_out_file(KNOB_MODE_WRITEONCE, "pintool", "o", "inscount.out", "specify output file name");

static uint64_t icount = 0;

void docount() {
    icount++;
}

void instruction(INS ins, void *v) {
    // AFUNPTR: pointer to void function
    // IPOINT_BEFORE: call provided function before ins gets called
    // IARG_END: Instruction() takes ..., specify none with IARG_END.
    INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)docount, IARG_END);
}

void done(int32_t code, void *v) {
    out_file.setf(std::ios::showbase);
    out_file << "count " << icount << std::endl;
    out_file.close();
}

// Specify help message.
int32_t usage() {
    std::cerr << "Count the number of dynamic instructions executed" << std::endl;
    std::cerr << std::endl << KNOB_BASE::StringKnobSummary() << std::endl;
    return -1;
}

int main(int argc, char *argv[]) {
    if (PIN_Init(argc, argv)) return usage();

    out_file.open(knob_out_file.Value().c_str());

    // 
    INS_AddInstrumentFunction(instruction, NULL);

    PIN_AddFiniFunction(done, NULL);

    PIN_StartProgram();
}