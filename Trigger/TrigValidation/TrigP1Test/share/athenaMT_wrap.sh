#!/bin/sh
exitcode=0
echo "checking if athenaMT is in the release, if not fake it"
if [ `which athenaMT.py` ] ; then 
  echo "athenaMT $*"
  athenaMT.py $*
  exitcode=$?
  echo "checking if output file exists"
  while [ $# -ge 1 ]; do 
      case $1 in 
          -o*)
              name=`echo $1 | cut -c3-`
              output_name="${name}._0001.data"
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
  echo "WARNING: Using regular athena to fake athenaMT"
  file=""
  jo=""
  command="doEF=False"
  while [ $# -ge 1 ]; do 
    echo $1
    case $1 in 
    -f )
    if test -s $2
     then
        file="BSRDOInput=[\"$2\"]"
     elif [ `echo $2 | cut -c1-7` =  "/castor" ]
     then
       rfdir $2
       file="BSRDOInput=[\"$2\"]"
     else 
        dir=`dirname $2`
        ls -l ${dir}/*.data
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
      echo "using file: ${file}"
      shift
    ;; 
    -n )
      command="${command};EvtMax=$2"
      echo "setting event max ${command}"
      shift 
    ;; 
    -c )
#     note this will auotmaticaly remove the quotes
      command="${command};$2"
      echo "adding options ${command}"
      shift 
    ;;
    -o*)
      echo "ignoring output option, this is fixed in athena"
      # no shift here 
    ;;
    * )
       jo=$1
    ;;
    esac 
   shift
  done 
  get_files ${jo}
  get_files BStoBS_post.py
# very ugly hack to get athena.py to see BSRDOInput line.
# this works on the command line, but not from inside my script
  echo $file >head.py
  echo "now split command into lines ${command}"
  echo "${command}" | sed "s/;/\n/g" >head2.py
  cat head.py head2.py ${jo} BStoBS_post.py > atn_jo.py
  athenacmd="athena.py  atn_jo.py" 
  echo $athenacmd 
  printenv >env.log 
  $athenacmd
  exitcode=$?
fi 
exit $exitcode
