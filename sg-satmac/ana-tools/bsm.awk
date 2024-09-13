#初始化设定
BEGIN {
        #node_count = 300;

	begin = 1;
        dead = 102000;
	dead_s = 102;
	read = 0;
        slot_time = 0.001;
        #frame_len = 100;
	street_num = 1;
	street_len = 1000;
	commu_range = 150;
	#frame_per_sec = 1000/frame_len;
        
        #frame_time = slot_time * frame_len;
	total_pdr = 0;
	pdr_c = 0;	
	pdr = 0;	
	total_pir_mean=0;
	total_delta_mean=0;

	total_frame = 0;
	total_wait = 0;
	total_collision = 0;
	total_req_fail = 0;
	total_no_valid = 0;
	total_adj_req = 0;
	total_adj_suc = 0;
        total_rx = 0;
        total_tx = 0;
	total_frame_len = 0;
	total_channel_utilization = 0.0;
	total_collision_trace = 0;
        now = 0;
	#thso = (node_count/street_num) * (2*commu_range/street_len) / frame_len;
         

}

#BSM
# 2 15 2000 8676 0.872837 0.872837 111020 497798 87197
# time collision_cumulation, TxCount, RxCount, PDR, PDR_C, PIR_Mean, PIR_Max, PIR,Min
$1 > 2 {
	read++;
        #now = $1;
        #if (now < dead_s) {
	if (read > begin) {
		total_collision = $2;
		pdr += $5;		
		pdr_c = $6;
		total_delta_mean += $10;
	}
}


# 最后输出结果
END {
#	printf "read: %d now %d\n", read/node_count, now;

	printf "avg_col %4.1f PDR %.2f PDR_C %.2f delta_mean %.2f\n", total_collision/node_count, pdr/(read-begin),pdr_c, total_delta_mean/1000/read;
}
