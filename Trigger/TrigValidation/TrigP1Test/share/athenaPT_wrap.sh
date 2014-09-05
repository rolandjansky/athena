#!/bin/sh
exitcode=0
echo "Checking if AthenaPT exists in this releae"
if [ `which athenaPT.py` ] ; then 
  echo "athenaPT.py $*"
  athenaPT.py $*
  exitcode=$?
  echo "checking if output file exists"
  while [ $# -ge 1 ]; do 
      case $1 in 
          -o*)
              name=`echo $1 | cut -c3-`
              output_name="${name}-1._0001.data"
              if test -s $output_name 
              then
		  echo "found file ${output_name}"
              else 
		  echo "WARNING:  output file ${output_name} not found, looking:"
		  ls -l
		  actual_file=`ls *.data`
		  nfiles=`echo $actual_file | wc -w`
		  if [ $nfiles -eq 1 ] 
		  then
                      echo "${actual_file} ${output_name}"
		      ln -s  ${actual_file} ${output_name}
		  fi 
              fi
            ;;
      esac  
      shift
  done   
else
  jo=""
  file=""
  command="doLVL2=False"
  while [ $# -ge 1 ]; do 
    echo $1
    case $1 in 
    -f )
     if test -s $2
     then
       file="BSRDOInput=[\"$2\"]"
     else 
       dir=`dirname $2`
       filename=`ls ${dir}/*.data`
       filename=`ls ${dir}/*.data`
       #
       # is there exactly one *.data file?  if so use it.
       #
       nfiles=`echo $filename | wc -w`
       if [ $nfiles -eq 1 ]
       then 
          echo "WARNING: Failed to find file ${2}, will try ${filename} instead"
         file="BSRDOInput=[\"${filename}\"]"
       else 
          echo "WARNING: Failed to find file ${2}, will use fixed copy on disk" 
         file="BSRDOInput=[\"/afs/cern.ch/atlas/project/trigger/pesa-sw/validation/references/data${2#..}\"]" 
       fi
     fi
       shift
    ;; 
    -c )
#     note this will auotmaticaly remove the quotes
      command="${command};$2"
      echo "command so far ${command}"
      shift 
    ;;
    -n )
      command="${command};EvtMax=$2"
      echo "setting event max ${command}$"
      shift 
    ;; 
    -o*)
      echo "ignoring ouput command, fixed in athena"
      #shift
    ;;
    *) 
     jo="$1" 
    ;; 
   esac 
   shift
  done 
  get_files ${jo}
  get_files BStoBS_post.py
# very ugly hack to get athena.py to see BSRDOInput line.
# this works on the command line, but not from inside my script
  echo $file >head.py
  cat head.py ${jo} > atn_jo.py
  echo "split up command line options: ${command}"
  echo "${command}" | sed "s/;/\n/g" >head2.py
  cat head.py head2.py ${jo} BStoBS_post.py > atn_jo.py
  athenacmd="athena.py atn_jo.py"
  echo $athenacmd 
  $athenacmd
  exitcode=$?
fi 
exit $exitcode
