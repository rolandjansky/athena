#!/usr/bin/env zsh

CURRDIR=`pwd`
echo in $CURRDIR
echo CMTPATH=$CMTPATH
#echo PATH=$PATH
#echo $LD_LIBRARY_PATH=$LD_LIBRARY_PATH

if [ "x${ATN_PACKAGE}" = "x" ] ; then
  ATN_PACKAGE=..
  echo Seem to be running outside of ATN. Have set ATN_PACKAGE to be ${ATN_PACKAGE}
fi


# echo "******  Directory contains:"
# ls

# different stages 
WRITE=X
CHECKESD=X
READ=X
CHECKREF=X
CHECKSUMMARIES=X
RDOTOBS="Not run"
BSTOESD="Not run"

echo "Part 1: Writing ESD." > logfile
rm -f ESD.pool.root
#athena.py -c "doRecoOff=True" -s WriteESDfromRDO/writeTopOptions.py >! write.log 2>&1
athena.py -c 'doRecoOff=False' MuonTestEDM/writeTopOptions.py >! write.log 2>&1
if [ "${?}" != 0 ] ; then
    echo " - FAILURE! "
    grep  WARNING write.log | grep -v OutputLevel
    grep  ERROR write.log | grep -v OutputLevel
else
    WRITE=OK
    echo " - Passed. Scanning for ERRORs, WARNINGs, FATALS (but will continue even if found)"
    grep  WARNING write.log | grep -v OutputLevel
    grep  ERROR write.log | grep -v OutputLevel
    echo 
    echo " ============ Summary:"
    cat WriteESDfromRDO.summary.log
    echo
    echo "Lengths of dump files:"
    wc -l WriteESDfromRDO.rdo.log
    wc -l WriteESDfromRDO.prd.log
    wc -l WriteESDfromRDO.track.log    
    wc -l WriteESDfromRDO.segment.log    
    echo 
    echo "Part 2: Checking contents of ESD file."
    checkFile.py ESD.pool.root >! checkFile.log 2>&1
    if [ "${?}" != 0 ] ; then
        echo " - FAILURE! "
        #cat checkFile.log
        #echo "Directory contains:"
        #ls
        #exit 999
    else
        CHECKESD=OK
        echo " - Passed."
        echo "Dumping checkFile.log:"
        cat checkFile.log
        echo 
        echo "Part 3: Reading ESD."

        athena.py  MuonTestEDM/readTopOptions.py >! read.log 2>&1
        if [ "${?}" != 0 ] ; then
            echo "FAILURE! "
            grep  WARNING write.log | grep -v OutputLevel
            grep  ERROR write.log | grep -v OutputLevel
            #cat read.log
            #exit 999
        else
            READ=OK
            echo " - Athena terminated okay. Now checking if MuonTestEDM found problems."
            grep MuonTestEDM read.log | grep ERROR
            if [ "${?}" != 1 ] ; then
                READ=X
                echo "FAILURE! MuonTestEDM reported an error!"
            fi
            
            echo 
            echo " ============ Summary:"
            cat ReadESD.summary.log
            echo
            echo "Part 4: Comparing dump files (ignoring positions)"

            diff -I "Position" --text WriteESDfromRDO.rdo.log ReadESD.rdo.log >! diffRDO.log
            wc -l diffRDO.log
            diff -I "Position" --text WriteESDfromRDO.prd.log ReadESD.prd.log >! diffPRD.log
            wc -l diffPRD.log
            diff -I "Position" --text WriteESDfromRDO.track.log ReadESD.track.log >! diffTrack.log
            wc -l diffTrack.log
            diff -I "Position" --text WriteESDfromRDO.segment.log ReadESD.segment.log >! diffSegment.log
            wc -l diffSegment.log
            echo
            echo "Part 5. Now dumping Diff files"
            echo " ============ diffRDO.log"
            # cat diffRDO.log
            echo " - Suppressed, until I can hide minor position / momentum shifts"
            
            echo " ============ diffPRD.log"
            cat diffPRD.log
            echo " ============ diffTrack.log"
            # cat diffTrack.log
            echo " - Suppressed, until I can hide minor position / momentum shifts"
            
            echo " ============ diffSegment.log"
            # cat diffSegment.log
            echo " - Suppressed, until I can hide minor position / momentum shifts"
            
        fi
    fi
