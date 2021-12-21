include.block ("RecExCommon/RecExCommon_topOptions.py")

############################
## Common job preparation ##
############################

import os

from AthenaCommon.Logging import logging
logRecExCommon_topOptions = logging.getLogger( 'RecExCommon_topOptions' )

from AthenaCommon.AlgSequence import AlgSequence
from AthenaCommon.AppMgr import ToolSvc,theApp,ServiceMgr
from AthenaCommon.AthenaCommonFlags  import athenaCommonFlags
from AthenaCommon.GlobalFlags  import globalflags
from AthenaCommon.BeamFlags import jobproperties
from AthenaCommon.Constants import DEBUG, INFO, WARNING, ERROR
from AthenaCommon import CfgMgr
from AthenaCommon import SystemOfUnits as Units
from RecExConfig.RecFlags import rec
from AthenaCommon.Resilience import treatException,protectedInclude

topSequence = AlgSequence()

import AthenaCommon.Debugging as _acdbg
if not _acdbg.DbgStage.value:
    topSequence.TimeOut = 3600 * Units.s

if rec.Production():
    from AthenaCommon import Configurable
    Configurable.log.setLevel( WARNING )

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
excludeTracePattern.append ( "*/MuonRecExample/MuonRecUtils.py")
excludeTracePattern.append ("athfile-cache.ascii")
excludeTracePattern.append ("*/IOVDbSvc/CondDB.py")
excludeTracePattern.append("*/LArCalib.py")
excludeTracePattern.append("*/_xmlplus/*")
excludeTracePattern.append("*/CaloClusterCorrection/CaloSwEtaoff*")
excludeTracePattern.append("*/PyUtils/Helpers.py")
excludeTracePattern.append("*/RecExConfig/RecoFunctions.py")
excludeTracePattern.append("*/PerfMonComps/DomainsRegistry.py")
excludeTracePattern.append("*/CaloClusterCorrection/common.py")
excludeTracePattern.append("*/D3PDMakerCoreComps/MakerAlg.py")
excludeTracePattern.append("*/D3PDMakerCoreComps/D3PDObject.py")
excludeTracePattern.append("*/RecExConfig/RecoFunctions.py")
excludeTracePattern.append("*/DQDefects/virtual*")
excludeTracePattern.append("*/TrigEDMConfig/TriggerEDM.py")
excludeTracePattern.append("*AthFile/impl.py")
excludeTracePattern.append("*/AthenaConfiguration/*")
excludeTracePattern.append("*ROOT/_facade.py")
#####################
# Flags (separated) #
#####################

include ( "RecExCond/RecExCommon_flags.py" )
from AthenaCommon.DetFlags import DetFlags   # this import has to be after RecExCommon_flags.py !

if (jobproperties.ConcurrencyFlags.NumThreads() > 0):
    logRecExCommon_topOptions.info("MT mode: Not scheduling RecoTiming")
    rec.doRecoTiming.set_Value_and_Lock(False)

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

if rec.readESD():
    rec.readRDO = False

#move AODFix here so that it can use rec.readRDO etc...

from AODFix import AODFix
AODFix.AODFix_Init()
AODFix.AODFix_preInclude()

from RecoFix import RecoFix
RecoFix.RecoFix_Init()


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
    # set by the following include
    recoMetadataItemList = None
    dfMetadataItemList = None
    ## compute ESD item list (in CILMergedESD )
    protectedInclude ( "RecExPers/RecoOutputMetadataList_jobOptions.py" )


#Output file TagInfo and metadata
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
svcMgr.TagInfoMgr.ExtraTagValuePairs.update({"beam_type": jobproperties.Beam.beamType(),
                                            "beam_energy": str(int(jobproperties.Beam.energy())),
                                            "triggerStreamOfFile": str(rec.triggerStream()),
                                            "project_name": str(rec.projectName()),
                                            "AtlasRelease_" + rec.OutputFileNameForRecoStep(): rec.AtlasReleaseVersion()
                                            })
# Set AMITag in /TagInfo
from PyUtils import AMITagHelper
AMITagHelper.SetAMITag(outputTag=rec.AMITag())

AODFix.AODFix_addMetaData()
RecoFix.RecoFix_addMetaData()

# end flag settings section
##########################################################################
# set up job

if 'doMixPoolInput' not in dir():
    doMixPoolInput = False


if rec.readTAG():
    logRecExCommon_topOptions.info("reading from TAG. Applying selection athenaCommonFlags.PoolInputQuery= %s ", athenaCommonFlags.PoolInputQuery() )
    logRecExCommon_topOptions.info("...to have the list of available variables. root myTAG.root. POOLCollectionTree->Show(0)")

