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
           diff $TheFile $TheFileReference >  TheDiff
           var=`ls -al TheDiff | awk '{print $5}'`
#          echo  $var 
	   if test ! "${var}" = 0 ; then
             echo " Problem! Files differ " 
             cat TheDiff 
	   else
             echo " Ok" 
	   fi
           /bin/rm -f TheDiff
         fi
      fi
     done
#
