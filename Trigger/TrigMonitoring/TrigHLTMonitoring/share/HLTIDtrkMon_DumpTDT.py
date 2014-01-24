#
# Author: S.Ask
#
# Run HLT ID Monitoring on ESD
# (copied from AnalysisExamples/ZeeOnESDExample_jobOptions.py)
#

##### Standard ESD Part ###################

# Standard and detailed monitoring
IDtrkMon  = False  
IDtrkDump = True
isCollisions = False

# get a handle on the ServiceManager which holds all the services
from AthenaCommon.AppMgr import ServiceMgr

# Event selector
import AthenaPoolCnvSvc.ReadAthenaPool
ServiceMgr.EventSelector.BackNavigation = True

# Particle Properties
from PartPropSvc.PartPropSvcConf import PartPropSvc

# the POOL converters
include( "ParticleBuilderOptions/ESD_PoolCnv_jobOptions.py" )
include( "ParticleBuilderOptions/AOD_PoolCnv_jobOptions.py" )
include( "ParticleBuilderOptions/McAOD_PoolCnv_jobOptions.py" )
include( "EventAthenaPool/EventAthenaPool_joboptions.py" )

# The ESD input files
#ServiceMgr.EventSelector.InputCollections = [ "/atlas/data3/userdata/nurse/TriggerMonitoring/ESD_tier0_14503_290109.pool.root" ]
ServiceMgr.EventSelector.InputCollections = [ "/tmp/nurse/ESD_tier0_14503_290109.pool.root" ]

## ServiceMgr.EventSelector.InputCollections = [
##     "rfio:/castor/cern.ch/grid/atlas/atlasmcdisk/valid1/ESD/valid1.105001.pythia_minbias.recon.ESD.e380_s558_r705_tid071701/ESD.071701._000001.pool.root.1",
    