if globalflags.InputFormat()=='pool':

    if doMixPoolInput:
        if not rec.readRDO():
            raise "doMixPoolInput only functional reading RDO"
        import StreamMix.ReadAthenaPool  # noqa: F401 (import with side-effect)
        from AthenaServices.AthenaServicesConf import MixingEventSelector
        svcMgr.ProxyProviderSvc.ProviderNames += [ "MixingEventSelector/EventMixer" ]
        EventMixer =  MixingEventSelector("EventMixer")
    else:
        # to read Pool data
        import AthenaPoolCnvSvc.ReadAthenaPool

        # if file not in catalog put it there
        svcMgr.PoolSvc.AttemptCatalogPatch=True

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


        # backward compatibility (needed for RTT overwriting InputCollections)
        EventSelector=ServiceMgr.EventSelector


from RecExConfig.RecAlgsFlags import recAlgs

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
        try:
            from TrigServices.TrigServicesConf import TrigMonTHistSvc
            THistSvc = TrigMonTHistSvc("THistSvc")
            svcMgr += THistSvc
        except Exception:
            logRecExCommon_topOptions.warning("isOnline=True but online Histogramming service not available, using offline THistSvc")
            from GaudiSvc.GaudiSvcConf import THistSvc
            svcMgr+=THistSvc()
            if os.path.exists(rec.RootHistoOutput()):
                os.remove(rec.RootHistoOutput())
            svcMgr.THistSvc.Output += ["GLOBAL DATAFILE='"+rec.RootHistoOutput()+"' OPT='RECREATE'"]

if rec.doFastPhysMonitoring():
    if not hasattr(svcMgr,"THistSvc"):
        from GaudiSvc.GaudiSvcConf import THistSvc
        svcMgr+=THistSvc()
    svcMgr.THistSvc.Output += ["FPMFILE DATAFILE='"+rec.RootFastPhysMonOutput()+"' OPT='RECREATE'"]


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
# for BS file this cannot be done already, see later
if globalflags.InputFormat.is_pool():
    logRecExCommon_topOptions.info("Pool file : storing in objKeyStore the list of input object directly from file")
    try:
        from PyUtils.MetaReaderPeeker import convert_itemList
        objKeyStore.addManyTypesInputFile(convert_itemList(layout='#join'))

    except Exception:
        logRecExCommon_topOptions.error("no input file defined in flags. Now continuing at own risk")


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


# Disable DPD making if neither script or passThroughMode set
if rec.doDPD() and not (rec.DPDMakerScripts() or rec.doDPD.passThroughMode):
    rec.doDPD = False

if rec.OutputLevel() <= DEBUG:
    print (" Initial content of objKeyStore ")
    print (objKeyStore)

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
from GaudiSequencer.GaudiSequencerConf import AthEventCounter as EventCounter

import PerfMonComps.DomainsRegistry as pdr
pdr.flag_domain('admin')
# one print every 100 event
topSequence+=EventCounter("EventCounter",Frequency=100)

#Temporary: Schedule conversion algorithm for EventInfo object:
# Note that we need to check whether the HLT already added this algorithm to the
# algorithm sequence!
#FIXME: Subsequent algorithms may alter the event info object (setting Error bits)
from AthenaCommon.AlgSequence import AthSequencer
condSeq = AthSequencer("AthCondSeq")

if( not globalflags.InputFormat.is_bytestream() and \
        ( not objKeyStore.isInInput( "xAOD::EventInfo") ) and \
        ( not hasattr( topSequence, "xAODMaker::EventInfoCnvAlg" ) ) ):
    from xAODEventInfoCnv.xAODEventInfoCnvAlgDefault import xAODEventInfoCnvAlgDefault
    xAODEventInfoCnvAlgDefault (sequence = condSeq)

# bytestream reading need to shedule some algorithm

if globalflags.InputFormat.is_bytestream():
    logRecExCommon_topOptions.info("Setting up ByteStream reading...")
    #Online and tag-reading are mutually exclusive
    if  athenaCommonFlags.isOnline():
        logRecExCommon_topOptions.info("Should set up  EmonByteStreamSvc here....")
        # only load if EmonByteStreamSvc is not setup already, as for example for isOnline nightly offline
        try:
            svcMgr.ByteStreamInputSvc
        except Exception:
            logRecExCommon_topOptions.warning("isOnline=True but EmonByteStreamSvc not available, probably running online nightly")
            from ByteStreamCnvSvc import ReadByteStream

            # Specify input file
            if len(athenaCommonFlags.FilesInput())>0:
                svcMgr.EventSelector.Input=athenaCommonFlags.FilesInput()
            elif len(athenaCommonFlags.BSRDOInput())>0:
                svcMgr.EventSelector.Input=athenaCommonFlags.BSRDOInput()
    else:
        logRecExCommon_topOptions.info("Read ByteStream file(s)")
        from ByteStreamCnvSvc import ReadByteStream   # noqa: F401 (import with side-effect)

        # Specify input file
        if len(athenaCommonFlags.FilesInput())>0:
            svcMgr.EventSelector.Input=athenaCommonFlags.FilesInput()
        elif len(athenaCommonFlags.BSRDOInput())>0:
            svcMgr.EventSelector.Input=athenaCommonFlags.BSRDOInput()

    #Set TypeNames of ByteStreamInputService according to global flags:
    protectedInclude("RecExCommon/BSRead_config.py")


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
    except Exception:
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


