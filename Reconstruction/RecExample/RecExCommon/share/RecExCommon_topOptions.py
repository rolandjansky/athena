include.block ("RecExCommon/RecExCommon_topOptions.py")

############################
## Common job preparation ##
############################

# gbl.AthenaServices.SetFatalHandler(438)
svcMgr.CoreDumpSvc.FatalHandler = 438
import traceback

from AthenaCommon.Logging import logging
logRecExCommon_topOptions = logging.getLogger( 'RecExCommon_topOptions' )

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
except Exception:
    logRecExCommon_topOptions.warning("Could not load or disable athfile cache")


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
    except Exception:
        logRecExCommon_topOptions.warning("could not set TimeOut (temporary)")

if rec.Production():
    AthenaCommon.Configurable.log.setLevel( WARNING )

# reduce amount of tracing from the most verbose
#to spot the most verbose jobo:
#  RecExCommon_links.sh
#  athena >! all.log
#grep "\-\ " all.log | awk {'print $2'} | sort | uniq -c | sort -n | tail -20
# then find jobo number in log file and add the jobo to exclude list
from AthenaCommon.Include import excludeTracePattern
excludeTracePattern.append("*/BTagging/BTaggingFlags.py")
excludeTracePattern.append("*/CLIDComps/clidGenerator.py")
excludeTracePattern.append("*/RecExConfig/Resilience.py")
excludeTracePattern.append("*/AthenaCommmon/Resilience.py")
excludeTracePattern.append("*/OutputStreamAthenaPool/MultipleStreamManager.py")
excludeTracePattern.append("*/GaudiKernel/GaudiHandles.py")
excludeTracePattern.append ("*/TriggerMenu/menu/HLTObjects.py")
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
excludeTracePattern.append("*/PanTauAnalysis/Class_FeatureHandler.py")
excludeTracePattern.append("*/TrigEDMConfig/TriggerEDM.py")
excludeTracePattern.append("*/TrigL2MissingET/TrigL2MissingETMonitoring.py")
#####################
# Flags (separated) #
#####################

include ( "RecExCond/RecExCommon_flags.py" )

if (rec.doRecoTiming() and rec.OutputFileNameForRecoStep() in ('RAWtoESD','ESDtoAOD','RAWtoALL')):

    from RecAlgs.RecAlgsConf import TimingAlg
    topSequence+=TimingAlg("RecoTimerBegin")
    topSequence.RecoTimerBegin.TimingObjOutputName=rec.OutputFileNameForRecoStep()+"_timings"
    try:
        from RecAlgs.RecAlgsConf import MemoryAlg
        topSequence+=MemoryAlg("RecoMemoryBegin")
        topSequence.RecoMemoryBegin.MemoryObjOutputName=rec.OutputFileNameForRecoStep()+"_mems"
    except Exception:
        logRecExCommon_topOptions.info("Could not instantiate MemoryAlg")


if rec.doESDReconstruction():
    from RecAlgs.RecAlgsConf import EventInfoUnlocker
    topSequence+=EventInfoUnlocker("UnlockEventInfo")

try:
    if rec.abortOnUncheckedStatusCode():
      svcMgr.StatusCodeSvc.AbortOnError=True
      logRecExCommon_topOptions.info("Abort on unchecked status code enabled !")
except Exception:
    logRecExCommon_topOptions.info("Did not enable aboort on unchecked status code")

if rec.readESD():
    rec.readRDO = False

#move AODFix here so that it can use rec.readRDO etc...

from AODFix.AODFix import *
AODFix_Init()
AODFix_preInclude()


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

if rec.doFileMetaData():
    ## compute ESD item list (in CILMergedESD )
    protectedInclude ( "RecExPers/RecoOutputMetadataList_jobOptions.py" )


#Output file TagInfo and metadata
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
svcMgr.TagInfoMgr.ExtraTagValuePairs += ["beam_type", jobproperties.Beam.beamType()]
svcMgr.TagInfoMgr.ExtraTagValuePairs += ["beam_energy", str(jobproperties.Beam.energy())]
svcMgr.TagInfoMgr.ExtraTagValuePairs += ["triggerStreamOfFile", str(rec.triggerStream())]
svcMgr.TagInfoMgr.ExtraTagValuePairs += ["project_name", str(rec.projectName())]
#if rec.AMITag()!="": svcMgr.TagInfoMgr.ExtraTagValuePairs += ["AMITag", rec.AMITag() ]
svcMgr.TagInfoMgr.ExtraTagValuePairs += ["AtlasRelease_" + rec.OutputFileNameForRecoStep(), rec.AtlasReleaseVersion() ]

# Build amitag list
amitag = ""
from RecExConfig.InputFilePeeker import inputFileSummary
try:
  amitag = inputFileSummary['metadata']['/TagInfo']['AMITag']
except:
  logRecExCommon_topOptions.info("Cannot access TagInfo/AMITag")

# append new if previous exists otherwise take the new alone 
if amitag != "":
  svcMgr.TagInfoMgr.ExtraTagValuePairs += ["AMITag", inputFileSummary['metadata']['/TagInfo']['AMITag'] + "_" + rec.AMITag() ]
else:
  svcMgr.TagInfoMgr.ExtraTagValuePairs += ["AMITag", rec.AMITag() ]



AODFix_addMetaData()

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

if not 'doMixPoolInput' in dir():
    doMixPoolInput = False


if rec.readTAG():
    logRecExCommon_topOptions.info("reading from TAG. Applying selection athenaCommonFlags.PoolInputQuery= %s " % athenaCommonFlags.PoolInputQuery() )
    logRecExCommon_topOptions.info("...to have the list of available variables. root myTAG.root. POOLCollectionTree->Show(0)")

if globalflags.InputFormat()=='pool':


    if doMixPoolInput:
        if not rec.readRDO():
            raise "doMixPoolInput only functional reading RDO"
        import StreamMix.ReadAthenaPool
        from EventSelectorAthenaPool.EventSelectorAthenaPoolConf   import EventSelectorAthenaPool
        from AthenaServices.AthenaServicesConf import MixingEventSelector
        svcMgr.ProxyProviderSvc.ProviderNames += [ "MixingEventSelector/EventMixer" ]
        EventMixer =  MixingEventSelector("EventMixer")
        # In UserAlgs, do the following for as many datasets as you like:
        # svcMgr += EventSelectorAthenaPool( "EventSelector5009" )
        # Service("EventSelector5009").InputCollections = [ 'misal1_mc12.005009.etc.RDO.pool.root' ]
        # EventMixer.TriggerList += [ "EventSelectorAthenaPool/EventSelector5009:0:500" ]
    else:
        # to read Pool data
        import AthenaPoolCnvSvc.ReadAthenaPool

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

        if rec.readTAG():
            svcMgr.EventSelector.InputCollections = athenaCommonFlags.FilesInput()
            svcMgr.EventSelector.CollectionType = "ExplicitROOT"
            svcMgr.EventSelector.Connection = ""
            svcMgr.EventSelector.Query = athenaCommonFlags.PoolInputQuery()
            if rec.readESD():
                svcMgr.EventSelector.RefName="StreamESD"
            elif not rec.readAOD() and not rec.TAGFromRDO() : # == read RDO
                svcMgr.EventSelector.RefName="StreamRDO"
        elif rec.readAOD():
            svcMgr.EventSelector.InputCollections = athenaCommonFlags.FilesInput()
        else:
            if not rec.readESD():
                svcMgr.EventSelector.InputCollections = athenaCommonFlags.FilesInput()
            else:
                svcMgr.EventSelector.InputCollections = athenaCommonFlags.FilesInput()


        from RecExConfig.RecConfFlags import recConfFlags
        ServiceMgr.EventSelector.BackNavigation = recConfFlags.AllowBackNavigation()
        # FIXME temporary
        #logRecExCommon_topOptions.warning("temporary : EventSelector.OverrideRunNumber = True")
        #ServiceMgr.EventSelector.OverrideRunNumber=True

        # backward compatibility (needed for RTT overwriting InputCollections)
        EventSelector=ServiceMgr.EventSelector



#######################################################################
# useful debugging/info tools

# Display detailed size and timing statistics for writing and reading
#  but not if trigger on, because of the randomize key
from RecExConfig.RecAlgsFlags import recAlgs


########################################################################

# Number of events to be processed
theApp.EvtMax = athenaCommonFlags.EvtMax()
# possibly skip events

