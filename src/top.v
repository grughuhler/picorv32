/* Copyright 2024 Grug Huhler.  License SPDX BSD-2-Clause. */
/* 
Top level module of simple SoC based on picorv32

It includes:
     * the picorv32 core
     * An 8192 byte SRAM which is initialzed within the Verilog.
     * A module to read/write LEDs on the Gowin Tang Nano 9K
     * A wrapped version of the UART from picorv32's picosoc.
     * A 32-bit count down timer.
  
Built and tested with the Gowin Eductional tool set on Tang Nano 9K.

The picorv32 core has a very simple memory interface.
See https://github.com/YosysHQ/picorv32

In this SoC, slave (target) device has signals:

   * SLAVE_sel - this is asserted when mem_valid == 1 and mem_addr targets the slave.
     It "tells" the slave that it is active.  It must accept a write for provide data
     for a read.
   * SLAVE_ready - this is asserted by the slave when it is done with the transaction.
     Core signal mem_ready is the OR of all of the SLAVE_ready signals.
   * Core mem_addr, mem_wdata, and mem_wstrb can be passed to all slaves directly.
     The latter is a byte lane enable for writes.
   * Each slave drives SLAVE_data_o.  The core's mem_rdata is formed by selecting the
     correct SLAVE_data_o based on SLAVE_sel.
*/

// Define this for logic analyer connections and enable picorv32_la.cst.
//`define USE_LA

module top (
            input wire        clk_in,
            input wire        reset_button_n,
            input wire        uart_rx,
            output wire       uart_tx,
`ifdef USE_LA
            output wire       clk_out,
            output wire       mem_instr, 
            output wire       mem_valid,
            output wire       mem_ready,
            output wire       b25,
            output wire       b24,
            output wire       b17,
            output wire       b16,
            output wire       b09,
            output wire       b08,
            output wire       b01,
            output wire       b00,
            output wire [3:0] mem_wstrb,
`endif
            output wire [5:0] leds
            );

   // This include gets SRAM_ADDR_WIDTH from software build process
   `include "sys_parameters.v"

   parameter BARREL_SHIFTER = 0;
   parameter ENABLE_MUL = 0;
   parameter ENABLE_DIV = 0;
   parameter ENABLE_FAST_MUL = 0;
   parameter ENABLE_COMPRESSED = 0;
   parameter ENABLE_IRQ_QREGS = 0;

   parameter          MEMBYTES = 4*(1 << SRAM_ADDR_WIDTH); 
   parameter [31:0] STACKADDR = (MEMBYTES);         // Grows down.  Software should set it.
   parameter [31:0] PROGADDR_RESET = 32'h0000_0000;
   parameter [31:0] PROGADDR_IRQ = 32'h0000_0000;

   wire                       reset_n; 
   wire                       mem_valid;
   wire                       mem_instr;
   wire [31:0]                mem_addr;
   wire [31:0]                mem_wdata;
   wire [31:0]                mem_rdata;
   wire [3:0]                 mem_wstrb;
   wire                       mem_ready;
   wire                       mem_inst;
   reg                        leds_sel;
   wire                       leds_ready;
   wire [31:0]                leds_data_o;
   reg                        sram_sel;
   wire                       sram_ready;
   wire [31:0]                sram_data_o;
   reg                        cdt_sel;
   wire                       cdt_ready;
   wire [31:0]                cdt_data_o;
   reg                        uart_sel;
   wire [31:0]                uart_data_o;
   wire                       uart_ready;
   reg                        uflash_sel;
   wire [31:0]                uflash_data_o;
   wire                       uflash_ready;
   // default_sel causes a response when nothing else does
   reg                        default_sel;
   wire                       clk;

`ifdef USE_LA
   // Assigns for external logic analyzer connction
   assign clk_out = clk;
   assign b25 = mem_rdata[25];
   assign b24 = mem_rdata[24];
   assign b17 = mem_rdata[17];
   assign b16 = mem_rdata[16];
   assign b09 = mem_rdata[9];
   assign b08 = mem_rdata[8];
   assign b01 = mem_rdata[1];
   assign b00 = mem_rdata[0];