#################################################################################
# Initialize ConfigFlags for use by CA-based code below
from AthenaConfiguration.OldFlags2NewFlags import getNewConfigFlags
ConfigFlags = getNewConfigFlags()

# Apply additional changes to the ConfigFlags:
if rec.doTrigger and globalflags.DataSource() == 'data' and globalflags.InputFormat == 'bytestream':
    ConfigFlags.Trigger.readBS = True

# For cosmics runs, we need to turn off doTIDE_Ambi to be consistent
# with the settings in InDetJobProperties.py.
# We used to do that there, but that doesn't work anymore
# since we started locking ConfigFlags in here.
from InDetRecExample.InDetJobProperties import InDetFlags
if jobproperties.Beam.beamType() == 'cosmics':
    ConfigFlags.InDet.doTIDE_Ambi = False

if rec.doMonitoring():
    include ("AthenaMonitoring/DataQualityInit_jobOptions.py")

if recAlgs.doEFlow():
    #Some settings for pflow have to toggle to a different setup for RecExCommon workflows.
    ConfigFlags.PF.useRecExCommon=True
    from eflowRec.eflowRecFlags import jobproperties
    if False == jobproperties.eflowRecFlags.usePFFlowElementAssoc:
        ConfigFlags.PF.useElPhotLinks = False
        ConfigFlags.PF.useMuLinks = False

HIDict = {}
if rec.doHeavyIon():
    # This is copy from the old style to the new
    # We need to have HI flags to do it nicer
    ConfigFlags.Egamma.Keys.Input.TopoClusters = 'SubtractedCaloTopoCluster'
    ConfigFlags.Egamma.Keys.Internal.EgammaTopoClusters = 'SubtractedEgammaTopoCluster'
    ConfigFlags.Egamma.Keys.Input.CaloCells = 'SubtractedCells'
    ConfigFlags.Egamma.doCentral = True
    ConfigFlags.Egamma.doForward = False
    # This is a trick : in HeavyIon, egammaTopoClusterCopier is run two times
    # one on the unsubtracted clusters (in SystemRec_config.py),
    # the other on subtracted clusters (in HIegamma_jobO).
    # Why is the first followed by InDetCaloClusterROISelector needed in Heavy Ion reco ?
    HIDict['InputTopoCollection'] = 'CaloTopoClusters'
    HIDict['OutputTopoCollection'] = 'egammaTopoClusters'
    HIDict['OutputTopoCollectionShallow'] = 'tmp_egammaTopoClusters'

# Lock the flags
logRecExCommon_topOptions.info("Locking ConfigFlags")
ConfigFlags.lock()
#################################################################################


pdr.flag_domain('trig')
# no trigger, if readESD _and_ doESD ! (from Simon George, #87654)
if rec.readESD() and rec.doESD():
    rec.doTrigger=False
    logRecExCommon_topOptions.info("detected re-reconstruction from ESD, will switch trigger OFF !")

# Disable Trigger output reading in MC if there is none
if not globalflags.InputFormat.is_bytestream():
    try:
        from RecExConfig.ObjKeyStore import cfgKeyStore
        from PyUtils.MetaReaderPeeker import convert_itemList
        cfgKeyStore.addManyTypesInputFile(convert_itemList(layout='#join'))
        # Check for Run-1, Run-2 or Run-3 Trigger content in the input file
        from TrigDecisionTool.TrigDecisionToolConfig import getRun3NavigationContainerFromInput
        if not cfgKeyStore.isInInputFile("HLT::HLTResult", "HLTResult_EF") \
                and not cfgKeyStore.isInInputFile("xAOD::TrigNavigation", "TrigNavigation") \
                and not cfgKeyStore.isInInputFile("xAOD::TrigCompositeContainer", getRun3NavigationContainerFromInput(ConfigFlags) ):
            logRecExCommon_topOptions.info('Disabled rec.doTrigger because there is no Trigger content in the input file')
            rec.doTrigger = False
    except Exception:
        logRecExCommon_topOptions.warning('Failed to check input file for Trigger content, leaving rec.doTrigger value unchanged (%s)', rec.doTrigger)

