/* Copyright 2024 Grug Huhler.  License SPDX BSD-2-Clause. */
/* 
Top level module of simple SoC based on picorv32

It includes:
     * the picorv32 core
     * An 8192 byte SRAM which is initialzed within the Verilog.
     * A module to read/write LEDs on the Gowin Tang Nano 9K
  
Built and tested with the Gowin Eductional tool set on Tang Nano 9K.

The picorv32 core has a very simple memory interface.  See https://github.com/YosysHQ/picorv32

In this SoC, slave (target) device has signals:

   * SLAVE_sel - this is asserted when mem_valid == 1 and mem_addr targets the slave.  It
     "tells" the slave that it is active.  It must accept a write for provide data for a
     read.
   * SLAVE_ready - this is asserted by the slave when it is done with the transaction.
     Core signal mem_ready is the OR of all of the SLAVE_ready signals.
   * Core mem_addr, mem_wdata, and mem_wstrb can be passed to all slaves directly.  The
     latter is a byte lane enable for writes.
   * Each slave drives SLAVE_data_o.  The core's mem_rdata is formed by selecting the
     correct SLAVE_data_o based on SLAVE_sel.
*/

module top (
            input wire        clk,
            input wire        reset_button_n,
            output wire [5:0] leds,
            output wire       clk_out, // This and below are for debug.
            output wire       mem_instr, 
            output wire       mem_valid,
            output wire       mem_ready,
            output wire [3:0] mem_wstrb
            );

   parameter [0:0] BARREL_SHIFTER = 0;
   parameter [0:0] ENABLE_MUL = 0;
   parameter [0:0] ENABLE_DIV = 0;
   parameter [0:0] ENABLE_FAST_MUL = 0;
   parameter [0:0] ENABLE_COMPRESSED = 0;
   parameter [0:0] ENABLE_IRQ_QREGS = 0;

   parameter integer          MEMBYTES = 8192;      // This is not easy to change
   parameter [31:0] STACKADDR = (MEMBYTES);   // Grows down.  Software should set it.
   parameter [31:0] PROGADDR_RESET = 32'h0000_0000;
   parameter [31:0] PROGADDR_IRQ = 32'h0000_0000;

   wire                       reset_n; 
   wire [31:0]                mem_addr;
   wire [31:0]                mem_wdata;
   wire [31:0]                mem_rdata;
   wire                       leds_sel;
   wire                       leds_ready;
   wire                       sram_sel;
   wire                       sram_ready;
   wire [31:0]                sram_data_o;
   wire [31:0]                leds_data_o;

   assign clk_out = clk;

   // Establish memory map for all slaves
   assign leds_sel = mem_valid && (mem_addr == 32'h80000000);
   assign sram_sel = mem_valid && (mem_addr < 32'h00002000);  // sram fro 0 to 0x1fff.

   // Core can proceed regardless of *which* slave was targetted and is now ready.
   assign mem_ready = sram_ready | leds_ready;

   // Select which slave's output data is to be fed to core.
   assign mem_rdata = sram_ready ? sram_data_o :
                              leds_ready ? leds_data_o : 32'h0;

   assign leds = ~leds_data_o[5:0]; // Connect to the LEDs off the FPGA

   reset_control reset_controller
     (
      .clk(clk),
      .reset_button_n(reset_button_n),
      .reset_n(reset_n)
      );

   sram #(.ADDRWIDTH(13)) memory
     (
      .clk(clk),
      .resetn(1'b1),
      .sram_sel(sram_sel),
      .wstrb(mem_wstrb),
      .addr(mem_addr[12:0]),
      .sram_data_i(mem_wdata),
      .sram_ready(sram_ready),
      .sram_data_o(sram_data_o)
      );

   tang_leds soc_leds
     (
      .clk(clk),
      .leds_sel(leds_sel),
      .leds_data_i(mem_wdata[5:0]),
       .we(mem_wstrb[0]),
      .leds_ready(leds_ready),
      .leds_data_o(leds_data_o)
      );

   picorv32
     #(
       .STACKADDR(STACKADDR),
       .PROGADDR_RESET(PROGADDR_RESET),
       .PROGADDR_IRQ(PROGADDR_IRQ),
       .BARREL_SHIFTER(BARREL_SHIFTER),
       .COMPRESSED_ISA(ENABLE_COMPRESSED),
       .ENABLE_MUL(ENABLE_MUL),
       .ENABLE_DIV(ENABLE_DIV),
       .ENABLE_FAST_MUL(ENABLE_FAST_MUL),
       .ENABLE_IRQ(1),
       .ENABLE_IRQ_QREGS(ENABLE_IRQ_QREGS)
       ) cpu
       (
        .clk         (clk),
        .resetn      (reset_n),
        .mem_valid   (mem_valid),
        .mem_instr   (mem_instr),
        .mem_ready   (mem_ready),
        .mem_addr    (mem_addr),
        .mem_wdata   (mem_wdata),
        .mem_wstrb   (mem_wstrb),
        .mem_rdata   (mem_rdata),
        .irq         ('b0)
        );

endmodule // top
