#!/bin/bash

if [[ $# < 4 ]] || [[ $# > 8 ]];
then
    echo "Syntax: $0 [-append] [-openiov] <tag> <Run1> <LB1>  <File> [Run2] [LB2]"
    echo "optional -append is adding the content of File to a DB"
    echo "optional -openiov is updating UPD4 with open end IOV, if Run2/LB2 is not given" 
    echo "<tag> can be 'UPD1', 'UPD4', 'UPD3' or 'BOTH' or 'All' or 'Bulk'.  'BOTH' means UPD1 and UPD4, UPD4 update is automatically updating also Bulk. All means UPD1,UPD3 and UPD4 with Bulk. Bulk is updating only Bulk"
    echo "<Run1> <LB1> are start IOV (for UPD4/Bulk)"
    echo "<File> is text file with changed channels, each line should have: B/E pos_neg FT Slot Channel CalibLine BadBitDescription"
    echo "optional Run2 LB2 are end IOV (for UPD4/Bulk) - first LB after the end of problem, if not given open end update..."
    exit
fi


 

echo "Resolving current folder-level tag suffix for /LAR/BadChannelsOfl/BadChannels...."
fulltag=`getCurrentFolderTag.py "COOLOFL_LAR/CONDBR2" /LAR/BadChannelsOfl/BadChannels` 
upd4TagName=`echo $fulltag | grep -o "RUN2-UPD4-[0-9][0-9]"` 
echo "Found UPD4 $upd4TagName"
upd1TagName="RUN2-UPD1-00"
BulkTagName="RUN2-Bulk-00"
upd3TagName="RUN2-UPD3-00"

outputSqlite="BadChannels.db"
outputSqliteOnl="BadChannelsOnl.db"
summaryFile="LArBuildBadChannelDB.summary.txt"

if [ $1 == "-append" ]
then
    echo "Appending to previous bad-channel list"
    append=1
    shift
else
    append=0
fi

if [ $1 == "-openiov" ]
then
    echo "Open ended IOV, if Run2/LB2 is not supplied"
    openiov=1
    shift
else
    openiov=0
fi

tag=$1
shift
if [ $tag == "UPD1" ]
    then
    echo "Working on UPD1 list"
    tags="${upd1TagName}"
elif [ $tag == "UPD4" ]
    then
    echo "Working on UPD4 list"
    tags="${upd4TagName}"
elif [ $tag == "UPD3" ]
    then
    echo "Working on UPD3 list"
    tags="${upd3TagName}"
elif [ $tag == "Bulk" ]
    then
    echo "Working on Bulk list"
    tags="${BulkTagName}"
elif [ $tag == "BOTH" ]
    then
    echo "Working on UPD1 and UPD4 lists"
    tags="${upd1TagName} ${upd4TagName}"
elif [ $tag == "All" ]
    then
    echo "Working on UPD1, UPD3 and UPD4 lists"
    tags="${upd1TagName} ${upd3TagName} ${upd4TagName}"
else
    echo "ERROR, expected 'UPD1', 'UPD4' or 'BOTH' or 'All' or 'Bulk' or 'UPD3' as type, got: $tag"
    exit
fi

echo "tags" ${tags}

#echo $1
if echo $1 | grep -q "^[0-9]*$";
then
    runnumber=$1
    shift
else
    echo "ERROR: Expected a run-number, got $1"
    exit
fi

if echo $1 | grep -q "^[0-9]*$";
then
    lbnumber=$1
    shift
else
    echo "ERROR: Expected a lumi-block-number, got $1"
    exit
fi

if [[  $# == 0 ]]
    then
    echo "ERROR: No input files found!"
    exit
fi

if [ ! -f $1 ];
      then
      echo "ERROR File $1 not found!"
      exit
fi
echo "Adding $1"
catfiles=" $1"
shift


if [[  $# > 0 ]]
then
   if echo $1 | grep -q "^[0-9]*$";
   then
     runnumber2=$1
     shift
   else
     echo "ERROR: Expected a run-number, got $1, not using this parameter !!!"
     runnumber2=-1
     shift
   fi
else   
   runnumber2=-1
fi   

if [[  $# > 0 ]]
then
  if echo $1 | grep -q "^[0-9]*$";
  then
    lbnumber2=$1
    shift
  else
    echo "ERROR: Expected a lumi-block-number, got $1, not using Run2/lb2 parameter !!!"
    runnumber2=-1
    lbnumber2=-1
  fi
#  if [[ $runnumber2 > 0 ]] && [[ $lbnumber2 == 0 ]]
#  then
#    runnumber2=$[ $runnumber2 - 1 ]
#    lbnumber2=4294967295 
#  fi  
else
  lbnumber2=-1
fi


if [ -f $outputSqlite ];
    then
    echo "WARNING: Output file $outputSqlite exists already. Will be overwritten or modified!"

fi

if [ -f $outputSqliteOnl ];
    then
    if echo $tags | grep -q UPD1
	then
	echo "WARNING: Output file $outputSqliteOnl exists already. Will be overwritten!"
    fi
fi

if [ -f $summaryFile ]
    then 
    rm -rf $summaryFile
fi

touch $summaryFile



if ! which AtlCoolCopy 1>/dev/null 2>&1
then
    echo "No offline setup found!"
    exit
fi

for t in $tags
do

  echo Working on tag $t 
  inputTextFile="bc_input_$t.txt"
  outputTextFile="bc_output_$t.txt"
  oldTextFile="bc_previous_$t.txt"
  diffTextFile="bc_diff_$t.txt"


  if [ -f $inputTextFile ];
      then
      echo "Temporary file $inputTextFile exists already. Please remove!"
      exit
  fi

  if [ -f $outputTextFile ];
      then
      echo "Temporary file $outputTextFile exists already. Please remove!"
      exit
  fi


  if [ -f $oldTextFile ];
      then
      echo "Output file $oldTextFile exists already. Please remove!"
      exit
  fi


  echo "Running athena to read current database content...with run number " $runnumber
  athena.py -c "IOVEndRun=$runnumber;OutputFile=\"${oldTextFile}\";tag=\"LARBadChannelsOflBadChannels-${t}\"" LArBadChannelTool/LArBadChannel2Ascii.py > oracle2ascii_$t.log 2>&1
  if [ $? -ne 0 ];  then
      echo "Athena reported an error reading back sqlite file ! Please check oracle2ascii_$t.log!"
      exit
  fi

  if [ $append == 1 ]
      then 
      catfiles1="$oldTextFile $catfiles"
  else
      catfiles1=$catfiles
  fi

  cat $catfiles1 > $inputTextFile
  if [ $? -ne 0 ];  then
      echo "Failed to concatenate input files!"
      exit
  fi

  if [ $t == ${upd1TagName} ]
      then
      prefix=""
  else
      prefix="IOVBeginRun=${runnumber};IOVBeginLB=${lbnumber};"
      if [[ $openiov == 0 ]]
      then
        if  [[ $runnumber2 > 0 ]] && [[ $lbnumber2 -ge 0 ]]
        then
          prefix="${prefix}IOVEndRun=${runnumber2};IOVEndLB=${lbnumber2};" 
        else  
#prefix="${prefix}IOVEndRun=$runnumber;IOVEndLB=4294967295;"
          prefix="${prefix}IOVEndRun=$[ $runnumber + 1];IOVEndLB=0;"
        fi  
      fi  
  fi
  prefix="${prefix}sqlite=\"${outputSqlite}\";TagPostfix=\"-$t\";InputFile=\"${inputTextFile}\""
  echo "Running athena to build sqlite database file ..."
  echo $t $prefix
  athena.py -c $prefix LArBadChannelTool/LArBadChannelDBAlg.py > ascii2sqlite_$t.log 2>&1
  if [ $? -ne 0 ];  then
    echo "Athena reported an error! Please check ascii2sqlite_$t.log!"
    exit
  fi

  if grep -q ERROR ascii2sqlite_$t.log
      then
      echo "An error occured during ascii2sqlite job! Please check ascii2sqlite_$t.log!"
      exit
  fi

  if grep -q "REJECTED" ascii2sqlite_$t.log
      then
      echo "ERROR: At least one line in the input text file could not be read. Syntax Error?"
  fi

  echo "Running athena to test readback of sqlite database file"
  if [[ $runnumber2 > 0 ]] && [[ $lbnumber2 > 0 ]]
  then
        if [[ $lbnumber2 < 2147483648 ]]
        then
           prefix="IOVEndRun=${runnumber2};IOVEndLB=$[ ${lbnumber2} - 1 ];" 
        else   
           prefix="IOVEndRun=${runnumber2};IOVEndLB=2147483647;" 
        fi   
  else      
        if [[ $openiov == 1 ]]
        then
          prefix=""
        else  
          prefix="IOVEndRun=${runnumber};IOVEndLB=$[ $lbnumber + 1 ];"
        fi  
  fi  
  athena.py -c"${prefix}sqlite=\"${outputSqlite}\";OutputFile=\"${outputTextFile}\";tag=\"LARBadChannelsOflBadChannels-${t}\"" LArBadChannelTool/LArBadChannel2Ascii.py > sqlite2ascii_$t.log 2>&1

  if [ $? -ne 0 ];  then
      echo "Athena reported an error reading back sqlite file ! Please check sqlite2ascii_$t.log!"
      exit
  fi


  if `grep  ERROR sqlite2ascii_$t.log | grep -v "dupe ignored"`
      then
      echo "An error occured during reading back sqlite file ! Please check sqlite2ascii_$t.log!"
      exit
  fi

  
  if [ $t == ${upd4TagName} ]
  then
     echo "Copying UPD4 to Bulk as well..."
     AtlCoolCopy "sqlite://;schema=${outputSqlite};dbname=CONDBR2"  "sqlite://;schema=${outputSqlite};dbname=CONDBR2"  -f /LAR/BadChannelsOfl/BadChannels -t LARBadChannelsOflBadChannels-${upd4TagName} -of /LAR/BadChannelsOfl/BadChannels -ot LARBadChannelsOflBadChannels-${BulkTagName}
  fi   

  if [ $t == ${upd1TagName} ]
      then
      echo "Copying UPD1 for online database..."
      AtlCoolCopy "sqlite://;schema=${outputSqlite};dbname=CONDBR2" "sqlite://;schema=${outputSqliteOnl};dbname=CONDBR2" -f /LAR/BadChannelsOfl/BadChannels -t LARBadChannelsOflBadChannels-${upd1TagName} -of  /LAR/BadChannels/BadChannels -ot LARBadChannelsBadChannels-${upd1TagName} -a -c > AtlCoolCopy.onl.log 2>&1
      
      if [ $? -ne 0 ];  then
	  echo "AtlCoolCopy reported an error! Please check AtlCoolCopy.onl.log!"
	  exit
      fi
  fi

  if [ -f $diffTextFile ]
      then 
      rm -rf $diffTextFile
  fi

  diff $oldTextFile $outputTextFile > $diffTextFile

  nNew=`grep -c ">" $diffTextFile`
  nGone=`grep -c "<" $diffTextFile`
  nTotal=`wc -l $outputTextFile | cut -f 1 -d " "`
  
  echo "" >> $summaryFile
  echo "Summary info for $t tag" >> $summaryFile
  echo "  Added $nNew bad channels and removed $nGone from $t list" >> $summaryFile
  echo "  Total number of channel in the new $t list: $nTotal"  >> $summaryFile
  echo "  Total number of channel in the new $t list: $nTotal"  >> $summaryFile
  echo " Output text files:" >> $summaryFile
  echo "  $outputTextFile: Text version of the new bad channel list (read back from sqlite)" >> $summaryFile
  echo "  $oldTextFile: Text version of the previous database content" >> $summaryFile
  echo "  $diffTextFile: Diff between the two lists" >> $summaryFile

  if [ $t == $upd4TagName ]
      then
      #echo "Was a UPD4 tag" 
      if ! grep -q ${runnumber} /afs/cern.ch/user/a/atlcond/scratch0/nemo/prod/web/calibruns.txt
	  then
	  echo " *** WARNING *** Run ${runnumber} is not on the NEMO watchlist! Outside of CalibLoop?" >> $summaryFile
      fi
  fi

  echo "Done with $t"
  echo ""
done


cat $summaryFile

echo "Output sqlite files:"
echo "$outputSqlite: Containing UPD1 and/or UPD4 and/or Bulk version of bad-channel list for OFFLINE DB. UPD4 valid as of run $runnumber"
echo "Upload to OFFLINE oracle server:"
echo "~atlcond/utils/AtlCoolMerge.py ${outputSqlite} CONDBR2 ATLAS_COOLWRITE ATLAS_COOLOFL_LAR_W <password>"
if [ -f $outputSqliteOnl ];
then
    echo "$outputSqliteOnl: Containing UPD1 version of bad-channel list for ONLINE DB."
    echo "Upload to ONLINE oracle server using"
    echo "/afs/cern.ch/user/a/atlcond/utils/AtlCoolMerge.py --online ${outputSqliteOnl} CONDBR2 ATONR_COOL ATLAS_COOLONL_LAR_W <password>"
fi 

