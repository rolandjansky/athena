include.block ("RecExCommon/AnalysisCommon_topOptions.py")

############################
## Common job preparation ##
############################

# gbl.AthenaServices.SetFatalHandler(438)
svcMgr.CoreDumpSvc.FatalHandler = 438
import traceback

from AthenaCommon.Logging import logging
logAnaCommon_topOptions = logging.getLogger( 'AnalysisCommon_topOptions' )

## try to load a catalogue of AthFiles from a cache (to speed-up tests)
try:
    import PyUtils.AthFile as af
    # this is the cache for the most current test files (saving 15 second initialisation, worth it for atn test)
    # it should be updated as follow. In a clean directory run RecExCommon_links.sh, then
    # run dump-athfile.py LFN:top_GEO-10-00-00_RDO_extract.pool (or whatever file now used in PoolFileCatalog.xml)
    # run dump-athfile.py on input file for GetCommand.py AMI=q121
    # run dump-athfile.py for any other frequently used test file
    # then cp athfile-cache.ascii.gz RecExCommon/data/recexcommon-afserver-cache.ascii.gz
    af.server.load_cache('recexcommon-afserver-cache.ascii.gz')
    # reenable caching (this was preventing cache for non standard test file e.g. user file to be saved)
    # af.server.disable_pers_cache()
    pass
except Exception:
    logAnaCommon_topOptions.warning("Could not load or disable athfile cache")
    pass


from AthenaCommon.AlgSequence import AlgSequence
from AthenaCommon.AppMgr import ToolSvc,theApp,ServiceMgr
from AthenaCommon.AthenaCommonFlags  import athenaCommonFlags
from AthenaCommon.GlobalFlags  import globalflags
from AthenaCommon.BeamFlags import jobproperties
from RecExConfig.RecFlags import rec
from AthenaCommon.Resilience import treatException,protectedInclude

topSequence = AlgSequence()

import AthenaCommon.Debugging as _acdbg
if not _acdbg.DbgStage.value:
    try:
        topSequence.TimeOut=3600 * Units.s
        pass
    except Exception:
        logAnaCommon_topOptions.warning("could not set TimeOut (temporary)")
        pass
    pass
if rec.Production():
    AthenaCommon.Configurable.log.setLevel( WARNING )
    pass


# reduce amount of tracing from the most verbose
#to spot the most verbose jobo:
#  RecExCommon_links.sh
#  athena >! all.log
#grep "\-\ " all.log | awk {'print $2'} | sort | uniq -c | sort -n | tail -20
# then find jobo number in log file and add the jobo to exclude list
from AthenaCommon.Include import excludeTracePattern
excludeTracePattern.append("*/CLIDComps/clidGenerator.py")
excludeTracePattern.append("*/RecExConfig/Resilience.py")
excludeTracePattern.append("*/AthenaCommmon/Resilience.py")
excludeTracePattern.append("*/OutputStreamAthenaPool/MultipleStreamManager.py")
excludeTracePattern.append("*/GaudiKernel/GaudiHandles.py")
excludeTracePattern.append ( "*/MuonRecExample/MuonRecUtils.py")
excludeTracePattern.append ("athfile-cache.ascii")
excludeTracePattern.append ("*/IOVDbSvc/CondDB.py")
excludeTracePattern.append("*/TrigConfigSvcConfig.py")
excludeTracePattern.append("*/LArCalib.py")
excludeTracePattern.append("*/_xmlplus/*")
excludeTracePattern.append("*/InDetTrigRecExample/EFInDetConfig.py")
excludeTracePattern.append("*/CaloClusterCorrection/CaloSwEtaoff*")
excludeTracePattern.append("*/PyUtils/Helpers.py")
excludeTracePattern.append("*/RecExConfig/RecoFunctions.py")
excludeTracePattern.append("*/TrigEgammaHypo/TrigEFElectronHypoMonitoring.py")
excludeTracePattern.append("*/PerfMonComps/DomainsRegistry.py")
excludeTracePattern.append("*/CaloClusterCorrection/common.py")
excludeTracePattern.append("*/TrigIDSCAN/TrigIDSCAN_Config.py")
excludeTracePattern.append("*/TrigSiTrack/TrigSiTrack_Config.py")
excludeTracePattern.append("*/TrigEgammaHypo/TrigEFElectronHypoConfig.py")
excludeTracePattern.append("*/TrigEgammaHypo/TrigL2CaloHypoMonitoring.py")
excludeTracePattern.append("*/TrigBphysHypo/TrigL2BMuMuFexMonitoring.py")
excludeTracePattern.append("*/TrigBphysHypo/TrigL2TrkMassFexMonitoring.py")
excludeTracePattern.append("*/TrigL2TrkMassFexMonitoring.py")
excludeTracePattern.append("*/TrigBphysHypo/TrigL2TrkMassFexMonitoring.py")
excludeTracePattern.append("*/TrigmuComb/TrigmuCombConfig.py")
excludeTracePattern.append("*/D3PDMakerCoreComps/MakerAlg.py")
excludeTracePattern.append("*/D3PDMakerCoreComps/D3PDObject.py")
excludeTracePattern.append("*/RecExConfig/RecoFunctions.py")
excludeTracePattern.append("*/DQDefects/virtual*")
#####################
# Flags (separated) #
#####################

