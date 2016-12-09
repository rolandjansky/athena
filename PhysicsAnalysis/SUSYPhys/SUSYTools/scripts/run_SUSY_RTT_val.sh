MYREL="rel_0"
if [ $# -eq 0 ]
then
    MYREL='rel_'$(date +%w)
else
    MYREL=$1
fi

OUTDIR="$HOME/www/public_html/BkgForum/RTTval/"


TheList=`ls /afs/cern.ch/atlas/project/RTT/prod/Results/rtt/$MYREL/20.7.X.Y-VAL/build/x86_64-slc6-gcc49-opt/AtlasDerivation/DerivationFrameworkRTT/*15SUSY*Merge/test.pool.root | sort`

if [ $# -eq 2 ] #you can pick one flavour only too
then
    TheList=`ls /afs/cern.ch/atlas/project/RTT/prod/Results/rtt/$MYREL/20.7.X.Y-VAL/build/x86_64-slc6-gcc49-opt/AtlasDerivation/DerivationFrameworkRTT/*$2*Merge/test.pool.root | sort`
fi

CSVOUT="$OUTDIR/rtt_susy_${MYREL}.csv"

#clear logs
rm -f $CSVOUT  #test results for rel_x
rm -f $CSVOUTSTR  #stream names

for sample in $TheList
do
    stream=${sample#*kRTT/}
    stream=${stream%Merge/test.pool.root}
    echo 'Validating : '$stream' : ('$sample')'

    isData="isData=0"
    PRWFile=""
    if echo "$stream" | grep -q "Data"; then
	isData="isData=1"
	PRWFile="PRWFile=/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/dev/SUSYTools/merged_prw_mc15c.root"
    fi
    
    MYLOG="$OUTDIR/logs/${MYREL}/SUSY_rtt_val_${stream}_${MYREL}.log"
    CSVOUTBR="$OUTDIR/logs/${MYREL}/SUSY_rtt_${stream}_${MYREL}_branches.csv"

    RTT_PATH="/afs/cern.ch/atlas/project/RTT/prod/Results/rtt/${MYREL}/20.7.X.Y-VAL/build/x86_64-slc6-gcc49-opt/AtlasDerivation/DerivationFrameworkRTT/${stream}Merge/"
  
    #run the test
    SUSYToolsTester ${RTT_PATH}/test.pool.root -1 ${isData} isAtlfast=0 Debug=0 NoSyst=0 ${PRWFile} ConfigFile=SUSYTools/SUSYTools_Default.conf > $MYLOG 2>&1

    if echo `tail -2 ${MYLOG} | head -1` | grep -q "No issues found"; then
	echo ${stream},"OK" >> $CSVOUT
    elif echo `tail -2 ${MYLOG} | head -1` | grep -q "ERROR"; then
	echo ${stream},"FAIL" >> $CSVOUT
    elif echo `tail -1 ${MYLOG} ` | grep -q "xAOD::TFileAccessTracer"; then
	echo ${stream},"WARNING" >> $CSVOUT
    else
	echo ${stream},"FAIL" >> $CSVOUT
    fi

    #get content analysis
    sed '/## Bye/q' ${RTT_PATH}/RttLibraryTools_CheckFileRunner.postprocessing.stdout | sed -e '1,/File:/d' > ${CSVOUTBR}
    
done