if rec.doCBNT():
    theApp.HistogramPersistency = "ROOT"
    from AthenaCommon.AppMgr import ServiceMgr
    if not hasattr(ServiceMgr, 'THistSvc'):
        from AthenaCommon import CfgMgr
        ServiceMgr += CfgMgr.THistSvc()

    ServiceMgr.THistSvc.Output += ["AANT DATAFILE='%s' OPT='RECREATE'" % rec.RootNtupleOutput()]
    # do not print the full dump of variables
    ServiceMgr.THistSvc.OutputLevel=WARNING

    try:
        from AnalysisTools.AnalysisToolsConf import AANTupleStream

        theAANTupleStream=AANTupleStream(OutputName=rec.RootNtupleOutput() )
        theAANTupleStream.ExtraRefNames = []
        if rec.doWriteRDO():
            theAANTupleStream.ExtraRefNames += [ "StreamRDO" ]
        if rec.doWriteESD():
            theAANTupleStream.ExtraRefNames += [ "StreamESD" ]
        if rec.doWriteAOD():
            theAANTupleStream.ExtraRefNames += [ "StreamAOD" ]

        if not rec.doWriteESD() and not rec.doWriteRDO() and not rec.doWriteAOD():
            theAANTupleStream.WriteInputDataHeader = True

        if globalflags.InputFormat()=='bytestream':
            theAANTupleStream.ExistDataHeader = False
    except Exception:
        treatException("could not load AnalysisTools.AnalysisToolsConf=>no ntuple! ")
        rec.doCBNT=False


# if need histogram output

if rec.doHist() :
    # if (not athenaCommonFlags.isOnline) :
    if (not athenaCommonFlags.isOnline()) or (athenaCommonFlags.isOnline() and athenaCommonFlags.isOnlineStateless()):
        theApp.HistogramPersistency = "ROOT"
        if not hasattr(svcMgr,"THistSvc"):
            from GaudiSvc.GaudiSvcConf import THistSvc
            svcMgr+=THistSvc()
        if os.path.exists(rec.RootHistoOutput()):
            os.remove(rec.RootHistoOutput())
        svcMgr.THistSvc.Output += ["GLOBAL DATAFILE='"+rec.RootHistoOutput()+"' OPT='RECREATE'"]
    else:
##        from TrigServices.TrigServicesConf import TrigMonTHistSvc
##        THistSvc = TrigMonTHistSvc("THistSvc")
##        svcMgr += THistSvc
    # --> AK
        try:
            from TrigServices.TrigServicesConf import TrigMonTHistSvc
            THistSvc = TrigMonTHistSvc("THistSvc")
            svcMgr += THistSvc
        except:
            logRecExCommon_topOptions.warning("isOnline=True but online Histogramming service not available, using offline THistSvc")
            from GaudiSvc.GaudiSvcConf import THistSvc
            svcMgr+=THistSvc()
            if os.path.exists(rec.RootHistoOutput()):
                os.remove(rec.RootHistoOutput())
            svcMgr.THistSvc.Output += ["GLOBAL DATAFILE='"+rec.RootHistoOutput()+"' OPT='RECREATE'"]
    # --> AK


if rec.doFastPhysMonitoring():
    if not hasattr(svcMgr,"THistSvc"):
        from GaudiSvc.GaudiSvcConf import THistSvc
        svcMgr+=THistSvc()
    svcMgr.THistSvc.Output += ["FPMFILE DATAFILE='"+rec.RootFastPhysMonOutput()+"' OPT='RECREATE'"]




##########################################################################

#--------------------------------------------------------------
# Needed for Configurables
#--------------------------------------------------------------

## ## # FIXME a bit strange
## ## doFastCaloSim=False
## ## try:
## ##     from CaloRec.CaloCellFlags import jobproperties
## ##     doFastCaloSim=jobproperties.CaloCellFlags.doFastCaloSim()
## ## except Exception:
## ##     logRecExCommon_topOptions.warning("could not load CaloCellFlags")


# should use AthenaCommon.KeyStore
from RecExConfig.ObjKeyStore import objKeyStore, CfgKeyStore

# keep a separate copy of what was read from recexpers (for later consistency check)
cfgRecExPers=CfgKeyStore("recexpers")


# FIXME should fine a better way to keep it uptodate
# read persistent OKS
# if input is pool, read list of input directly from file
if globalflags.InputFormat.is_pool():
    logRecExCommon_topOptions.info("Pool file : storing in objKeyStore the list of input object directly from file")
    try:
        from RecExConfig.InputFilePeeker import inputFileSummary
        objKeyStore.addManyTypesInputFile(inputFileSummary['eventdata_itemsList'])
    except:
        logRecExCommon_topOptions.error("no input file defined in flags. If using RTT please use tag <athenaCommonFlags/>. Now continuing at own riske")
# for BS file this cannot be done already, see later


if rec.doWriteAOD():
    cfgRecExPers.read('RecExPers/OKS_streamAOD_fromESD.py','streamAOD')


if not rec.readESD() and not rec.readAOD():
    ##objKeyStore.readInputFile('RecExPers/OKS_streamRDO.py')
    pass
elif rec.readESD():
    ##objKeyStore.readInputFile('RecExPers/OKS_streamESD.py')
    ##objKeyStore.readInputBackNav('RecExPers/OKS_streamRDO.py')
    # still need to read in AOD content built at ESD stage
    objKeyStore.readStreamAOD('RecExPers/OKS_streamAOD_fromESD.py')
    # at this point all streamAOD there should already be in ESD
    # for k in objKeyStore['streamAOD'].list():
    #     if not objKeyStore['inputFile'].has_item (k):
    #         logRecExCommon_topOptions.info("Warning item %s scheduled co be copied from ESD to  AOD, but missing in ESD! Dropped. " % k )
    #         objKeyStore['streamAOD'].removeItem (k)

elif rec.readAOD():
    pass
    ##objKeyStore.readInputFile('RecExPers/OKS_streamAOD.py')
    ##objKeyStore.readInputBackNav('RecExPers/OKS_streamESD.py')
    ##objKeyStore.readInputBackNav('RecExPers/OKS_streamRDO.py')



if rec.OutputLevel() <= DEBUG:
    print " Initial content of objKeyStore "
    print objKeyStore

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

#add remapping if have old names. Currently check for a few (but one should work for any)
if rec.doContainerRemapping() and (rec.readESD() or rec.readAOD()):
    if (objKeyStore.isInInput("xAOD::ElectronContainer", "ElectronCollection") or
        objKeyStore.isInInput("xAOD::TrackParticleContainer", "InDetTrackParticlesForward") or
        objKeyStore.isInInput("xAOD::CaloClusterContainer", "CaloCalTopoCluster")):
        include ("RecExCommon/ContainerRemapping.py")




#if jivexml required and id is enabled, make sure space points are generated
if rec.doJiveXML() and DetFlags.detdescr.ID_on() :
    if not objKeyStore.isInInput("SpacePointContainer","SCT_SpacePoints") or not objKeyStore.isInInput("SpacePointContainer","PixelSpacePoints") :

        if objKeyStore.isInInput("InDet::SCT_ClusterContainer","*") or objKeyStore.isInInput("InDet::PixelClusterContainer","*") or DetFlags.haveRIO.pixel_on() or DetFlags.haveRIO.SCT_on():
            from InDetRecExample.InDetJobProperties import InDetFlags
            InDetFlags.doSpacePointFormation.set_Value_and_Lock(True)
            logRecExCommon_topOptions.info("JiveXML asked but no SpacePointContainer in input file...scheduling space point formation." )
        else:
            logRecExCommon_topOptions.warning("JiveXML asked but no SpacePointContainer in input file...however no clusters available ! will not schedule space point formation." )



# put quasi empty first algorithm so that the first real
# algorithm does not see the memory change due to event manipulation
#from AthenaPoolTools.AthenaPoolToolsConf import EventCounter
from GaudiAlg.GaudiAlgConf import EventCounter


import PerfMonComps.DomainsRegistry as pdr
pdr.flag_domain('admin')
# one print every 100 event
topSequence+=EventCounter(Frequency=100)

#Temporary: Schedule conversion algorithm for EventInfo object:
# Note that we need to check whether the HLT already added this algorithm to the
# algorithm sequence!
#FIXME: Subsequent algorithms may alter the event info object (setting Error bits)
from AthenaCommon.AlgSequence import AthSequencer
condSeq = AthSequencer("AthCondSeq")

if( ( not objKeyStore.isInInput( "xAOD::EventInfo") ) and \
        ( not hasattr( topSequence, "xAODMaker::EventInfoCnvAlg" ) ) ):
    from xAODEventInfoCnv.xAODEventInfoCreator import xAODMaker__EventInfoCnvAlg
    condSeq+=xAODMaker__EventInfoCnvAlg()
    pass

# bytestream reading need to shedule some algorithm

