
myjets = [ 'AntiKt4TopoEM','AntiKt6TopoEM','AntiKt4Truth' ]

from RecExConfig.RecFlags import rec
from RecExConfig.RecAlgsFlags import recAlgs
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags

athenaCommonFlags.EvtMax.set_Value_and_Lock(10)
athenaCommonFlags.SkipEvents.set_Value_and_Lock(0)

##athenaCommonFlags.FilesInput = [ '/afs/cern.ch/user/a/aad/scratch0/myESD.root' ]
#athenaCommonFlags.FilesInput = [ 'root://castoratlas//castor/cern.ch/user/a/aad/Test/data10_7TeV.00161948.physics_L1Calo.recon.ESD.r1647_tid186146_00/ESD.186146._000036.pool.root.1' ]

#athenaCommonFlags.FilesInput = [ 'root://castoratlas//castor/cern.ch/user/a/aad/Test/data10_7TeV.00161948.physics_Muons.recon.ESD.r1647_tid186186_00/ESD.186186._001566.pool.root.1' ]

#athenaCommonFlags.FilesInput = [ 'root://castoratlas//castor/cern.ch/user/a/aad/Test/mc09_7TeV.113145.AlpgenJimmyNjetsNp4_J5.recon.ESD.e530_s765_s767_r1430_tid173744_00/ESD.173744._000015.pool.root.6' ]

#athenaCommonFlags.FilesInput = ['root://castoratlas//castor/cern.ch/user/a/aad/Test/mc10_7TeV.109279.J3_pythia_jetjet_1muon.recon.ESD.e574_s933_s946_r1652_tid190585_00/ESD.190585._000048.pool.root.1' ]

athenaCommonFlags.FilesInput = ['root://castoratlas//castor/cern.ch/user/a/aad/Test/mc10_7TeV.109279.J3_pythia_jetjet_1muon.recon.ESD.e574_s933_s946_r1831_tid214420_00/ESD.214420._000266.pool.root.1']

#athenaCommonFlags.FilesInput = ['/tmp/aad/ESD.213390._001231.pool.root.1']

#athenaCommonFlags.FilesInput = ['root://castoratlas//castor/cern.ch/user/a/aad/Test/mc10_7TeV.107281.AlpgenJimmyWbbFullNp1_pt20.merge.AOD.e600_s933_s946_r1831_r1700_tid219424_00/AOD.219424._000005.pool.root.1']



from TriggerJobOpts.TriggerConfigGetter import TriggerConfigGetter
cfg = TriggerConfigGetter("ReadPool")

rec.doTrigger.set_Value_and_Lock(False)
rec.readAOD.set_Value_and_Lock(False)
rec.readESD.set_Value_and_Lock(True)
rec.doESD.set_Value_and_Lock(False)
rec.doAOD.set_Value_and_Lock(False)
rec.doWriteAOD.set_Value_and_Lock(False)
rec.doWriteTAG.set_Value_and_Lock(False)
rec.doCBNT.set_Value_and_Lock(False)
rec.doHist.set_Value_and_Lock(False)
rec.doDumpTES.set_Value_and_Lock(False)
rec.doPerfMon.set_Value_and_Lock(False)
rec.doDetailedPerfMon.set_Value_and_Lock(False)


# --- jet builder configuration:
# specific configuration for fixes from Jet group
from JetRec.JetRecFlags import jetFlags
jetFlags.noStandardConfig = True
jetFlags.doJVF            = False
jetFlags.doBTagging       = True
jetFlags.inputFileType    = "ESD"
jetFlags.Enabled          = False

# the main jobOpt
include("RecExCommon/RecExCommon_topOptions.py")


# --- b-tagging configuration:
if not 'BTaggingFlags' in dir():
  from BTagging.BTaggingFlags import BTaggingFlags
