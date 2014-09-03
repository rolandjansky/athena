#set -x
#
#single script to run RecExCommon in an empty directory
#
#    MYRELEASE need to be set before hand if the script 
#is not taken from a release (official or nightly)
#    export MYRELEASE=/afs/cern.ch/atlas/software/dist/6.2.0
#
#    MYTAGS need to be set to configure cmt if something 
#  different from the default ( opt ) is needed
#     export MYTAGS=dbg
#
# no setup of CMT should have been done
#
# possible argument (one value allowed)
#  copy if the content should be copied rather than checked out
#         this allows testing of private modifications. However, cvs 
#         can then not be used on this directory
#  head if the content should be checked out from the head rather 
#         than from the tag taken from the directory tree
#  RecExCommon-xx-yy-zz if a specific tag should be used, different 
#               from the one in the directory sturcture
#  ""     default is to check out from the tag in directory structure
#
#examples:
#  source /afs/cern.ch/atlas/software/dist/3.0.1/Reconstruction/RecExample/RecExCommon/*/share/RecExCommon_doall.sh
#
# or 
# export MYRELEASE=/afs/cern.ch/atlas/software/dist/3.0.1
# source ~user/mycbnt/Reconstruction/RexExample/RecExCommon/*/share/RecExCommon_doall.sh copy
#
#directory where the script is run
export WORKAREA=`pwd`
#cmt version 

#if test "${CMTVERSION}" = "" ; then
#CMT version, might be overriden by the one defined in the release


export CMTVERSION=v1r20p20090520



#fi

#see if there is anything in the directory

ANYTHING=`ls`
if test ! "${ANYTHING}" = "" ; then
  echo "## working directory not empty"
  echo "## RecExCommon_doall.sh need be run in a clean directory. Stop"
  return
fi



#get where this script is taken (this work only if the script 
# is taken from  a RecExCommon package that has been cmt checked out
# wether in any release (even nightly) or in a private area
# and only an absolute path to the script is used

script=$0
if test "`dirname ${script}`" = ".."; then
  echo "##Please use an absolute path to reach this script"
  return
fi


FROMDIR=`dirname $script`

FROMDIR=`dirname ${FROMDIR}`

VERSIONFILE=${FROMDIR}/cmt/version.cmt

if [ -f $VERSIONFILE ]; then
    MYTAG=`cat ${FROMDIR}/cmt/version.cmt`
    PROJECTBUILD=1
    echo "## Project build recognised"
else
    MYTAG=${FROMDIR}
    FROMDIR=`dirname ${FROMDIR}`
    PROJECTBUILD=0
    echo "## Monolithic build recognised. ERROR nnot supported anymore !"
    return
fi
echo "MYTAG=" $MYTAG


FROMDIR=`dirname ${FROMDIR}`
FROMDIR=`dirname ${FROMDIR}`
FROMDIR=`dirname ${FROMDIR}`

TOPREL=`basename ${FROMDIR}`

MYTAG=`basename ${MYTAG}`

echo FROMDIR=$FROMDIR
echo MYTAG=$MYTAG
echo TOPREL=$TOPREL

