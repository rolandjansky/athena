#!/bin/sh
#/** @file post.sh
# @brief sh script that check the return code of an executable and compares 
# its output with a reference (if available).
# @param test_name 
#
# @author Paolo Calafiura <pcalafiura@lbl.gov> - ATLAS Collaboration.
# $Id: post_check.sh,v 1.32 2009-05-06 18:10:12 ssnyder Exp $
# **/

PP="adummythatshouldntmatch"

# Timestamps.
PP="$PP"'|^... ... [ 0-9][0-9] [0-9]{2}:[0-9]{2}:[0-9]{2} ....? 2[0-9]{3}$'

# Package versions.
PP="$PP"'|\w+-[[:digit:]]{2}-[[:digit:]]{2}-[[:digit:]]{2}'

# This gets written if the module cache gets rebuilt.
PP="$PP"'|^==> New TileCablingService created$'

# CLIDsvc output depends on module cache, existing files?
PP="$PP"'|^ClassIDSvc *INFO'

# File catalog stuff.
PP="$PP"'|^Domain.ROOT_All.:'
PP="$PP"'|XMLFileCatalog'

PP="$PP"'|^ToolSvc'
PP="$PP"'|^HistorySvc *INFO Registered'
PP="$PP"'|Root file version'
PP="$PP"'|^Loaded dictionary'
PP="$PP"'|^IOVSvc *INFO'
PP="$PP"'|Failed to load dictionary for native class'
PP="$PP"'|^AthenaSealSvc *INFO'
PP="$PP"'|^Note: .file .* already loaded'
PP="$PP"'|^EventSelector *WARNING Service already offline'
PP="$PP"'|^cleaning up'
PP="$PP"'|^StatusCodeSvc *INFO'
PP="$PP"'|^-E- IOVObjectManager *: Could not get reference to DataSource'
PP="$PP"'|SealSTLRflx'
PP="$PP"'|^Num . Function'
PP="$PP"'|^----'
PP="$PP"'|^ *$'
PP="$PP"'|local *. libGaudiKernelDict.so'
PP="$PP"'|^StorageSvc: level.Info. '
PP="$PP"'|ROOT::Reflex::NewDelFunctionsT<StatusCode>::delete_T.void.'
PP="$PP"'|Message: The value .* for attribute .ID. must be Name or Nmtoken'
PP="$PP"'|^XMLDOMErrorMessanger: level.Error. Error at file ".*PoolFileCatalog.xml", line .*, column .*'
PP="$PP"'|^DbSession: level.Info.     Open     DbSession'
PP="$PP"'|^AthenaRootStrea.*INFO.*POOL/ROOT class loader initialized SUCCESSFULLY!'
PP="$PP"'|Warning in <TClass::TClass>: no dictionary for class InterfaceID is available'
PP="$PP"'|^..STLRflx..$'
PP="$PP"'|^..STLRflx., .DetDescrDict..$'
PP="$PP"'|Read module info for'
PP="$PP"'|loaded plugin info for'
PP="$PP"'|Service factory for type .* already declared'
PP="$PP"'|Deaccess DbDatabase'
PP="$PP"'|Warning in <TEnvRec::ChangeValue>: duplicate entry <Library.*> for level 0; ignored'
PP="$PP"'|TEnvRec::ChangeValue:0: RuntimeWarning: duplicate entry <Library.*> for level 0; ignored'
PP="$PP"'|ApplicationMgr *INFO Successfully loaded modules :'
PP="$PP"'|DetDescrCnvSvc *INFO  filling address for'
PP="$PP"'|Py:AthenaSealSvc     INFO DictNames: '
PP="$PP"'|STLRflx.*STLAddRflx.*DetDescrDict'
PP="$PP"'|Py:Athena            INFO including file "AthenaSealSvc/AthenaSealSvcIgnore_joboptions.py"'
PP="$PP"'|Py:PropertyProxy  WARNING Found .* duplicates among the .* genConfDb files :'
PP="$PP"'|Py:PropertyProxy  WARNING --------------------------------------------------'
PP="$PP"'|Py:PropertyProxy  WARNING   -<component name>: <module> - . <duplicates> .'
PP="$PP"'|Py:PropertyProxy  WARNING   -.* - '
PP="$PP"'|^ApplicationMgr *SUCCESS *$'
PP="$PP"'|^=+$'
PP="$PP"'|^ *Welcome to ApplicationMgr'
PP="$PP"'|^ *running on .* on '
PP="$PP"'|^//GP: '
PP="$PP"'|^Py:Athena            INFO including file'
PP="$PP"'|^EventPersistenc.*INFO.*CnvServices.:.*DetDescrCnvSvc.*AthenaPoolCnvSvc'
PP="$PP"'|^DetDescrCnvSvc +INFO'
PP="$PP"'|^Warning in <TClass::TClass>: no dictionary for class'
PP="$PP"'|^.+ *INFO Initializing .+ - package version .*'
PP="$PP"'|^.+ *INFO Finalizing .+ - package version .*'
PP="$PP"'|^PoolSvc *INFO Database .* attribute .FILE_SIZE.:'
PP="$PP"'|^ChronoStatSvc        INFO  Number of skipped events for MemStat-1'
PP="$PP"'|^  StorageSvc    Error Cannot connect to proper technology domain.'
PP="$PP"'|^PoolSvc             ERROR Failed to set POOL property, TREE_MAX_SIZE to 1099511627776L'
PP="$PP"'|^AthenaPoolCnvSvc  WARNING setAttribute FAILED for TREE_MAX_SIZE'
PP="$PP"'|INFO Found HelperTools = PublicToolHandleArray'
PP="$PP"'|FILE_SIZE'
PP="$PP"'|No duplicates have been found: that.s good'
PP="$PP"'|Py:PropertyProxy  WARNING Fix your cmt/requirements file'
PP="$PP"'|SGAudSvc             INFO Initializing SGAudSvc'
PP="$PP"'|SGAudSvc             INFO Finalizing SGAudSvc'
PP="$PP"'|PoolSvc              INFO finalize.. in PoolSvc'
PP="$PP"'|MetaDataSvc       WARNING MetaDataSvc called without MetaDataContainer set.'
PP="$PP"'|MetaDataSvc *ERROR MetaDataSvc::loadAddresses'
PP="$PP"'|HistorySvc           INFO Service finalised successfully'
PP="$PP"'|Found HelperTools = PublicToolHandleArray'
PP="$PP"'|Py:Athena            INFO leaving with code 0: "successful run"'
PP="$PP"'|DBReplicaSvc +INFO'
PP="$PP"'|ApplicationMgr +INFO Updating ROOT::Reflex::PluginService'
PP="$PP"'|^Py:ConfigurableDb +(WARNING|ERROR)'
PP="$PP"'|^ClassIDSvc +ERROR processCLIDDB:'
PP="$PP"'|^PoolSvc +INFO Set connectionsvc retry'
PP="$PP"'|^PoolSvc +INFO createCollection'
PP="$PP"'|^removing: .*_atexit_'
PP="$PP"'|^ *DbSession *Info'
PP="$PP"'|^EventPersistenc.*INFO '
PP="$PP"'|^Domain.ROOT_All. +Info'
PP="$PP"'|^CoreDumpSvc +INFO Handling signals'
PP="$PP"'|^EventSelector +INFO EventSelection with query'
PP="$PP"'|Info Database being retired'
PP="$PP"'|ImplicitCollection +Info Opened the implicit collection'
PP="$PP"'|ImplicitCollection +Info and a name'
PP="$PP"'|^AthDictLoaderSvc +INFO'
PP="$PP"'|^ApplicationMgr +INFO Application Manager (Started|Stopped) successfully'
PP="$PP"'|^RootDBase.open Success'
PP="$PP"'|^InputMetaDataStore +INFO Start InputMetaDataStore'
PP="$PP"'|^TagMetaDataStore +INFO Start TagMetaDataStore'
PP="$PP"'|^MetaDataStore +INFO Start MetaDataStore'
PP="$PP"'|^StoreGateSvc +INFO Start StoreGateSvc'
PP="$PP"'|^DetectorStore +INFO Start DetectorStore'
PP="$PP"'|^EventSelector +INFO ----- EventSelectorAthenaPool Initialized Properly'
PP="$PP"'|^Py:AthenaDsoDb       INFO could not install alias'
PP="$PP"'|^EventSelector.* INFO in (initialize|finalize)'
PP="$PP"'|Time left before interrupting'
PP="$PP"'|^DecisionSvc +INFO'
PP="$PP"'|TCMALLOCDIR not defined|Preloading tcmalloc.so'
PP="$PP"'|Reordering baskets on file'
PP="$PP"'|Finished optimizing'
PP="$PP"'|INFO Finalizing'
PP="$PP"'|^hostname: Unknown host|^hostname: Host name lookup failure'
PP="$PP"'|^MetaDataSvc       WARNING Unable to load MetaData Proxies'
PP="$PP"'|^InputMetaDataStoreWARNING retrieve.const.: No valid proxy'
PP="$PP"'|^CoreDumpSvc +INFO'
PP="$PP"'|INFO +Object/count:'
PP="$PP"'|INFO +Finalizing'
PP="$PP"'|EventSelector +INFO finalize'
PP="$PP"'|Preloading tcmalloc'
PP="$PP"'|TCMALLOCDIR not defined'
PP="$PP"'|^Py:Athena +INFO'
PP="$PP"'|^PoolSvc +INFO'
PP="$PP"'|PoolFileCatalog'
PP="$PP"'|POOL_CATALOG'
PP="$PP"'|Stream1 +INFO Did final commit'
PP="$PP"'|Stream1 +INFO Records written'
PP="$PP"'|EventSelector +INFO stop'
PP="$PP"'|RooFit|NIKHEF|roofit'
PP="$PP"'|^hostname: Unknown server error'
PP="$PP"'|INFO  Service finalized'
PP="$PP"'|DeprecationWarning.* takes no parameters'
PP="$PP"'|^ *newobj = object.__new__'
PP="$PP"'|^MetaDataSvc +(ERROR|WARNING) Could not retrieve all versions for DataHeader'
PP="$PP"'|^CORAL/Services/ConnectionService Info Deleting the ConnectionPool'
PP="$PP"'|^EventStore +INFO|^CutFlowSvc +(INFO|WARNING)'
PP="$PP"'|^\*+$|drop-and-reload|we will keep the configuration around'
PP="$PP"'|^EventSelector +INFO  McEventSelector Initialized Properly'
PP="$PP"'|^HistogramPersis.* INFO'
PP="$PP"'|^IoComponentMgr +INFO'
PP="$PP"'|Histograms saving not required'
PP="$PP"'|RootDbase: all good'
PP="$PP"'|^StorageSvc Info EventInfo_p'
PP="$PP"'|^EventSelector +INFO reinit'
PP="$PP"'|^evt no'
PP="$PP"'|^ItemListSvc +INFO'
PP="$PP"'|^RootDatabase.open (Success|Always)|I/O reinitialization'
PP="$PP"'|^StorageSvc Info'
PP="$PP"'|^RootCollection Info'
PP="$PP"'|^RootCollectionSchemaEditor Warning'
PP="$PP"'|already in TClassTable'
PP="$PP"'|PluginService::SetDebug|setting LC_ALL'
PP="$PP"'|including file'
PP="$PP"'|Environment initialised for data access|building of dictionaries now off|Cannot find any ShowMembers function'
PP="$PP"'|^...1034h$'
PP="$PP"'|^GUID: Class|^WARNING: Cannot import TrigEDMConfig.TriggerEDM.getARATypesRenaming'
PP="$PP"'|^GUID: Class|^AthenaRootStr.* INFO|^Warning in .* found in .* is already in'
PP="$PP"'|no dictionary for class|INFO eformat version|INFO event storage'