#include ( "RecExCond/RecExCommon_flags.py" )


## What should be done when encountering an unchecked StatusCode
try:
    if rec.abortOnUncheckedStatusCode():
        svcMgr.StatusCodeSvc.AbortOnError=True
        logAnaCommon_topOptions.debug("Abort on unchecked status code enabled !")
        pass
    pass
except Exception:
    logAnaCommon_topOptions.debug("Did not enable aboort on unchecked status code")
    pass



###################
# Common Services #
###################


# Particle Property
protectedInclude( "PartPropSvc/PartPropSvc.py" )
include.block( "PartPropSvc/PartPropSvc.py" )

# Detector Status
if rec.doDetStatus() and not athenaCommonFlags.isOnline():
    try:
        include("DetectorStatus/DetStatusSvc_CondDB.py")
        pass
    except Exception:
        treatException("Could not load DetStatusSvc_CondDb !")
        rec.doFileMetaData=False
        pass

    if rec.doFileMetaData():
        #DR FIXME not sure about commissioing exception, filemetadata should be in filteredESD as well
        if rec.doWriteRDO() or rec.doWriteESD() or rec.doWriteAOD() or rec.doDPD():
            protectedInclude("DetectorStatus/DetStatusSvc_ToFileMetaData.py")
            pass
        pass
    pass

#Output file TagInfo and metadata
#from AthenaCommon.AppMgr import ServiceMgr as svcMgr
#svcMgr.TagInfoMgr.ExtraTagValuePairs += ["beam_type", jobproperties.Beam.beamType()]
#svcMgr.TagInfoMgr.ExtraTagValuePairs += ["beam_energy", str(jobproperties.Beam.energy())]
#svcMgr.TagInfoMgr.ExtraTagValuePairs += ["triggerStreamOfFile", str(rec.triggerStream())]
#svcMgr.TagInfoMgr.ExtraTagValuePairs += ["project_name", str(rec.projectName())]
#if rec.AMITag()!="": svcMgr.TagInfoMgr.ExtraTagValuePairs += ["AMITag", rec.AMITag() ]
#svcMgr.TagInfoMgr.ExtraTagValuePairs += ["AtlasRelease_" + rec.OutputFileNameForRecoStep(), rec.AtlasReleaseVersion() ]



# end flag settings section
##########################################################################
# set up job