if globalflags.InputFormat.is_bytestream():
    logRecExCommon_topOptions.info("Setting up ByteStream reading...")
    #Online and tag-reading are mutually exclusive
    if  athenaCommonFlags.isOnline():
        logRecExCommon_topOptions.info("Should set up  EmonByteStreamSvc here....")
        # --> AK: only load if EmonByteStreamSvc is not setup already, as for example for isOnline nightly offline
        try:
            svcMgr.ByteStreamInputSvc
        except:
            logRecExCommon_topOptions.warning("isOnline=True but EmonByteStreamSvc not available, probably running online nightly")
            from ByteStreamCnvSvc import ReadByteStream

            # Specify input file
            if len(athenaCommonFlags.FilesInput())>0:
                svcMgr.ByteStreamInputSvc.FullFileName=athenaCommonFlags.FilesInput()
            elif len(athenaCommonFlags.BSRDOInput())>0:
                svcMgr.ByteStreamInputSvc.FullFileName=athenaCommonFlags.BSRDOInput()
        # --> AK
    else:
        logRecExCommon_topOptions.info("Read ByteStream file(s)")
        if rec.readTAG():
            # FIXME need cleaner merger between ReadAthenaPool.py and ReadByteStream.py

            # for EventType
            from ByteStreamCnvSvc.ByteStreamCnvSvcConf import ByteStreamCnvSvc
            svcMgr += ByteStreamCnvSvc()
            #EventPersistencySvc = svcMgr.EventPersistencySvc
            svcMgr.EventPersistencySvc.CnvServices += [ "ByteStreamCnvSvc" ]

            # ByteStreamAddressProviderSvc
            from ByteStreamCnvSvcBase.ByteStreamCnvSvcBaseConf import ByteStreamAddressProviderSvc
            svcMgr += ByteStreamAddressProviderSvc()
            ByteStreamAddressProviderSvc = svcMgr.ByteStreamAddressProviderSvc

            # proxy provider
            from SGComps.SGCompsConf import ProxyProviderSvc
            svcMgr += ProxyProviderSvc()

            #specific for tag
            from ByteStreamCnvSvc.ByteStreamCnvSvcConf import ByteStreamNavigationProviderSvc
            svcMgr += ByteStreamNavigationProviderSvc( "ByteStreamNavigationProviderSvc" )

            import AthenaPoolCnvSvc.ReadAthenaPool

            svcMgr.ProxyProviderSvc.ProviderNames += [ "ByteStreamNavigationProviderSvc" ]

            EventSelector = svcMgr.EventSelector
            # List of input collections:
            EventSelector.InputCollections = athenaCommonFlags.FilesInput()
            # Type of input collections:
            EventSelector.CollectionType = "ExplicitROOT"
            # Query applied to event tag collection metadata:
            EventSelector.Query = athenaCommonFlags.PoolInputQuery()
            EventSelector.RefName = "StreamRAW"


        else: #Regular offline case:
            from ByteStreamCnvSvc import ReadByteStream


            # Specify input file
            if len(athenaCommonFlags.FilesInput())>0:
                svcMgr.ByteStreamInputSvc.FullFileName=athenaCommonFlags.FilesInput()
            elif len(athenaCommonFlags.BSRDOInput())>0:
                svcMgr.ByteStreamInputSvc.FullFileName=athenaCommonFlags.BSRDOInput()

    if globalflags.DataSource()=='geant4':
        logRecExCommon_topOptions.info("DataSource is 'geant4'")
        #Special add-ons for simulation based bytestream
        from TriggerJobOpts.TriggerFlags import TriggerFlags as tf
        tf.configurationSourceList = ['ds']
        tf.readLVL1configFromXML = True
        tf.readHLTconfigFromXML = True
        svcMgr.ByteStreamCnvSvc.IsSimulation = True


    #Set TypeNames of ByteStreamInputService according to global flags:
    protectedInclude("RecExCommon/BSRead_config.py")


try:
    svcMgr.EventSelector.SkipEvents = athenaCommonFlags.SkipEvents()
except Exception:
    treatException("Could not set EventSelector.SkipEvents")


if rec.doEdmMonitor() and DetFlags.detdescr.ID_on():
    try:
        from AthenaCommon.ConfigurableDb import getConfigurable
        topSequence += getConfigurable("Trk::EventDataModelMonitor")("EventDataModelMonitor")
    except Exception:
        treatException("Could not load EventDataModelMonitor" )




if rec.doDumpPoolInputContent() and globalflags.InputFormat()=='pool':
    try:
        include("AthenaPoolTools/EventCount_jobOptions.py")
        topSequence.EventCount.Dump=True
        topSequence.EventCount.OutputLevel=DEBUG
    except:
        treatException("EventCount_jobOptions.py" )


#--------------------------------------------------------------
# Now specify the list of algorithms to be run
# The order of the jobOption specify the order of the algorithms
# (don't modify it)
#--------------------------------------------------------------


#
#
# functionality : read truth
#
if rec.doTruth():
    # this algorithm dump the content of the MC event: big output
    if rec.doDumpMC():
        # generator truth
        from TruthExamples.TruthExamplesConf import DumpMC
        topSequence+=DumpMC()

    protectedInclude( "McParticleAlgs/TruthParticleBuilder_jobOptions.py" )

if rec.readESD():
   doMuonboyEDM=False

#EventInfoCnv xAOD conversion was here - moved up to run before BS reading algos

if recAlgs.doAtlfast():
    protectedInclude ("AtlfastAlgs/Atlfast_RecExCommon_Fragment.py")
AODFix_postAtlfast()


pdr.flag_domain('trig')
# no trigger, if readESD _and_ doESD ! (from Simon George, #87654)
if rec.readESD() and rec.doESD():
    rec.doTrigger=False
    recAlgs.doTrigger=False
    logRecExCommon_topOptions.info("detected re-reconstruction from ESD, will switch trigger OFF !")
#try:
if rec.doTrigger:
    if globalflags.DataSource() == 'data'and globalflags.InputFormat == 'bytestream':
        try:
            include("TriggerRelease/BStoESD_Tier0_HLTConfig_jobOptions.py")
        except Exception:
            treatException("Could not import TriggerRelease/BStoESD_Tier0_HLTConfig_jobOptions.py . Switching trigger off !" )
            recAlgs.doTrigger=False
    else:
        try:
            from TriggerJobOpts.TriggerGetter import TriggerGetter
            triggerGetter = TriggerGetter()
        except Exception:
            treatException("Could not import TriggerJobOpts.TriggerGetter . Switched off !" )
            recAlgs.doTrigger=False

AODFix_postTrigger()

if globalflags.DataSource()=='geant4':
    if (rec.doRecoTiming() and rec.OutputFileNameForRecoStep() in ('RAWtoESD','ESDtoAOD','RAWtoALL')):
        topSequence+=TimingAlg("RecoTimerAfterTrigger")
        topSequence.RecoTimerAfterTrigger.TimingObjOutputName=rec.OutputFileNameForRecoStep()+"_timings"
        try:
            topSequence+=MemoryAlg("RecoMemoryAfterTrigger")
            topSequence.RecoMemoryAfterTrigger.MemoryObjOutputName=rec.OutputFileNameForRecoStep()+"_mems"
        except Exception:
            logRecExCommon_topOptions.info("Could not instantiate MemoryAlg")

if not 'disableRPC' in dir():
    disableRPC = False


if globalflags.InputFormat.is_bytestream():
    logRecExCommon_topOptions.info("BS file : storing in objKeyStore the list of input object from ByteStreamAddressProviderSvc.TypeNames. Unsafe ! ")
    newTN=[]
    for a in svcMgr.ByteStreamAddressProviderSvc.TypeNames:
        newTN+=[a.replace("/","#")]
        objKeyStore.addManyTypesInputFile(newTN)
        pass
    pass

### write mu values into EventInfo
if rec.doESD() and rec.readRDO() and not (globalflags.DataSource()=='geant4') and jobproperties.Beam.beamType()=="collisions" and not athenaCommonFlags.isOnline():
    try:
        include ("LumiBlockComps/LumiBlockMuWriter_jobOptions.py")
    except Exception:
        treatException("Could not load LumiBlockMuWriter_jobOptions.py")
        pass
#    muWriter.OutputLevel=DEBUG
    pass

if rec.doMonitoring():
    try:
        include ("AthenaMonitoring/DataQualityInit_jobOptions.py")
    except Exception:
        treatException("Could not load AthenaMonitoring/DataQualityInit_jobOptions.py")


#
# System Reconstruction
#
include ("RecExCommon/SystemRec_config.py")
AODFix_postSystemRec()



#
# Combined reconstruction
#
include ("RecExCommon/CombinedRec_config.py")
AODFix_postCombinedRec()




#
# Heavy ion reconstruction  special configuration
#
if rec.doHeavyIon():
    protectedInclude ("HIRecExample/HIRec_jobOptions.py")

if rec.doHIP ():
    protectedInclude ("HIRecExample/HIPRec_jobOptions.py")


if rec.doWriteBS() and not recAlgs.doTrigger():
    include( "ByteStreamCnvSvc/RDP_ByteStream_jobOptions.py" )
    pass

## add in RawInfoSummaryForTagWriter
if rec.doESD() and not rec.readESD() and rec.doTagRawSummary():
    try:
        include("EventTagRawAlgs/RawInfoSummaryForTagWriter_jobOptions.py")
    except:
        logRecExCommon_topOptions.warning("Could not load RawInfoSummaryForTagWriter_joboptions !" )
        #treatException("Could not load RawInfoSummaryForTagWriter_joboptions !" )
        pass
    pass