# StoreGate INFO messages changed to VERBOSE
PP="$PP"'|^(StoreGateSvc|DetectorStore|MetaDataStore|InputMetaDataStore|TagMetaDataStore) +(INFO|VERBOSE) (Stop|stop|Start)'

test=$1
if [ -z "$testStatus" ]; then
    echo "post.sh> Warning: athena exit status is not available "
else 
    # check exit status
    joblog=${test}.log
    if [ "$testStatus" = 0 ]; then
	reflog=../share/${test}.ref

        # If we can't find the reference file, maybe it's located outside
        # the repo.  With the switch to git, we have to fall back
        # to stone knives and bearskins to manage versioning of these files.
        # ATLAS_REFERENCE_TAG should be a string of the form PACKAGE/VERSION.
        # We first look for it in DATAPATH.  If we don't find it,
        # we then look under ATLAS_REFERENCE_DATA, which falls back
        # to an afs path if it's not found.
        # Isn't this so much easier than svn?
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

	if [ -r $reflog ]; then
            jobdiff=${joblog}-todiff
            refdiff=`basename ${reflog}`-todiff
            sed 's/.[[][?]1034h//' < $joblog | egrep -v "$PP" > $jobdiff
            sed 's/.[[][?]1034h//' < $reflog | egrep -v "$PP" > $refdiff
            diff -a -u $jobdiff $refdiff
	    diffStatus=$?
	    if [ $diffStatus != 0 ] ; then
		echo "post.sh> ERROR: $joblog and $reflog differ"
                exit $diffStatus
	    fi
	else
	    tail $joblog
	    echo "post.sh> WARNING: reference output $reflog not available "
	    echo  " post.sh> Please check ${PWD}/$joblog"
            exit 1
	fi
    else
	tail $joblog
	echo  "post.sh> ERROR: Athena exited abnormally! Exit code: $testStatus "
	echo  " post.sh> Please check ${PWD}/$joblog"
    fi
fi

# Check output for ERROR/FATAL
joblog=${test}.log

exit $testStatus