if rec.doTrigger:
    from TriggerJobOpts.TriggerRecoGetter import TriggerRecoGetter
    triggerGetter = TriggerRecoGetter()

    # ESDtoAOD Run-3 Trigger Outputs: Don't run any trigger - only pass the HLT contents from ESD to AOD
    if rec.readESD() and rec.doAOD():
        if ConfigFlags.Trigger.EDMVersion == 3:
            # Add HLT output
            from TriggerJobOpts.HLTTriggerResultGetter import HLTTriggerResultGetter
            hltOutput = HLTTriggerResultGetter()
            # Add Trigger menu metadata
            if rec.doFileMetaData():
                from RecExConfig.ObjKeyStore import objKeyStore
                metadataItems = [ "xAOD::TriggerMenuContainer#TriggerMenu",
                                "xAOD::TriggerMenuAuxContainer#TriggerMenuAux." ]
                objKeyStore.addManyTypesMetaData( metadataItems )
            # Add L1 output (to be consistent with R2)
            from TrigEDMConfig.TriggerEDM import getLvl1AODList
            objKeyStore.addManyTypesStreamAOD(getLvl1AODList())

AODFix.AODFix_postTrigger()

if globalflags.DataSource()=='geant4':
    if (rec.doRecoTiming() and rec.OutputFileNameForRecoStep() in ('RAWtoESD','ESDtoAOD','RAWtoALL')):
        topSequence+=TimingAlg("RecoTimerAfterTrigger")
        topSequence.RecoTimerAfterTrigger.TimingObjOutputName=rec.OutputFileNameForRecoStep()+"_timings"
        try:
            topSequence+=MemoryAlg("RecoMemoryAfterTrigger")
            topSequence.RecoMemoryAfterTrigger.MemoryObjOutputName=rec.OutputFileNameForRecoStep()+"_mems"
        except Exception:
            logRecExCommon_topOptions.info("Could not instantiate MemoryAlg")

if globalflags.InputFormat.is_bytestream():
    logRecExCommon_topOptions.info("BS file : storing in objKeyStore the list of input object from ByteStreamAddressProviderSvc.TypeNames. Unsafe ! ")
    newTN=[]
    for a in svcMgr.ByteStreamAddressProviderSvc.TypeNames:
        newTN+=[a.replace("/","#")]
        objKeyStore.addManyTypesInputFile(newTN)
        pass
    pass

### write mu values into xAOD::EventInfo
if rec.readRDO():
    if globalflags.DataSource()=='geant4':
        include_muwriter = (globalflags.InputFormat.is_bytestream() or
                            hasattr( condSeq, "xAODMaker::EventInfoCnvAlg" ) or
                            objKeyStore.isInInput( "xAOD::EventInfo"))
    else:
        include_muwriter = not athenaCommonFlags.isOnline()

    if include_muwriter:
        from LumiBlockComps.LumiBlockMuWriterDefault import LumiBlockMuWriterDefault
        LumiBlockMuWriterDefault()

#
# Write beamspot information into xAOD::EventInfo.
#
if globalflags.InputFormat.is_bytestream():
    topSequence += CfgMgr.xAODMaker__EventInfoBeamSpotDecoratorAlg()
    pass

#
# System Reconstruction
#
include ("RecExCommon/SystemRec_config.py")
AODFix.AODFix_postSystemRec()



#
# Combined reconstruction
#
include ("RecExCommon/CombinedRec_config.py")
AODFix.AODFix_postCombinedRec()




#
# Heavy ion reconstruction  special configuration
#
pdr.flag_domain('HI')
if rec.doHeavyIon():
    protectedInclude ("HIRecExample/HIRec_jobOptions.py")

if rec.doHIP ():
    protectedInclude ("HIRecExample/HIPRec_jobOptions.py")

if rec.doWriteBS():
    include( "ByteStreamCnvSvc/RDP_ByteStream_jobOptions.py" )
    pass

pdr.flag_domain('tagraw')
## add in RawInfoSummaryForTagWriter
if rec.doESD() and not rec.readESD() and (rec.doBeamBackgroundFiller() or rec.doTagRawSummary()):
    try:
        include("EventTagRawAlgs/RawInfoSummaryForTagWriter_jobOptions.py")
    except Exception:
        logRecExCommon_topOptions.warning("Could not load RawInfoSummaryForTagWriter_joboptions !" )
        pass
    pass
# write the background word into EventInfo (Jamie Boyd)
# need to go here for ordering reasons...
if rec.doESD() and not rec.readESD() and rec.doBeamBackgroundFiller():
    try:
        from AthenaCommon.Configurable import Configurable
        Configurable.configurableRun3Behavior=1
        from AthenaConfiguration.ComponentAccumulator import appendCAtoAthena
        from AthenaConfiguration.AllConfigFlags import ConfigFlags
        from RecBackgroundAlgs.BackgroundAlgsConfig import BackgroundAlgsCfg
        ca=BackgroundAlgsCfg(ConfigFlags)

        for el in ca._allSequences:
            el.name = "TopAlg"

            appendCAtoAthena(ca)

    except Exception:
        treatException("Could not translate BackgroundAlgsCfg to old cfg")
    finally:
         Configurable.configurableRun3Behavior=0
    pass


# ----------------------------------------------------------------------------

