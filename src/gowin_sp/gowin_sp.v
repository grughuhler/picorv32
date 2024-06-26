//Copyright (C)2014-2023 Gowin Semiconductor Corporation.
//All rights reserved.
//File Title: IP file
//GOWIN Version: V1.9.9 Beta-4 Education
//Part Number: GW1NR-LV9QN88PC6/I5
//Device: GW1NR-9
//Device Version: C
//Created Time: Fri May 10 10:33:29 2024

module Gowin_SP #(
    parameter INIT_RAM_00=256'h0,
    parameter INIT_RAM_01=256'h0,
    parameter INIT_RAM_02=256'h0,
    parameter INIT_RAM_03=256'h0,
    parameter INIT_RAM_04=256'h0,
    parameter INIT_RAM_05=256'h0,
    parameter INIT_RAM_06=256'h0,
    parameter INIT_RAM_07=256'h0,
    parameter INIT_RAM_08=256'h0,
    parameter INIT_RAM_09=256'h0,
    parameter INIT_RAM_0A=256'h0,
    parameter INIT_RAM_0B=256'h0,
    parameter INIT_RAM_0C=256'h0,
    parameter INIT_RAM_0D=256'h0,
    parameter INIT_RAM_0E=256'h0,
    parameter INIT_RAM_0F=256'h0,
    parameter INIT_RAM_10=256'h0,
    parameter INIT_RAM_11=256'h0,
    parameter INIT_RAM_12=256'h0,
    parameter INIT_RAM_13=256'h0,
    parameter INIT_RAM_14=256'h0,
    parameter INIT_RAM_15=256'h0,
    parameter INIT_RAM_16=256'h0,
    parameter INIT_RAM_17=256'h0,
    parameter INIT_RAM_18=256'h0,
    parameter INIT_RAM_19=256'h0,
    parameter INIT_RAM_1A=256'h0,
    parameter INIT_RAM_1B=256'h0,
    parameter INIT_RAM_1C=256'h0,
    parameter INIT_RAM_1D=256'h0,
    parameter INIT_RAM_1E=256'h0,
    parameter INIT_RAM_1F=256'h0,
    parameter INIT_RAM_20=256'h0,
    parameter INIT_RAM_21=256'h0,
    parameter INIT_RAM_22=256'h0,
    parameter INIT_RAM_23=256'h0,
    parameter INIT_RAM_24=256'h0,
    parameter INIT_RAM_25=256'h0,
    parameter INIT_RAM_26=256'h0,
    parameter INIT_RAM_27=256'h0,
    parameter INIT_RAM_28=256'h0,
    parameter INIT_RAM_29=256'h0,
    parameter INIT_RAM_2A=256'h0,
    parameter INIT_RAM_2B=256'h0,
    parameter INIT_RAM_2C=256'h0,
    parameter INIT_RAM_2D=256'h0,
    parameter INIT_RAM_2E=256'h0,
    parameter INIT_RAM_2F=256'h0,
    parameter INIT_RAM_30=256'h0,
    parameter INIT_RAM_31=256'h0,
    parameter INIT_RAM_32=256'h0,
    parameter INIT_RAM_33=256'h0,
    parameter INIT_RAM_34=256'h0,
    parameter INIT_RAM_35=256'h0,
    parameter INIT_RAM_36=256'h0,
    parameter INIT_RAM_37=256'h0,
    parameter INIT_RAM_38=256'h0,
    parameter INIT_RAM_39=256'h0,
    parameter INIT_RAM_3A=256'h0,
    parameter INIT_RAM_3B=256'h0,
    parameter INIT_RAM_3C=256'h0,
    parameter INIT_RAM_3D=256'h0,
    parameter INIT_RAM_3E=256'h0,
    parameter INIT_RAM_3F=256'h0
    )
    (dout, clk, oce, ce, reset, wre, ad, din);

output [7:0] dout;
input clk;
input oce;
input ce;
input reset;
input wre;
input [10:0] ad;
input [7:0] din;

wire [23:0] sp_inst_0_dout_w;
wire gw_gnd;

assign gw_gnd = 1'b0;