if globalflags.InputFormat()=='pool':

    # to read Pool data
    import AthenaPoolCnvSvc.ReadAthenaPool
    #set elsewhere svcMgr.AthenaPoolCnvSvc.PoolAttributes += [ "DEFAULT_BUFFERSIZE = '2048'" ]
    #set elsewhere svcMgr.AthenaPoolCnvSvc.PoolAttributes += [  "TREE_BRANCH_OFFSETTAB_LEN ='100'" ]

    # if file not in catalog put it there
    svcMgr.PoolSvc.AttemptCatalogPatch=True

    # skip missing file with a warning instead of stopping the job
    # not to be done in production
    # svcMgr.EventSelector.SkipBadFiles=True

    # G4 Pool input
    # it is possible to specify a list of files to be processed consecutively
    # If using logical file name or using back navigation the relevant input
    # files should be listed in the PoolFileCatalog.xml

    if rec.doShowSizeStatistics():
        svcMgr.EventSelector.ShowSizeStatistics = True #  show size inform
        pass

    # Read the input file(s) that should be processed
    svcMgr.EventSelector.InputCollections = athenaCommonFlags.FilesInput()


    from RecExConfig.RecConfFlags import recConfFlags
    ServiceMgr.EventSelector.BackNavigation = recConfFlags.AllowBackNavigation()
    # FIXME temporary
    #logRecExCommon_topOptions.warning("temporary : EventSelector.OverrideRunNumber = True")
    #ServiceMgr.EventSelector.OverrideRunNumber=True

    # backward compatibility (needed for RTT overwriting InputCollections)
    EventSelector=ServiceMgr.EventSelector
    pass



########################################################################

# Number of events to be processed
theApp.EvtMax = athenaCommonFlags.EvtMax()
# possibly skip events


# if need histogram output
try:
    theApp.HistogramPersistency = "ROOT"
    if not hasattr(svcMgr,"THistSvc"):
        from GaudiSvc.GaudiSvcConf import THistSvc
        svcMgr+=THistSvc()
        pass
    if os.path.exists(rec.RootHistoOutput()):
        os.remove(rec.RootHistoOutput())
        pass
    svcMgr.THistSvc.Output += ["GLOBAL DATAFILE='"+rec.RootHistoOutput()+"' OPT='RECREATE'"]
    pass
except:
    logAnaCommon_topOptions.warning("Some problem in registering the services for histogram writing")
    pass




##########################################################################

#--------------------------------------------------------------
# Needed for Configurables
#--------------------------------------------------------------

# should use AthenaCommon.KeyStore
from RecExConfig.ObjKeyStore import objKeyStore, CfgKeyStore

# keep a separate copy of what was read from recexpers (for later consistency check)
cfgRecExPers=CfgKeyStore("recexpers")


# FIXME should fine a better way to keep it uptodate
# read persistent OKS
# if input is pool, read list of input directly from file
if globalflags.InputFormat.is_pool():
    logAnaCommon_topOptions.info("Pool file : storing in objKeyStore the list of input object directly from file")
    try:
        from RecExConfig.InputFilePeeker import inputFileSummary
        objKeyStore.addManyTypesInputFile(inputFileSummary['eventdata_itemsList'])
        pass
    except:
        logAnaCommon_topOptions.error("no input file defined in flags. If using RTT please use tag <athenaCommonFlags/>. Now continuing at own riske")
        pass
    pass
# for BS file this cannot be done already, see later



if not rec.readESD() and not rec.readAOD():
    ##objKeyStore.readInputFile('RecExPers/OKS_streamRDO.py')
    pass
elif rec.readESD():
    ##objKeyStore.readInputFile('RecExPers/OKS_streamESD.py')
    ##objKeyStore.readInputBackNav('RecExPers/OKS_streamRDO.py')
    # still need to read in AOD content built at ESD stage
    objKeyStore.readStreamAOD('RecExPers/OKS_streamAOD_fromESD.py')
    # at this point all streamAOD there should already be in ESD
    for k in objKeyStore['streamAOD'].list():
        if not objKeyStore['inputFile'].has_item (k):
            logAnaCommon_topOptions.info("Warning item %s scheduled co be copied from ESD to  AOD, but missing in ESD! Dropped. " % k )
            objKeyStore['streamAOD'].removeItem (k)
            pass
        pass
    pass
