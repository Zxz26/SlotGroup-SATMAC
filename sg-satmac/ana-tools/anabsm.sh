echo "VEMAC"
gawk -v node_count=200 -f bsm.awk VEMAC/log_collision_n200.txt
gawk -v node_count=400 -f bsm.awk VEMAC/log_collision_n400.txt
gawk -v node_count=600 -f bsm.awk VEMAC/log_collision_n600.txt
gawk -v node_count=800 -f bsm.awk VEMAC/log_collision_n800.txt
gawk -v node_count=1000 -f bsm.awk VEMAC/log_collision_n1000.txt
gawk -v node_count=1200 -f bsm.awk VEMAC/log_collision_n1200.txt
gawk -v node_count=1400 -f bsm.awk VEMAC/log_collision_n1400.txt
gawk -v node_count=1600 -f bsm.awk VEMAC/log_collision_n1600.txt

echo "NOADJ"
gawk -v node_count=200 -f bsm.awk NOADJ/log_collision_n200.txt
gawk -v node_count=400 -f bsm.awk NOADJ/log_collision_n400.txt
gawk -v node_count=600 -f bsm.awk NOADJ/log_collision_n600.txt
gawk -v node_count=800 -f bsm.awk NOADJ/log_collision_n800.txt
gawk -v node_count=1000 -f bsm.awk NOADJ/log_collision_n1000.txt
gawk -v node_count=1200 -f bsm.awk NOADJ/log_collision_n1200.txt
gawk -v node_count=1400 -f bsm.awk NOADJ/log_collision_n1400.txt
gawk -v node_count=1600 -f bsm.awk NOADJ/log_collision_n1600.txt

echo "ADJ-HALF"
gawk -v node_count=200 -f bsm.awk HALFADJ/log_collision_n200.txt
gawk -v node_count=400 -f bsm.awk HALFADJ/log_collision_n400.txt
gawk -v node_count=600 -f bsm.awk HALFADJ/log_collision_n600.txt
gawk -v node_count=800 -f bsm.awk HALFADJ/log_collision_n800.txt
gawk -v node_count=1000 -f bsm.awk HALFADJ/log_collision_n1000.txt
gawk -v node_count=1200 -f bsm.awk HALFADJ/log_collision_n1200.txt
gawk -v node_count=1400 -f bsm.awk HALFADJ/log_collision_n1400.txt
gawk -v node_count=1600 -f bsm.awk HALFADJ/log_collision_n1600.txt

echo "80211p"
gawk -v node_count=200 -f bsm.awk 80211p/s200i0.1/log_collision_n200.txt
gawk -v node_count=400 -f bsm.awk 80211p/s200i0.1/log_collision_n400.txt
gawk -v node_count=600 -f bsm.awk 80211p/s200i0.1/log_collision_n600.txt
gawk -v node_count=800 -f bsm.awk 80211p/s200i0.1/log_collision_n800.txt
gawk -v node_count=1000 -f bsm.awk 80211p/s200i0.1/log_collision_n1000.txt
gawk -v node_count=1200 -f bsm.awk 80211p/s200i0.1/log_collision_n1200.txt
gawk -v node_count=1400 -f bsm.awk 80211p/s200i0.1/log_collision_n1400.txt
gawk -v node_count=1600 -f bsm.awk 80211p/s200i0.1/log_collision_n1600.txt

echo "ADJ_ALL T0.3E0.5"
gawk -v node_count=200 -f bsm.awk SATMAC/T0.3E0.5/log_collision_n200.txt
gawk -v node_count=400 -f bsm.awk SATMAC/T0.3E0.5/log_collision_n400.txt
gawk -v node_count=600 -f bsm.awk SATMAC/T0.3E0.5/log_collision_n600.txt
gawk -v node_count=800 -f bsm.awk SATMAC/T0.3E0.5/log_collision_n800.txt
gawk -v node_count=1000 -f bsm.awk SATMAC/T0.3E0.5/log_collision_n1000.txt
gawk -v node_count=1200 -f bsm.awk SATMAC/T0.3E0.5/log_collision_n1200.txt
gawk -v node_count=1400 -f bsm.awk SATMAC/T0.3E0.5/log_collision_n1400.txt
gawk -v node_count=1600 -f bsm.awk SATMAC/T0.3E0.5/log_collision_n1600.txt

echo "ADJ_ALL T0.4E0.5"
gawk -v node_count=200 -f bsm.awk SATMAC/T0.4E0.5/log_collision_n200.txt
gawk -v node_count=400 -f bsm.awk SATMAC/T0.4E0.5/log_collision_n400.txt
gawk -v node_count=600 -f bsm.awk SATMAC/T0.4E0.5/log_collision_n600.txt
gawk -v node_count=800 -f bsm.awk SATMAC/T0.4E0.5/log_collision_n800.txt
gawk -v node_count=1000 -f bsm.awk SATMAC/T0.4E0.5/log_collision_n1000.txt
gawk -v node_count=1200 -f bsm.awk SATMAC/T0.4E0.5/log_collision_n1200.txt
gawk -v node_count=1400 -f bsm.awk SATMAC/T0.4E0.5/log_collision_n1400.txt
gawk -v node_count=1600 -f bsm.awk SATMAC/T0.4E0.5/log_collision_n1600.txt

echo "ADJ_ALL T0.4E0.6"
gawk -v node_count=200 -f bsm.awk SATMAC/T0.4E0.6/log_collision_n200.txt
gawk -v node_count=400 -f bsm.awk SATMAC/T0.4E0.6/log_collision_n400.txt
gawk -v node_count=600 -f bsm.awk SATMAC/T0.4E0.6/log_collision_n600.txt
gawk -v node_count=800 -f bsm.awk SATMAC/T0.4E0.6/log_collision_n800.txt
gawk -v node_count=1000 -f bsm.awk SATMAC/T0.4E0.6/log_collision_n1000.txt
gawk -v node_count=1200 -f bsm.awk SATMAC/T0.4E0.6/log_collision_n1200.txt
gawk -v node_count=1400 -f bsm.awk SATMAC/T0.4E0.6/log_collision_n1400.txt
gawk -v node_count=1600 -f bsm.awk SATMAC/T0.4E0.6/log_collision_n1600.txt

echo "ADJ_ALL T0.5E0.6"
gawk -v node_count=200 -f bsm.awk SATMAC/T0.5E0.6/log_collision_n200.txt
gawk -v node_count=400 -f bsm.awk SATMAC/T0.5E0.6/log_collision_n400.txt
gawk -v node_count=600 -f bsm.awk SATMAC/T0.5E0.6/log_collision_n600.txt
gawk -v node_count=800 -f bsm.awk SATMAC/T0.5E0.6/log_collision_n800.txt
gawk -v node_count=1000 -f bsm.awk SATMAC/T0.5E0.6/log_collision_n1000.txt
gawk -v node_count=1200 -f bsm.awk SATMAC/T0.4E0.6/log_collision_n1200.txt
gawk -v node_count=1400 -f bsm.awk SATMAC/T0.4E0.6/log_collision_n1400.txt
gawk -v node_count=1600 -f bsm.awk SATMAC/T0.4E0.6/log_collision_n1600.txt
