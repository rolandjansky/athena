echo " Compare output file with reference one "
#
     for TheFile in $* ; do
       echo "-------->Comparing" $TheFile 
       if test ! -f $TheFile ; then
         echo " Problem! Not existing file " $TheFile 
       else
         TheFileReference="../Testing/Reference_${TheFile}"
#        echo  $TheFileReference 
         if test ! -f $TheFileReference ; then
           echo " Problem! Not existing file " $TheFileReference 
         else
#only keep line with "error" or "warning" or "fatal"
#keep last message from Application Manger to checkfinalized successfull
           /bin/rm -f TheDiff ${TheFile}_stripped ${TheFileReference}_stripped
           grep -Ei "WARNING|ERROR|FATAL" $TheFile > ${TheFile}_stripped
           grep -Ei "Application Manager Finalized successfully" $TheFile >> ${TheFile}_stripped
           grep -Ei "WARNING|ERROR|FATAL" $TheFileReference > ${TheFileReference}_stripped
           grep -Ei "Application Manager Finalized successfully" $TheFileReference >> ${TheFileReference}_stripped

           diff ${TheFile}_stripped ${TheFileReference}_stripped >  TheDiff
           var=`ls -al TheDiff | awk '{print $5}'`
#          echo  $var 
	   if test ! "${var}" = 0 ; then
             echo " FAILURE Files differ " 
             cat TheDiff 
	   else
             echo " Ok" 
	   fi
#           /bin/rm -f TheDiff ${TheFile}_stripped ${TheFileReference}_stripped 
         fi
      fi
     done
#


