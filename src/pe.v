module PE#(
    parameter    knl_size     = 5 ,
    parameter    data_width   = 16,
    parameter    weight_width = 16,
    parameter    result_width = data_width + weight_width + knl_size
)
(
    input clk,
    input rst_n,
    input [(data_width  -1) :0] picDat,
    input [(weight_width-1) :0] weightDat,
    output[(result_width-1) :0] result,
    output valid
);
reg [(result_width-1) :0] result;

wire [(data_width + weight_width):0]      w_tmp_result;
mul mul_u(
    .clk(clk),
    .a(picDat),
    .b(weightDat),
    .p(w_tmp_result)
);

reg [4:0] cnt;
always@(posedge clk or negedge rst_n) begin
    if (!rst_n)
        cnt <= 5'd0;
    else if (cnt = 5'd24)
        cnt <= 5'd0;
    else
        cnt <= cnt + 5'd1;
end

always@(posedge clk or negedge rst_n) begin
    if (!rst_n || cnt = 5'd24)
        result <= 0;     //data width ?
    else
        result <= result + w_tmp_result;
end
endmodule