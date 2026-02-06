module test1 ( clk, a, b, c, y );

input clk;
input a;
input b;
input c;

output y;

wire w1;
wire w2;

assign w1 = a & b;
assign w2 = w1 | c;

always @(posedge clk) y <= w2;

endmodule