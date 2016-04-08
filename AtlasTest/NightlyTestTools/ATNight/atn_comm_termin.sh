#!/bin/sh
pattern1="python"
pattern2="itx"

 sppg=`ps -ww -p $$ -o pgid=,ppid=,args= | grep -v grep | awk '{print $1}'` 
# echo "sppg $sppg";
# 
 ps -ww -u $UID -o pgid,pid,ppid,args | grep -e "$pattern1" | grep -e "$pattern2" | grep -v grep |  while read apgid apid appid acmd; do 
# echo "apgid apid appid acmd $apgid $apid $appid $acmd"
 if [ "$appid" = "1" ]; then ${NICOS_HOME}/nicos_kill_fam.pl $apid 9; echo "[ATN, runaway python process terminator] process $apid is run away, killed"; echo " "; fi;
 done
#
exit
