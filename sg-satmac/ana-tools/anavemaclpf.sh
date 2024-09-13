
echo "VEMAC"
gawk -v node_count=200 -f lpfvemac.awk VEMAC/log_lpf_n200.txt
gawk -v node_count=400 -f lpfvemac.awk VEMAC/log_lpf_n400.txt
gawk -v node_count=600 -f lpfvemac.awk VEMAC/log_lpf_n600.txt
gawk -v node_count=800 -f lpfvemac.awk VEMAC/log_lpf_n800.txt
gawk -v node_count=1000 -f lpfvemac.awk VEMAC/log_lpf_n1000.txt
gawk -v node_count=1200 -f lpfvemac.awk VEMAC/log_lpf_n1200.txt
gawk -v node_count=1400 -f lpfvemac.awk VEMAC/log_lpf_n1400.txt
gawk -v node_count=1400 -f lpfvemac.awk VEMAC/log_lpf_n1600.txt