# kind of hack. Note that ByteStreamAddressProviderSvc.TypeNames is continuously updated, also in mon configuration
# do it now, because monitoring is actually adding stuff to ByteStreamAddressProvider
if globalflags.InputFormat.is_bytestream():
    logRecExCommon_topOptions.info("BS file : store again in objKeyStore the list of input object from svcMgr.ByteStreamAddressProviderSvc.TypeNames. Unsafe ! ")
    newTN=[]
    for a in svcMgr.ByteStreamAddressProviderSvc.TypeNames:
        newTN+=[a.replace("/","#")]
        objKeyStore.addManyTypesInputFile(newTN)


################################################################"


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


from RecAlgs.RecAlgsConf import JobInfo
topSequence+=JobInfo(PrintFATAL=False)

if rec.doPhysValMonHists():
    include("PhysValMon/PhysValMon_RecoOpt.py")

pdr.flag_domain('output')


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



###################################################################
#
# now write out Transient Event Store content in POOL
#
if rec.doWriteESD() or rec.doWriteAOD() or rec.doWriteRDO() or rec.doWriteTAG():
    import AthenaPoolCnvSvc.WriteAthenaPool  # noqa: F401 (import with side-effect)


# Must make sure that no OutStream's have been declared
#FIXME
theApp.OutStream = []

if rec.doWriteTAG():
    logRecExCommon_topOptions.error("Producing TAG files is not supported anymore, disabling it.")
    rec.doWriteTAG = False

if rec.doWriteRDO():
    #Create output StreamRDO
    from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
    StreamRDO_Augmented=MSMgr.NewPoolStream("StreamRDO",athenaCommonFlags.PoolRDOOutput(),asAlg=True)

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
        StreamRDO_Augmented.GetEventStream().WritingTool.AttributeListKey = "SimpleTag"
    except Exception:
        logRecExCommon_topOptions.warning("Failed to add TAG attribute list to payload data")

if globalflags.InputFormat()=='bytestream':
    # FIXME : metadata store definition is in ReadAthenaPool_jobOptions.py
    # copy it there for BS Reading for 13..0.X
    # Add in MetaDataSvc
    from AthenaServices.AthenaServicesConf import MetaDataSvc
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


# Lumiblocks and ByteStreamMetadata
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
            ToolSvc += LumiBlockMetaDataTool()
            svcMgr.MetaDataSvc.MetaDataTools += [ ToolSvc.LumiBlockMetaDataTool ]
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

    try:
        # ByteStreamMetadata
        from ByteStreamCnvSvc.ByteStreamCnvSvcConf import ByteStreamMetadataTool
        if not hasattr (svcMgr.ToolSvc, 'ByteStreamMetadataTool'):
            ToolSvc += ByteStreamMetadataTool()
        svcMgr.MetaDataSvc.MetaDataTools += [ ToolSvc.ByteStreamMetadataTool ]
    except Exception:
        treatException("Could not load ByteStreamMetadataTool")
    pass


##--------------------------------------------------------
###=== Only run reco on events that pass selected triggers
##--------------------------------------------------------
if rec.doTrigger and rec.doTriggerFilter() and globalflags.DataSource() == 'data' and globalflags.InputFormat == 'bytestream':
    logRecExCommon_topOptions.info('Setting up trigger filtering')
    try:
### seq will be our filter sequence
        from AthenaCommon.AlgSequence import AthSequencer
        seq=AthSequencer("AthMasterSeq")

        from EventBookkeeperTools.CutFlowHelpers import CreateCutFlowSvc
        logRecExCommon_topOptions.debug("Calling CreateCutFlowSvc")
        CreateCutFlowSvc( svcName="CutFlowSvc", seq=seq, addMetaDataToAllOutputFiles=True )

        seq+=topSequence.RoIBResultToxAOD
        seq+=topSequence.TrigBSExtraction
        seq+=topSequence.TrigDecMaker

        from GoodRunsListsUser.GoodRunsListsUserConf import TriggerSelectorAlg
        seq += TriggerSelectorAlg('TriggerAlg1')
        seq.TriggerAlg1.TriggerSelection = rec.triggerFilterList()
        pass
    except Exception as e:
        logRecExCommon_topOptions.error('Trigger filtering not set up, reason: %s', e)
        pass
##--------------------------------------------------------


