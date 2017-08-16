# ATN test script to check ID reconstruction with Pixel, SCT or TRT off in turn
# Carl Gwilliam <gwilliam@mail.cern.ch> 

splitPoint() {
    # Split before InDetRec_all called and InDetFlags locked
    splitLine=`grep -n "IMPORTANT NOTE" $ATN_PACKAGE/share/jobOptions_monitoring.py | cut -d ':' -f 1 | grep -v "^$"`
    echo $splitLine
}

makePixOpts() {
    # Make Options for pixel off

    splitLine=`splitPoint`
    splitLineMinus1=$(( splitLine - 1 ))

    head -n $splitLineMinus1  $ATN_PACKAGE/share/jobOptions_monitoring.py > jobOptions_nopixel.py
    cat >> jobOptions_nopixel.py << EOF
# Test script options
DetFlags.pixel_setOff()
DetFlags.detdescr.pixel_setOn()
#InDetFlags.doiPatRec             = False
InDetFlags.doTrackSegmentsPixel  = False
InDetFlags.doTrackSegmentsPixelPrdAssociation = False
InDetFlags.doMonitoringPixel     = False
InDetFlags.doPixelClusterSplitting = False
InDetFlags.doTIDE_Ambi = False
# End

EOF
    tail -n +$splitLine  $ATN_PACKAGE/share/jobOptions_monitoring.py >> jobOptions_nopixel.py
    echo "theApp.EvtMax = 1" >> jobOptions_nopixel.py
}

makeSCTOpts() {
    # Make Options for SCT off

    splitLine=`splitPoint`
    splitLineMinus1=$(( splitLine - 1 ))

    head -n $splitLineMinus1  $ATN_PACKAGE/share/jobOptions_monitoring.py > jobOptions_nosct.py
    cat >> jobOptions_nosct.py << EOF
# Test script options
DetFlags.SCT_setOff()
DetFlags.detdescr.SCT_setOn()
InDetFlags.doTrackSegmentsSCT    = False
InDetFlags.doMonitoringSCT       = False
# End

EOF
    tail -n +$splitLine  $ATN_PACKAGE/share/jobOptions_monitoring.py >> jobOptions_nosct.py
    echo "theApp.EvtMax = 1" >> jobOptions_nosct.py
}

makeTRTOpts() {
    # Make Options for TRT off

    splitLine=`splitPoint`
    splitLineMinus1=$(( splitLine - 1 ))

    head -n $splitLineMinus1  $ATN_PACKAGE/share/jobOptions_monitoring.py > jobOptions_notrt.py
    cat >> jobOptions_notrt.py << EOF
# Test script options
DetFlags.TRT_setOff()
DetFlags.detdescr.TRT_setOn()
InDetFlags.doTrackSegmentsTRT    = False
InDetFlags.doMonitoringTRT       = False
InDetFlags.doTrackSegmentsPixelPrdAssociation = False
# End

EOF
    tail -n +$splitLine  $ATN_PACKAGE/share/jobOptions_monitoring.py >> jobOptions_notrt.py
    echo "theApp.EvtMax = 1" >> jobOptions_notrt.py
}

preCheck() {
    # Pre test steps
    echo "## Starting pre-checks"
    
    # Cleaning
    echo "## Cleaning"
    /bin/rm -f geomDB sqlite200
       
    # Local DB 
    #echo "## Setting up local DB access"
    #setupLocalDBReplica_CERN.sh
       
    # Make job options
    makePixOpts
    makeSCTOpts
    makeTRTOpts

    echo "## ready to run"
    echo -e "##\n##\n##"
}

postCheck() {
    # Post test steps
    echo "### Starting post-checks"
    
    echo "## Running in $pwd"

    # Log file
    echo -e "##\n##\n## Dumping log file for pixel-off run ...\n##\n##"
    cat nopixel.log
    echo -e "##\n##\n## Dumping log file for SCT-off run ...\n##\n##"
    cat nosct.log
    echo -e "##\n##\n## Dumping log file for TRT-off run ...\n##\n##"
    cat notrt.log
 
    # Cleaning
    echo "## Cleaning"
    /bin/rm -f geomDB sqlite200 # nopixel.log nosct.log notrt.log jobOptions_nopixel.py jobOptions_nosct.py jobOptions_notrt.py
}

logCheck() {
    return `grep -E " ERROR | FATAL " $1 | wc -l`
}

# Running 

preCheck

athena jobOptions_nopixel.py >& nopixel.log
rcPix=$?
logCheck nopixel.log 
rcPix=$(( $rcPix + $? )) 

athena jobOptions_nosct.py >& nosct.log
rcSCT=$?
logCheck nosct.log 
rcSCT=$(( $rcSCT + $? )) 

athena jobOptions_notrt.py >& notrt.log
rcTRT=$?
logCheck notrt.log 
rcTRT=$(( $rcTRT + $? )) 

postCheck

exit $(( $rcPix + $rcSCT * 100 + $rcTRT * 10000 ))