##     "rfio:/castor/cern.ch/grid/atlas/atlasmcdisk/valid1/ESD/valid1.105001.pythia_minbias.recon.ESD.e380_s558_r705_tid071701/ESD.071701._000002.pool.root.1",
##     "rfio:/castor/cern.ch/grid/atlas/atlasmcdisk/valid1/ESD/valid1.105001.pythia_minbias.recon.ESD.e380_s558_r705_tid071701/ESD.071701._000003.pool.root.1",
##     "rfio:/castor/cern.ch/grid/atlas/atlasmcdisk/valid1/ESD/valid1.105001.pythia_minbias.recon.ESD.e380_s558_r705_tid071701/ESD.071701._000004.pool.root.1",
##     "rfio:/castor/cern.ch/grid/atlas/atlasmcdisk/valid1/ESD/valid1.105001.pythia_minbias.recon.ESD.e380_s558_r705_tid071701/ESD.071701._000005.pool.root.1",
##     "rfio:/castor/cern.ch/grid/atlas/atlasmcdisk/valid1/ESD/valid1.105001.pythia_minbias.recon.ESD.e380_s558_r705_tid071701/ESD.071701._000006.pool.root.1",
##     "rfio:/castor/cern.ch/grid/atlas/atlasmcdisk/valid1/ESD/valid1.105001.pythia_minbias.recon.ESD.e380_s558_r705_tid071701/ESD.071701._000007.pool.root.1",
##     "rfio:/castor/cern.ch/grid/atlas/atlasmcdisk/valid1/ESD/valid1.105001.pythia_minbias.recon.ESD.e380_s558_r705_tid071701/ESD.071701._000008.pool.root.1",
##     "rfio:/castor/cern.ch/grid/atlas/atlasmcdisk/valid1/ESD/valid1.105001.pythia_minbias.recon.ESD.e380_s558_r705_tid071701/ESD.071701._000009.pool.root.1",
##     "rfio:/castor/cern.ch/grid/atlas/atlasmcdisk/valid1/ESD/valid1.105001.pythia_minbias.recon.ESD.e380_s558_r705_tid071701/ESD.071701._000010.pool.root.1",
##     "rfio:/castor/cern.ch/grid/atlas/atlasmcdisk/valid1/ESD/valid1.105001.pythia_minbias.recon.ESD.e380_s558_r705_tid071701/ESD.071701._000011.pool.root.1",
##     "rfio:/castor/cern.ch/grid/atlas/atlasmcdisk/valid1/ESD/valid1.105001.pythia_minbias.recon.ESD.e380_s558_r705_tid071701/ESD.071701._000012.pool.root.1",
##     "rfio:/castor/cern.ch/grid/atlas/atlasmcdisk/valid1/ESD/valid1.105001.pythia_minbias.recon.ESD.e380_s558_r705_tid071701/ESD.071701._000013.pool.root.1",
##     "rfio:/castor/cern.ch/grid/atlas/atlasmcdisk/valid1/ESD/valid1.105001.pythia_minbias.recon.ESD.e380_s558_r705_tid071701/ESD.071701._000014.pool.root.1",
##    "rfio:/castor/cern.ch/grid/atlas/atlasmcdisk/valid1/ESD/valid1.105001.pythia_minbias.recon.ESD.e380_s558_r705_tid071701/ESD.071701._000015.pool.root.1",
##     "rfio:/castor/cern.ch/grid/atlas/atlasmcdisk/valid1/ESD/valid1.105001.pythia_minbias.recon.ESD.e380_s558_r705_tid071701/ESD.071701._000016.pool.root.1",
##     "rfio:/castor/cern.ch/grid/atlas/atlasmcdisk/valid1/ESD/valid1.105001.pythia_minbias.recon.ESD.e380_s558_r705_tid071701/ESD.071701._000017.pool.root.1",
##     "rfio:/castor/cern.ch/grid/atlas/atlasmcdisk/valid1/ESD/valid1.105001.pythia_minbias.recon.ESD.e380_s558_r705_tid071701/ESD.071701._000018.pool.root.1",
##     "rfio:/castor/cern.ch/grid/atlas/atlasmcdisk/valid1/ESD/valid1.105001.pythia_minbias.recon.ESD.e380_s558_r705_tid071701/ESD.071701._000019.pool.root.1",
##     "rfio:/castor/cern.ch/grid/atlas/atlasmcdisk/valid1/ESD/valid1.105001.pythia_minbias.recon.ESD.e380_s558_r705_tid071701/ESD.071701._000020.pool.root.1",
##     "rfio:/castor/cern.ch/grid/atlas/atlasmcdisk/valid1/ESD/valid1.105001.pythia_minbias.recon.ESD.e380_s558_r705_tid071701/ESD.071701._000021.pool.root.1",
##     "rfio:/castor/cern.ch/grid/atlas/atlasmcdisk/valid1/ESD/valid1.105001.pythia_minbias.recon.ESD.e380_s558_r705_tid071701/ESD.071701._000022.pool.root.1",
##     "rfio:/castor/cern.ch/grid/atlas/atlasmcdisk/valid1/ESD/valid1.105001.pythia_minbias.recon.ESD.e380_s558_r705_tid071701/ESD.071701._000023.pool.root.1",
##     "rfio:/castor/cern.ch/grid/atlas/atlasmcdisk/valid1/ESD/valid1.105001.pythia_minbias.recon.ESD.e380_s558_r705_tid071701/ESD.071701._000024.pool.root.1",
##     "rfio:/castor/cern.ch/grid/atlas/atlasmcdisk/valid1/ESD/valid1.105001.pythia_minbias.recon.ESD.e380_s558_r705_tid071701/ESD.071701._000025.pool.root.1",
##     "rfio:/castor/cern.ch/grid/atlas/atlasmcdisk/valid1/ESD/valid1.105001.pythia_minbias.recon.ESD.e380_s558_r705_tid071701/ESD.071701._000026.pool.root.1",
##     "rfio:/castor/cern.ch/grid/atlas/atlasmcdisk/valid1/ESD/valid1.105001.pythia_minbias.recon.ESD.e380_s558_r705_tid071701/ESD.071701._000027.pool.root.1",
##     "rfio:/castor/cern.ch/grid/atlas/atlasmcdisk/valid1/ESD/valid1.105001.pythia_minbias.recon.ESD.e380_s558_r705_tid071701/ESD.071701._000028.pool.root.1",
##     "rfio:/castor/cern.ch/grid/atlas/atlasmcdisk/valid1/ESD/valid1.105001.pythia_minbias.recon.ESD.e380_s558_r705_tid071701/ESD.071701._000029.pool.root.1",
##     "rfio:/castor/cern.ch/grid/atlas/atlasmcdisk/valid1/ESD/valid1.105001.pythia_minbias.recon.ESD.e380_s558_r705_tid071701/ESD.071701._000030.pool.root.1",
##     "rfio:/castor/cern.ch/grid/atlas/atlasmcdisk/valid1/ESD/valid1.105001.pythia_minbias.recon.ESD.e380_s558_r705_tid071701/ESD.071701._000031.pool.root.1",
##     "rfio:/castor/cern.ch/grid/atlas/atlasmcdisk/valid1/ESD/valid1.105001.pythia_minbias.recon.ESD.e380_s558_r705_tid071701/ESD.071701._000032.pool.root.1",
##     "rfio:/castor/cern.ch/grid/atlas/atlasmcdisk/valid1/ESD/valid1.105001.pythia_minbias.recon.ESD.e380_s558_r705_tid071701/ESD.071701._000033.pool.root.1",
##     "rfio:/castor/cern.ch/grid/atlas/atlasmcdisk/valid1/ESD/valid1.105001.pythia_minbias.recon.ESD.e380_s558_r705_tid071701/ESD.071701._000034.pool.root.1",
##     "rfio:/castor/cern.ch/grid/atlas/atlasmcdisk/valid1/ESD/valid1.105001.pythia_minbias.recon.ESD.e380_s558_r705_tid071701/ESD.071701._000035.pool.root.1",
##     "rfio:/castor/cern.ch/grid/atlas/atlasmcdisk/valid1/ESD/valid1.105001.pythia_minbias.recon.ESD.e380_s558_r705_tid071701/ESD.071701._000036.pool.root.1",
##     "rfio:/castor/cern.ch/grid/atlas/atlasmcdisk/valid1/ESD/valid1.105001.pythia_minbias.recon.ESD.e380_s558_r705_tid071701/ESD.071701._000037.pool.root.1",
##     "rfio:/castor/cern.ch/grid/atlas/atlasmcdisk/valid1/ESD/valid1.105001.pythia_minbias.recon.ESD.e380_s558_r705_tid071701/ESD.071701._000038.pool.root.1",
##     "rfio:/castor/cern.ch/grid/atlas/atlasmcdisk/valid1/ESD/valid1.105001.pythia_minbias.recon.ESD.e380_s558_r705_tid071701/ESD.071701._000039.pool.root.1",
##     "rfio:/castor/cern.ch/grid/atlas/atlasmcdisk/valid1/ESD/valid1.105001.pythia_minbias.recon.ESD.e380_s558_r705_tid071701/ESD.071701._000040.pool.root.1"
##     ]

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