if rec.doWriteESD():
    if rec.doWriteRDO():
        # mark the RDO DataHeader as the input DataHeader

        from OutputStreamAthenaPool.OutputStreamAthenaPoolConf import MakeInputDataHeader
        from AthenaCommon.AlgSequence import AthSequencer
        outSequence = AthSequencer("AthOutSeq")
        outSequence+=MakeInputDataHeader("MakeInputDataHeaderRDO",
                                         StreamName="StreamRDO")
        pass

    #Create output StreamESD
    streamESDName="StreamESD"
    sdwContainerName=''
    #special option for ESD/dESD merging
    if rec.readESD() and rec.doWriteESD() and rec.doESD() is False and rec.mergingStreamName()!="":
        streamESDName=rec.mergingStreamName()
        pass

    ## compute ESD item list (in CILMergeESD )
    CILMergeESD = None  # set by following include
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
            logRecExCommon_topOptions.info("Adding the following list of algorithms to the %s: %s",
                                           sdwContainerName, desdEventSkimmingFilterNamesList)
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
            if not (rec.doTrigger and rec.doCalo and rec.doInDet and rec.doMuon):
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
        StreamESD_Augmented.GetEventStream().WritingTool.AttributeListKey = "SimpleTag"
    except Exception:
        logRecExCommon_topOptions.warning("Failed to add TAG attribute list to payload data")

    # really add item to StreamESD. CILMergeESd was computed by RecoOutputESDList_jobOptions.py" )
    StreamESD.ItemList = CILMergeESD()

    # consistency check : make sure oks streamESD==CILMergeESD and included in transient
    #FIXME many problem. #* to remove, datavector to be removed plus basic thing missing
    print ("DRDR now consistency checks with three list")
    streamesd=objKeyStore['streamESD']()
    transient=objKeyStore['transient']()
    mergeesd=CILMergeESD()
    # and not item.endswith("#*")
    for item in mergeesd:
        if not item.endswith("#*"):
            if item not in transient:
                logRecExCommon_topOptions.warning("item %s in CILMergeESD but not in objKeyStore['transient']", item)
            if item not in streamesd:
                logRecExCommon_topOptions.warning("item %s in CILMergeESD but not in objKeyStore['streamESD']", item)

    for item in streamesd:
        #sthg fishy with LArCLusterEM to sort out
        if not item.endswith("#*") and not item=="DataVector<INavigable4Momentum>#LArClusterEM":
            if item not in transient:
                logRecExCommon_topOptions.warning("item %s in streamesd but not in objKeyStore['transient']", item)
            if item not in mergeesd:
                logRecExCommon_topOptions.warning("item %s in objKeyStore['streamESD'] but not in  CILMergeESD", item)

    del streamesd,transient,mergeesd



    if sdwContainerName:
        StreamESD_Augmented.AddItem("SkimDecisionCollection#" + sdwContainerName)
        pass

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
if rec.doDPD():
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

    #SkimDecision objects may once migrate to CutFlowSvc or DecisionSvc, but not yet
    logRecExCommon_topOptions.info( "primDPD.WriteSkimDecisions =  %s", primDPD.WriteSkimDecisions() )
    if primDPD.WriteSkimDecisions():
        MSMgr.WriteSkimDecisionsOfAllStreams()
        pass

    #Configure CutFlowSv and common metadata
    if rec.doFileMetaData():

        #Exception for DPD pass-through mode
        if rec.doDPD.passThroughMode:
            svcMgr.CutFlowSvc.InputStream="Virtual"
            pass

        if rec.DPDMakerScripts()!=[] and not rec.doDPD.passThroughMode :
            # Add the needed stuff for cut-flow bookkeeping.
            # Only the configurables that are not already present will be created
            from EventBookkeeperTools.CutFlowHelpers import CreateCutFlowSvc
            logRecExCommon_topOptions.debug("Calling CreateCutFlowSvc")
            CreateCutFlowSvc( svcName="CutFlowSvc", seq=topSequence, addMetaDataToAllOutputFiles=True )

            from PyUtils.MetaReaderPeeker import convert_metadata_items
            #Explicitely add file metadata from input and from transient store
            MSMgr.AddMetaDataItemToAllStreams(convert_metadata_items(layout='#join'))
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
        from AthenaCommon.AlgSequence import AthSequencer
        outSequence = AthSequencer("AthOutSeq")
        outSequence+=MakeInputDataHeader("MakeInputDataHeaderESD",
                                         StreamName="StreamESD")

if ( rec.doAOD() or rec.doWriteAOD()) and not rec.readAOD() :
    # special slimmed cell in AOD
    if DetFlags.detdescr.Calo_on() and rec.doAODCaloCells():
        try:
            from CaloRec.CaloCellAODGetter import addClusterToCaloCellAOD

            if ( rec.readESD() or ConfigFlags.Reco.EnableEgamma ) and not rec.ScopingLevel()==4 and rec.doEgamma :
                from egammaRec import egammaKeys
                addClusterToCaloCellAOD(egammaKeys.outputClusterKey())
                addClusterToCaloCellAOD(egammaKeys.outputFwdClusterKey())
                addClusterToCaloCellAOD(egammaKeys.outputEgammaLargeFWDClustersKey())
                if "itemList" in metadata:
                    if ('xAOD::CaloClusterContainer', egammaKeys.EgammaLargeClustersKey()) in metadata["itemList"]:
                        # check first for priority if both keys are in metadata
                        addClusterToCaloCellAOD(egammaKeys.EgammaLargeClustersKey())
                    elif ('xAOD::CaloClusterContainer', 'LArClusterEM7_11Nocorr') in metadata["itemList"]:
                        addClusterToCaloCellAOD('LArClusterEM7_11Nocorr')
                    else:
                        addClusterToCaloCellAOD(egammaKeys.EgammaLargeClustersKey())
                else:
                    addClusterToCaloCellAOD(egammaKeys.EgammaLargeClustersKey())

            from MuonCombinedRecExample.MuonCombinedRecFlags import muonCombinedRecFlags
            if ( rec.readESD() or muonCombinedRecFlags.doMuonClusters() ) and rec.doMuon:
                addClusterToCaloCellAOD("MuonClusterCollection")

            if rec.readESD() or recAlgs.doTrackParticleCellAssociation():
                addClusterToCaloCellAOD("InDetTrackParticlesAssociatedClusters")

        except Exception:
            treatException("Could not make AOD cells" )


