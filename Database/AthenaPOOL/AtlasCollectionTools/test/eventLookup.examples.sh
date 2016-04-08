
OUTFILE=$TMPDIR/eventLookup.out
rm -f $OUTFILE
export DIR=`dirname $0`
#export PROG="python $DIR/../python/"
#PROG=
export OKMSG="=========  Tests OK ========="
export LC_ALL=C

#export SERVER="--worker voatlas237"

echo
if [ -n "$SERVER" ] ; then 
   echo "Running against server: $SERVER" ; 
else
   echo "Running against the default production server/port" ; 
fi
echo
echo "===> Note: some warning messages are to be expected during the test.  Look for $OKMSG at the end"

runtest() {
( 
  echo
  echo "----- Running test: $*"
  ${PROG}$1 $SERVER "$2" "$3" "$4" "$5" "$6" "$7" "$8" "$9" "${10}"  | sort
  ) | tee -a $OUTFILE
}

# test dataset with 2 first passes version
runtest runEventLookup.py -e '191933 60929518' -s physics_Egamma -t StreamAOD_ref 

runtest runEventLookup.py -e "160530 12345, 160530 1" --gettags
runtest runEventLookup.py -e "160530 12345" -t StreamESD_ref
runtest runEventLookup.py -e "160530 12345" -t StreamESD_ref -a
runtest runEventLookup.py -e "160530 12345" -s WRONG_STREAM_NAME

# standard tests
runtest runGuidsCount.py --ds data10_7TeV.00162882.physics_Egamma.merge.TAG.f287_m588_p250_p160  --query "EventNumber>299700 and EventNumber < 300000"  
runtest runEventLookup.py -f $DIR/events.1000.txt 
runtest runEventLookup.py -f $DIR/events.1000.txt --gettags

# test for merging identical GUID lines (bugfix)
runtest runGuidsCount.py --ds data11_7TeV.00180710.physics_JetTauEtmiss.merge.TAG.f371_m824_m823 -t 'StreamRAW_ref,StreamTAG_ref' -q "StreamRAW_ref.DATABASE_ID=='CAD8BFC1-9D74-E011-A744-003048D37340'"

# test Trigger decoding functionality
runtest runGuidsCount.py --ds data10_7TeV.00162882.physics_Egamma.merge.TAG.f287_m588_p250_p160 --query "EventNumber>299700 and EventNumber < 350000 and TriggerPassedInclusive(EF_e10_loose)" -t StreamRAW_ref 
# tag extraction for MC data with different Token names
runtest runEventLookup.py -e " 105013 85927" 
runtest runEventLookup.py -e "165767 14455783" --amitag '*f*'
runtest runEventLookup.py -e "165767 14455783" --amitag 'f293' --resolve_guids
runtest runEventLookup.py -e " 105013 85927" --gettags 
runtest runEventLookup.py -e " 105013 85927" --gettags -t 'StreamRDO_ref'
# test for one event from older repro pulling in a duplicate TAG of another event
runtest runEventLookup.py -s physics_Egamma --gett -t StreamAOD_ref -e '190872 85634590, 190878 7476299'
# test for file in more than 1 dataset
runtest runEventLookup.py -e '169927 95572, 169927 95609, 169927 95326' -t StreamRAW_ref --resolve

if [ $? = 0 ] ; then
    diff ${DIR}/eventLookup.examples.ref $OUTFILE
    if [ $? = 0 ] ; then
        echo $OKMSG
        rm -f $OUTFILE
    else 
        echo -e "\n Reference file $DIR/eventLookup.examples.ref is different than current output $OUTFILE \n"
    fi
else
    echo "Return code: $?"
fi