##### Trigger Decision Part #####################

## set up trigger decision tool
from TrigDecisionTool.TrigDecisionToolConf import Trig__TrigDecisionTool
tdt = Trig__TrigDecisionTool("TrigDecisionTool")
ToolSvc += tdt
tdt.OutputLevel = INFO

# might be needed for TriggerConfigGetter...
from RecExConfig.RecFlags  import rec
rec.readRDO=False
rec.readAOD=True
rec.doWriteAOD=False
rec.doWriteESD=False

# To read files with trigger config stored as in-file meta-data,
# i.e. 13.0.40 and above: ds
# To read AOD produced with 13.0.30 you need to change ds to aod:
from TriggerJobOpts.TriggerFlags import TriggerFlags
TriggerFlags.configurationSourceList = ['ds']

# set up trigger config service
from TriggerJobOpts.TriggerConfigGetter import TriggerConfigGetter
cfg =  TriggerConfigGetter()


##### Monitoring Part #####################

from AthenaMonitoring.AthenaMonitoringConf import AthenaMonManager
topSequence += AthenaMonManager( "HLTMonManager")
HLTMonManager = topSequence.HLTMonManager

##### HLTIDtrkMon Part ####################

if (IDtrkMon):
    from TrigHLTMonitoring.TrigHLTMonitoringConf import HLTIDtrkMonTool
    HLTIDtrkMon = HLTIDtrkMonTool(name               = 'HLTIDtrkMon',
                                  histoPathBase      = "/Trigger/HLT");
    HLTIDtrkMon.doMonCommon      = True
    HLTIDtrkMon.ElectronCollName = "n_Electron"
    HLTIDtrkMon.TauCollName      = "n_Tau"
    HLTIDtrkMon.TRTOnlyElectronCollName  = "TRTOnly_Electron"
    HLTIDtrkMon.CombinedElectronCollName = "Combined_Electron"
    HLTIDtrkMon.CosmicsCollName          = "CosmicsN"
    HLTIDtrkMon.IDSCANCollectionType   = "IDSCAN"
    HLTIDtrkMon.SiTrackCollectionType  = "SiTrack"
    HLTIDtrkMon.TRTCollectionType      = "TRT"
    HLTIDtrkMon.OfflineCollection      = "TrackParticleCandidate"
    HLTIDtrkMon.CombEFCollection       = "CosmicsN"
    HLTIDtrkMon.CombIDSCANCollection   = "Cosmics"
    HLTIDtrkMon.CombSiTrackCollection  = "Cosmics"
    HLTIDtrkMon.CombTRTCollection      = "SegmentFinder"
    HLTIDtrkMon.DeltaRMatch = 9.99
    HLTIDtrkMon.PtMin       = 0.
    HLTIDtrkMon.EtaMax      = 5.0
    HLTIDtrkMon.SiHitMin    = 3
    ToolSvc += HLTIDtrkMon;
    HLTMonManager.AthenaMonTools += [ "HLTIDtrkMonTool/HLTIDtrkMon" ];

    
