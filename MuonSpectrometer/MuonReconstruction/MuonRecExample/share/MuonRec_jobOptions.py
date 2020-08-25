# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#
## @file MuonRec_jobOptions.py
#
# @brief Main jobOptions to setup muon reconstruction. Main muon entry point for RecExCommon.
#
# Setup the data converters depending on the type of input file and run muon standalone reconstruction
# It can optionally write out the calibration ntuple (depending on muonRecFlags.doCalib)
#
# @author Martin Woudstra
# @author Edward Moyse

include.block("MuonRecExample/MuonRec_jobOptions.py")

from MuonRecExample.MuonRecFlags import muonRecFlags
from MuonRecExample.ConfiguredMuonRec import GetConfiguredMuonRec
from MuonRecExample.MuonRecUtils import logMuon, logMuonResil
from MuonRecExample.MuonStandaloneFlags import muonStandaloneFlags

from AthenaCommon.AlgSequence import AlgSequence
from AthenaCommon.AppMgr import ServiceMgr
from AthenaCommon.GlobalFlags import globalflags
from AthenaCommon.DetFlags import DetFlags
from AthenaCommon import CfgMgr
from RecExConfig.RecFlags import rec
from RecExConfig.RecAlgsFlags import recAlgs
from MuonRecExample.MuonAlignFlags import muonAlignFlags
from AthenaCommon.AppMgr import ToolSvc

from AtlasGeoModel.MuonGMJobProperties import MuonGeometryFlags

muonRecFlags.setDefaults()

topSequence = AlgSequence()

# ESDtoAOD and AODtoTAG need a configured MuonIdHelperSvc (e.g. for the RPC_ResidualPullCalculator)
# Since it is not automatically created by the job configuration (as for RDOtoESD),
# do it here manually (hope this will be fixed with the movement to the new configuration for release 22)
if rec.readESD() or rec.readAOD():
    from MuonRecExample.MuonRecTools import MuonIdHelperSvc
    MuonIdHelperSvc()

if muonRecFlags.doCSCs() and not MuonGeometryFlags.hasCSC(): muonRecFlags.doCSCs = False
if muonRecFlags.dosTGCs() and not MuonGeometryFlags.hasSTGC(): muonRecFlags.dosTGCs = False
if muonRecFlags.doMicromegas() and not MuonGeometryFlags.hasMM(): muonRecFlags.doMicromegas = False

if muonRecFlags.doDigitization():
    include("MuonRecExample/MuonDigitization_jobOptions.py")

if rec.readRDO():
    if globalflags.InputFormat == 'bytestream':
        include("MuonCnvExample/MuonReadBS_jobOptions.py")
    else:
        include("MuonCnvExample/MuonReadRDO_jobOptions.py")
    if DetFlags.makeRIO.Muon_on():
        include("MuonRecExample/MuonRDO_to_PRD_jobOptions.py")

### add PRD -> xAOD
if (rec.readRDO() or rec.readESD()) and muonRecFlags.prdToxAOD():
    from AthenaCommon import CfgMgr
    topSequence += CfgMgr.MDT_PrepDataToxAOD()
    topSequence += CfgMgr.RPC_PrepDataToxAOD()
    topSequence += CfgMgr.TGC_PrepDataToxAOD()
    if MuonGeometryFlags.hasCSC(): topSequence += CfgMgr.CSC_PrepDataToxAOD()
    
    if muonRecFlags.doCreateClusters():
        topSequence += CfgMgr.RPC_PrepDataToxAOD("RPC_ClusterToxAOD",InputContainerName="RPC_Clusters")
        topSequence += CfgMgr.TGC_PrepDataToxAOD("TGC_ClusterToxAOD",InputContainerName="TGC_Clusters")

### add RPC RDO -> xAOD
if (rec.readRDO() or rec.readESD()) and muonRecFlags.rpcRawToxAOD():
    from AthenaCommon import CfgMgr
    topSequence += CfgMgr.RPC_RDOToxAOD()
    
    #if muonRecFlags.doCreateClusters():
    #    topSequence += CfgMgr.rpcRawToxAOD("rpcRawToxAOD",InputContainerName="rpcRaw")
        

###### from Tomoe & Susumu
## For backward compatibility of TGC hit PRD containers in ESD
if rec.readESD() and DetFlags.readRIOPool.TGC_on():
    include("MuonTGC_CnvTools/TgcPrepDataReplicationAlg_jopOptions.py")

if muonRecFlags.doFastDigitization():
    if (MuonGeometryFlags.hasSTGC() and MuonGeometryFlags.hasMM()):
        #if DetFlags.Micromegas_on() and DetFlags.digitize.Micromegas_on():    
        from MuonFastDigitization.MuonFastDigitizationConf import MM_FastDigitizer
        topSequence += MM_FastDigitizer("MM_FastDigitizer")
        #if DetFlags.sTGC_on() and DetFlags.digitize.sTGC_on():    
        from MuonFastDigitization.MuonFastDigitizationConf import sTgcFastDigitizer
        topSequence += sTgcFastDigitizer("sTgcFastDigitizer")
   

# filter TrackRecordCollection (true particles in muon spectrometer)
if recAlgs.doTrackRecordFilter() and rec.doTruth():
    from MCTruthAlgs.MCTruthAlgsConf import TrackRecordFilter
    topSequence += TrackRecordFilter()
    # container had a different name in older releases
    import re
    if re.match("DC1|DC2|Rome|ATLAS-DC3-01|ATLAS-DC3-02", globalflags.DetDescrVersion()):
        topSequence.TrackRecordFilter.inputName="MuonEntryRecord"
    if muonRecFlags.writeMuonExitLayer():
        topSequence += TrackRecordFilter("TrackRecordFilterMuonExitLayer",
                                         inputName="MuonExitLayer",
                                         outputName="MuonExitLayerFilter")