# write the background word into EventInfo (Jamie Boyd)
# need to go here for ordering reasons...
if rec.doESD() and not rec.readESD():
    try:
        protectedInclude ("RecBackgroundAlgs/RecBackground_jobOptions.py")
    except Exception:
        treatException("Problem including RecBackgroundAlgs/RecBackground_jobOptions.py !!")
        pass
    pass


#
# MonteCarloReact AOD->AOD corrections (in fact these objects are in ESD also)
#

if recAlgs.doMonteCarloReact():
    protectedInclude ("MonteCarloReactTools/MonteCarloReact_for_RecExCommon.py")

# run monitoring
# ----------------------------------------------------------------------------
# Monitoring Algorithms and Tools
# ----------------------------------------------------------------------------

if rec.doMonitoring():
    protectedInclude ("AthenaMonitoring/DataQualitySteering_jobOptions.py")



# run"Fast Phsyics Monitoring"
if rec.doFastPhysMonitoring():
    protectedInclude("FastPhysMonExample/FastPhysicsMonitoring_jobOptions.py")

# ----------------------------------------------------------------------------

# kind of hack. Note that ByteStreamAddressProviderSvc.TypeNames is continuously updated, also in mon configuration
if globalflags.InputFormat.is_bytestream() and disableRPC:
   newList=[]
   for i in svcMgr.ByteStreamAddressProviderSvc.TypeNames:
      if i.startswith("Rpc"):
         print "removing from ByteStreamAddressProviderSvc ",i
      else:
         newList+=[i]

   svcMgr.ByteStreamAddressProviderSvc.TypeNames=newList
   print svcMgr.ByteStreamAddressProviderSvc.TypeNames

# do it now, because monitoring is actually adding stuff to ByteStreamAddressProvider
if globalflags.InputFormat.is_bytestream():
    logRecExCommon_topOptions.info("BS file : store again in objKeyStore the list of input object from svcMgr.ByteStreamAddressProviderSvc.TypeNames. Unsafe ! ")
    newTN=[]
    for a in svcMgr.ByteStreamAddressProviderSvc.TypeNames:
        newTN+=[a.replace("/","#")]
        objKeyStore.addManyTypesInputFile(newTN)


################################################################"


# CBNT_Athena.Members specify the ntuple block corresponding to a given ntuple
# Comment CBNT_Athena.Members line to remove a ntuple block
# It is also possible to disable a ntuple block by
#       disabling the corresponding
#       CBNT_Athena.Members with the line: <Member>.Enable = False
# The ntuple specification is in file CBNT_jobOptions.py
# and can be modified by adding properties below
#
# ----- CBNT_Athena algorithm
#

#pdr.flag_domain('aod')
if rec.doAOD():
    try:
        include( "ParticleBuilderOptions/AOD_Builder_jobOptions.py")
    except Exception:
        treatException("Could not load AOD_Builder_joboptions. Switched off !" )
        rec.doAOD=False

#
# possible user code and property modifiers should come here
# could be several algorithms, or python lines

allAlgs=rec.UserAlgs()
if type(allAlgs)!=type([]):
    allAlgs=[allAlgs]

for UserAlg in allAlgs:
    include (UserAlg)
del allAlgs

if len(rec.UserExecs())>0:
    from PyJobTransformsCore.trfutil import StringToExec
    allExecs=StringToExec(rec.UserExecs())
    for uExec in allExecs:
        exec(uExec)
    del allExecs

if (rec.doRecoTiming() and rec.OutputFileNameForRecoStep() in ('RAWtoESD','ESDtoAOD','RAWtoALL')):
    topSequence+=TimingAlg("RecoTimerBeforeOutput")
    topSequence.RecoTimerBeforeOutput.TimingObjOutputName=rec.OutputFileNameForRecoStep()+"_timings"
    try:
        topSequence+=MemoryAlg("RecoMemoryBeforeOutput")
        topSequence.RecoMemoryBeforeOutput.MemoryObjOutputName=rec.OutputFileNameForRecoStep()+"_mems"
    except Exception:
        logRecExCommon_topOptions.info("Could not instantiate MemoryAlg")

    from RecExConfig.ObjKeyStore import objKeyStore
    objKeyStore.addStreamESD("RecoTimingObj",rec.OutputFileNameForRecoStep()+"_timings" )
    objKeyStore.addStreamAOD("RecoTimingObj",rec.OutputFileNameForRecoStep()+"_timings" )
    objKeyStore.addStreamESD("RecoTimingObj",rec.OutputFileNameForRecoStep()+"_mems" )
    objKeyStore.addStreamAOD("RecoTimingObj",rec.OutputFileNameForRecoStep()+"_mems" )

    # transfer RecoTimingObj from previous steps from RDO to ESD.
    # The transfer from ESD to AOD is done already in OKS_StreamESD_fromESD.py
    if ( rec.OutputFileNameForRecoStep() == 'RAWtoESD' and
         globalflags.DataSource()=='geant4' ):
        objKeyStore.addStreamESD("RecoTimingObj","EVNTtoHITS_timings" )
        objKeyStore.addStreamESD("RecoTimingObj","HITStoRDO_timings" )


import os
atlasversionstr=""
if os.getenv("AtlasVersion") != None:
    atlasversionstr=os.getenv("AtlasVersion")

from RecAlgs.RecAlgsConf import JobInfo
#topSequence+=JobInfo(PrintFATAL=atlasversionstr.startswith("rel_"))
topSequence+=JobInfo(PrintFATAL=False)
del atlasversionstr

if rec.doPhysValMonHists():
    include("PhysValMon/PhysValMon_RecoOpt.py")

pdr.flag_domain('output')
if rec.doCBNT():
    protectedInclude( "RecExCommon/CBNT_config.py" )

# trigger extension to the TrkValNtuple (it needs to be included after TrkValNtuple) from Jiri Masik and Clemencia Mora
# test if ID is on also (might not be sufficient)
if recAlgs.doTrigger() and DetFlags.detdescr.ID_on() :
    if globalflags.DataSource() == 'data'and globalflags.InputFormat == 'bytestream':
        from InDetRecExample.InDetJobProperties import InDetFlags
        from InDetTrigRecExample.InDetTrigFlags import InDetTrigFlags
        if InDetFlags.doTrkNtuple() and InDetTrigFlags.doTrkNtuple():
            protectedInclude("InDetTrigRecExample/InDetTrigRecNtupleCreation.py")

#-----------------------------------------------------------------------------
# Virtual Point1 Display
#-----------------------------------------------------------------------------
if rec.doVP1():
  from VP1Algs.VP1AlgsConf import VP1Alg
  topSequence += VP1Alg()
  topSequence.TimeOut = 0


#-----------------------------------------------------------------------------
# Atlantis xml
#-----------------------------------------------------------------------------

if rec.doJiveXML():
    protectedInclude ( "JiveXML/JiveXML_RecEx_config.py")


#-----------------------------------------------------------------------------
# PERSINT graphics for muons + calorimeters
#-----------------------------------------------------------------------------
if rec.doPersint()  :
    if rec.doMuon():
        from MboyView.ConfiguredMboyView import theMboyView





###################################################################
#
# functionality : monitor memory and cpu time
#


#
# functionality : build combined ntuple,
# gathering info from all the reco algorithms
#




# check dictionary all the time
ServiceMgr.AthenaSealSvc.CheckDictionary = True
if not rec.doCheckDictionary():
    ServiceMgr.AthenaSealSvc.OutputLevel=WARNING



#
#
#now write out Transient Event Store content in POOL
#
#
#
if rec.doWriteESD() or rec.doWriteAOD() or rec.doWriteRDO() or rec.doWriteTAG():
    import AthenaPoolCnvSvc.WriteAthenaPool


# Must make sure that no OutStream's have been declared
#FIXME
theApp.OutStream = []

if rec.doWriteTAG():
    # the TAG making algorithm
    try:
        include( "EventTagAlgs/EventTag_jobOptions.py" )
    except Exception:
        rec.doWriteTAG=False
        treatException("Could not include EventTagAlgs/EventTag_jobOptions.py. Disable TAG writing")
else: # minimal TAG to be written into AOD
    print "Using EventInfoAttList"

