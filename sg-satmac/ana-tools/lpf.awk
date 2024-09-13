#��ʼ���趨
BEGIN {
        #node_count = 300;


        dead = 102000;
	read = 0;
        slot_time = 0.001;
        #frame_len = 100;
	street_num = 1;
	street_len = 1000;
	commu_range = 150;
	#frame_per_sec = 1000/frame_len;
        
        #frame_time = slot_time * frame_len;

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
         
        for(i=1; i<= node_count; i++){ 
             waiting_frame_count[i] = 0;
             request_fail_times[i] = 0;
	     no_valid_count[i] = 0;
             collision_count[i] = 0;
             frame_count[i] = 0;
             continuous_work_fi_max_[i] = 0;
             adj_count_total[i] = 0;
             adj_count_success[i] = 0;
             tx_count[i] = 0;
             rx_count[i] = 0;
	     slot_num[i] = -1;
	     frame_len[i] = 0;
	     localmerge_collision_count[i] = 0;
	     total_frame_len_pernode[i] = 0;
	     channel_utilization[i] = 0.0;
	     collision_trace[i]=0;
        }  
}

#LPF
# m <time> t[<current_slot>] _<node_id>_ LPF <waiting_frame_count> <request_fail_times> <collision_count> 
# m 1.032000000 t[0] _8_ SOR 5 1 1
$1 == "m" && $5 == "LPF" {
	read++;
        now = $2;
        if (now < dead) {
        len = length($4);
        node_id = substr($4,2,len-2);
        waiting_frame_count[node_id] = $6;
        request_fail_times[node_id] = $7;
        collision_count[node_id] = $8;
	frame_count[node_id] = $9;
        continuous_work_fi_max_[node_id] = $10;
	adj_count_success[node_id] = $11;
	if ($12 < 10000)
		adj_count_total[node_id] = $12;
	else
		adj_count_total[node_id] = 0;
        tx_count[node_id] = $13;
        rx_count[node_id] = $14;
	no_valid_count[node_id] = $15;
	slot_num[node_id] = $16;
	frame_len[node_id] = $17;
	localmerge_collision_count[node_id] = $18;
	channel_utilization[node_id] = channel_utilization[node_id]+$21;
	total_frame_len_pernode[node_id] += frame_len[node_id];
	
        }
}


# ���������
END {
#	printf "read: %d now %d\n", read/node_count, now;
       for(j=1;j<= node_count; j++){
            total_frame += frame_count[j];
	    total_wait += waiting_frame_count[j];
	    total_collision += collision_count[j];
	    total_req_fail += request_fail_times[j];
	    total_adj_req += adj_count_total[j];
	    total_adj_suc += adj_count_success[j];
            total_rx += rx_count[j];
            total_tx += tx_count[j];
	    total_no_valid += no_valid_count[j];
	    total_frame_len += total_frame_len_pernode[j];
	    total_channel_utilization += channel_utilization[j];
	    
		
#            printf "t:%d Node %d, total frame %d wait %d no_valid %d req_fail %d col_count %d local_col %d  conti_work_fi %d adj_suc %d adj_total %d slot_no %d frame_len %d\n",now, j, frame_count[j],waiting_frame_count[j], no_valid_count[j],request_fail_times[j], collision_count[j],localmerge_collision_count[j], continuous_work_fi_max_[j], adj_count_success[j], adj_count_total[j], slot_num[j], frame_len[j];
       }
#	printf "total_frame %d, total_wait %d\n", total_frame, total_wait;
	#printf "%f, %d, %d\n",total_channel_utilization,node_count,now;
	now = now/1000;
#printf "%d %d\n" ,now,read;
	printf "wait %.4f no_valid %4.1f req_fail %5.1f adj_req %5.1f adj_suc %5.1f rx: %5.1f tx: %5.1f frame_len %5.1f ch_util %.3f\n", total_wait/total_frame, total_no_valid/node_count, total_req_fail/node_count, total_adj_req, total_adj_suc, total_rx/now/node_count, total_tx/now/node_count, total_frame_len/now/node_count, total_channel_utilization/now/node_count;
}
