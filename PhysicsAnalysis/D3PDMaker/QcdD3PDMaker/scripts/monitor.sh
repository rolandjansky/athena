#!/bin/sh

#usage: change athenasetup.sh and user in kin()
#then make file "list" which has list of pathena jobs just like:
#=======
# 356
# 357
# 358
#======
# then just ./monitor.sh
# and you can add jobs while the job is working

#set variables
sleeptime=600
athenasetup=athenasetup.sh

#set trap
trap "stty echo ; echo 'Interrupted' ; exit 1" 1 2 3 15

#set passs phrase
stty -echo
echo -n "pass phrase for grid: "
read passphrase
echo ""
echo -n "password for kerberos: "
read password
echo ""
stty echo

#certification for grid
function voms(){
  expect -c "
  spawn voms-proxy-init -hours 48 -voms atlas:/atlas/perf-jets/Role=production;
  expect \"Enter GRID pass phrase:\" ; send \"$passphrase\r\";
  interact;
  "
}
function kin(){
  expect -c "
  spawn kinit;
  expect \"Password for user@CERN.CH:\" ; send \"$password\r\";
  interact;
  "
}


#setup athena
echo source $athenasetup
source $athenasetup

#setup grid, pathena
source /afs/cern.ch/atlas/offline/external/GRID/ddm/DQ2Clients/setup.sh
source /afs/cern.ch/atlas/offline/external/GRID/DA/panda-client/latest/etc/panda/panda_setup.sh

#certification for grid
voms
kin

#sync at first fime
pbook -c "sync()"

#check list (voms must be listed)
vomsflag=0
while read num;do
  if [ "$num" = "voms" ];then
    vomsflag=1
    break
  fi
done < list
if [ $vomsflag -eq 0 ];then
  echo voms >> list
fi

#start loop
while [ 1 ];do
  num=`head -n1 list`
  sed -e '1d' list > list.tmp
  mv list.tmp list

  if [ "$num" = "" ];then
    echo voms >> list
    continue
  elif [ "$num" = "voms" ];then
    voms > /dev/null
    kin > /dev/null
    echo voms >> list
    echo at `hostname`
    date
    sleep $sleeptime
    pbook -c "sync()"
    continue
  else
    #check jobID in list
    pbook -c "show(${num})" > job.tmp
    type=`grep " type : " job.tmp|awk '{print $3}'`
    status=`grep " jobStatus : " job.tmp|awk '{print $3}'`
    if [ "$type" != "pathena" ];then
      echo "$num is not pathena job"
      continue
    elif [ ! "$status" = "frozen" ];then
      echo jobID = $num running
      echo $num  >> list
    else
      retry=`grep " retryID : " job.tmp|awk '{print $3}'`
      outDS=`grep " outDS : " job.tmp|awk '{print $3}'`
      if [ "$retry" -ne 0 ];then
        echo $retry >> list
        echo "$num was already retried -> $retry : $outDS"
        continue
      fi
      build=`grep " nJobs : " job.tmp|grep build`
      nJobs=`grep " nJobs : " job.tmp|awk '{print $3}'`
      if [ "$build" != "" ];then
        nJobs=`expr $nJobs + 1`
      fi
      finished=`grep " finished : " job.tmp|awk '{print $3}'`
      if [ "$finished" != $nJobs ];then
        pbook  -c "retry($num)" > retry.tmp 2>&1
        retry=`grep " New JobID=" retry.tmp|awk '{split($5,tmp,"=")}{print tmp[2]}'`
        echo $retry >> list
        echo "$num was not completed, then retry -> $retry: $outDS"
      else
        echo "$num was completed : $outDS"
        outDSs=(`echo "$outDS"|tr -s ',' ' '`)
        for out in ${outDSs[*]};do
          dq2-freeze-dataset $out
        done
      fi
    fi
  fi
done