`endif

   // Reduce clock from 27 to 5 MHz to help Gowin Analysis Oscilloscope
   Gowin_rPLL pll (
      .clkout(clk), //output clkout
      .clkin(clk_in) //input clkin
   );

   // Establish memory map for all slaves:
   //    SRAM 00000000 - 0001ffff
   //  uflash 00020000 - 00032fff
   //    LED  80000000
   //    UART 80000008 - 8000000f
   //    CDT  80000010 - 80000014

   always @(*)
      if (mem_valid) begin
         if (mem_addr < MEMBYTES) begin
            sram_sel = 1'b1;
            uflash_sel = 1'b0;
            leds_sel = 1'b0;
            uart_sel = 1'b0;
            cdt_sel = 1'b0;
            default_sel = 1'b0;
         end
         else if ((mem_addr >= 32'h2_0000) && (mem_addr < 32'h3_3000)) begin
            sram_sel = 1'b0;
            uflash_sel = 1'b1;
            leds_sel = 1'b0;
            uart_sel = 1'b0;
            cdt_sel = 1'b0;
            default_sel = 1'b0;
         end
         else if (mem_addr == 32'h8000_0000) begin
            sram_sel = 1'b0;
            uflash_sel = 1'b0;
            leds_sel = 1'b1;
            uart_sel = 1'b0;
            cdt_sel = 1'b0;
            default_sel = 1'b0;
         end
         else if ((mem_addr >= 32'h8000_0008) && (mem_addr < 32'h8000_0010)) begin
            sram_sel = 1'b0;
            uflash_sel = 1'b0;
            leds_sel = 1'b0;
            uart_sel = 1'b1;
            cdt_sel = 1'b0;
            default_sel = 1'b0;
         end
         else if (mem_addr == 32'h8000_0010) begin
            sram_sel = 1'b0;
            uflash_sel = 1'b0;
            leds_sel = 1'b0;
            uart_sel = 1'b0;
            cdt_sel = 1'b1;
            default_sel = 1'b0;
         end
         else begin
            sram_sel = 1'b0;
            uflash_sel = 1'b0;
            leds_sel = 1'b0;
            uart_sel = 1'b0;
            cdt_sel = 1'b0;
            default_sel = 1'b1;
         end
      end
      else begin
            sram_sel = 1'b0;
            uflash_sel = 1'b0;
            leds_sel = 1'b0;
            uart_sel = 1'b0;
            cdt_sel = 1'b0;
            default_sel = 1'b0;
      end

   // Core can proceed based on which slave was targetted and is now ready.
   assign mem_ready = mem_valid &
      (sram_ready | leds_ready | uart_ready | cdt_ready | uflash_ready | default_sel);


   // Select which slave's output data is to be fed to core.
   assign mem_rdata = sram_sel    ? sram_data_o :
                      leds_sel    ? leds_data_o :
                      uart_sel    ? uart_data_o :
                      uflash_sel  ? uflash_data_o :
                      cdt_sel     ? cdt_data_o  : 32'hdeadbeef;

   assign leds = ~leds_data_o[5:0]; // Connect to the LEDs off the FPGA

   reset_control reset_controller
     (
      .clk(clk),
      .reset_button_n(reset_button_n),
      .reset_n(reset_n)
      );

   uart_wrap uart
     (
      .clk(clk),
      .reset_n(reset_n),
      .uart_tx(uart_tx),
      .uart_rx(uart_rx),
      .uart_sel(uart_sel),
      .addr(mem_addr[3:0]),
      .uart_wstrb(mem_wstrb),
      .uart_di(mem_wdata),
      .uart_do(uart_data_o),
      .uart_ready(uart_ready)
      );

   countdown_timer cdt
     (
      .clk(clk),
      .reset_n(reset_n),
      .cdt_sel(cdt_sel),
      .cdt_data_i(mem_wdata),
      .we(mem_wstrb),
      .cdt_ready(cdt_ready),
      .cdt_data_o(cdt_data_o)
      );

   uflash uflash0 (
     .reset_n(reset_n),
     .clk(clk),
     .sel(uflash_sel),
     .wstrb(mem_wstrb),
     .addr(mem_addr[16:2]), // word address, 9-bits row, 6 bits col
     .data_i(mem_wdata),
     .ready(uflash_ready),
     .data_o(uflash_data_o)
   );

   sram #(.SRAM_ADDR_WIDTH(SRAM_ADDR_WIDTH)) memory
     (
      .clk(clk),
      .reset_n(reset_n),
      .sram_sel(sram_sel),
      .wstrb(mem_wstrb),
      .addr(mem_addr[SRAM_ADDR_WIDTH + 1:0]),
      .sram_data_i(mem_wdata),
      .sram_ready(sram_ready),
      .sram_data_o(sram_data_o)
      );
   
   tang_leds soc_leds
     (
      .clk(clk),
      .reset_n(reset_n),
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
