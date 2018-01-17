#!/bin/sh
#/** @file post.sh
# @brief sh script that check the return code of an executable and compares
# its output with a reference (if available).
# @param test_name
#
# @author Scott Snyder <snyder@fnal.gov> - ATLAS Collaboration.
# @author Paolo Calafiura <pcalafiura@lbl.gov> - ATLAS Collaboration.
# $Id: post.sh,v 1.23 2007-11-10 00:00:07 calaf Exp $
# **/
test=$1
extrapatterns="$2"
#verbose="1"
if [ "$POST_SH_NOCOLOR" = "" ]; then
 GREEN="[92;1m"
 YELLOW="[93;1m"
 RED="[97;101;1m"
 RESET="[m"
else
 GREEN=""
 YELLOW=""
 RED=""
 RESET=""
fi

if [ "$ATLAS_CTEST_PACKAGE" = "" ]; then
  ATLAS_CTEST_PACKAGE="__NOPACKAGE__"
fi

# consider these name pairs identical in the diff
read -d '' II <<EOF
s/^StoreGateSvc_Impl VERBOSE/StoreGateSvc      VERBOSE/
s/^StoreGateSvc_Impl   DEBUG/StoreGateSvc        DEBUG/
s/StoreGateSvc_Impl/StoreGateSvc/
s/SGImplSvc/StoreGateSvc/
s/SG::DataProxyHolder::sgkey_t/sgkey_t/
s!(ERROR|INFO|WARNING|FATAL) [^ ]*/!\\\\1 ../!
s/(\.cxx|\.cpp|\.h|\.icc|LINE):[0-9]+/\\\\1/
s/.[[][?]1034h//
EOF

# ignore diff annotations
PP='^---|^[[:digit:]]+[acd,][[:digit:]]+'

# ignore hex addresses
PP="$PP"'|0x\w{4,}'

# ignore package names e.g. Package-00-00-00
PP="$PP"'|\w+-[[:digit:]]{2}-[[:digit:]]{2}-[[:digit:]]{2}'
# ignore trunk package names e.g. Package-r123456
PP="$PP"'|\w+-r[[:digit:]]+'
# ignore cpu usage printouts
PP="$PP"'|ChronoStatSvc +INFO Time'
PP="$PP"'|Time left.+ Seconds'
PP="$PP"'|Timeleft.+ sec'
PP="$PP"'|INFO Time User'
# ignore clid db file name
PP="$PP"'|from CLIDDB file'
# ignore slug machine printout
PP="$PP"'| Machine: .* System and Processor Info'
PP="$PP"'| Jobname = .* Machine ='
# ignore slug pid printout
PP="$PP"'|Atlas Detector Simulation, Reconstruction and Analysis Running on'
PP="$PP"'|Program:  Slug-Dice-Arecon .+ pid +[[:digit:]]+'
#ignore DllClassManager DEBUG messages
PP="$PP"'|DllClassManager     DEBUG'
# ignore slug Library printout
PP="$PP"'|Library of +[[:digit:]]+ at +[[:digit:]]+'
PP="$PP"'|Library compiled on +[[:digit:]]'
# ignore ClassIDSvc "in memory db" printouts
PP="$PP"'|CLID: .* - type name:'
# ignore ClassIDSvc "already set" printouts
PP="$PP"'|ClassIDSvc .* setTypeNameForID: .* already set for'
# ignore ClassIDSvc finalize output
PP="$PP"'|ClassIDSvc .* finalize: wrote .*'
# ignore rcs version comments
PP="$PP"'|Id: .+ Exp \$'
# ignore plugin count
PP="$PP"'|PluginMgr +INFO loaded plugin info for'
# ignore HistorySvc registered count
PP="$PP"'|HistorySvc +INFO Registered'
# ignore clid registry entries count
PP="$PP"'|ClassIDSvc +INFO  getRegistryEntries: read'
# ignore existsDir path WARNINGS
PP="$PP"'|DirSearchPath::existsDir: WARNING not a directory'
# ignore warnings about duplicate services/converters.
PP="$PP"'|Service factory for type [^ ]+ already declared'
PP="$PP"'|Converter for class:[^ ]+ already exists'
# Number of configurables read can vary from build to build.
PP="$PP"'|INFO Read module info for'
# ignore ApplicationMgr header.
PP="$PP"'|^ApplicationMgr *SUCCESS *$'
PP="$PP"'|^=+$'
PP="$PP"'|^ *Welcome to ApplicationMgr'
PP="$PP"'|^ *running on .* on '
PP="$PP"'|//GP: '
#ignore which malloc we are using
PP="$PP"'|^Preloading tcmalloc'
PP="$PP"'|^WARNING: TCMALLOCDIR not defined'
#Sebastien says not to worry about this...
PP="$PP"'|^Py:AthFile .*shutting down athfile-server'
PP="$PP"'|^HistogramPersis...   INFO *.CnvServices.:'
PP="$PP"'|StatusCodeSvc        INFO initialize'
PP="$PP"'|^ApplicationMgr +INFO Successfully loaded'
PP="$PP"'|^IncidentSvc +DEBUG Service base class'
PP="$PP"'|^ClassIDSvc +WARNING Could not resolve clid DB path notthere.db'
PP="$PP"'|^IncidentSvc         DEBUG Adding .* listener '.*' with priority .*'
PP="$PP"'|MessageSvc not found, will use std::cerr'
PP="$PP"'|^AtRndmGenSvc         INFO Initializing AtRndmGenSvc'
PP="$PP"'|^AtRanluxGenSvc2      INFO Initializing AtRanluxGenSvc2'
PP="$PP"'|^AtRanluxGenSvc       INFO Initializing AtRanluxGenSvc'
#ignore personal .athenarc files
PP="$PP"'|including file \"\$HOME/.athenarc'
#ignore known gaudi python warning
PP="$PP"'|Bindings.py:660: DeprecationWarning'
#ignore the ignored
PP="$PP"'|Warning in <TEnvRec::ChangeValue>: duplicate entry <Root.ErrorIgnoreLevel=Print> for level 1; ignored'
PP="$PP"'|^JobOptionsSvc +INFO'
# Gaudi 31
PP="$PP"'|PluginService::SetDebug|setting LC_ALL'
# Ignore root6 duplicate dictionary warnings
PP="$PP"'|^Warning in .* (header|class) .* is already in'
# Ignore GaudiHive timeline printouts
PP="$PP"'|^TimelineSvc +INFO'
# StoreGate v3 migration
PP="$PP"'|VERBOSE ServiceLocatorHelper::service: found service IncidentSvc'
PP="$PP"'|VERBOSE ServiceLocatorHelper::service: found service ProxyProviderSvc'
# Pathnames / versions / times / hosts
PP="$PP"'|^IOVDbSvc +INFO (Folder|Connection|Total payload)'
PP="$PP"'|^DBReplicaSvc +INFO Read replica configuration'
PP="$PP"'|^EventInfoMgtInit: Got release version'
PP="$PP"'|^Py:Athena +INFO using release'
PP="$PP"'|servers found for host'
PP="$PP"'|^(Sun|Mon|Tue|Wed|Thu|Fri|Sat) '

# Outputs dependent on whether or not a file catalog already exists.
PP="$PP"'|XMLFileCatalog|File is not in Catalog|Failed to open container to check POOL collection|Open     DbSession|Access   DbDomain|Access   DbDatabase|^RootDatabase.open|Deaccess DbDatabase'

PP="$PP"'|^Py:ConfigurableDb'
PP="$PP"'|^DBReplicaSvc +INFO'
PP="$PP"'|INFO ... COOL  exception caught: The database does not exist|Create a new conditions database'
PP="$PP"'|^SetGeometryVersion.py obtained'
PP="$PP"'|^ConditionStore +INFO Start ConditionStore'
PP="$PP"'|^ConditionStore +INFO Stop ConditionStore'

# Differences between Gaudi versions.
PP="$PP"'|DEBUG input handles:|DEBUG output handles:|DEBUG Data Deps for|DEBUG Property update for OutputLevel :|-ExtraInputs |-ExtraOutputs |-Cardinality |-IsClonable |-NeededResources |-Timeline |Service base class initialized successfully'

# StoreGate INFO messages changed to VERBOSE
PP="$PP"'|^(StoreGateSvc|[^ ]+Store) +(INFO|VERBOSE) (Stop|stop|Start)'

# Transient frontier warnings.
PP="$PP"'|^warn  .fn-'

# ubsan
PP="$PP"'|bits/regex.h:1545'


if [ "$extrapatterns" != "" ]; then
 PP="$PP""|$extrapatterns"
fi

if [ -z "$testStatus" ]
   then
   echo "$YELLOW post.sh> Warning: athena exit status is not available $RESET"
else
   # check exit status
   joblog=${test}.log
   if [ "$testStatus" = 0 ]
       then
       if [ "$verbose" != "" ]; then
         echo "$GREEN post.sh> OK: ${test} exited normally. Output is in $joblog $RESET"
       fi
       reflog=../share/${test}.ref

       # If we can't find the reference file, maybe it's located outside
       # the repo.  With the switch to git, we have to fall back
       # to handling the versioning manually.
       # ATLAS_REFERENCE_TAG should be a string of the form PACKAGE/VERSION.
       # We first look for it in DATAPATH.  If we don't find it,
       # we then look under ATLAS_REFERENCE_DATA, which falls back
       # to an afs path if it's not found.
       if [ \( ! -r $reflog \) -a "$ATLAS_REFERENCE_TAG" != "" ]; then
           # Look for the file in DATAPATH.
           # We have to look for the directory, not the file itself,
           # since get_files is hardcoded not to look more than two
           # levels down.
           get_files -data -symlink $ATLAS_REFERENCE_TAG > /dev/null
           reflog=`basename $ATLAS_REFERENCE_TAG`/${test}.ref
           if [ ! -r $reflog ]; then
               testdata=$ATLAS_REFERENCE_DATA
               if [ "$testdata" = "" ]; then
                   testdata=/afs/cern.ch/atlas/maxidisk/d33/referencefiles
               fi
               reflog=$testdata/$ATLAS_REFERENCE_TAG/${test}.ref
           fi
       fi

       if [ -r $reflog ]
           then
	   jobrep=${joblog}-rep
	   sed -r "$II" $joblog > $jobrep
	   refrep=`basename ${reflog}`-rep
	   sed -r "$II" $reflog > $refrep
           jobdiff=${joblog}-todiff
           refdiff=`basename ${reflog}`-todiff
           egrep -a -v "$PP" < $jobrep > $jobdiff
           egrep -a -v "$PP" < $refrep > $refdiff
           diff -a -b -E -B -u $jobdiff $refdiff
           diffStatus=$?
           if [ $diffStatus != 0 ] ; then
               echo "$RED post.sh> ERROR: $joblog and $reflog differ $RESET"
               # Return with failure in this case:
               exit 1
           else
               if [ "$verbose" != "" ]; then
                 echo "$GREEN post.sh> OK: $joblog and $reflog identical $RESET"
               fi
           fi
       else
           tail $joblog
           echo "$YELLOW post.sh> WARNING: reference output $reflog not available $RESET"
           echo  " post.sh> Please check ${PWD}/$joblog"
       fi
   else
       tail $joblog
       echo  "$RED post.sh> ERROR: Athena exited abnormally! Exit code: $testStatus $RESET"
       echo  " post.sh> Please check ${PWD}/$joblog"
   fi
fi
exit $testStatus
