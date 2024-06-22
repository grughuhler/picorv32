/* Copyright 2024 Grug Huhler.  License SPDX BSD-2-Clause. */

// This module integrates the user flash on the Tang Nano 9K.  The Tang
// Nano 20K does not contain this flash.
//
// Flash is 608 Kbits, 32-bits wide, organized into 304 rows of 64
// columns each.  The erase page size is 2048 bytes, so there are
// 38 pages that may be separately erased.
//
// This integration uses the Gowin (User) flash controller rather than
// implementing a controller from scratch.  The Gowin controller is
// encrypted and is placed into the project using the "IP Core Generator"
// tool in the Gowin IDE.
//
// See docs IPUG901 "Gowin Flash Controller IP" and UG295 "Gowin User Flash".
//
// The max clock frequency is confusing.  The controller appears to be
// designed for 50 MHz but works (with reduced performance) at slower
// clock speeds.  The read times are quite slow.  This has been lightly
// tested at speeds between 5.4 MHz and 40.5 MHz.
//
// Reads can be 8, 16, or 32 bits wide.  Erasing is done on a page basis.
// To erase a page, do an 8 bit write to a 32-bit aligned address in the
// page. To program (write), do a 32-bit write to the address to be
// programmed.

module uflash #(parameter CLK_FREQ=5400000)
(
 input wire         reset_n,
 input wire         clk,
 input wire         sel,
 input wire [3:0]   wstrb,
 input wire [14:0]  addr, // word address, 9-bits row, 6 bits col
 input wire [31:0]  data_i,
 output wire        ready,
 output wire [31:0] data_o
);

   localparam       IDLE = 2'b00;
   localparam       ACTING = 2'b01;
   localparam       DONE = 2'b10;

   wire             wr_en;
   reg [1:0]        state = IDLE;
   reg              start_flag = 1'b0;
   wire             done_flag;
   wire             erase_en;

   // write enable is asserted when all 4 strobe bits are 1.
   assign wr_en = &wstrb;
   // erase enable is asserted only for a byte-wide write to a word-aligned address.
   assign erase_en = (wstrb == 4'b0001);
   assign ready = (state == DONE);

   always @(posedge clk or negedge reset_n)
     if (!reset_n) begin
        state <= IDLE;
        start_flag = 1'b0;
     end
     else begin
        case (state)
          IDLE: begin
             if (sel) begin
                state <= ACTING;
                start_flag <= 1'b1;
             end
             else
               state <= IDLE;
          end
          ACTING: begin
             start_flag <= 1'b0;
             if (done_flag)
               state <= DONE;
             else
               state <= ACTING;
          end
          DONE: begin
             state <= IDLE;
          end
        endcase
     end

   Gowin_Flash_Controller_Top flash0
     (
      .wdata_i(data_i),
      .wyaddr_i(addr[5:0]),  //column addr
      .wxaddr_i(addr[14:6]), // row addr
      .erase_en_i(erase_en),
      .done_flag_o(done_flag),
      .start_flag_i(start_flag),
      .clk_i(clk),
      .nrst_i(reset_n),
      .rdata_o(data_o),
      .wr_en_i(wr_en)
      );

endmodule