pdr.flag_domain('aod')

# EventBookkeepers
if rec.doFileMetaData() or rec.OutputFileNameForRecoStep() == 'EVNTtoDAOD':
    # Add the needed stuff for cut-flow bookkeeping.
    # Only the configurables that are not already present will be created
    hasBookkeepers = False
    if 'metadata_items' in metadata:
        metadata_items = metadata['metadata_items']
        if 'xAOD::CutBookkeeperContainer_v1' in set(metadata_items.values()):
            logRecExCommon_topOptions.debug("Existing CutBookkeeperContainer found")
            hasBookkeepers = True
    if hasBookkeepers or rec.OutputFileNameForRecoStep() in ['EVNTtoDAOD', 'AODtoDAOD']:
        from EventBookkeeperTools.CutFlowHelpers import CreateCutFlowSvc
        logRecExCommon_topOptions.debug("Going to call CreateCutFlowSvc")
        CreateCutFlowSvc( svcName="CutFlowSvc", seq=topSequence, addMetaDataToAllOutputFiles=True )
        if rec.readAOD() or rec.readESD():
            #force CutFlowSvc execution (necessary for file merging)
            theApp.CreateSvc+=['CutFlowSvc']
            logRecExCommon_topOptions.debug("Added CutFlowSvc to theApp")
            pass
        pass
    pass


if rec.doWriteAOD():
    from ParticleBuilderOptions.AODFlags import AODFlags
    # Particle Builders
    from AthenaCommon.AppMgr import ServiceMgr as svcMgr


    # cannot redo the slimming if readAOD and writeAOD
    if not rec.readAOD() and (rec.doESD() or rec.readESD()):
        if AODFlags.ThinTRTStandaloneTracks:
            from ThinningUtils.ThinningUtilsConf import ThinTRTStandaloneTrackAlg
            thinTRTStandaloneTrackAlg = ThinTRTStandaloneTrackAlg('ThinTRTStandaloneTrackAlg',
                                                                  doElectron = (rec.doEgamma() and AODFlags.Electron()),
                                                                  doPhoton = (rec.doEgamma() and AODFlags.Photon()),
                                                                  doTau = rec.doTau(),
                                                                  doMuon = rec.doMuonCombined())
            topSequence += thinTRTStandaloneTrackAlg

        if rec.doEgamma() and (AODFlags.Photon or AODFlags.Electron):
            if AODFlags.egammaTrackSlimmer:
                from AthenaConfiguration.ComponentAccumulator import CAtoGlobalWrapper
                from egammaAlgs.egammaTrackSlimmerConfig import egammaTrackSlimmerCfg
                CAtoGlobalWrapper(egammaTrackSlimmerCfg,ConfigFlags,StreamName='StreamAOD')

        if rec.doTau() and AODFlags.ThinTaus:
            # tau-related thinning: taus, clusters, cells, cell links, PFOs, tracks, vertices
            from tauRec.tauRecConf import TauThinningAlg
            tauThinningAlg = TauThinningAlg('TauThinningAlg')
            topSequence += tauThinningAlg

        if rec.doTruth() and AODFlags.ThinGeantTruth:
            from ThinningUtils.ThinGeantTruth import ThinGeantTruth
            ThinGeantTruth()

        if rec.doCalo and AODFlags.ThinNegativeEnergyCaloClusters:
            from ThinningUtils.ThinNegativeEnergyCaloClusters import ThinNegativeEnergyCaloClusters
            ThinNegativeEnergyCaloClusters()
        if rec.doCalo and AODFlags.ThinNegativeEnergyNeutralPFOs:
            from ThinningUtils.ThinNegativeEnergyNeutralPFOs import ThinNegativeEnergyNeutralPFOs
            ThinNegativeEnergyNeutralPFOs()
        from InDetRecExample.InDetJobProperties import InDetFlags
        if (AODFlags.ThinInDetForwardTrackParticles() and
            not (rec.readESD() and not objKeyStore.isInInput('xAOD::TrackParticleContainer',
                                                             'InDetForwardTrackParticles'))
            and InDetFlags.doForwardTracks()):
            from ThinningUtils.ThinInDetForwardTrackParticles import ThinInDetForwardTrackParticles
            ThinInDetForwardTrackParticles()

        #Thin Trk::Tracks for Electons and Muons (GSF/Combined)
        if  (AODFlags.AddEgammaMuonTracksInAOD and not rec.doTruth()) or (AODFlags.AddEgammaTracksInMCAOD and rec.doTruth()):
            from ThinningUtils.ThinTrkTrack import ThinTrkTrack
            ThinTrkTrack()


    pdr.flag_domain('output')
    # Create output StreamAOD
    streamAODName="StreamAOD"
    #special option for AOD/dAOD merging
    if rec.readAOD() and rec.doWriteAOD() and rec.doAOD() is False and rec.mergingStreamName()!="":
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

        # FileMetaData tool
        ToolSvc += CfgMgr.xAODMaker__FileMetaDataTool("FileMetaDataTool")
        svcMgr.MetaDataSvc.MetaDataTools += [ToolSvc.FileMetaDataTool]

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
    try:
        StreamAOD.WritingTool.AttributeListKey = "SimpleTag"
    except Exception:
        logRecExCommon_topOptions.warning("Failed to add TAG attribute list to payload data")

    # protectedInclude( "ParticleBuilderOptions/AOD_OutputList_jobOptions.py")
    protectedInclude( "RecExPers/RecoOutputAODList_jobOptions.py")
    StreamAOD_Augmented.AddItem("SkimDecisionCollection#*")
    #FIXME HACK remove faulty object
    StreamAOD_Augmented.GetEventStream().ItemList = [ e for e in StreamAOD_Augmented.GetEventStream().ItemList if e not in [ 'CaloTowerContainer#HLT_TrigCaloTowerMaker'] ]

    # this is AOD->AOD copy
    if rec.readAOD():
        # copy does not point to input
        StreamAOD_Augmented.GetEventStream().ExtendProvenanceRecord = False
        # all input to be copied to output
        StreamAOD_Augmented.GetEventStream().TakeItemsFromInput =True

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


