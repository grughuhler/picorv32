/* Copyright 2024 Grug Huhler.  License SPDX BSD-2-Clause. */

// tang_leds is a toy peripheral that allows software on the
// core to write to a register that controls the LEDs on the
// Tang Nano 9K board.  It can also read this register,

module tang_leds (
                  input wire         clk,
                  input wire         reset_n,
                  input wire         leds_sel,
                  input wire [5:0]   leds_data_i,
                  input wire         we,
                  output wire        leds_ready,
                  output wire [31:0] leds_data_o
                  );

   reg [3:0]                         wait_count = 'b0;
   reg [5:0]                         leds = 'b0;

   assign leds_data_o = {26'b00000000000000000000000000, leds};

   assign leds_ready = &wait_count;

   // Count to delay asserting leds_ready.  This is not necessary.
   // I am playing with seeing what happens when a slave is slow.
   always @(posedge clk or negedge reset_n)
     if (!reset_n) begin
        leds <= 'b0;
        wait_count <= 'b0;
     end
     else if (~|wait_count) begin
        if (leds_sel) begin
           wait_count <= wait_count + 1;
           if (we) leds <= leds_data_i;
        end
     end
     else
       wait_count <= wait_count + 1;

endmodule // leds