# only do PRD truth if PRD's are being made
if rec.doTruth() and DetFlags.makeRIO.Muon_on():
   from MuonTruthAlgs.MuonTruthAlgsConf import MuonPRD_MultiTruthMaker
   topSequence+=MuonPRD_MultiTruthMaker()

   from AthenaCommon.CfgGetter import getService
   getService("AtlasTrackingGeometrySvc")
   from MuonTruthAlgs.MuonTruthAlgsConf import Muon__MuonTruthDecorationAlg
   topSequence += Muon__MuonTruthDecorationAlg("MuonTruthDecorationAlg")
   from MCTruthClassifier.MCTruthClassifierConf import MCTruthClassifier
   from AthenaCommon import CfgGetter
   topSequence.MuonTruthDecorationAlg.MCTruthClassifier = CfgGetter.getPublicTool(MCTruthClassifier(name="MCTruthClassifier",ParticleCaloExtensionTool=""))
   topSequence.MuonTruthDecorationAlg.SDOs=["RPC_SDO","TGC_SDO","MDT_SDO"]
   PRD_TruthMaps = ["RPC_TruthMap","TGC_TruthMap","MDT_TruthMap"]
   if (MuonGeometryFlags.hasSTGC() and MuonGeometryFlags.hasMM()):
       topSequence.MuonTruthDecorationAlg.SDOs+=["MM_SDO","sTGC_SDO"]
       PRD_TruthMaps += ["MM_TruthMap", "STGC_TruthMap"]
   if not MuonGeometryFlags.hasCSC(): topSequence.MuonTruthDecorationAlg.CSCSDOs = ""
   else: PRD_TruthMaps += ["CSC_TruthMap"]
   topSequence.MuonTruthDecorationAlg.PRD_TruthMaps = PRD_TruthMaps

   try:
       from PyUtils.MetaReaderPeeker import metadata
       truthStrategy = metadata['TruthStrategy']
       if truthStrategy in ['MC15','MC18','MC18LLP']:
           topSequence.MuonTruthDecorationAlg.BarcodeOffset = 10000000

   except:
       printfunc ("Failed to read /Simulation/Parameters/ metadata")
       pass

#load default tools:
if muonRecFlags.doStandalone() or muonRecFlags.doPseudoTracking():
    include ("MuonRecExample/MuonRecLoadTools.py")


if muonRecFlags.doPseudoTracking():
  include("MuonRecExample/MuonTrackTruthCreation.py")

if muonRecFlags.doStandalone():
    # 
    # Load reconstruction algorithms (MuonStandalone)
    #
    from MuonRecExample.MuonRec import muonRec

    import MuonCondAlg.MdtCondDbAlgConfig # MT-safe conditions access

    if rec.doTruth():   
        from MuonTruthAlgs.MuonTruthAlgsConf import MuonDetailedTrackTruthMaker
        from MuonTruthAlgs.MuonTruthAlgsConf import Muon__MuonSegmentTruthAssociationAlg
        from TrkTruthAlgs.TrkTruthAlgsConf import TrackTruthSelector
        from TrkTruthAlgs.TrkTruthAlgsConf import TrackParticleTruthAlg
        col =  "MuonSpectrometerTracks" 
        topSequence += MuonDetailedTrackTruthMaker(name="MuonStandaloneDetailedTrackTruthMaker", TrackCollectionNames = [col], HasCSC=MuonGeometryFlags.hasCSC(), HasSTgc=MuonGeometryFlags.hasSTGC(), HasMM=MuonGeometryFlags.hasMM())
        topSequence += TrackTruthSelector(name= col + "Selector", 
                                          DetailedTrackTruthName = col + "DetailedTruth",
                                          OutputName             = col + "Truth") 
        topSequence += TrackParticleTruthAlg(name = col+"TruthAlg",
                                             TrackTruthName=col+"Truth",
                                             TrackParticleName = "MuonSpectrometerTrackParticles" )

        topSequence += Muon__MuonSegmentTruthAssociationAlg("MuonSegmentTruthAssociationAlg")

        try:
            from PyUtils.MetaReaderPeeker import metadata
            truthStrategy = metadata['TruthStrategy']
            if truthStrategy in ['MC15', 'MC18', 'MC18LLP']:
                topSequence.MuonSegmentTruthAssociationAlg.BarcodeOffset = 10000000
        except:
            printfunc ("Failed to read /Simulation/Parameters/ metadata")
            pass

#--------------------------------------------------------------------------
# Apply alignment corrections to geometry
#--------------------------------------------------------------------------
if muonRecFlags.useAlignmentCorrections():
    from MuonRecExample import MuonAlignConfig

#--------------------------------------------------------------------------
# Make Calibration Ntuple or run Calibration Algorithm
#--------------------------------------------------------------------------
if muonRecFlags.doCalib() or muonRecFlags.doCalibNtuple():
    from MuonRecExample import MuonAlignConfig
    from MuonRecExample import MuonCalibConfig

#--------------------------------------------------------------------------
# Evaluate tracking performance
#--------------------------------------------------------------------------
if muonRecFlags.doTrackPerformance():
    include("MuonRecExample/MuonTrackPerformance_jobOptions.py")

if muonRecFlags.doMSVertex():
    from MSVertexRecoAlg.MSVertexRecoAlgConf import MSVertexRecoAlg
    topSequence+=MSVertexRecoAlg("MSVertexRecoAlg")
