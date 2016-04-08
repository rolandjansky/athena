#!/bin/sh
timeminutes=$1
shift
command=$@
pattern="athena.py"

echo "------------------------------------------------------------------"
echo "[ATN] Starting test script $command"
echo "[ATN] on machine ${NICOS_HOSTNAME}" 
echo "[ATN] Wall clock time limit is $timeminutes minutes" 
echo "------------------------------------------------------------------"

(sleep 1; \
 spp=`ps -ww -u $UID -o pid,ppid,args | grep -e "$command" | grep -v grep | grep -v "wrapper" | head -1 | awk '{print $1}'` ;\
 if [ "$spp" = "" ]; then spp=99999999; fi;\
 sppg=`ps -ww -u $UID -o pgid,ppid,args | grep -e "$command" | grep -v grep | grep -v "wrapper" | head -1 |  awk '{print $1}'` ;\
 a=1; while [ "$a" -le "$timeminutes" ]; do \
 sleep 60; \
 echo "[ATN, test wrapper] minutes elapsed: $a "; \
 let "a+=1"; \
 spp_check=`ps -ww -u $UID -o pid,ppid,args | grep -e "$command" | grep -v grep | grep -v "wrapper" | head -1 | awk '{print $1}'` ;\
 if [ "$spp" != "$spp_check" ]; then echo "[ATN, test wrapper] completion of test process is detected"; break; fi; done;\
 spp_check=`ps -ww -u $UID -o pid,ppid,args | grep -e "$command" | grep -v grep | grep -v "wrapper" | head -1 | awk '{print $1}'` ;\
 if [ "$spp" = "$spp_check" ]; then echo "[ATN, test wrapper] test killed as time quota spent, test warning is issued"; ${NICOS_HOME}/nicos_kill_fam.pl $spp 9 > ${HOME}/b.log 2>&1; fi;\
 ps -ww -u $UID -o pgid,pid,args | grep -e "$command" | grep -v grep | grep -v "wrapper" | while read apgid apid acmd; do \
 if [ "$sppg" != "" -a "$apgid" = "$sppg" -a "$apid" != "" ]; then kill -9 $apid; echo "[ATN, test wrapper] process $apid is run away, killed"; echo " "; fi;\
done)&

$@
exit