elif rec.readAOD():
    pass
    ##objKeyStore.readInputFile('RecExPers/OKS_streamAOD.py')
    ##objKeyStore.readInputBackNav('RecExPers/OKS_streamESD.py')
    ##objKeyStore.readInputBackNav('RecExPers/OKS_streamRDO.py')



if rec.OutputLevel() <= DEBUG:
    print " Initial content of objKeyStore "
    print objKeyStore
    pass

# typical objKeyStore usage
# objKeyStore.addStreamESD("Type1","Key1"] )
# objKeyStore.addManyTypesStreamESD(["Type1#Key1","Type2#Key2"] )
# objKeyStore['streamESD'].has_item("Type1#Key1"]
# objKeyStore['streamESD'].has_item("Type1#*"]   # for any key of this type
# wether an object is available should be tested with
# objKeyStore.isInInput("Type1","Key1") or objKeyStore.isInInput("Type1","*")
# which is an or of 'inputFile' and 'transient'. While everythin in StreamXYZ is also in 'transient'

#one canalso remove object
#objKeyStore['inputFile'].removeItem(["SomeType#SomeKey"])



if rec.readAOD():
    from AthenaServices.AthenaServicesConf import AthenaEventLoopMgr
    ServiceMgr += AthenaEventLoopMgr()
    ServiceMgr.AthenaEventLoopMgr.EventPrintoutInterval = 100
    logAnaCommon_topOptions.debug("AOD reading case: Set EventPrintoutInterval=100")
    pass



# put quasi empty first algorithm so that the first real
# algorithm does not see the memory change due to event manipulation
#from AthenaPoolTools.AthenaPoolToolsConf import EventCounter
from GaudiAlg.GaudiAlgConf import EventCounter


#import PerfMonComps.DomainsRegistry as pdr
#pdr.flag_domain('admin')
# one print every 100 events
eventPrintFrequency = vars().get('EVTPRINTFREQ', 100)
topSequence += EventCounter(Frequency=eventPrintFrequency)
if hasattr(ServiceMgr,"AthenaEventLoopMgr"):
    ServiceMgr.AthenaEventLoopMgr.EventPrintoutInterval = eventPrintFrequency
    topSequence.EventCounter.Frequency = ServiceMgr.AthenaEventLoopMgr.EventPrintoutInterval
    pass


try:
    svcMgr.EventSelector.SkipEvents = athenaCommonFlags.SkipEvents()
    pass
except Exception:
    treatException("Could not set EventSelector.SkipEvents")
    pass


#--------------------------------------------------------------
# Now specify the list of algorithms to be run
# The order of the jobOption specify the order of the algorithms
# (don't modify it)
#--------------------------------------------------------------


#Temporary: Schedule conversion algorithm for EventInfo object:
if not objKeyStore.isInInput( "xAOD::EventInfo" ):
    if not hasattr( topSequence, "xAODMaker::EventInfoCnvAlg" ):
        from xAODEventInfoCnv.xAODEventInfoCreator import xAODMaker__EventInfoCnvAlg
        topSequence += xAODMaker__EventInfoCnvAlg()
        pass
else:
    if not hasattr( topSequence, "xAODMaker::EventInfoNonConstCnvAlg" ):
        topSequence += CfgMgr.xAODMaker__EventInfoNonConstCnvAlg()
        pass



###################################################################
#
# functionality : monitor memory and cpu time
#

# check dictionary all the time
ServiceMgr.AthenaSealSvc.CheckDictionary = True
if not rec.doCheckDictionary():
    ServiceMgr.AthenaSealSvc.OutputLevel=WARNING
    pass

