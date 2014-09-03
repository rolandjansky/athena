echo " Compare output file with reference one "
#
     cmt show path
     TheRelease=`cmt show path | sed -n "s#.\+/\(.\+\)/AtlasProduction/.\+#\1#gp" | sed "s/\-Prod//"`
     if [ -z $TheRelease ]; then
	 TheRelease=`cmt show path | sed -n "s#.\+/\(.\+\)/AtlasReconstruction/.\+#\1#gp"`
     fi
     echo "running in $TheRelease"
     Result=0
     for TheFile in $* ; do
       echo "-------->Comparing" $TheFile 
       if test ! -f $TheFile ; then
         echo " Problem! Not existing file " $TheFile 
       else
         TheFileReference="../Testing/Reference_${TheFile}"
         if test -f /afs/cern.ch/user/r/rig/ReferenceFiles/RecExCommon/$TheRelease/Reference_${TheFile} ; then
	     TheFileReference="/afs/cern.ch/user/r/rig/ReferenceFiles/RecExCommon/$TheRelease/Reference_${TheFile}"
	     echo " Found Reference file for release $TheRelease in 'rig' area, using this one instead !" $TheFileReference
	 fi
#        echo  $TheFileReference 
	 ThisFileReference=`basename $TheFileReference`
         if test ! -f $TheFileReference ; then
           echo " Problem! Not existing file " $TheFileReference 
	   Result=2
         else
#only keep line with "error" or "warning" or "fatal"
#keep last message from Application Manger to checkfinalized successfull
           /bin/rm -f TheDiff ${TheFile}_stripped ${TheFileReference}_stripped
           grep -E "WARNING|ERROR|FATAL" $TheFile | grep  -v " [Ss]unday conditions" > ${TheFile}_stripped
           grep -Ei "Application Manager Finalized successfully" $TheFile >> ${TheFile}_stripped
           grep -E "WARNING|ERROR|FATAL" $TheFileReference | grep  -v " [Ss]unday conditions" > ${ThisFileReference}_stripped
           grep -Ei "Application Manager Finalized successfully" $TheFileReference >> ${ThisFileReference}_stripped
	   echo "Current Job" > TheLogs
	   echo -n " INFO " >> TheLogs
	   grep " INFO " ${TheFile} | wc -l >> TheLogs
	   grep " INFO " ${TheFile} | gawk '{print $1 " " $2 }' | sort | uniq -c | sort -nr | head -10 >> TheLogs
	   grep " DEBUG " ${TheFile} >> TheLogs
	   grep " VERBOSE " ${TheFile} >> TheLogs

           diff ${ThisFileReference}_stripped ${TheFile}_stripped >  TheDiff
	   if [ $? -ne 0 ]
	   then
             echo " FAILURE Files differ " 
             cat TheDiff
	     Result=1
	   else
             echo " Ok" 
	   fi
	   echo
	   echo
	   echo "count of INFO, DEBUG and VERBOSE messages:"
	   grep -A8000 "^Referencefile" $TheFileReference | diff -y TheLogs -
#           /bin/rm -f TheDiff ${TheFile}_stripped ${TheFileReference}_stripped 
         fi
      fi
     done
     return $Result
#