# event dumper at the very end
if rec.doPyDump():
    from RecExConfig.RecoFunctions import OutputFileName
    OutFileName=OutputFileName(rec.OutputSuffix())

    from PyDumper.PyComps import PySgDumper
    topSequence += PySgDumper(ofile='PyDump_'+OutFileName+'.txt')

    # preliminary
    topSequence += CfgMgr.INav4MomDumper(INav4Moms = ['CaloMuonCollection'],
                                         OutputStream = "inav4moms_"+OutFileName+'.txt',
                                         OutputLevel  = INFO )


# detailed auditor of time to read/write pool object
if hasattr(ServiceMgr, 'AthenaPoolCnvSvc'):
    ServiceMgr.AthenaPoolCnvSvc.UseDetailChronoStat = True


# BS writing (T Bold)
if rec.doWriteBS():

    #Persistent BS construction and intialization
    from ByteStreamCnvSvc import WriteByteStream
    StreamBSFileOutput = WriteByteStream.getStream("EventStorage","StreamBSFileOutput")

    ServiceMgr.ByteStreamCnvSvc.IsSimulation = True

    # LVL1
    from TrigT1ResultByteStream.TrigT1ResultByteStreamConfig import L1ByteStreamEncodersRecExSetup
    L1ByteStreamEncodersRecExSetup()  # Configure BS encoder for RoIBResult
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


    # LAr
    #        StreamBS.ItemList +=["LArRawChannels#*"]
    StreamBSFileOutput.ItemList +=["2721#*"]

    from LArByteStream.LArByteStreamConfig import LArRawDataContByteStreamToolConfig
    ToolSvc+=LArRawDataContByteStreamToolConfig(InitializeForWriting=True,
                                                stream = StreamBSFileOutput)

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


if rec.readAOD():
    from AthenaServices.AthenaServicesConf import AthenaEventLoopMgr
    ServiceMgr += AthenaEventLoopMgr()
    ServiceMgr.AthenaEventLoopMgr.EventPrintoutInterval = 100
    logRecExCommon_topOptions.info("AOD reading case: Set EventPrintoutInterval=100")

# run monitoring
# ----------------------------------------------------------------------------
# Monitoring Algorithms and Tools
# ----------------------------------------------------------------------------

pdr.flag_domain('monitoring')
if rec.doMonitoring():
    protectedInclude ("AthenaMonitoring/DataQualitySteering_jobOptions.py")

# run"Fast Phsyics Monitoring"
if rec.doFastPhysMonitoring():
    protectedInclude("FastPhysMonExample/FastPhysicsMonitoring_jobOptions.py")


###################
## Common Utils  ##
###################

include("RecExCommon/RecoUtils.py")
include("RecExCommon/PrintRecoSummary.py")

from RecAlgs.RecAlgsConf import AppStopAlg
topSequence+=AppStopAlg()