#
#
#now write out Transient Event Store content in POOL
if rec.doWriteESD() or rec.doWriteAOD() or rec.doWriteRDO() or rec.doWriteTAG():
    import AthenaPoolCnvSvc.WriteAthenaPool


# Must make sure that no OutStream's have been declared
#FIXME
theApp.OutStream = []




MetaDataStore=svcMgr.MetaDataStore


#Lumiblocks and EventBookkeepers
if rec.doFileMetaData():

    #lumiblocks
    if rec.readESD() or rec.readAOD():
        from LumiBlockComps.LumiBlockCompsConf import LumiBlockMetaDataTool
        svcMgr.MetaDataSvc.MetaDataTools += [ "LumiBlockMetaDataTool" ]
        pass
    else:
        # Create LumiBlock meta data containers *before* creating the output StreamESD/AOD
        include ("LumiBlockAthenaPool/LumiBlockAthenaPool_joboptions.py")
        from LumiBlockComps.LumiBlockCompsConf import CreateLumiBlockCollectionFromFile
        topSequence += CreateLumiBlockCollectionFromFile()
        pass
    #EventBookkeepers
    if not hasattr(svcMgr,"CutFlowSvc"):
        from EventBookkeeperTools.CutFlowHelpers import CreateCutFlowSvc
        CreateCutFlowSvc( svcName="CutFlowSvc", athFile=af, seq=topSequence, addMetaDataToAllOutputFiles=False )
        #from EventBookkeeperTools.EventBookkeeperToolsConf import CutFlowSvc
        #svcMgr+=CutFlowSvc()
        pass
    if rec.readAOD() or rec.readESD():
        #force CutFlowSvc execution (necessary for file merging)
        theApp.CreateSvc+=['CutFlowSvc']
        pass
    pass




#pdr.flag_domain('aod')

if rec.doWriteAOD() or rec.doWriteESD(): #For xAOD writing:
    try:
        if rec.doFileMetaData(): #needed to have xAOD readable outside athena
            theApp.CreateSvc += [ "xAODMaker::EventFormatSvc" ]
            if rec.doWriteAOD():
                StreamAOD_Augmented.AddMetaDataItem("xAOD::EventFormat#EventFormat")
                pass
            if rec.doWriteESD():
                StreamESD_Augmented.AddMetaDataItem("xAOD::EventFormat#EventFormat")
                pass
            pass
        pass
    except Exception:
        treatException("Problem with extra attributes for xAOD output")
        pass
    pass


