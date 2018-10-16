#########################################################################################################################################################
#########################################################################################################################################################
### Define input xAOD and output ntuple file name
import glob
from AthenaCommon.AthenaCommonFlags import jobproperties as jp
#jp.AthenaCommonFlags.EvtMax.set_Value_and_Lock( vars().get('EVTMAX', -1) )
#jp.AthenaCommonFlags.SkipEvents.set_Value_and_Lock(16400)
#jp.AthenaCommonFlags.EvtMax.set_Value_and_Lock(5000)
jp.AthenaCommonFlags.EvtMax.set_Value_and_Lock(10)

#suffix = "mc16d"
#suffix = "mc16a"
suffix = "v1"

#sample = "bg"
sample = "signal"
#sample = "data"

if sample == "signal":

  jp.AthenaCommonFlags.FilesInput = [
    #'/afs/cern.ch/work/s/sdjones/SusxFlavTool/run/AOD.13167662._000004.pool.root.1' # on lxplus
    #"/lustre/scratch/epp/atlas/sdj21/Data/DAOD_SUSY7X.600_580.pool.root"
    "/lustre/scratch/epp/atlas/sdj21/Data/mc16_13TeV.390863.MGPy8EG_A14N23LO_TT_bffN_400_380_MadSpin_m100.merge.AOD.e5548_e5984_a875_r10201_r10210/AOD.13167662._000004.pool.root.1"
    #"/lustre/scratch/epp/atlas/sdj21/Data/mc16_13TeV.387040.MGPy8EG_A14N_BB_direct_600_580.merge.AOD.e3994_e5984_a875_r10201_r10210/AOD.13485052._000002.pool.root.1" # mc16d
    #"/lustre/scratch/epp/atlas/sdj21/Data/mc16_13TeV.387040.MGPy8EG_A14N_BB_direct_600_580.merge.AOD.e3994_e5984_a875_r9364_r9315/AOD.12760437._000001.pool.root.1" # mc16a
    ]


elif sample == "bg":

  jp.AthenaCommonFlags.FilesInput = [
    #"/lustre/scratch/epp/atlas/sdj21/MyFlavourTaggingFramework/data/mc16_13TeV/AOD.12975098._000481.pool.root.1" # Znunu b-filter
    #"/lustre/scratch/epp/atlas/sdj21/Data/ttbar_dilep/mc16_13TeV/AOD.13122714._000052.pool.root.1"
    "/lustre/scratch/epp/atlas/sdj21/Data/mc16_13TeV.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.merge.AOD.e5602_s3126_r10201_r10210/AOD.12977028._000001.pool.root.1"
    ]

elif sample == "data":

  jp.AthenaCommonFlags.FilesInput = [
    #"/lustre/scratch/epp/atlas/sdj21/Data/data17_13TeV.00339435.physics_Main.merge.AOD.f887_m1897._lb1619._0001.1"
    "/lustre/scratch/epp/atlas/sdj21/Data/AOD.13253856._009708.pool.root.1" # data17_13TeV.00338608.physics_Main.merge.AOD.r10258_p3399
    ]

# from PyUtils import AthFile
# af = AthFile.fopen( jp.AthenaCommonFlags.FilesInput()[0] )

evtPrintoutInterval = vars().get('EVTPRINT', 5000)
svcMgr += CfgMgr.AthenaEventLoopMgr( EventPrintoutInterval=evtPrintoutInterval )

##########################################################################################################################################################
##########################################################################################################################################################

from RecExConfig.RecFlags import rec
rec.doESD.set_Value_and_Lock        (False)
rec.doWriteESD.set_Value_and_Lock   (False)
rec.doAOD.set_Value_and_Lock        (False)
rec.doWriteAOD.set_Value_and_Lock   (False)
rec.doWriteTAG.set_Value_and_Lock   (False)
rec.doDPD.set_Value_and_Lock        (False)
rec.doTruth.set_Value_and_Lock      (False)


rec.doApplyAODFix.set_Value_and_Lock(True)
include ("RecExCommon/RecExCommon_topOptions.py")

if sample != "data":

  from AthenaCommon.AlgSequence import AlgSequence
  algSeq = AlgSequence()

else:

  from AthenaCommon.AlgSequence import AlgSequence
  algSeq = AlgSequence()
  #algSeq += CfgMgr.xAODMaker__DynVarFixerAlg("InDetTrackParticlesFixer", Containers = [ "InDetTrackParticlesAux." ] )

if sample != "data":

  from DerivationFrameworkMCTruth.MCTruthCommon import addStandardTruthContents
  addStandardTruthContents()

from DerivationFrameworkJetEtMiss.ExtendedJetCommon import replaceAODReducedJets

reducedJetList = ["AntiKt2PV0TrackJets", "AntiKt4PV0TrackJets", "AntiKt10LCTopoJets"]

replaceAODReducedJets(reducedJetList, algSeq, "SUSY1")

from BTagging.BTaggingFlags import BTaggingFlags

BTaggingFlags.CalibrationChannelAliases += ["AntiKtVR30Rmax4Rmin02Track->AntiKtVR30Rmax4Rmin02Track,AntiKt4EMTopo"]

##########################################################################################################################################################
##########################################################################################################################################################
### GEO Business
# from AthenaCommon.GlobalFlags import globalflags
# print "detDescr from global flags= "+str(globalflags.DetDescrVersion)
# from AtlasGeoModel.InDetGMJobProperties import GeometryFlags as geoFlags
# print "geoFlags.Run()   = "+geoFlags.Run()
# print "geoFlags.isIBL() = "+str(  geoFlags.isIBL() )


## Vadim's code

m_cone_for_tag = 0.75

from InDetVKalVxInJetTool.InDetVKalVxInJetFinder import InDetVKalVxInJetFinder
BJetSVFinderTool      = InDetVKalVxInJetFinder("BJetSVFinder")
ToolSvc += BJetSVFinderTool
BJetSVFinderTool.ConeForTag = m_cone_for_tag # for example

algSeq += CfgMgr.BTagVertexAugmenter()

alg = CfgMgr.SoftBVrt__SoftBVrtClusterTool(  "SoftBVrtClusterTool",
                           OutputLevel=INFO, #DEBUG
                           )

alg.TrackSelectionTool.CutLevel = "LoosePrimary"

alg.DoJetVeto = True

#grid_ND_0.5_1.5_0.75_0.25_1500

alg.SeedPt = 1500
alg.SeedZ0SinTheta = 1000000
alg.SeedZ0 = 100.0
alg.SeedZ0Significance = 0.0
alg.SeedD0Significance = 0.5
alg.ClusterDistance = 0.25
alg.ClusterZ0 = 10.0
alg.ClusterZ0Significance = 0.0
alg.ClusterD0Significance = 1.5
alg.ClusterDR = m_cone_for_tag

alg.JetCollectionName = 'AntiKt4EMTopoJets'
alg.TrackJetCollectionName = 'AntiKt4PV0TrackJets'

algSeq += alg
  
from PerfMonComps.PerfMonFlags import jobproperties as PerfMon_jp
PerfMon_jp.PerfMonFlags.doMonitoring = False
PerfMon_jp.PerfMonFlags.doFastMon = False

###########################################################################################################################################################################
