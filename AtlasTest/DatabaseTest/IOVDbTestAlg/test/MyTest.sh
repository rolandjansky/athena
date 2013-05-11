#!/bin/zsh -xv

export POOL_CATALOG=relationalcatalog_sqlite_file:pool_db.db

#this one waits for nothing 
ATH=`which athena.py`
echo $ATH
pwd
rm -f *.db I.log II.log III.log *.root *xml* msg_send
cp ../share/AllWriteCool.py .
$ATH -c 'exRun=1;exTime=0' AllWriteCool.py>I.log
if [[ $? != 0 ]]; then
	return
fi

echo "First DONE!"
cp ../share/AllReadCool.py .
cp ../share/msg_send .
#this one after the first run waits for the msg from the third athena to finish.
$ATH AllReadCool.py>II.log &

sleep 60
$ATH -c 'exRun=3;exTime=55' AllWriteCool.py>III.log 
if [[ $? == 0 ]]; then
	msg_send
fi

cd ../test
post_check_with_select.sh I ^..IOVDbTestAlg
post_check_with_select.sh II ^..IOVDbTestAlg
post_check_with_select.sh III ^..IOVDbTestAlg

echo "ONLINE TEST FINISHED!"