if rec.doWriteRDO():
    #Create output StreamRDO
    from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
    StreamRDO_Augmented=MSMgr.NewPoolStream("StreamRDO",athenaCommonFlags.PoolRDOOutput(),asAlg=True)

    #topSequence+=AthenaOutputStream("StreamRDO",
                                 #>=12.3.0 StreamAOD.EvtConversionSvc = "AthenaPoolCnvSvc"
    #                             WritingTool = "AthenaPoolOutputStreamTool",
                                 #force read of input data to write in output data
    #                             ForceRead = True,
                                 # Define the output file name
    #                             OutputFile    = athenaCommonFlags.PoolRDOOutput())

    if rec.doFileMetaData():
        StreamRDO_Augmented.AddMetaDataItem(recoMetadataItemList())


    if rec.readRDO():
        if globalflags.InputFormat()=='pool':
            # copy does not point to input
            StreamRDO_Augmented.GetEventStream().ExtendProvenanceRecord = False
            # all input to be copied to output
            StreamRDO_Augmented.GetEventStream().TakeItemsFromInput=True
        else:
            # reading BS , so need explicit list of item
            # should eventually be shared with Digitization
            # Only include non-truth

            StreamRDO_Augmented.AddItem(["LArRawChannelContainer#*"])

            StreamRDO_Augmented.AddItem(["TileRawChannelContainer#*"])
            StreamRDO_Augmented.AddItem(["EventInfo#*"])
            StreamRDO_Augmented.AddItem(["PileUpEventInfo#*"])
            StreamRDO_Augmented.AddItem(["PixelRDO_Container#*"])
            StreamRDO_Augmented.AddItem(["SCT_RDO_Container#*"])
            StreamRDO_Augmented.AddItem(["TRT_RDO_Container#*"])
            StreamRDO_Augmented.AddItem(["ROIB::RoIBResult#*", "MuCTPI_RDO#*","CTP_RDO#*","LArTTL1Container#*","TileTTL1Container#*"])
            StreamRDO_Augmented.AddItem(["CscDigitContainer#*"])
            StreamRDO_Augmented.AddItem(["MdtDigitContainer#*"])
            StreamRDO_Augmented.AddItem(["RpcDigitContainer#*"])
            StreamRDO_Augmented.AddItem(["TgcDigitContainer#*"])
            StreamRDO_Augmented.AddItem(["CscRawDataContainer#*"])
            StreamRDO_Augmented.AddItem(["MdtCsmContainer#*"])
            StreamRDO_Augmented.AddItem(["RpcPadContainer#*"])
            StreamRDO_Augmented.AddItem(["TgcRdoContainer#*"])
            StreamRDO_Augmented.AddItem(["CscRawDataContainer#CSC_Hits"])
            StreamRDO_Augmented.AddItem(["MdtCsmContainer#MDT_Hits"])
            StreamRDO_Augmented.AddItem(["RpcPadContainer#RPC_Hits"])
            StreamRDO_Augmented.AddItem(["TgcRdoContainer#TGC_Hits"])


    ## This line provides the 'old' StreamRDO (which is the Event Stream only)
    ## for backward compatibility
    StreamRDO=StreamRDO_Augmented.GetEventStream()

    ## Add TAG attribute list to payload data
    try:
        StreamRDO_Augmented.GetEventStream().WritingTool.AttributeListKey = EventTagGlobal.AttributeList
    except:
        logRecExCommon_topOptions.warning("Failed to add TAG attribute list to payload data")

if globalflags.InputFormat()=='bytestream':
    # FIXME : metadata store definition is in ReadAthenaPool_jobOptions.py
    # copy it there for BS Reading for 13..0.X
    # Add in MetaDataSvc
    from EventSelectorAthenaPool.EventSelectorAthenaPoolConf import MetaDataSvc
    svcMgr += MetaDataSvc( "MetaDataSvc" )
    # Add in MetaData Stores
    from StoreGate.StoreGateConf import StoreGateSvc
    svcMgr += StoreGateSvc( "MetaDataStore" )
    svcMgr += StoreGateSvc( "InputMetaDataStore" )

    # Make MetaDataSvc an AddressProvider
    svcMgr.ProxyProviderSvc.ProviderNames += [ "MetaDataSvc" ]

    # enable IOVDbSvc to read metadata
    svcMgr.MetaDataSvc.MetaDataContainer = "MetaDataHdr"
    if not hasattr (svcMgr.ToolSvc, 'IOVDbMetaDataTool'):
        svcMgr.MetaDataSvc.MetaDataTools += [ "IOVDbMetaDataTool" ]

MetaDataStore=svcMgr.MetaDataStore


#Lumiblocks and EventBookkeepers
if rec.doFileMetaData():

    #lumiblocks
    if rec.readESD() or rec.readAOD():
        # Lumi counting tool ... but not if in athenaMP ...
        from AthenaCommon.ConcurrencyFlags import jobproperties as jps
        if jps.ConcurrencyFlags.NumProcs>=1 or jps.ConcurrencyFlags.NumProcs==-1:
            #athenaMP ... use the lumiblock making algorithm for now. This will be useless after skimming has occured though!
            include ("LumiBlockComps/CreateLumiBlockFromFile_jobOptions.py")
        else:
            #ok to use the metadata tool if single process
            from LumiBlockComps.LumiBlockCompsConf import LumiBlockMetaDataTool
            svcMgr.MetaDataSvc.MetaDataTools += [ "LumiBlockMetaDataTool" ]
        # Trigger tool
        ToolSvc += CfgMgr.xAODMaker__TriggerMenuMetaDataTool( "TriggerMenuMetaDataTool" )
        svcMgr.MetaDataSvc.MetaDataTools += [ ToolSvc.TriggerMenuMetaDataTool ]
        # EventFormat tool
        ToolSvc += CfgMgr.xAODMaker__EventFormatMetaDataTool( "EventFormatMetaDataTool" )
        svcMgr.MetaDataSvc.MetaDataTools += [ ToolSvc.EventFormatMetaDataTool ]

    else:
        # Create LumiBlock meta data containers *before* creating the output StreamESD/AOD
        include ("LumiBlockComps/CreateLumiBlockFromFile_jobOptions.py")
        pass

    # Add the needed stuff for cut-flow bookkeeping.
    # Only the configurables that are not already present will be created
    from EventBookkeeperTools.CutFlowHelpers import CreateCutFlowSvc
    logRecExCommon_topOptions.debug("Going to call CreateCutFlowSvc")
    CreateCutFlowSvc( svcName="CutFlowSvc", athFile=af, seq=topSequence, addMetaDataToAllOutputFiles=True ) 
    if rec.readAOD() or rec.readESD():
        #force CutFlowSvc execution (necessary for file merging)
        theApp.CreateSvc+=['CutFlowSvc']
        logRecExCommon_topOptions.debug("Added CutFlowSvc to theApp")
        pass    

    try:
        # ByteStreamMetadata
        from ByteStreamCnvSvc.ByteStreamCnvSvcConf import ByteStreamMetadataTool        
        if not hasattr (svcMgr.ToolSvc, 'ByteStreamMetadataTool'):
            svcMgr.MetaDataSvc.MetaDataTools += [ "ByteStreamMetadataTool" ]
    except Exception:
        treatException("Could not load ByteStreamMetadataTool")


    pass

##--------------------------------------------------------
###=== Only run reco on events that pass selected triggers
##--------------------------------------------------------
if rec.doTrigger and rec.doTriggerFilter() and globalflags.DataSource() == 'data' and globalflags.InputFormat == 'bytestream':
    try:
### seq will be our filter sequence
        from AthenaCommon.AlgSequence import AthSequencer
        seq=AthSequencer("AthFilterSeq")
        seq+=CfgMgr.EventCounterAlg("AllExecutedEventsAthFilterSeq")
        seq+=topSequence.TrigConfDataIOVChanger
        seq+=topSequence.RoIBResultToAOD
        seq+=topSequence.TrigBSExtraction
        seq+=topSequence.TrigDecMaker

        from GoodRunsListsUser.GoodRunsListsUserConf import TriggerSelectorAlg
        seq += TriggerSelectorAlg('TriggerAlg1')
        seq.TriggerAlg1.TriggerSelection = rec.triggerFilterList()
        pass
    except:
        pass
    pass
##--------------------------------------------------------


