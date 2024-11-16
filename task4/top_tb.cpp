#include "Vtop.h"
#include "verilated.h"
#include "verilated_vcd_c.h"
#include "vbuddy.cpp"

int main(int argc, char **argv, char **env) {
    int i;
    int clk;

    Verilated::commandArgs(argc, argv);
    Vtop* top = new Vtop;
    
    Verilated::traceEverOn(true);
    VerilatedVcdC* tfp = new VerilatedVcdC;
    top->trace(tfp, 99);
    tfp->open("top.vcd");

    if (vbdOpen() != 1) return (-1);
    vbdHeader("Lab 1: Counter");
    vbdSetMode(1);

    top->clk = 1;
    top->rst = 1;

    for (i = 0; i < 300; i++) {
        for (clk = 0; clk < 2; clk++) {
            tfp->dump(2 * i + clk);
            top->clk = !top->clk;
            top->eval();
        }

        // ++++ Send Count value to Vbuddy
        vbdHex(4, (int(top->bcd) >> 16) & 0xF);
        vbdHex(3, (int(top->bcd) >> 8) & 0xF);
        vbdHex(2, (int(top->bcd) >> 4) & 0xF);
        vbdHex(1, int(top->bcd) & 0xF);
        vbdCycle(i+1);
        // ---- end of Vbuddy output selection

        //vbdPlot(int(top->count), 0, 255);

        // change input stimuli
        top->rst = (i < 2) | (i == 15);
        top->v = 50;
        top->ld = vbdFlag();
        if (Verilated::gotFinish()) exit(0);
    }

    vbdClose();
    tfp->close();   
    exit(0);
}
