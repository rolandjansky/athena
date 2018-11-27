include.block ("L1TopoSimulation/L1Simulation_topOptions.py")

# gbl.AthenaServices.SetFatalHandler(438)
svcMgr.CoreDumpSvc.FatalHandler = 438
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
   
svcMgr.StatusCodeSvc.AbortOnError=True
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

# Detector Status
if rec.doDetStatus() and not athenaCommonFlags.isOnline():
    try:
        include("DetectorStatus/DetStatusSvc_CondDB.py")                  
    except Exception:
        treatException("Could not load DetStatusSvc_CondDb !")
        rec.doFileMetaData=False

    if rec.doFileMetaData():
        #DR FIXME not sure about commissioing exception, filemetadata should be in filteredESD as well
        if rec.doWriteRDO() or rec.doWriteESD() or rec.doWriteAOD() or rec.doDPD():
            protectedInclude("DetectorStatus/DetStatusSvc_ToFileMetaData.py")      



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

    from RecExConfig.RecConfFlags import recConfFlags
    ServiceMgr.EventSelector.BackNavigation = recConfFlags.AllowBackNavigation()

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
        from RecExConfig.InputFilePeeker import inputFileSummary
        objKeyStore.addManyTypesInputFile(inputFileSummary['eventdata_itemsList'])
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

if not objKeyStore.isInInput( "xAOD::EventInfo" ):
    if not hasattr( topSequence, "xAODMaker::EventInfoCnvAlg" ):
        from xAODEventInfoCnv.xAODEventInfoCreator import xAODMaker__EventInfoCnvAlg
        topSequence += xAODMaker__EventInfoCnvAlg()
        pass
else:
    if not hasattr( topSequence, "xAODMaker::EventInfoNonConstCnvAlg" ):
        topSequence += CfgMgr.xAODMaker__EventInfoNonConstCnvAlg()
        pass

if rec.doTrigger:
    try:
        from TriggerJobOpts.TriggerGetter import TriggerGetter
        triggerGetter = TriggerGetter()
    except Exception:
        treatException("Could not import TriggerJobOpts.TriggerGetter . Switched off !" )
        from RecExConfig.RecAlgsFlags import recAlgs
        recAlgs.doTrigger=False