TODO="$1"
echo argument = $1
if [ $# -eq 0  ] ; then
    echo no arguments
    echo I check out  RecExCommon with  tag  $MYTAG 
    TODO="nothing"
elif [ $# -eq 1  ] ; then
 echo one argument seen
 if [ $TODO = "copy" ] ; then
    echo I copy RecExCommon from $FROMDIR with tag $MYTAG 
 elif [ $TODO = "head" ] ; then
    MYTAG="HEAD"
    echo I check out  RecExCommon from the HEAD 
 else
    MYTAG=$TODO
    echo I check out  RecExCommon with  tag  $MYTAG with 
 fi
else
  echo Invalid number of arguments. Should be 0 or 1.
  echo stop
  return
fi

if test ! "${MYRELEASE}" = ""; then
    echo I use additional release $MYRELEASE
    RELEASE=$MYRELEASE

else

    echo I use same release as from $FROMDIR
    RELEASE=$TOPREL

fi

echo RELEASE=$RELEASE


echo "##using tags $MYTAGS"  

#the release to be used
#CMTPATH: packages are taken first in the new area,
#then in the area where the script reside
#then (possibly) in a release specified as follows
#export MYRELEASE=/afs/cern.ch/atlas/software/dist/2.4.0
#It is not necessary to specify MYRELEASE is the script is taken
#directly from a release (even a nightly one)

mkdir -p cmtdir
cd cmtdir

rm -f requirements

echo WORKAREA=$WORKAREA

echo "## set up CMT with user home requirements file in cvs"


    echo "## ...  project build"
  # take into account that AtlasAnalysis build 1.3.0 correspond to release 11.3.0 
  # while AtlasAnalysis rel_1 correspond to release rel_1
  NPAT=`echo $RELEASE | cut -d. -f3`
  if [ $RELEASE = $NPAT ] ; then
      RELEASETAG=$RELEASE
      echo "## nightly project build use release tag:"$RELEASETAG
  else
      NMAJ=`echo $RELEASE | cut -d. -f1`
      NMIN=`echo $RELEASE | cut -d. -f2`
      #only project AtlasOffline NEWMAJ=$(($NMAJ+10))
      NEWMAJ=$NMAJ
      NCACHE=`echo $RELEASE | cut -d. -f4`
      if [ ! -z $NCACHE ] ; then
         RELEASETAG=$NEWMAJ.$NMIN.$NPAT.$NCACHE
      else
	  RELEASETAG=$NEWMAJ.$NMIN.$NPAT
      fi	  
      echo "## project release use release tag:"$RELEASETAG
  fi



echo "## ...  building requirements "
    cat <<EOF >|requirements
set   CMTSITE  CERN
set   SITEROOT /afs/cern.ch
set ATLAS_DIST_AREA /afs/cern.ch/atlas/software/dist                               
# do not append release number to test area
apply_tag oneTest
macro ATLAS_TEST_AREA "${WORKAREA}"
# optimised by default
# apply_tag  dbg
apply_tag  setup
apply_tag  setupCMT
apply_tag 32
apply_tag ${RELEASETAG} 
EOF


#idem if test "${MYTAGS}X" != "X" ; then
#idem if [ "${MYTAGS}X" != "X" ] ; then
if [ ${(c)#MYTAGS} -ne 0 ] ; then
    echo "apply_tag ${MYTAGS} " >> requirements
fi




    if [ ! -z `echo $FROMDIR | awk /"14.1.0.Y"/` ]; then
	echo "apply_tag 14.1.0.Y " >> requirements
    fi



    if [ ! -z `echo $FROMDIR | awk /"14.2.0.Y"/` ]; then
	echo "apply_tag 14.2.0.Y " >> requirements
    fi


    if [ ! -z `echo $FROMDIR | awk /"14.2.10.Y"/` ]; then
	echo "apply_tag 14.2.10.Y " >> requirements
    fi

    if [ ! -z `echo $FROMDIR | awk /"14.4.X.Y-VAL"/` ]; then
	echo "apply_tag 14.4.X.Y-VAL " >> requirements
    elif [ ! -z `echo $FROMDIR | awk /"14.4.X.Y"/` ]; then
	echo "apply_tag 14.4.X.Y " >> requirements
    fi

    if [ ! -z `echo $FROMDIR | awk /"14.5.X.Y-VAL"/` ]; then
	echo "apply_tag 14.5.X.Y-VAL " >> requirements
    elif [ ! -z `echo $FROMDIR | awk /"14.5.X.Y"/` ]; then
	echo "apply_tag 14.5.X.Y " >> requirements
    fi

    if [ ! -z `echo $FROMDIR | awk /"14.5.X-VAL"/` ]; then
	echo "apply_tag 14.5.X-VAL " >> requirements
    elif [ ! -z `echo $FROMDIR | awk /"14.5.X"/` ]; then
	echo "apply_tag 14.5.X " >> requirements
    fi

    if [ ! -z `echo $FROMDIR | awk /"15.0.0.Y-VAL"/` ]; then
	echo "apply_tag 15.0.0.Y-VAL " >> requirements
    elif [ ! -z `echo $FROMDIR | awk /"15.0.0.Y"/` ]; then
	echo "apply_tag 15.0.0.Y " >> requirements
    fi

    if [ ! -z `echo $FROMDIR | awk /"15.1.X.Y-VAL"/` ]; then
	echo "apply_tag 15.1.X.Y-VAL " >> requirements
    elif [ ! -z `echo $FROMDIR | awk /"15.1.X.Y"/` ]; then
	echo "apply_tag 15.1.X.Y " >> requirements
    fi

    if [ ! -z `echo $FROMDIR | awk /"15.2.X.Y-VAL"/` ]; then
	echo "apply_tag 15.2.X.Y-VAL " >> requirements
    elif [ ! -z `echo $FROMDIR | awk /"15.2.X.Y"/` ]; then
	echo "apply_tag 15.2.X.Y " >> requirements
    fi

    if [ ! -z `echo $FROMDIR | awk /"15.3.X.Y-VAL"/` ]; then
	echo "apply_tag 15.3.X.Y-VAL " >> requirements
    elif [ ! -z `echo $FROMDIR | awk /"15.3.X.Y"/` ]; then
	echo "apply_tag 15.3.X.Y " >> requirements
    fi

    if [ ! -z `echo $FROMDIR | awk /"15.4.X.Y-VAL"/` ]; then
	echo "apply_tag 15.4.X.Y-VAL " >> requirements
    elif [ ! -z `echo $FROMDIR | awk /"15.4.X.Y"/` ]; then
	echo "apply_tag 15.4.X.Y " >> requirements
    fi

    if [ ! -z `echo $FROMDIR | awk /"15.5.X.Y-VAL"/` ]; then
	echo "apply_tag 15.5.X.Y-VAL " >> requirements
    elif [ ! -z `echo $FROMDIR | awk /"15.5.X.Y"/` ]; then
	echo "apply_tag 15.5.X.Y " >> requirements
    elif [ ! -z `echo $FROMDIR | awk /"15.5.X-VAL"/` ]; then
	echo "apply_tag 15.5.X-VAL " >> requirements
    elif [ ! -z `echo $FROMDIR | awk /"15.5.X"/` ]; then
	echo "apply_tag 15.5.X " >> requirements
    fi

    if [ ! -z `echo $FROMDIR | awk /"15.6.X.Y-VAL"/` ]; then
	echo "apply_tag 15.6.X.Y-VAL " >> requirements
    elif [ ! -z `echo $FROMDIR | awk /"15.6.X.Y"/` ]; then
	echo "apply_tag 15.6.X.Y " >> requirements
    elif [ ! -z `echo $FROMDIR | awk /"15.6.X-VAL"/` ]; then
	echo "apply_tag 15.6.X-VAL " >> requirements
    elif [ ! -z `echo $FROMDIR | awk /"15.6.X"/` ]; then
	echo "apply_tag 15.6.X " >> requirements
    fi


    if [ ! -z `echo $FROMDIR | awk /"16.0.X.Y-VAL"/` ]; then
	echo "apply_tag 15.3.X.Y-VAL " >> requirements
    elif [ ! -z `echo $FROMDIR | awk /"16.0.X.Y"/` ]; then
	echo "apply_tag 15.3.X.Y " >> requirements
    fi



    if [ ! -z `echo $FROMDIR | awk /"14.2.2X.Y-VAL"/` ]; then
	echo "apply_tag 14.2.2X.Y-VAL " >> requirements
    elif [ ! -z `echo $FROMDIR | awk /"14.2.2X.Y"/` ]; then
	echo "apply_tag 14.2.2X.Y " >> requirements
    elif [ ! -z `echo $FROMDIR | awk /"14.2.2X-VAL"/` ]; then
	echo "apply_tag 14.2.2X-VAL " >> requirements
    elif [ ! -z `echo $FROMDIR | awk /"14.2.2X"/` ]; then
	echo "apply_tag 14.2.2X " >> requirements
    fi


    if [ ! -z `echo $FROMDIR | awk /AtlasPoint1/` ]; then
	echo "##bugfix nightly release"
	echo "apply_tag AtlasPoint1 " >> requirements
    fi

    if [ ! -z `echo $FROMDIR | awk /AtlasTier0/` ]; then
	echo "apply_tag AtlasTier0 " >> requirements
    fi

    if [ ! -z `echo $FROMDIR | awk /AtlasProduction/` ]; then
	echo "##bugfix nightly release"
	echo "apply_tag AtlasProduction " >> requirements
    fi


    if [ ! -z `echo $FROMDIR | awk /bugfix/` ]; then
	echo "##bugfix nightly release"
	echo "apply_tag bugfix " >> requirements
    fi

    if [ ! -z `echo $FROMDIR | awk /devval/` ]; then
	echo "##bugfix nightly release"
	echo "apply_tag devval " >> requirements
    fi

    if [ ! -z `echo $FROMDIR | awk /"\/val"/` ]; then
	echo "##bugfix nightly release"
	echo "apply_tag val " >> requirements
    fi

#    if [ ! -z `echo $FROMDIR | awk /"VAL"/` ]; then
#	echo "##bugfix nightly release"
#	echo "apply_tag val " >> requirements
#    fi

    if [ ! -z `echo $FROMDIR | awk /pcache/` ]; then
	echo "##bugfix nightly release"
	echo "apply_tag pcache " >> requirements
	echo "apply_tag AtlasProduction " >> requirements
	echo "apply_tag gcc323 " >> requirements
    fi

    if [ ! -z `echo $FROMDIR | awk /devmig0/` ]; then
	echo "##bugfix nightly release"
	echo "apply_tag devmig0 " >> requirements
	echo "apply_tag dbg " >> requirements
    fi

    if [ ! -z `echo $FROMDIR | awk /devmig1/` ]; then
	echo "##bugfix nightly release"
	echo "apply_tag devmig1 " >> requirements
	echo "apply_tag dbg " >> requirements
    fi

    if [ ! -z `echo $FROMDIR | awk /devmig2/` ]; then
	echo "##bugfix nightly release"
	echo "apply_tag devmig2 " >> requirements
	echo "apply_tag dbg " >> requirements
    fi

    if [ ! -z `echo $FROMDIR | awk /devmig3/` ]; then
	echo "##bugfix nightly release"
	echo "apply_tag devmig3 " >> requirements
	echo "apply_tag dbg " >> requirements
    fi

    if [ ! -z `echo $FROMDIR | awk /devmig4/` ]; then
	echo "##bugfix nightly release"
	echo "apply_tag devmig4 " >> requirements
	echo "apply_tag dbg " >> requirements
    fi

    if [ ! -z `echo $FROMDIR | awk /lcg/` ]; then
	echo "##bugfix nightly release"
	echo "apply_tag lcg " >> requirements
    fi


echo "use AtlasLogin AtlasLogin-* /afs/cern.ch/atlas/software/dist" >> requirements




echo "## requirements built"
cat requirements

unset CMTPATH
unset CMTCONFIG

CMTROOT=/afs/cern.ch/sw/contrib/CMT/$CMTVERSION; export CMTROOT
. ${CMTROOT}/mgr/setup.sh ""
which cmt
cmt config
source setup.sh 

cd ../

echo CMTROOT=$CMTROOT
echo CMTCONFIG=$CMTCONFIG
echo CMTPATH=$CMTPATH

NEWDIR=Reconstruction/RecExample/RecExCommon

if test "${TODO}" = "copy" ; then
#create by hand the correct structure

  mkdir -p $NEWDIR
  cd $NEWDIR
  mkdir share
  mkdir cmt
  mkdir Testing
#copy the files accompanying the script
  cp -pf  ${FROMDIR}/${NEWDIR}/* .
  cp -pf  ${FROMDIR}/${NEWDIR}/share/* share/
  cp -pf  ${FROMDIR}/${NEWDIR}/Testing/* Testing/
  cp -pf  ${FROMDIR}/${NEWDIR}/cmt/* cmt/
#clean
  cd cmt
  cmt config
  gmake clean
else
#checkout
 cmt co -r $MYTAG Reconstruction/RecExample/RecExCommon   
 cd $NEWDIR
 cd cmt
fi

echo "##Build"

echo "##Setup environment variables" 
# (to be done once per window )
# empty argument necessary in scripts
# should not be necessary anymore when using apply_tag setup
#source setup.sh ""
gmake

echo "##now run" 
gmake check

echo "##done \a" 