SP #(
    .INIT_RAM_00(INIT_RAM_00),
    .INIT_RAM_01(INIT_RAM_01),
    .INIT_RAM_02(INIT_RAM_02),
    .INIT_RAM_03(INIT_RAM_03),
    .INIT_RAM_04(INIT_RAM_04),
    .INIT_RAM_05(INIT_RAM_05),
    .INIT_RAM_06(INIT_RAM_06),
    .INIT_RAM_07(INIT_RAM_07),
    .INIT_RAM_08(INIT_RAM_08),
    .INIT_RAM_09(INIT_RAM_09),
    .INIT_RAM_0A(INIT_RAM_0A),
    .INIT_RAM_0B(INIT_RAM_0B),
    .INIT_RAM_0C(INIT_RAM_0C),
    .INIT_RAM_0D(INIT_RAM_0D),
    .INIT_RAM_0E(INIT_RAM_0E),
    .INIT_RAM_0F(INIT_RAM_0F),
    .INIT_RAM_10(INIT_RAM_10),
    .INIT_RAM_11(INIT_RAM_11),
    .INIT_RAM_12(INIT_RAM_12),
    .INIT_RAM_13(INIT_RAM_13),
    .INIT_RAM_14(INIT_RAM_14),
    .INIT_RAM_15(INIT_RAM_15),
    .INIT_RAM_16(INIT_RAM_16),
    .INIT_RAM_17(INIT_RAM_17),
    .INIT_RAM_18(INIT_RAM_18),
    .INIT_RAM_19(INIT_RAM_19),
    .INIT_RAM_1A(INIT_RAM_1A),
    .INIT_RAM_1B(INIT_RAM_1B),
    .INIT_RAM_1C(INIT_RAM_1C),
    .INIT_RAM_1D(INIT_RAM_1D),
    .INIT_RAM_1E(INIT_RAM_1E),
    .INIT_RAM_1F(INIT_RAM_1F),
    .INIT_RAM_20(INIT_RAM_20),
    .INIT_RAM_21(INIT_RAM_21),
    .INIT_RAM_22(INIT_RAM_22),
    .INIT_RAM_23(INIT_RAM_23),
    .INIT_RAM_24(INIT_RAM_24),
    .INIT_RAM_25(INIT_RAM_25),
    .INIT_RAM_26(INIT_RAM_26),
    .INIT_RAM_27(INIT_RAM_27),
    .INIT_RAM_28(INIT_RAM_28),
    .INIT_RAM_29(INIT_RAM_29),
    .INIT_RAM_2A(INIT_RAM_2A),
    .INIT_RAM_2B(INIT_RAM_2B),
    .INIT_RAM_2C(INIT_RAM_2C),
    .INIT_RAM_2D(INIT_RAM_2D),
    .INIT_RAM_2E(INIT_RAM_2E),
    .INIT_RAM_2F(INIT_RAM_2F),
    .INIT_RAM_30(INIT_RAM_30),
    .INIT_RAM_31(INIT_RAM_31),
    .INIT_RAM_32(INIT_RAM_32),
    .INIT_RAM_33(INIT_RAM_33),
    .INIT_RAM_34(INIT_RAM_34),
    .INIT_RAM_35(INIT_RAM_35),
    .INIT_RAM_36(INIT_RAM_36),
    .INIT_RAM_37(INIT_RAM_37),
    .INIT_RAM_38(INIT_RAM_38),
    .INIT_RAM_39(INIT_RAM_39),
    .INIT_RAM_3A(INIT_RAM_3A),
    .INIT_RAM_3B(INIT_RAM_3B),
    .INIT_RAM_3C(INIT_RAM_3C),
    .INIT_RAM_3D(INIT_RAM_3D),
    .INIT_RAM_3E(INIT_RAM_3E),
    .INIT_RAM_3F(INIT_RAM_3F)
)
sp_inst_0 
(
    .DO({sp_inst_0_dout_w[23:0],dout[7:0]}),
    .CLK(clk),
    .OCE(oce),
    .CE(ce),
    .RESET(reset),
    .WRE(wre),
    .BLKSEL({gw_gnd,gw_gnd,gw_gnd}),
    .AD({ad[10:0],gw_gnd,gw_gnd,gw_gnd}),
    .DI({gw_gnd,gw_gnd,gw_gnd,gw_gnd,gw_gnd,gw_gnd,gw_gnd,gw_gnd,gw_gnd,gw_gnd,gw_gnd,gw_gnd,gw_gnd,gw_gnd,gw_gnd,gw_gnd,gw_gnd,gw_gnd,gw_gnd,gw_gnd,gw_gnd,gw_gnd,gw_gnd,gw_gnd,din[7:0]})
);

defparam sp_inst_0.READ_MODE = 1'b0;
defparam sp_inst_0.WRITE_MODE = 2'b00;
defparam sp_inst_0.BIT_WIDTH = 8;
defparam sp_inst_0.BLK_SEL = 3'b000;
defparam sp_inst_0.RESET_MODE = "SYNC";
//defparam sp_inst_0.INIT_RAM_00 = 256'h0000000000000000000000000000000000000000000000000706050404020100;

endmodule //Gowin_SP