##### HLTIDtrkMon Part ####################

if (IDtrkDump):
    from TrigHLTMonitoring.TrigHLTMonitoringConf import HLTIDtrkDumpTool
    HLTIDtrkDump = HLTIDtrkDumpTool(name               = 'HLTIDtrkDump',
                                    histoPathBase      = "/Trigger/HLT");
    HLTIDtrkDump.UseTrigDecisionTool = True
    HLTIDtrkDump.Data                = True                      # true = data, false = MC
    HLTIDtrkDump.OfflineCollection   = "TrackParticleCandidate"
    HLTIDtrkDump.TrackVariables      = ["pT", "phi", "eta"]      # pT, phi, eta
    HLTIDtrkDump.DeltaR    = 0.02
    HLTIDtrkDump.MinPt     = 1000.0
    HLTIDtrkDump.MaxAbsEta = 2.5
    HLTIDtrkDump.MinSiHits = 3

 
    if (isCollisions):

        HLTIDtrkDump.DeltaR    = 0.02

        HLTIDtrkDump.eGammaMon               = True
        HLTIDtrkDump.eGammaROIHalfWidth      = 0.1
        HLTIDtrkDump.eGammaTrigSigTracks     = ["e20_loose_NoTrkCut"]                  # The size of the vector of signatures has to match
        HLTIDtrkDump.eGammaTrigSigSelect     = ["e20_loose_NoTrkCut"]                  # "selecting signature" vector size 
        HLTIDtrkDump.eGammaIDSCAN            = [True]                                  # and the following parameters
        HLTIDtrkDump.eGammaSiTrack           = [True]                
        HLTIDtrkDump.eGammaTRTSegF           = [True]
        HLTIDtrkDump.eGammaEF                = [True]
        HLTIDtrkDump.eGammaIDSCANCollection  = ["TrigIDSCAN_eGamma"]
        HLTIDtrkDump.eGammaSiTrackCollection = ["TrigSiTrack_eGamma"]
        HLTIDtrkDump.eGammaTRTSegFCollection = ["TrigTRTSegFinder_eGamma"]
        HLTIDtrkDump.eGammaEFCollection      = ["InDetTrigParticleCreation_eGamma_EFID"]
    
        HLTIDtrkDump.TauMon                  = True
        HLTIDtrkDump.TauROIHalfWidth         = 0.3
        HLTIDtrkDump.TauTrigSigTracks        = ["tauNoCut_NoTrkCut","tau16i_NoTrkCut"] # The size of the vector of signatures has to match
        HLTIDtrkDump.TauTrigSigSelect        = ["tauNoCut_NoTrkCut","tau16i_NoTrkCut"] # "selecting signature" vector size 
        HLTIDtrkDump.TauIDSCAN               = [True,True]                             # and the following parameters
        HLTIDtrkDump.TauSiTrack              = [True,True]                
        HLTIDtrkDump.TauTRTSegF              = [True,True]
        HLTIDtrkDump.TauEF                   = [True, True]
        HLTIDtrkDump.TauIDSCANCollection     = ["TrigIDSCAN_Tau","TrigIDSCAN_Tau"]
        HLTIDtrkDump.TauSiTrackCollection    = ["TrigSiTrack_Tau","TrigSiTrack_Tau"]
        HLTIDtrkDump.TauTRTSegFCollection    = ["TrigTRTSegFinder_Tau","TRTSegmentFinder"]
        HLTIDtrkDump.TauEFCollection         = ["InDetTrigParticleCreation_Tau_EFID","InDetTrigParticleCreation_Tau_EFID"]


        HLTIDtrkDump.MuMon                   = True
        HLTIDtrkDump.MuROIHalfWidth          = 0.1
        HLTIDtrkDump.MuTrigSigTracks         = ["mu20","mu10_SiTrack","mu10_TRT"]          # The size of the vector of signatures has to match
        HLTIDtrkDump.MuTrigSigSelect         = ["mu20_MSonly","mu20_MSonly","mu20_MSonly"] # "selecting signature" vector size 
        HLTIDtrkDump.MuIDSCAN                = [True,False,False]                          # and the following parameters
        HLTIDtrkDump.MuSiTrack               = [False,True,False]
        HLTIDtrkDump.MuTRTSegF               = [False,False,True]
        HLTIDtrkDump.MuEF                    = [True,True,True]
        HLTIDtrkDump.MuIDSCANCollection      = ["TrigIDSCAN_Muon","TrigIDSCAN_Muon","TrigIDSCAN_Muon"]
        HLTIDtrkDump.MuSiTrackCollection     = ["TrigSiTrack_Muon","TrigSiTrack_Muon","TrigSiTrack_Muon"]
        HLTIDtrkDump.MuTRTSegFCollection     = ["TrigTRTSegFinder_Muon","TrigTRTSegFinder_Muon","TrigTRTSegFinder_Muon"]
        HLTIDtrkDump.MuEFCollection          = ["InDetTrigParticleCreation_Muon_EFID","InDetTrigParticleCreation_Muon_EFID","InDetTrigParticleCreation_Muon_EFID"]

        
    else :

        HLTIDtrkDump.DeltaR    = 9.99
        HLTIDtrkDump.CosMon                      = True
        HLTIDtrkDump.CosROIHalfWidth             = 3.2
        HLTIDtrkDump.CosTrigSigTracks            = ["CosNoTDT"]                        # The size of the vector of signatures has to match
        HLTIDtrkDump.CosTrigSigSelect            = ["CosNoTDT"]                        # "selecting signature" vector size 
        HLTIDtrkDump.CosIDSCAN                   = [True]                              # and the following parameters
        HLTIDtrkDump.CosSiTrack                  = [True]   
        HLTIDtrkDump.CosTRTSegF                  = [True]
        HLTIDtrkDump.CosEF                       = [True]
        HLTIDtrkDump.CosIDSCANCollection         = ["HLT_TrigIDSCAN_Cosmics"]
        HLTIDtrkDump.CosSiTrackCollection        = ["HLT_TrigSiTrack_Cosmics"]
        HLTIDtrkDump.CosTRTSegFCollection        = ["HLT_TRTSegmentFinder"]
        HLTIDtrkDump.CosEFCollection             = ["HLT_InDetTrigParticleCreationIOTRT_CosmicsN_EFID"]


    
    ToolSvc += HLTIDtrkDump;
    HLTMonManager.AthenaMonTools += [ "HLTIDtrkDumpTool/HLTIDtrkDump" ];
    

##### Histogram File Part #################

from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr += THistSvc()
ServiceMgr.THistSvc.Output = ["AANT DATAFILE='HLTIDtrkMon_DumpTDT.root' OPT='RECREATE'"]
HLTMonManager.FileKey = "AANT"

# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
ServiceMgr.MessageSvc.OutputLevel = INFO

# Number of Events to process
theApp.EvtMax = -1

#--------------------------------------------------------------
# Dump StoreGate (for tests)
#--------------------------------------------------------------
#StoreGateSvc = Service( "StoreGateSvc" )
#StoreGateSvc.Dump = True  #true will dump data store contents

