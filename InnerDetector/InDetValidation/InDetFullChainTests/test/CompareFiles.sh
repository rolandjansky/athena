echo " Compare output file with reference one "
#
     for TheFile in $* ; do
       echo "-------->Comparing" $TheFile 
       if test ! -f $TheFile ; then
         echo " Problem! Not existing file " $TheFile 
       else
#        discriminate which suite is running to pick up the 
#        corresponding Reference File
	 if grep -i "jobOptions_simulation_R1" $TheFile 
	 then
	     echo "Sim R1 test is running"
             Suite="Sim_R1"
	 elif grep -i "jobOptions_digitization_R1" $TheFile
	 then
	     echo "Digi R1 test is running"
	     Suite="Digi_R1"
	 elif grep -i "jobOptions_tracking_R1" $TheFile
	 then
	     echo "Reco R1 test is running"
	     Suite="Reco_R1"
	 elif grep -i "jobOptions_simulation_R2" $TheFile 
	 then
	     echo "Sim R2 test is running"
             Suite="Sim_R2"
	 elif grep -i "jobOptions_digitization_R2" $TheFile
	 then
	     echo "Digi R2 test is running"
	     Suite="Digi_R2"
	 elif grep -i "jobOptions_tracking_R2" $TheFile
	 then
	     echo "Reco R2 test is running"
	     Suite="Reco_R2"
	 else
	     echo "no test key string found in test log"
	 fi
#         TheFileReference="../test/Reference_${TheFile}"
         TheFileReference="/afs/cern.ch/atlas/maxidisk/d33/referencefiles/InDetFullChainTests/InDetFullChainTests-01-00-00/Reference_InDetFullChain_ExampleTesting${Suite}.log"
         echo  "TheFileReference name" $TheFileReference 
         TheFileReference_stripped=`basename $TheFileReference`_stripped
         if test ! -f $TheFileReference ; then
           echo " Problem! Not existing file " $TheFileReference 
         else
# only keep line with "error" or "warning" or "fatal"
# keep last message from Application Manger to checkfinalized successfull
           /bin/rm -f TheDiff ${TheFile}_stripped ${TheFileReference_stripped}
# grep the logfile
#           grep -Ei "WARNING|ERROR|FATAL" $TheFile > ${TheFile}_stripped
	   grep -Ei "Detailed Statistics for Hits on Reconstructed tracks" $TheFile >> ${TheFile}_stripped
           grep -Ei "Application Manager Finalized successfully" $TheFile >> ${TheFile}_stripped
# grep the reference
#           grep -Ei "WARNING|ERROR|FATAL" $TheFileReference > ${TheFileReference_stripped}
	   grep -Ei "Detailed Statistics for Hits on Reconstructed tracks" $TheFileReference >> ${TheFileReference_stripped}
           grep -Ei "Application Manager Finalized successfully" $TheFileReference >> ${TheFileReference_stripped}
# get the diffs
           diff ${TheFile}_stripped ${TheFileReference_stripped} >  TheDiff_all
# fix the warning from seal ...
	   grep -v "seal.opts" TheDiff_all > TheDiff

           var=`ls -al TheDiff | awk '{print $5}'`
#          echo  $var 
	   if test ! "${var}" = 0 ; then
             echo " -------------------- " 
             echo " FAILURE Files differ "
             echo " -------------------- " 
	     cat TheDiff 
	     exit 1
	   else
	     echo " -------------------- " 
	     echo " ---   Check Ok   --- " 
	     echo " -------------------- " 
	   fi
#           /bin/rm -f TheDiff ${TheFile}_stripped ${TheFileReference_stripped} 
         fi
      fi
     done
#