fi


# echo
# echo "Part 6i. Now checking reference ESD file (from 13.0.30)."
# #athena.py -c 'PoolESDInput=["/afs/cern.ch/atlas/maxidisk/d33/referencefiles/T1_5200.13.0.30.2.ESD.013836._00001.pool.root"]' ../share/readTopOptions.py >! readOld.log 2>&1
# # athena.py -c 'PoolESDInput=["/afs/cern.ch/user/e/emoyse/public/ReferenceESD/ESD.13.0.40.pool.root"] ; DumpFileName="ReadOut13040"' ../share/readTopOptions.py >! readOld.log 2>&1
# # if [ "${?}" != 0 ] ; then
# #   echo "FAILURE! Tail of readOld.log: "
# #   #tail readOld.log
# #   #exit 999
# #   CHECKREF=X
# # else 
# #   CHECKREF=OK
# # fi
# echo "Test temporarily disabled, until I find a new reference file"

echo
echo "Part 6i. Now checking reference ESD file (from 15.0.1)"
athena.py -s -c 'EvtMax=-1; DetDescrVersion="ATLAS-GEO-02-01-00" ; PoolESDInput=["/afs/cern.ch/user/e/emoyse/public/ReferenceESD/ESD_15.0.1_ttbar.pool.root"] ; PoolESDOutput="copy_ESD.15.0.1.pool.root" ;DumpFileName="ReadOut150001"' ../share/readTopOptions.py >! read15.0.1.log 2>&1

if [ "${?}" != 0 ] ; then
  echo "FAILURE!"
  CHECKREF=X
else 
  CHECKREF=OK
fi

if test "${ATN_PACKAGE+set}" != set ; then
   ATN_PACKAGE=..
fi

echo "Part 6ii. Now check positions"
cd ${ATN_PACKAGE}/test
make
cd $CURRDIR
# Do not check local positions for RPC PRDs for this sample becayuse of https://savannah.cern.ch/bugs/?56227
${ATN_PACKAGE}/test/comparePositions -l --prd RPC --ref /afs/cern.ch/user/e/emoyse/public/ReferenceESD/ESD_15.0.1_ttbar.root --file ReadOut150001.root
if [ "${?}" != 0 ] ; then
  echo "RPC FAILURE!"
  CHECKREF=X
fi
${ATN_PACKAGE}/test/comparePositions  --prd MDT --ref /afs/cern.ch/user/e/emoyse/public/ReferenceESD/ESD_15.0.1_ttbar.root --file ReadOut150001.root
if [ "${?}" != 0 ] ; then
  echo "MDT FAILURE!"
  CHECKREF=X
fi
${ATN_PACKAGE}/test/comparePositions  --prd CSC --ref /afs/cern.ch/user/e/emoyse/public/ReferenceESD/ESD_15.0.1_ttbar.root --file ReadOut150001.root
if [ "${?}" != 0 ] ; then
  echo "CSC FAILURE!"
  CHECKREF=X
fi
${ATN_PACKAGE}/test/comparePositions  --prd TGC --ref /afs/cern.ch/user/e/emoyse/public/ReferenceESD/ESD_15.0.1_ttbar.root --file ReadOut150001.root
if [ "${?}" != 0 ] ; then
  echo "TGC FAILURE!"
  CHECKREF=X
fi