if rec.doWriteESD():
    if rec.doWriteRDO():
        # mark the RDO DataHeader as the input DataHeader

        from OutputStreamAthenaPool.OutputStreamAthenaPoolConf import MakeInputDataHeader
        topSequence+=MakeInputDataHeader("MakeInputDataHeaderRDO",
                                         StreamName="StreamRDO")
        pass

    #Create output StreamESD
    streamESDName="StreamESD"
    sdwContainerName=''
    #special option for ESD/dESD merging
    if rec.readESD() and rec.doWriteESD() and rec.doESD()==False and rec.mergingStreamName()!="":
        streamESDName=rec.mergingStreamName()
        pass

    ## compute ESD item list (in CILMergedESD )
    protectedInclude ( "RecExPers/RecoOutputESDList_jobOptions.py" )


    #special option for DPD in pass through mode (only during RDO->ESD)
    if rec.doDPD.ESDDESDEventTagging and rec.readRDO() and rec.doWriteESD() and jobproperties.Beam.beamType()=="collisions" :
        #This cannot be with the rest of the DPD stuff below as long as StreamESD is an algorithm)
        #Execute all filters and explicitely write their skim decisions, cannot be done by MSMgr again because of asAlg=True
        #Do NOT explicitely write their EventBookkeepers.
        try:
            # dpd filtering can only run if trigger and all detector geo enabled
            if not (rec.doTrigger and rec.doCalo and rec.doInDet and rec.doMuon):
                raise Exception
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

            # Now, get the needed flags and lists and set the output DPD streams to virual, i.e., no output file written
            from PrimaryDPDMaker.PrimaryDPDFlags import desdEventSkimmingFilterNamesList,primDPD
            primDPD.isVirtual = True
            # Include all the needed DESD definition files here such that the filter algorithms get scheduled
            #include("PrimaryDPDMaker/DESD_GOODTIME.py")  # Not produced any more in 2012
            include("PrimaryDPDMaker/PerfDPD_EGamma.py")
            include("PrimaryDPDMaker/PerfDPD_Jet.py")
            #include("PrimaryDPDMaker/PerfDPD_LargeMet.py") # Not produced any more in 2012
            include("PrimaryDPDMaker/PerfDPD_MinBias.py")
            include("PrimaryDPDMaker/PerfDPD_PhotonJet.py")
            include("PrimaryDPDMaker/PerfDPD_SingleElectron.py")
            include("PrimaryDPDMaker/PerfDPD_SingleMuon.py")
            include("PrimaryDPDMaker/PerfDPD_Tracking.py")

            # Now, do the bookkeeping for each event
            from EventBookkeeperTools.BookkeepingInfoWriter import SkimDecisionsWriter
            sdw = SkimDecisionsWriter( streamESDName + "_SkimDecisionsWriter" )
            sdwContainerName = streamESDName + "_" + sdw.SkimDecisionsContainerName
            sdw.SkimDecisionsContainerName = sdwContainerName
            logRecExCommon_topOptions.info("Adding the following list of algorithms to the %s: %s" % ( sdwContainerName,
                                                                                                       desdEventSkimmingFilterNamesList ) )
            for a in desdEventSkimmingFilterNamesList:
                sdw.addOtherAlg(a)
                pass
            topSequence += sdw
            logRecExCommon_topOptions.info("Running Primary DESD fitlers in pass-through mode.")
            rec.doDPD.passThroughMode = True
        except Exception:
            rec.doDPD.passThroughMode = False
            rec.DPDMakerScripts = [] # Clear all DPDMakerSkripts, just in case one or more got appended, but then en error occured
            # if the import worked, there was another error we caught
            if not "AtlasAnalysis" in os.getenv("CMTPATH"):
                logRecExCommon_topOptions.warning("It seems, you have setup only AtlasReconstruction or AtlasTrigger/AtlasHLT project !")
                logRecExCommon_topOptions.warning("Bookkeeping of DESD event skimming setup failed. DESD pass-though mode disabled.")
            elif not (rec.doTrigger and rec.doCalo and rec.doInDet and rec.doMuon):
                logRecExCommon_topOptions.warning("trigger or one detector switch off. Not able setup Bookkeeping of DESD skimming. DESD pass-though mode disabled.")

            else:
                logRecExCommon_topOptions.error("Bookkeeping of DESD event skimming setup failed. DESD pass-though mode disabled.")
                pass
            pass
        pass

    from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
    StreamESD_Augmented=MSMgr.NewPoolStream(streamESDName,athenaCommonFlags.PoolESDOutput(),asAlg=True)

    if rec.doFileMetaData():
        StreamESD_Augmented.AddMetaDataItem(recoMetadataItemList())
        StreamESD_Augmented.AddMetaDataItem( objKeyStore._store.metaData() )
        pass

    ## This line provides the 'old' StreamESD (which is the Event Stream only)
    ## for backward compatibility
    StreamESD=StreamESD_Augmented.GetEventStream()

    ## Add TAG attribute list to payload data
    try:
        StreamESD_Augmented.GetEventStream().WritingTool.AttributeListKey = EventTagGlobal.AttributeList
    except:
        logRecExCommon_topOptions.warning("Failed to add TAG attribute list to payload data")

    ## now done earlier
    ## protectedInclude ( "RecExPers/RecoOutputESDList_jobOptions.py" )
    # really add item to StreamESD. CILMergeESd was computed by RecoOutputESDList_jobOptions.py" )
    StreamESD.ItemList = CILMergeESD()

    # consistency check : make sure oks streamESD==CILMergeESD and included in transient

    # consistency check : make sure oks streamESD==CILMergeESD and included in transient
    #FIXME many problem. #* to remove, datavector to be removed plus basic thing missing
    print "DRDR now consistency checks with three list"
    streamesd=objKeyStore['streamESD']()
    transient=objKeyStore['transient']()
    mergeesd=CILMergeESD()
    # and not item.endswith("#*")
    for item in mergeesd:
        if not item.endswith("#*"):
            if not item in transient:
                logRecExCommon_topOptions.warning("item %s in CILMergeESD but not in objKeyStore['transient']" % item)
            if not item in streamesd:
                logRecExCommon_topOptions.warning("item %s in CILMergeESD but not in objKeyStore['streamESD']" % item)

    for item in streamesd:
        #sthg fishy with LArCLusterEM to sort out
        if not item.endswith("#*") and not item=="DataVector<INavigable4Momentum>#LArClusterEM":
            if not item in transient:
                logRecExCommon_topOptions.warning("item %s in streamesd but not in objKeyStore['transient']" % item)
            if not item in mergeesd:
                logRecExCommon_topOptions.warning("item %s in objKeyStore['streamESD'] but not in  CILMergeESD" % item)

    del streamesd,transient,mergeesd



    if sdwContainerName:
        StreamESD_Augmented.AddItem("SkimDecisionCollection#" + sdwContainerName)
        pass
    # print one line for each object to be written out
    #StreamESD.OutputLevel = DEBUG


    # this is ESD->ESD copy
    if rec.readESD():
        # copy does not point to input
        StreamESD_Augmented.GetEventStream().ExtendProvenanceRecord = False
        # all input to be copied to output
        StreamESD_Augmented.GetEventStream().TakeItemsFromInput=True

        # FIXME in ESD jet points to non existent cluster, hide WARNING
        ServiceMgr.MessageSvc.setError +=  [ "SG::ELVRef"]

        pass
    pass

if rec.doESD() or rec.doWriteESD():
    #
    # Heavy ion reconstruction  special configuration
    #
    if rec.doHeavyIon():
        protectedInclude ("HIRecExample/heavyion_postOptionsESD.py")


#########
## DPD ##
#########
if rec.doDPD() and (rec.DPDMakerScripts()!=[] or rec.doDPD.passThroughMode):
    from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
    from PrimaryDPDMaker.PrimaryDPDFlags import primDPD

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
    if rec.doAODSelect():
        try:
            include("AODSelect/AODSelect_setupOptions.py")
        except Exception:
            treatException("Could not load AODSelect/AODSelect_setupOptions.py !")
            rec.doAODSelect = False
            pass
        pass

    #This block may not be needed... something to check if somebody has time!
    if rec.DPDMakerScripts()!=[]:
        if globalflags.InputFormat()=='pool':
            include("PyAnalysisCore/InitPyAnalysisCore.py")
            pass
        pass

    #Then include all requested DPD makers
    logRecExCommon_topOptions.info( "Content of rec.DPDMakerSkripts = %s", rec.DPDMakerScripts() )
    for DPDMaker in rec.DPDMakerScripts():
        DPDMakerName = str(DPDMaker)
        logRecExCommon_topOptions.info( "Including %s...",DPDMakerName )
        include(DPDMaker)
        pass

    # Schedule the AODSelect algorithms
    if rec.doAODSelect():
        try:
            include("AODSelect/AODSelect_mainOptions.py")
        except Exception:
            treatException("Could not load AODSelect/AODSelect_mainOptions.py !")
            rec.doAODSelect = False
            pass
        pass

    #SkimDecision objects may once migrate to CutFlowSvc or DecisionSvc, but not yet
    logRecExCommon_topOptions.info( "primDPD.WriteSkimDecisions =  %s", primDPD.WriteSkimDecisions() )
    if primDPD.WriteSkimDecisions():
        MSMgr.WriteSkimDecisionsOfAllStreams()
        pass

    #Configure CutFlowSv and common metadata
    if rec.doFileMetaData():

        #Exception for DPD pass-through mode
        if rec.doDPD.passThroughMode:
            svcMgr.CutFlowSvc.SkimmingCycle=0
            svcMgr.CutFlowSvc.InputStream="Virtual"
            pass

        if rec.DPDMakerScripts()!=[] and not rec.doDPD.passThroughMode :
            # Add the needed stuff for cut-flow bookkeeping.
            # Only the configurables that are not already present will be created
            from EventBookkeeperTools.CutFlowHelpers import CreateCutFlowSvc
            logRecExCommon_topOptions.debug("Calling CreateCutFlowSvc")
            CreateCutFlowSvc( svcName="CutFlowSvc", athFile=af, seq=topSequence, addMetaDataToAllOutputFiles=True )

            from RecExConfig.InputFilePeeker import inputFileSummary
            #Explicitely add file metadata from input and from transient store
            MSMgr.AddMetaDataItemToAllStreams(inputFileSummary['metadata_itemsList'])
            MSMgr.AddMetaDataItemToAllStreams(dfMetadataItemList())
            pass
        pass
    pass


