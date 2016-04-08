#!/bin/bash

##########################################################################
#
# Loops over all directories given as first parameter, looking for files
# atlantis and VP1 output files and keeping their number limited to the 
# paramter given as second argument. This is repeated in the interval given
# as third argument
#
# author: Sebastian Boeser <sboeser@hep.ucl.ac.uk>
#
###########################################################################

## Printout environment
print_setup(){
  echo "DATAPATH=${DATAPATH}"
  echo "KEEPFILES=${KEEPFILES}"
  echo "UPDATEINT=${UPDATEINT}"
  echo "================================================================================"
}

## Print a short usage statement
print_usage(){
  echo "Usage: $0 <data-path> <keep-files> <update-intervall>"
  echo "   <data-path>: directories to search for Atlantis and VP1 files"
  echo "   <keep-files>: number of files to keep in each directory"
  echo "   <update-intervall>: repeat removal at this interval in seconds"
}

## get the commandline args
get_args(){
  #Check we have the proper number
  if test $# != 3; then
    print_usage
    exit 1
  fi
  DATAPATH="" 
  #First is pathes to look at
  for dir in `echo $1 | sed 's/:/ /g'`; do
    if [ ! -d $dir ]; then
      echo "Directory '$dir' does not exist - removed from list!"
    else
      DATAPATH="${DATAPATH} $dir"
    fi
  done
  #Check there is at least one dir left
  if [[ $DATAPATH == "" ]]; then
    echo "No directories to prune - exiting"
    exit 1
  fi
  KEEPFILES=0
  #now check keepfiles
  if (( $2 > 0 )); then
    if (($2 < 100000 )) ; then
      KEEPFILES=$2
    fi
  fi
  if (( $KEEPFILES == 0 )); then
    echo "Invalid number of files to keep: $2 (allowed range 1-99999) - exiting!"
    exit 1;
  fi 
  UPDATEINT=0
  #now check update interval
  if (( $3 > 0 )); then
    if (($3 < 10000 )) ; then
      UPDATEINT=$3
    fi
  fi
  if (( $UPDATEINT == 0 )); then
    echo "Invalid update intervall: $3 (allowed range 1-9999) - exiting!"
    exit 1;
  fi 
}

prune_dir(){
  if [ ! -z "${EVENTDISPLAY_VERBOSE}" ]; then
    echo "Entering $1 looking for $3 ..."
  fi
  #Get list of Atlantis files
  #Find file matching pattern, print last change time (unix secs) and name, 
  #sort by time, then only keep filename
  FILES=$( find $1 -name "$3" -printf "%C@ %f \n" | sort -n | awk '{print $2}' )
  #Calculate total number of files to remove
  NTOTAL=`echo $FILES | wc -w` 
  #Calculate number of files to delete
  NDELETE=$(( $NTOTAL-$2 ))
  if (( $NDELETE <= 0 )); then
    echo "Only $NTOTAL files total - not deleting any!"
    return
  fi 
  #Be verbose
  echo "Removing $NDELETE files of $NTOTAL total"
  if [ ! -z "${EVENTDISPLAY_VERBOSE}" ]; then
    VERBOSE=" -v"
  fi
  #Now loop and delete
  for file in  $FILES; do
    #Delete files
    if rm $VERBOSE -f $1/$file; then
      #Count successfully deleted
      NDELETE=$(( $NDELETE-1 ))
    fi
    #Break if we have deleted enough
    if (( $NDELETE == 0)); then
      break
    fi
  done
}

## Wait for a given time
wait_for(){
  if [ ! -z "${EVENTDISPLAY_VERBOSE}" ]; then
    date "+%Y-%m-%d %H:%M:%S"
    echo -n "Now sleeping for $1 seconds ..."
  fi
  sleep $1
  if [ ! -z "${EVENTDISPLAY_VERBOSE}" ]; then
    echo " - done"
  fi
}

## Write a file list consisting of run number, event number and associated data files
file_list(){
  if [ ! -z "${EVENTDISPLAY_VERBOSE}" ]; then
    echo "Creating file list for directory $1 ..."
  fi

  #Now generat the listing by
  #finding all files
  find $1 -name "*[0-9]*_[0-9]*" -printf "%f\n" |
  #printing run_event;filename and sorting it
  sed 's/vp1_\([0-9]*\)_\([0-9]*\).*/\1_\2;\0/'  |
  sed 's/JiveXML_\([0-9]*\)_0*\([0-9]*\)\.xml/\1_\2;\0/' | 
  sort -n |
  #Using awk to build an event table and print it
  awk -F ";" '/JiveXML/{ events[$1]=(events[$1] ",atlantis:" $2) }
             /vp1/{ events[$1]=(events[$1] ",vp1:" $2) }
             END{ for (evt in events) { 
               if ( (index(events[evt],"vp1") > 0) && (index(events[evt],"atlantis") > 0)){
                 split(evt,nums,"_");
                 printf "run:%012i,event:%012i%s\n",nums[1],nums[2],events[evt] 
               } 
             }}' |
  #And sorting again as awk does not provide sorted output
  sort -n >| $1/events.list.new
  #Now move the new list on the old one
  mv -f $1/events.list.new $1/event.list
}

##### This is the start of the actual script ####

#First of all setup the path
PATH=$PATH:$EVENTDISPLAY_SW_EXTRA_PATH

echo "EVENTDISPLAY_SW_EXTRA_PATH=$EVENTDISPLAY_SW_EXTRA_PATH"
echo "EVENTDISPLAY_VERBOSE=$EVENTDISPLAY_VERBOSE"
echo "PATH=$PATH"

echo "================== Start trimming of EventDisplay data ========================="
get_args $@
print_setup

#enter endless loop
while true; do
  #Now enter loop
  echo "==================== Entering loop over directories ============================"
  for dir in $DATAPATH; do
    prune_dir $dir $KEEPFILES "JiveXML_*.xml"
    prune_dir $dir $KEEPFILES "vp1_*.pool.root"
    file_list $dir
  done

  #then sleep
  wait_for $UPDATEINT
done

echo "===================== Finished pruning event display data ======================"
