/* Copyright 2024 Grug Huhler.  License SPDX BSD-2-Clause. */

// Timer that can be written and read and always counts
// down, stopping at zero.

module countdown_timer (
                        input wire         clk,
                        input wire         reset_n,
                        input wire         cdt_sel,
                        input wire [31:0]   cdt_data_i,
                        input wire [3:0]   we,
                        output wire        cdt_ready,
                        output wire [31:0] cdt_data_o
                  );

   reg                                     ready_delay = 'b0;
   reg [31:0]                              counter = 'b0;

   assign cdt_data_o = counter;
   assign cdt_ready = ready_delay;

   always @(posedge clk or negedge reset_n)
     if (!reset_n) begin
        counter <= 'b0;
        ready_delay <= 'b0;
     end
     else if (cdt_sel) begin
        ready_delay <= 'b1;
        if (we) begin
           if (we[0]) counter[31:24] <= cdt_data_i[31:24];
           if (we[1]) counter[23:16] <= cdt_data_i[23:16];
           if (we[2]) counter[15:8] <= cdt_data_i[15:8];
           if (we[3]) counter[7:0] <= cdt_data_i[7:0];
        end
        else
           if (counter != 'b0) counter <= counter - 1;
     end
     else begin
        if (counter != 'b0) counter <= counter - 1;
        ready_delay <= 'b0;
     end

endmodule // countdown_timer