# echo "Part 6iii.Now checking reference ESD file (from 15.5.1)"
# athena.py -s -c 'EvtMax=-1; DetDescrVersion="ATLAS-GEO-08-00-00" ; PoolESDInput=["/afs/cern.ch/user/e/emoyse/public/ReferenceESD/ESD_15.5.1.pool.root"] ; DumpFileName="ReadOut150501"' ../share/readTopOptions.py >! read15.5.1.log 2>&1
# 
# if [ "${?}" != 0 ] ; then
#   echo "FAILURE!"
#   #tail read15.5.1.log
#   CHECKREF=X
# else 
#   CHECKREF=OK
# fi



echo "Part 7. BS to ESD test"
#athena.py ../share/bstoesd.py >! bstoESD.log 2>&1
# if [ "${?}" != 0 ] ; then
#   echo "FAILURE!"
#   BSTOESD=X
# else 
#   BSTOESD=OK
# fi

# echo "Part 8. RDO to BS test (just see if it runs for now)"
# athena.py ../share/rdotobs.py >! rdoToBS.log 2>&1
# if [ "${?}" != 0 ] ; then
#   echo "FAILURE! Tail of rdotobs.log: "
#   #tail readOld.log
#   #exit 999
# else 
#   RDOTOBS=OK
# fi

echo "Part 8. Finalise. Comparing summaries (diff WriteESDfromRDO.summary.log  ReadESD.summary.log)"
diff --text WriteESDfromRDO.summary.log  ReadESD.summary.log
if [ "${?}" != 0 ] ; then
    echo " - FAILURE! Summaries differ when comparing written and read ESDs!"
    #exit 999
else
    echo " - OKAY : Summaries appear to match."
    CHECKSUMMARIES=OK
    echo Summaries match. All okay.
fi

echo "Part 9. Check to see if ANY of the summary files reported errors."
grep -L "Detailed tests found : 0 problems."  *.summary.log >! listOfSummariesWithErrors.txt
LENGTH=`wc -l listOfSummariesWithErrors.txt | awk '{ print $1 }' - `
if [[ $LENGTH -gt 0 ]] ; then
    echo " - FAILURE! Summaries below have found errors with the detailed checks! "
    cat listOfSummariesWithErrors.txt
    CHECKSUMMARIES=X
else
    CHECKSUMMARIES=OK
    echo Summaries match. All okay.
fi

#echo "And for 13.0.40 ESDs (diff ReadOut13040.summary.log  ReadOut13040.ref.summary.log)"
#diff ReadOut13040.summary.log  /afs/cern.ch/user/e/emoyse/public/WriteESDfromRDOFiles/ReadOut13040.ref.summary.log
#if [ "${?}" != 0 ] ; then
#    echo " - FAILURE! Summaries differ when reading reference 13.0.40 file!"
#fi


echo  "| Write File | Check File | Read File | Read 13.0.40 | Check Summaries | BS to ESD | RDO to BS "
echo  "-------------|------------|-----------|--------------|-----------------|------------"
echo  "      ${WRITE}           $CHECKESD          $READ            $CHECKREF               $CHECKSUMMARIES        $BSTOESD     $RDOTOBS"
echo  "-------------|------------|-----------|--------------|-----------------|------------"

FAILURE=NO
if test "$WRITE" = 'X'; then echo "Write test failed. Dumping log"; cat write.log; FAILURE=YES ;fi
if test "$READ" = 'X'; then echo "Read test failed. Dumping log"; cat read.log; FAILURE=YES ;fi
if test "$CHECKREF" = 'X'; then echo "Read ref test failed. Dumping log"; cat read15.0.1.log; FAILURE=YES ;fi
if test "$CHECKSUMMARIES" = 'X'; then echo "Summaries test failed."; FAILURE=YES ;fi
if test "$RDOTOBS" = 'X'; then echo "RDO to BS test failed. Dumping log"; cat rdoToBS.log; FAILURE=YES ;fi
if test "$BSTOESD" = 'X'; then echo "BS to ESD test failed. Dumping log"; cat bstoESD.log; FAILURE=YES ;fi

# make test fail
if test "$FAILURE" = 'YES'; then echo "Reporting failure and exiting." ; exit 1 ;fi
