#!/bin/sh
# ==================================================================================================================================#
#                                                                                                                                   #
#    MakeDoxygenClassSum -r <release>                                                                                               #
#                                                                                                                                   #
#    Unix script to create Doxygen summary for every class which has XML output already produced.                                   #
#    The script uses the information already created by doxygen in the AFSDOXYGENPATH.                                              #
#    It operates on the packages described in XMLList.txt file                                                                      #
#                                                                                                                                   #
#    Options:                                                                                                                       #
#     -r|--release <rel> Nightly or build release.                                                                                  #
#     -a|--afspath AFSDOXYGENPATH  AFS location where the doxygen is located.                                                       #
#          By default for nightly it is                                                                                             #
#                /afs/cern.ch/atlas/software/builds/nightlies/dev/AtlasOffline/<rel>/InstallArea/doc/                               #
#          for build releases it is                                                                                                 #
#               /afs/cern.ch/atlas/software/builds/AtlasOffline/<rel>/InstallArea/doc/                                              #
#                                                                                                                                   #
#####################################################################################################################################

function show_help() {
    echo "Usage: ${script} -r <rel> [-a <AFSDOXYGENPATH> ]"
    echo "  Options:"
    echo "    -r|--release : rel Nightly or build release."
    echo "    -a|--afspath : AFSDOXYTAGPATH  AFS location where the doxygen is located. "
    echo
}

script=`basename $0`
rel=
AFSDOXYGENPATH=
# --- Input parameters

if [ $# = 0 ]; then 
	show_help;exit 1
fi

while [ $# -ne 0 ]; do
	
    case $1 in
        -r   | --release)     rel=$2; shift;;
        -a   | --afspath)     AFSDOXYGENPATH=$2; shift;;
        -*   | --*)           show_help; exit 1;;

    esac
	shift
done

if [ `echo $rel | awk '{print substr($1,1,4)}'` == "rel_" ]; then
    # locations of the nightly
    if [ -z "$AFSDOXYGENPATH" ]; then 
    	AFSDOXYGENPATH="/afs/cern.ch/atlas/software/builds/nightlies/dev/AtlasOffline/$rel/InstallArea/doc"
    fi
else 
    # locations of the build release
    if [ -z "$AFSDOXYGENPATH" ]; then 
    	AFSDOXYGENPATH="/afs/cern.ch/atlas/software/builds/AtlasOffline/$rel/InstallArea/doc"
    fi

fi

if [ ! -d "$AFSDOXYGENPATH" ]; then
	echo "Directory $AFSDOXYGENPATH does not exist "
	exit 1
fi


OUTPUTPATH=${AFSDOXYGENPATH}/Summaries
for PACKAGE in `grep -v '#' ../cmt/XMLList.txt`
do
   mkdir -p ${OUTPUTPATH}/${PACKAGE}
   for CLASS in `ls --color=no ${AFSDOXYGENPATH}/${PACKAGE}/xml/class*xml`
   do 
     CLASSBASE=`basename ${CLASS} .xml`
     echo "Processing $CLASSBASE and outputing in ${OUTPUTPATH}/${CLASSBASE}.html"
     xsltproc ./pageClassExtraction.xsl ${CLASS} > ${OUTPUTPATH}/${PACKAGE}/${CLASSBASE}.html
   done
done
