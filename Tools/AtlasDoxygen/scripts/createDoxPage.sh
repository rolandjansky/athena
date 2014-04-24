#!/bin/sh
# ==================================================================================================================================#
#                                                                                                                                   #
#    createDoxPage -d <DOXYTAGPATH> -a <AFSDOXYTAGPATH>                                                                             #
#                                                                                                                                   #
#    Unix script to create one summary Doxygen web page for all packages.                                                           #
#    The script uses the information already created by doxygen in the AFSDOXYTAGPATH.                                              #
#                                                                                                                                   #
#    Options:                                                                                                                       #
#     -r|--release <rel> Nightly or build release.                                                                                  #
#     -d|--doxpath DOXYTAGPATH     Http location where the doxygen is located.                                                      #
#          By default for nightly it is                                                                                             #
#                  http://atlas-computing.web.cern.ch/atlas-computing/links/nightlyDevDirectory/AtlasOffline/<rel>/InstallArea/doc/ #
#          for build releases it is                                                                                                 #
#                 http://atlas-computing.web.cern.ch/atlas-computing/links/buildDirectory/AtlasOffline/<rel>/InstallArea/doc/       #
#     -a|--afspath AFSDOXYTAGPATH  AFS location where the doxygen is located.                                                       #
#          By default for nightly it is                                                                                             #
#                /afs/cern.ch/atlas/software/builds/nightlies/dev/AtlasOffline/<rel>/InstallArea/doc/                               #
#          for build releases it is                                                                                                 #
#               /afs/cern.ch/atlas/software/builds/AtlasOffline/<rel>/InstallArea/doc/                                              #
#                                                                                                                                   #
#####################################################################################################################################

function show_help() {
    echo "Usage: ${script} -r <rel> [-d <DOXYTAGPATH> -a <AFSDOXYTAGPATH>]"
    echo "  Options:"
    echo "    -r|--release rel Nightly or build release."
    echo "    -d|--doxpath DOXYTAGPATH     Http location where the doxygen is located."
    echo "    -a|--afspath AFSDOXYTAGPATH  AFS location where the doxygen is located. "
    echo
}

script=`basename $0`
rel=
DOXYTAGPATH=
AFSDOXYTAGPATH=
# --- Input parameters

if [ $# = 0 ]; then 
	show_help;exit 1
fi


while [ $# -ne 0 ]; do
	
    case $1 in
        -r   | --release)     rel=$2; shift;;
        -d   | --doxpath)     DOXYTAGPATH=$2; shift;;
        -a   | --afspath)     AFSDOXYTAGPATH=$2; shift;;
        -*   | --*)           show_help; exit 1;;

    esac
	shift
done

if [ -z "$rel" ]; then
    echo "Please specify nightly or build release with -r option."
    exit 1
fi

if [ `echo $rel | awk '{print substr($1,1,4)}'` == "rel_" ]; then
    # locations of the nightly
    if [ -z "$DOXYTAGPATH" ]; then 
    	DOXYTAGPATH="http://atlas-computing.web.cern.ch/atlas-computing/links/nightlyDevDirectory/AtlasOffline/$rel/InstallArea/doc/"
    fi
    if [ -z "$AFSDOXYTAGPATH" ]; then 
    	AFSDOXYTAGPATH="/afs/cern.ch/atlas/software/builds/nightlies/dev/AtlasOffline/$rel/InstallArea/doc/"
    fi
else 
    # locations of the build release
    if [ -z "$DOXYTAGPATH" ]; then 
    	DOXYTAGPATH="http://atlas-computing.web.cern.ch/atlas-computing/links/buildDirectory/AtlasOffline/$rel/InstallArea/doc/"
    fi
    if [ -z "$AFSDOXYTAGPATH" ]; then 
    	AFSDOXYTAGPATH="/afs/cern.ch/atlas/software/builds/AtlasOffline/$rel/InstallArea/doc/"
    fi

fi

if [ ! -d "$AFSDOXYTAGPATH" ]; then
	echo "Directory $AFSDOXYTAGPATH does not exist "
	exit 1
fi


TEMPATH=`pwd`

cd ${AFSDOXYTAGPATH}
cat $TEMPATH/BeginFragment > allpackages.html
for TAG in `ls --color=no *.tag`
do
  PACKAGENAME=`basename ${TAG} .tag`
  xsltproc --stringparam path "${PACKAGENAME}/html/" --stringparam packageName "$PACKAGENAME" $TEMPATH/pageExtraction.xsl "$TAG" >> allpackages.html 
  RETVALUE="$?"
  if [ "${RETVALUE}" = "6" ] ; 
    then echo "<h3> $PACKAGENAME </h3> \n <small><ul><li>Xsltproc returned ${RETVALUE} - tag file corrupted?</li> </ul></small>" >>allpackages.html ; 
  fi
  echo "Returned : \t${RETVALUE} after processing ${PACKAGENAME}" 

done
echo '<hr size="1"><small>Doxygen tag merging done by Edward Moyse and Shaun Roe. This file was generated on ' >> allpackages.html                                               
date >> allpackages.html
echo '</small>' >> allpackages.html                                               
cat $TEMPATH/EndFragment >> allpackages.html

if [ ! -f "allpackages.html" ]; then
	echo "Problem: File allpackages.html not created"
	exit 1
else
	echo "Done "`pwd`"/allpackages.html"
	exit 0
fi