if rec.doWriteAOD():

    # build list of cells from clusters
    # (since cluster element link are reset to the new list this must
    # be done after StreamESD )

    if rec.doWriteESD():
        # mark the ESD DataHeader as the input DataHeader
        from OutputStreamAthenaPool.OutputStreamAthenaPoolConf import MakeInputDataHeader
        topSequence+=MakeInputDataHeader("MakeInputDataHeaderESD",
                                         StreamName="StreamESD")

if ( rec.doAOD() or rec.doWriteAOD()) and not rec.readAOD() :
    # special slimmed cell in AOD
    if DetFlags.detdescr.Calo_on() and rec.doAODCaloCells():
        try:
            from CaloRec.CaloCellAODGetter import addClusterToCaloCellAOD
            if  rec.readESD() or jobproperties.CaloRecFlags.doEmCluster() :
                addClusterToCaloCellAOD("LArClusterEM7_11Nocorr")

            from egammaRec.egammaRecFlags import jobproperties
            if ( rec.readESD() or jobproperties.egammaRecFlags.Enabled ) and not rec.ScopingLevel()==4  :
                from egammaRec import egammaKeys
                addClusterToCaloCellAOD(egammaKeys.outputClusterKey())

            from MuonCombinedRecExample.MuonCombinedRecFlags import muonCombinedRecFlags
            if rec.readESD() or muonCombinedRecFlags.doMuonClusters():
                addClusterToCaloCellAOD("MuonClusterCollection")

            if rec.readESD() or recAlgs.doTrackParticleCellAssociation():
                addClusterToCaloCellAOD("InDetTrackParticlesAssociatedClusters")

        except Exception:
            treatException("Could not make AOD cells" )


pdr.flag_domain('aod')

if rec.doWriteAOD():
    from ParticleBuilderOptions.AODFlags import AODFlags
    # Particle Builders
    from AthenaCommon.AppMgr import ServiceMgr as svcMgr
    from AthenaServices.Configurables import ThinningSvc
    if not hasattr(svcMgr, 'ThinningSvc'):
       svcMgr += ThinningSvc(OutputLevel=INFO)
    svcMgr.ThinningSvc.Streams += ['StreamAOD']


    # cannot redo the slimming if readAOD and writeAOD
    if not rec.readAOD() and (rec.doESD() or rec.readESD()):
        if rec.doEgamma() and (AODFlags.Photon or AODFlags.Electron):
            doEgammaPhoton = AODFlags.Photon
            doEgammaElectron= AODFlags.Electron
            from egammaRec.egammaAODGetter import egammaAODGetter
            egammaAODGetter()
            if AODFlags.egammaTrackSlimmer:
                from egammaRec.egammaTrackSlimmer import egammaTrackSlimmer
                egammaTrackSlimmer()

        if AODFlags.TauTrackSlimmer:
            protectedInclude("tauRec/tauMerged_trackslim_jobOptions.py")

        if rec.doTruth() and AODFlags.ThinGeantTruth:
            from ThinningUtils.ThinGeantTruth import ThinGeantTruth
            ThinGeantTruth()

        if AODFlags.ThinNegativeEnergyCaloClusters:
            from ThinningUtils.ThinNegativeEnergyCaloClusters import ThinNegativeEnergyCaloClusters
            ThinNegativeEnergyCaloClusters()            
        if AODFlags.ThinNegativeEnergyNeutralPFOs:
            from ThinningUtils.ThinNegativeEnergyNeutralPFOs import ThinNegativeEnergyNeutralPFOs
            ThinNegativeEnergyNeutralPFOs()

        #Thin Trk::Tracks for Electons and Muons (GSF/Combined)
        if  (AODFlags.AddEgammaMuonTracksInAOD and not rec.doTruth()) or (AODFlags.AddEgammaTracksInMCAOD and rec.doTruth()): 
            from ThinningUtils.ThinTrkTrack import ThinTrkTrack
            ThinTrkTrack()
            
       # Doens't exist in xAOD world:
       # if AODFlags.TrackParticleSlimmer or AODFlags.TrackParticleLastHitAndPerigeeSlimmer:
       #     from PrimaryDPDMaker.PrimaryDPDMakerConf import SlimTrackInfo
       #     topSequence += SlimTrackInfo( "SlimTrackParticles",
       #                                   thinSvc             = 'ThinningSvc/ThinningSvc',
       #                                   TrackPartContName   = 'TrackParticleCandidate',
       #                                   SlimPerigee=AODFlags.TrackParticleLastHitAndPerigeeSlimmer() )

    pdr.flag_domain('output')
    # Create output StreamAOD
    streamAODName="StreamAOD"
    #special option for AOD/dAOD merging
    if rec.readAOD() and rec.doWriteAOD() and rec.doAOD()==False and rec.mergingStreamName()!="":
        streamAODName=rec.mergingStreamName()

    from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
    StreamAOD_Augmented=MSMgr.NewPoolStream(streamAODName,athenaCommonFlags.PoolAODOutput(),asAlg=True)

    if rec.doFileMetaData():
        # Trigger tool
        ToolSvc += CfgMgr.xAODMaker__TriggerMenuMetaDataTool( "TriggerMenuMetaDataTool")

        svcMgr.MetaDataSvc.MetaDataTools += [ ToolSvc.TriggerMenuMetaDataTool ]
        # EventFormat tool
        ToolSvc += CfgMgr.xAODMaker__EventFormatMetaDataTool( "EventFormatMetaDataTool")

        svcMgr.MetaDataSvc.MetaDataTools += [ ToolSvc.EventFormatMetaDataTool ]
        # Put MetaData in AOD stream via AugmentedPoolStream_
        # Write all meta data containers
        StreamAOD_Augmented.AddMetaDataItem(dfMetadataItemList())
        # Metadata declared by the sub-systems:
        StreamAOD_Augmented.AddMetaDataItem( objKeyStore._store.metaData() )
        pass
        
    ## This line provides the 'old' StreamAOD (which is the Event Stream only)
    ## for backward compatibility
    StreamAOD=StreamAOD_Augmented.GetEventStream()

    ## Add TAG attribute list to payload data
    from EventTagAlgs.EventTagGlobal import EventTagGlobal
    try:
        StreamAOD.WritingTool.AttributeListKey = EventTagGlobal.AttributeList
    except:
        logRecExCommon_topOptions.warning("Failed to add TAG attribute list to payload data")

    # protectedInclude( "ParticleBuilderOptions/AOD_OutputList_jobOptions.py")
    protectedInclude( "RecExPers/RecoOutputAODList_jobOptions.py")
    StreamAOD_Augmented.AddItem("SkimDecisionCollection#*")
    #FIXME HACK remove faulty object
    StreamAOD_Augmented.GetEventStream().ItemList = [ e for e in StreamAOD_Augmented.GetEventStream().ItemList if not e in [ 'CaloTowerContainer#HLT_TrigCaloTowerMaker'] ]

    if AODFlags.TrackParticleSlimmer or AODFlags.TrackParticleLastHitAndPerigeeSlimmer:
        from AthenaCommon.AppMgr import ServiceMgr as svcMgr
        from AthenaServices.Configurables import ThinningSvc, createThinningSvc
        if not hasattr(svcMgr, 'ThinningSvc'):
            svcMgr += createThinningSvc( svcName="ThinningSvc", outStreams=[StreamAOD] )

    # this is AOD->AOD copy
    if rec.readAOD():
        # copy does not point to input
        StreamAOD_Augmented.GetEventStream().ExtendProvenanceRecord = False
        # all input to be copied to output
        StreamAOD_Augmented.GetEventStream().TakeItemsFromInput =True

    # print one line for each object to be written out
    #StreamAOD.OutputLevel=DEBUG

    # StreamAOD_Augmented.AddItem( "RecoTimingObj#RAWtoESD_timings" )
    # StreamAOD_Augmented.AddItem( "RecoTimingObj#ESDtoAOD_timings" )
    # StreamAOD_Augmented.AddItem( "RecoTimingObj#ESDtoAOD_mems" )

    # Add the Thinned Trk Tracks associated to egamma and muon objects in data
    if  AODFlags.AddEgammaMuonTracksInAOD and not rec.doTruth():
        StreamAOD_Augmented.AddItem("TrackCollection#GSFTracks")
        StreamAOD_Augmented.AddItem("TrackCollection#CombinedMuonTracks")

    # Add the Thinned Trk Tracks associated to egamma objects in MC
    elif  AODFlags.AddEgammaTracksInMCAOD and rec.doTruth():
        StreamAOD_Augmented.AddItem("TrackCollection#GSFTracks")


