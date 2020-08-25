include.block ("L1TopoSimulation/L1Simulation_topOptions.py")

import traceback

from AthenaCommon.Logging import logging
logRecExCommon_topOptions = logging.getLogger( 'RecExCommon_topOptions' )

## try to load a catalogue of AthFiles from a cache (to speed-up tests)
try:
    import PyUtils.AthFile as af
    af.server.load_cache('recexcommon-afserver-cache.ascii.gz')
except Exception:
    logRecExCommon_topOptions.warning("Could not load or disable athfile cache")


from AthenaCommon.AlgSequence import AlgSequence
from AthenaCommon.AppMgr import ToolSvc,theApp,ServiceMgr
from AthenaCommon.AthenaCommonFlags  import athenaCommonFlags
from AthenaCommon.GlobalFlags  import globalflags
from RecExConfig.RecFlags import rec
from AthenaCommon.Resilience import treatException,protectedInclude

topSequence = AlgSequence()

if rec.Production():
    AthenaCommon.Configurable.log.setLevel( WARNING ) 

include ( "RecExCond/RecExCommon_flags.py" )
   
logRecExCommon_topOptions.info("Abort on unchecked status code enabled !")


###################
# Common Services #
###################

# ConditionsTag
from IOVDbSvc.CondDB import conddb
if len(globalflags.ConditionsTag())!=0:
    conddb.setGlobalTag(globalflags.ConditionsTag())


# Detector geometry and magnetic field 
if rec.LoadGeometry():
    include("RecExCond/AllDet_detDescr.py")
    
# Particle Property
protectedInclude( "PartPropSvc/PartPropSvc.py" )
include.block( "PartPropSvc/PartPropSvc.py" )

#Output file TagInfo and metadata
if rec.oldFlagCompatibility:
    print "RecExCommon_flags.py flags values:"
    try:
        for o in RecExCommonFlags.keys():
            exec 'print "%s =",%s ' % (o,o)
    except Exception:
        print "WARNING RecExCommonFlags not available, cannot delete"
else:
    print "Old flags have been deleted"

# end flag settings section
##########################################################################
# set up job


if globalflags.InputFormat()=='pool':
    # to read Pool data
    import AthenaPoolCnvSvc.ReadAthenaPool

    # if file not in catalog put it there
    svcMgr.PoolSvc.AttemptCatalogPatch=True 

    svcMgr.EventSelector.InputCollections = athenaCommonFlags.FilesInput()

    # backward compatibility (needed for RTT overwriting InputCollections)
    EventSelector=ServiceMgr.EventSelector


########################################################################

# Number of events to be processed 
theApp.EvtMax = athenaCommonFlags.EvtMax()


# should use AthenaCommon.KeyStore                          
from RecExConfig.ObjKeyStore import objKeyStore, CfgKeyStore

# keep a separate copy of what was read from recexpers (for later consistency check)
cfgRecExPers=CfgKeyStore("recexpers")


# FIXME should find a better way to keep it uptodate
# read persistent OKS
# if input is pool, read list of input directly from file
if globalflags.InputFormat.is_pool():
    logRecExCommon_topOptions.info("Pool file : storing in objKeyStore the list of input object directly from file")
    try:
        from PyUtils.MetaReaderPeeker import convert_itemList
        objKeyStore.addManyTypesInputFile(convert_itemList(layout='#join'))
    except:
        logRecExCommon_topOptions.error("no input file defined in flags. If using RTT please use tag <athenaCommonFlags/>. Now continuing at own riske")

if rec.OutputLevel() <= DEBUG:
    print "Initial content of objKeyStore "
    print objKeyStore

if rec.doDumpPoolInputContent() and globalflags.InputFormat()=='pool':
    try:
        include("AthenaPoolTools/EventCount_jobOptions.py")
        topSequence.EventCount.Dump=True
        topSequence.EventCount.OutputLevel=DEBUG   
    except:
        treatException("EventCount_jobOptions.py" )

# functionality : read truth
if rec.doTruth():
    # this algorithm dump the content of the MC event: big output
    if rec.doDumpMC():
        # generator truth
        from TruthExamples.TruthExamplesConf import DumpMC
        topSequence+=DumpMC()

# If no xAOD::EventInfo is found in a POOL file, schedule conversion from old EventInfo
if globalflags.InputFormat.is_pool():
    from RecExConfig.ObjKeyStore import objKeyStore
    from PyUtils.MetaReaderPeeker import convert_itemList
    objKeyStore.addManyTypesInputFile(convert_itemList(layout='#join'))
    from AthenaCommon.AlgSequence import AthSequencer
    condSeq = AthSequencer("AthCondSeq")
    if ( not objKeyStore.isInInput("xAOD::EventInfo") ) and ( not hasattr(condSeq, "xAODMaker::EventInfoCnvAlg") ):
        from xAODEventInfoCnv.xAODEventInfoCnvAlgDefault import xAODEventInfoCnvAlgDefault
        xAODEventInfoCnvAlgDefault(sequence=condSeq)

if rec.doTrigger:
    try:
        from TriggerJobOpts.TriggerGetter import TriggerGetter
        triggerGetter = TriggerGetter()
    except Exception:
        treatException("Could not import TriggerJobOpts.TriggerGetter . Switched off !" )
        from RecExConfig.RecAlgsFlags import recAlgs
        recAlgs.doTrigger=False

