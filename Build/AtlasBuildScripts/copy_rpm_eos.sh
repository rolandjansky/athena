#!/bin/bash
#
# Script to copy local nightly RPMs to EOS and run createrepo
#

ERROR_COUNT=0

# Function printing the usage information for the script
usage() {
    echo "Usage: copy_rpm_eos.sh <-b branch> <-a arch> <-d week_day> <-s source dir> " 
	echo "copy_rpm_eos.sh -b 22.0.X -a x86_64-slc6-gcc49-opt -s /build2/atnight/localbuilds/nightlies/22.0.X-GIT/build"

}

# Parse the command line arguments
BRANCH=""
ARCH=""
SOURCEDIR=""
WEEKDAY=""
DESTDIR="/eos/project/a/atlas-software-dist/www/RPMs/nightlies"

while getopts ":b:a:s:d:" opt; do
	case $opt in
	    b)
			BRANCH=$OPTARG
			;;
		a) 
			ARCH=$OPTARG
			;;
		s)
			SOURCEDIR=$OPTARG
			;;
		d)
			WEEKDAY=$OPTARG
			;;
			
		:)
			echo "Argument -$OPTARG requires a parameter!"
            usage
            exit 1
            ;;
        ?)
            echo "Unknown argument: -$OPTARG"
            usage
            exit 1
            ;;
    esac
done
# Make sure that the required options were all specified:
if  [ "$BRANCH" = "" ] || [ "$ARCH" = "" ] || [ "$SOURCEDIR" = "" ] ; then
    echo "Not all required parameters received!"
    usage
    exit 1
fi

if [ "$WEEKDAY" = "" ]; then
   DDAY=rel_`date +%w`
else 
   DDAY=${WEEKDAY}
fi
echo "====================================================="
echo "=== STARTING RPMs copy to /eos at `date`"
echo "====================================================="


DESTDIR=${DESTDIR}/${BRANCH}/${ARCH}/${DDAY}

if [ ! -d ${DESTDIR} ] ; then 
   echo "mkdir -p ${DESTDIR}"
   mkdir -p ${DESTDIR} 
   if [ ! -d ${DESTDIR} ] ; then ((ERROR_COUNT++)) ; fi  #avoid false positive eos error if the directory was actually created
fi

arr_rpm=(`(shopt -s nocaseglob; ls ${SOURCEDIR}/*.rpm)`)
if [ "${#arr_rpm[@]}" -le 0 ]; then
   echo "nicos_rpm: Warning: no rpm files are found in ${SOURCEDIR}"
   ((ERROR_COUNT++))
 else
   for ele in "${arr_rpm[@]}" 
   do
      echo "Info: copying $ele to ${DESTDIR}"                                                 
      cp -a $ele ${DESTDIR} || ((ERROR_COUNT++))
   done
fi 
echo "====================================================="
echo "=== Update http RPMs location"
echo "====================================================="
echo "nicos_rpm::::::: createrepo --workers 8 --update ${DESTDIR} :::::::" `date`
createrepo --workers 8 --update ${DESTDIR} || ((ERROR_COUNT++))

exit ${ERROR_COUNT}