if rec.doAOD() or rec.doWriteAOD():
    #
    # Heavy ion reconstruction  special configuration
    #
    if rec.doHeavyIon():
        protectedInclude ("HIRecExample/heavyion_postOptionsAOD.py")


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

try:
  # event dumper at the very end
  if rec.doPyDump():
      from RecExConfig.RecoFunctions import OutputFileName
      OutFileName=OutputFileName(rec.OutputSuffix())

      from PyDumper.PyComps import PySgDumper
      topSequence += PySgDumper(ofile='PyDump_'+OutFileName+'.txt')

      # preliminary
      topSequence += CfgMgr.INav4MomDumper(INav4Moms    = ['CaloMuonCollection'],OutputStream = "inav4moms_"+OutFileName+'.txt',OutputLevel  = INFO   )
except Exception:
     # protect until doPyDump is defined in all releases
     treatException("problem with the dumpers")

#Trigger ntuple (from Clemencia Mora). Not if not rec.doTrigger
if rec.readESD() and rec.doTrigger() and  TriggerFlags.NtupleProductionFlags.produceNtuples():
    #old Tier0TriggerFlags.doTier0TriggerNtuple():
    protectedInclude("TriggerRelease/TriggerNTupleProduction.py")

if rec.doCBNT():
    # AANTupleStream should be at the very end
    topSequence+=theAANTupleStream


# detailed auditor of time to read/write pool object
if hasattr(ServiceMgr, 'AthenaPoolCnvSvc'):
    ServiceMgr.AthenaPoolCnvSvc.UseDetailChronoStat = True


# BS writing (T Bold)
if rec.doWriteBS():

    #Persistent BS construction and intialization
    from ByteStreamCnvSvc import WriteByteStream
    StreamBSFileOutput = WriteByteStream.getStream("EventStorage","StreamBSFileOutput")

    ServiceMgr.ByteStreamCnvSvc.IsSimulation = True

    # BS content definition
    # commented out since it was causing duplicates
    #if hasattr( topSequence, "StreamBS") and recAlgs.doTrigger() :
    #    StreamBSFileOutput.ItemList += topSequence.StreamBS.ItemList

    # LVL1
    StreamBSFileOutput.ItemList += [ "ROIB::RoIBResult#*" ]

    StreamBSFileOutput.ItemList += [ "DataVector<LVL1::TriggerTower>#TriggerTowers" ]
    StreamBSFileOutput.ItemList += [ "LVL1::JEPBSCollection#JEPBSCollection" ]
    StreamBSFileOutput.ItemList += [ "LVL1::JEPRoIBSCollection#JEPRoIBSCollection" ]
    StreamBSFileOutput.ItemList += [ "LVL1::CPBSCollection#CPBSCollection" ]
    StreamBSFileOutput.ItemList += [ "DataVector<LVL1::CPMRoI>#CPMRoIs" ]

    # LVL2
    StreamBSFileOutput.ItemList   += [ "HLT::HLTResult#HLTResult_L2" ]
    StreamBSFileOutput.ItemList   += [ "HLT::HLTResult#HLTResult_EF" ]

    StreamBSFileOutput.ItemList += [ "TRT_RDO_Container#TRT_RDOs" ]
    StreamBSFileOutput.ItemList += [ "SCT_RDO_Container#SCT_RDOs" ]
    StreamBSFileOutput.ItemList += [ "PixelRDO_Container#PixelRDOs" ]

    # CTP from attila
    StreamBSFileOutput.ItemList += [ "CTP_RDO#*", "MuCTPI_RDO#*" ]


    #Lucid
    StreamBSFileOutput.ItemList += ["LUCID_DigitContainer#Lucid_Digits"]


    # special SCT CABLING (ONLY FOR OLD FDR DATA)
    #from InDetCabling.InDetCablingConf import SCT_CablingSelector
    #SCT_CablingSelector = SCT_CablingSelector(Method = "MANUAL", Layout = "FromTextFile", Filename = "SCT_MC_FullCabling.dat")
    #ToolSvc            += SCT_CablingSelector



    # LAr
    #        StreamBS.ItemList +=["LArRawChannels#*"]
    StreamBSFileOutput.ItemList +=["2721#*"]

    # GU LAr bytestream writing now using calonoisetool, make sure it is configured
    from CaloTools.CaloNoiseToolDefault import CaloNoiseToolDefault
    theNoiseTool = CaloNoiseToolDefault()
    ToolSvc += theNoiseTool

    from LArByteStream.LArByteStreamConfig import LArRawDataContByteStreamToolConfig
    ToolSvc+=LArRawDataContByteStreamToolConfig(InitializeForWriting=True)

    # Tile
    #        StreamBS.ItemList +=["TileRawChannelCnt#*"]
    StreamBSFileOutput.ItemList +=["2927#*"]
    StreamBSFileOutput.ItemList +=["2934#*"]


    # Muon
    StreamBSFileOutput.ItemList +=["MdtCsmContainer#*"]
    StreamBSFileOutput.ItemList +=["RpcPadContainer#*"]
    StreamBSFileOutput.ItemList +=["TgcRdoContainer#*"]
    StreamBSFileOutput.ItemList +=["CscRawDataContainer#*"]

    # EOF BS Writting (T Bold)





# end of configuration : check that some standalone flag have not been reinstantiated
varInit=dir()
if not rec.oldFlagCompatibility:
    try:
        for i in RecExCommonFlags.keys():
            if i in varInit:
                logRecExCommon_topOptions.warning("Variable %s has been re-declared, forbidden !" % i)
    except Exception:
        print "WARNING RecExCommonFlags not available, cannot check"





# -------------------------------------------------------------
# TAG and TAGCOMM making+writing
# -------------------------------------------------------------
pdr.flag_domain('tag')
if rec.doWriteTAGCOM():
    # include( "RecExCommission/RecExCommissionTAG_jobOptions.py" )
    logRecExCommon_topOptions.warning( "TAGCOM has been retired !! Please use doWriteTAG instead !!" )

try:
    if rec.doWriteTAG():
        from RegistrationServices.RegistrationServicesConf import RegistrationStream


        try:
            from RegistrationServices.RegistrationServicesConf import RegistrationStreamTagTool
            TagTool = RegistrationStreamTagTool("TagTool")
            StreamTAG = RegistrationStream("StreamTAG",
                                        CollectionType="ExplicitROOT",
                                       Tool=TagTool)
            StreamTAG.ItemList += tagmetadatalist
            logRecExCommon_topOptions.info( "StreamTAG set up the new way (with StreamTagTool" )
        except Exception:
            # if not new tag
            StreamTAG = RegistrationStream("StreamTAG",
                                           CollectionType="ExplicitROOT")
            StreamTAG.ItemList += [ "AthenaAttributeList#"+EventTagGlobal.AttributeList ]
            logRecExCommon_topOptions.info( "StreamTAG set up the old way (with StreamTagTool" )

        from AthenaCommon.AlgSequence import AthSequencer
        regSequence = AthSequencer( "AthRegSeq" )
        regSequence   += StreamTAG

        TagStreamName="*"

        # Default is that TAG will point at an output file
        StreamTAG.WriteInputDataHeader = False
        # ... unless nothing is written out
        if (rec.readRDO() or rec.readESD()) and (not rec.doWriteESD() and not rec.doWriteAOD()):
            StreamTAG.WriteInputDataHeader = True
        elif rec.readAOD() and not rec.doWriteAOD():
            StreamTAG.WriteInputDataHeader = True

        # and then specify which output dataheader to take

        if rec.doWriteAOD() and (rec.readRDO() or rec.readESD() or rec.readAOD()):
            TagStreamName="StreamAOD"
        elif not rec.readAOD() and (rec.readRDO() or rec.readESD()) and rec.doWriteESD():
            TagStreamName="StreamESD"

        logRecExCommon_topOptions.info( "TAG primary ref points to "+ TagStreamName)
        StreamTAG.ItemList += [ "DataHeader#"+TagStreamName ]

        pdr.flag_domain('output')
        # Define the output file name
        StreamTAG.OutputCollection = athenaCommonFlags.PoolTAGOutput()
        logRecExCommon_topOptions.info("StreamTAG Itemlist dump:")
        print StreamTAG.ItemList

except Exception:
    treatException ("problem setting up TAG output")


if rec.readAOD():
    from AthenaServices.AthenaServicesConf import AthenaEventLoopMgr
    ServiceMgr += AthenaEventLoopMgr()
    ServiceMgr.AthenaEventLoopMgr.EventPrintoutInterval = 100
    logRecExCommon_topOptions.info("AOD reading case: Set EventPrintoutInterval=100")


###################
## Common Utils  ##
###################

include("RecExCommon/RecoUtils.py")
include("RecExCommon/PrintRecoSummary.py")