BTaggingFlags.Runmodus                   = "analysis"
BTaggingFlags.Jets                       = myjets[:]
BTaggingFlags.JetsWithInfoPlus           = myjets[:]
BTaggingFlags.OutputLevel                = INFO
BTaggingFlags.doJetTagNtuple             = True
BTaggingFlags.SoftMuChi2=True
BTaggingFlags.writeSecondaryVertices=True
BTaggingFlags.SecondSoftMu=True
BTaggingFlags.IP2DFlip=True
BTaggingFlags.IP2DPos=True
BTaggingFlags.IP2DSpc=True
BTaggingFlags.IP2DSpcFlip=True
BTaggingFlags.IP2DSpcPos=True
BTaggingFlags.IP2DSpcNeg=True
BTaggingFlags.IP3DFlip=True
BTaggingFlags.IP3DPos=True
BTaggingFlags.IP3DSpc=True
BTaggingFlags.IP3DSpcFlip=True
BTaggingFlags.IP3DSpcPos=True
BTaggingFlags.IP3DSpcNeg=True
BTaggingFlags.SV1Flip =True
BTaggingFlags.SV2Flip=True
BTaggingFlags.JetFitterNNFlip=True

## configure JetTagNtuple
BTaggingFlags.UseLatestJetCollsForNtuple=True
BTaggingFlags.JetsForNtuple=["AntiKt4TopoEMJets", "AntiKt6TopoEMJets"] 
BTaggingFlags.doJetTagSlimNtuple=False
BTaggingFlags.doJetTagFakeRateNtuple=False
BTaggingFlags.doJetTagEfficiencyNtuple=False
##if rec.doTruth:
##  BTaggingFlags.JetsForNtuple+=['AntiKt4TruthJetsAOD']

include( "BTagging/BTagging_LoadTools.py" )
include( "BTagging/BTagging_jobOptions.py" )

## no track selection yet in TrackD3PD
## stop selection in ntuple

ToolSvc.JetTagNtupleTrackTool.RecoTrackPtCut=-1;
ToolSvc.JetTagNtupleTrackTool.RecoTrackEtaCut=999999;
ToolSvc.JetTagNtupleTrackTool.TrueTrackPtCut=100.;
ToolSvc.JetTagNtupleTrackTool.TrueTrackEtaCut=3.;

#include("McParticleAlgs/TruthParticleBuilder_jobOptions.py")


## configure D3PD
from D3PDMakerConfig.D3PDMakerFlags  import D3PDMakerFlags
D3PDMakerFlags.TruthSGKey = "TruthEvent"


from JetTagD3PDMaker.JetTagD3PDMakerFlags import JetTagD3PDFlags
JetTagD3PDFlags.JetCollections=['AntiKt4TopoEMJetsAOD',
                                'AntiKt6TopoEMJetsAOD']

if rec.doTruth:
  JetTagD3PDFlags.JetCollections+=['AntiKt4TruthJetsAOD']



JetTagD3PDFlags.NtupleType = ""

include("JetTagD3PDMaker/FullNtuple_LoadConfig.py")



### change some flags
JetTagD3PDFlags.MCPtMinCut = -1.
JetTagD3PDFlags.MCEtaCut =-1.
JetTagD3PDFlags.MCEmptyEvents = True


from TruthD3PDMaker.TruthD3PDMakerFlags import TruthD3PDFlags
TruthD3PDFlags.GenVertexOutPartAssoc = True
TruthD3PDFlags.GenVertexInPartAssoc = True


## put the d3dp tree in the same file as the jettagntuple
from JetTagD3PDMaker.JetTagD3PDMakerKeys import JetTagD3PDKeys
JetTagD3PDKeys.D3PDFileName = "JetTagNtuple.root"

include ("JetTagD3PDMaker/JetTagD3PD_jobOptions.py")

#include ("TruthD3PDMaker/GenD3PDExample_jobOptions.py")

print "List of all top algorithms"
print theApp.TopAlg
print topSequence
#ServiceMgr.StoreGateSvc.Dump=True

from AthenaCommon.AlgSequence import *
dumpSequence(topSequence)