#########
## DPD ##
#########
if rec.doDPD() and (rec.DPDMakerScripts()!=[] or rec.doDPD.passThroughMode):
    from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
    #from PrimaryDPDMaker.PrimaryDPDFlags import primDPD

    # Get an instance of the random number generator
    # The actual seeds are dummies since event reseeding is used
    from AthenaServices.AthenaServicesConf import AtRanluxGenSvc
    if not hasattr(svcMgr,'DPDAtRanluxGenSvc'):
        svcMgr += AtRanluxGenSvc( "DPDAtRanluxGenSvc",
                                  OutputLevel    = ERROR,
                                  Seeds          = [ "DPDRANDOMNUMBERSTREAM 2 3" ],
                                  SaveToFile     = False,
                                  EventReseeding = True
                                  )
        pass

    # Schedule the AODSelect setup
    #if rec.doAODSelect():
    #    try:
    #        include("AODSelect/AODSelect_setupOptions.py")
    #    except Exception:
    #        treatException("Could not load AODSelect/AODSelect_setupOptions.py !")
    #        rec.doAODSelect = False
    #        pass
    #    pass

    #This block may not be needed... something to check if somebody has time!
    if rec.DPDMakerScripts()!=[]:
        if globalflags.InputFormat()=='pool':
            #include("PyAnalysisCore/InitPyAnalysisCore.py")
            pass
        #DR not needed
        # to be double check LArTool MuonEventCnvTools remove LArTowerBuild
        # if rec.readAOD():
        #    import AthenaPoolCnvSvc.ReadAthenaPool
        #    include( "ParticleBuilderOptions/AOD_PoolCnv_jobOptions.py")
        #    include( "ParticleBuilderOptions/McAOD_PoolCnv_jobOptions.py")
        #    include( "ParticleBuilderOptions/ESD_PoolCnv_jobOptions.py" )
        #    pass
        pass

    # #First of all, schedule EventCounterAlg
    # from EventBookkeeperTools.EventCounterAlg import EventCounterAlg
    # topSequence+=EventCounterAlg("AllExecutedEvents")

    #Then include all requested DPD makers
    logAnaCommon_topOptions.debug( "Content of rec.DPDMakerSkripts = %s", rec.DPDMakerScripts() )
    for DPDMaker in rec.DPDMakerScripts():
        DPDMakerName = str(DPDMaker)
        logAnaCommon_topOptions.info( "Including %s...",DPDMakerName )
        include(DPDMaker)
        pass

    # Schedule the AODSelect algorithms
    #if rec.doAODSelect():
    #    try:
    #        include("AODSelect/AODSelect_mainOptions.py")
    #    except Exception:
    #        treatException("Could not load AODSelect/AODSelect_mainOptions.py !")
    #        rec.doAODSelect = False
    #        pass
    #    pass

    #SkimDecision objects may once migrate to CutFlowSvc or DecisionSvc, but not yet
    #logAnaCommon_topOptions.info( "primDPD.WriteSkimDecisions =  %s", primDPD.WriteSkimDecisions() )
    #if primDPD.WriteSkimDecisions():
    #    MSMgr.WriteSkimDecisionsOfAllStreams()
    #    pass

    #Configure CutFlowSv and common metadata
    if rec.doFileMetaData():

        # # Determine current skimming cycle and input stream name
        # from RecExConfig.InputFilePeeker import inputFileSummary
        # from RecExConfig.RecoFunctions import GetSkimCycle
        # inputCycle=GetSkimCycle(inputFileSummary)
        # if inputCycle<0:
        #     currentCycle=1
        # else:
        #     currentCycle=inputCycle+1
        #     pass
        # svcMgr.CutFlowSvc.SkimmingCycle=currentCycle
        # svcMgr.CutFlowSvc.InputStream=rec.mergingStreamName()
        # #svcMgr.CutFlowSvc.OutputLevel=DEBUG

        #Exception for DPD pass-through mode
        if rec.doDPD.passThroughMode:
            svcMgr.CutFlowSvc.SkimmingCycle=0
            svcMgr.CutFlowSvc.InputStream="Virtual"
            pass

        if rec.DPDMakerScripts()!=[] and not rec.doDPD.passThroughMode :
            # #Create a separate EventBookkeeper list to persistify skimming cycle info
            # from EventBookkeeperTools.BookkeepingInfoWriter import CutCycleWriter
            # topSequence += CutCycleWriter("CutCycleWriter",
            #                               OutputName   = "ProcessingCycle",
            #                               CurrentCycle = currentCycle)


            #Explicitely add file metadata from input and from transient store
            MSMgr.AddMetaDataItemToAllStreams(inputFileSummary['metadata_itemsList'])
            MSMgr.AddMetaDataItemToAllStreams( "LumiBlockCollection#*" )
            MSMgr.AddMetaDataItemToAllStreams( "xAOD::CutBookkeeperContainer#*" )
            MSMgr.AddMetaDataItemToAllStreams( "xAOD::CutBookkeeperAuxContainer#*" )
            #MSMgr.AddMetaDataItemToAllStreams( "EventBookkeeperCollection#*" )
            MSMgr.AddMetaDataItemToAllStreams( "IOVMetaDataContainer#*" )
            pass
        pass
    pass



###################
## Common Utils  ##
###################

#include("RecExCommon/RecoUtils.py")
