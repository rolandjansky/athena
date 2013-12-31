from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from RecExConfig.RecFlags import rec
from RecExConfig.RecAlgsFlags import recAlgs

athenaCommonFlags.FilesInput = ["/home/dferreira/sample_data/valid1.105200.T1_McAtNlo_Jimmy.recon.AOD.e510_s765_s767_r1358_tid147795_00/AOD.147795._000039.pool.root.1"]
athenaCommonFlags.EvtMax = -1

rec.doCBNT=False
rec.doTrigger=True

rec.doWriteAOD=False
rec.doWriteESD=False
rec.doWriteTAG=False
rec.doAOD=False 
rec.doESD=False 
rec.doDPD=False
rec.doTruth=True

from InDetRecExample.InDetJobProperties import InDetFlags
InDetFlags.doTrackSegmentsPixel = True
InDetFlags.doTrackSegmentsSCT = True
InDetFlags.doTrackSegmentsTRT = True

include("RecExCond/RecExCommon_flags.py")

TriggerFlags.readHLTconfigFromXML=False
TriggerFlags.readLVL1configFromXML=False
TriggerFlags.useL1CaloCalibration = False

TriggerFlags.enableMonitoring = [ ]

TriggerFlags.triggerMenuSetup = 'MC_lumi1E31_simpleL1Calib_no_prescale'

from RecExConfig.RecConfFlags import recConfFlags
recConfFlags.AllowIgnoreConfigError=False
athenaCommonFlags.AllowIgnoreConfigError=False

def bjetOnly():
    TriggerFlags.Slices_all_setOff()
    TriggerFlags.BjetSlice.setAll()
    TriggerFlags.BphysicsSlice.signatures = []
    #TriggerFlags.BjetSlice.signatures = []
    #TriggerFlags.EgammaSlice.signatures = []
    TriggerFlags.JetSlice.signatures = []
    TriggerFlags.METSlice.signatures = []
    TriggerFlags.MinBiasSlice.signatures = []
    TriggerFlags.MuonSlice.signatures = []
    #TriggerFlags.TauSlice.signatures = []
    TriggerFlags.BjetSlice.signatures = TriggerFlags.BjetSlice.signatures()
    #+ [  ]

from TriggerMenuPython.GenerateMenu import GenerateMenu
GenerateMenu.overwriteSignaturesWith(bjetOnly)

###### This is a temporary fix ################
DetFlags.simulateLVL1.Muon_setOn()

include("RecExCommon/RecExCommon_topOptions.py")

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

# Add D3PD stuff ...
# ------------------- BJET D3PD LINES START --------------------------

# The stream is being created by hand, so we can also add the debug histograms ...
from AnalysisTools.AthAnalysisToolsConf import AANTupleStream
st = AANTupleStream ('BjetFile',
                     ExtraRefNames = ['StreamESD',
                                      'StreamRDO',
                                      'StreamAOD'],
                     OutputName = 'bjet.root',
                     WriteInputDataHeader = True,
                     StreamName = 'BjetFile')
topSequence += [st]

svcMgr.THistSvc.Output += ["BjetFile DATAFILE='bjet.root' OPT='RECREATE'"]

## D3PD Maker alg
import D3PDMakerCoreComps
alg = D3PDMakerCoreComps.MakerAlg('BjetTree',
                                  topSequence,
                                  'bjet.root',
                                  'BjetFile')

## EventInfo
from EventCommonD3PDMaker.EventInfoD3PDObject import EventInfoD3PDObject
alg += EventInfoD3PDObject(10, prefix = 'ei_')

# Offline Jet information - this is the only one with btagging information for now
from JetD3PDMaker.JetD3PDObject               import JetD3PDObject
alg += JetD3PDObject(10, sgkey='AntiKt4H1TopoJets', prefix='AntiKt4H1Topo_')

# L2 and EF Bjet code
from TrigBJetD3PDMaker.TrigBJetD3PDObject import TrigBJetD3PDObjects
TrigBJetD3PDObjects(alg, 10);

# Configuration metadata
from EventCommonD3PDMaker.LBMetadataConfig    import LBMetadataConfig
alg.MetadataTools += [LBMetadataConfig()]  

# ------------------ BJET D3PD LINES END ------------------------

# Extra D3PD blocks

# Electrons, photons and muons (Offline)
from egammaD3PDMaker.ElectronD3PDObject       import ElectronD3PDObject
alg += ElectronD3PDObject(10, prefix = 'el_')
from egammaD3PDMaker.PhotonD3PDObject         import PhotonD3PDObject
alg += PhotonD3PDObject(10, prefix = 'ph_')
from MuonD3PDMaker.MuonD3PDObject             import MuonD3PDObject
alg += MuonD3PDObject(10, prefix = 'mu_')

# Trigger Egamma information
from TrigEgammaD3PDMaker.TrigEgammaD3PD import TrigEgammaD3PDObjects
TrigEgammaD3PDObjects(alg, 10)

# Track information (Offline)
from TrackD3PDMaker.TrackD3PDObject import TrackParticleD3PDObject
alg += TrackParticleD3PDObject(8)
from TrackD3PDMaker.TrackD3PDObject import PixelTrackD3PDObject
#alg += PixelTrackD3PDObject(8)
from TrackD3PDMaker.TrackD3PDObject import SCTTrackD3PDObject
#alg += SCTTrackD3PDObject(8)
from TrackD3PDMaker.TrackD3PDObject import TRTTrackD3PDObject
#alg += TRTTrackD3PDObject(8)

# Primary Vertices
from TrackD3PDMaker.VertexD3PDObject import PrimaryVertexD3PDObject
alg += PrimaryVertexD3PDObject(10, allowMissing = True, sgkey = 'VxPrimaryCandidate', prefix='vxp_')

# jetMET triggers
from JetD3PDMaker.jetMETD3PDTrigger           import jetMETTriggerBitsD3PDObject
alg +=  jetMETTriggerBitsD3PDObject(10, prefix = 'jetMETTriggerBitsFiller_')

# Truth information
from TrackD3PDMaker.TruthVertexD3PDObject import TruthVertexD3PDObject
alg += TruthVertexD3PDObject(10, prefix = 'mcVx_', sgkey = "GEN_AOD")
from TrackD3PDMaker.TruthTrackD3PDObject import TruthTrackD3PDObject
alg += TruthTrackD3PDObject(10, prefix = 'mcTrk_', sgkey = "GEN_AOD")
from EventCommonD3PDMaker.TruthParticleD3PDObject import TruthParticleD3PDObject
alg += TruthParticleD3PDObject(10, prefix = 'mc_', sgkey = "GEN_AOD")
from EventCommonD3PDMaker.GenEventD3PDObject  import GenEventD3PDObject
alg += GenEventD3PDObject(10, prefix = 'mcEvt_', sgkey = "GEN_AOD")
#from JetD3PDMaker.JetD3PDObject               import JetD3PDObject
alg += JetD3PDObject(10, sgkey='AntiKt4TruthJets', prefix='AntiKt4Truth_')


# For debugging purposes:
#from TrigBJetD3PDMaker.TrigBJetD3PDMakerConf import BjetAnalysis
#bjetAnalysis = BjetAnalysis()
#bjetAnalysis.File = "BjetFile"
#bjetAnalysis.OutputLevel = DEBUG
#topSequence += bjetAnalysis

