from AthenaCommon.AppMgr import ToolSvc

from JetTagD3PDMaker.JetTagD3PDMakerKeys import JetTagD3PDKeys
from JetTagD3PDMaker.JetTagD3PDMakerFlags import JetTagD3PDFlags
from RecExConfig.RecFlags import rec


### hack to make it work until remi fixes the label in BuildVertexD3PDObject
JetTagD3PDKeys.PrimaryVertexGetterLabel = JetTagD3PDKeys.PrimaryVertexPrefix()

### switch on second muon
from BTagging.BTaggingFlags import BTaggingFlags
if BTaggingFlags.SecondSoftMu:
    JetTagD3PDFlags.AddSecondMuonCollection=True

### which ntuple configuration to load?
if JetTagD3PDFlags.NtupleType() == "":
    print "custom configuration used for JetTagD3PD"
if JetTagD3PDFlags.NtupleType() == "Debug":
    include("JetTagD3PDMaker/DebugNtuple_LoadConfig.py")
    print "Debug configuration used for JetTagD3PD"
if JetTagD3PDFlags.NtupleType() == "Full":
    include("JetTagD3PDMaker/FullNtuple_LoadConfig.py")
    print "Full configuration used for JetTagD3PD"
if JetTagD3PDFlags.NtupleType() == "Efficiency":
    include("JetTagD3PDMaker/EffNtuple_LoadConfig.py")
    print "Efficiency configuration used for JetTagD3PD"
    
## create the D3PD alg
import D3PDMakerCoreComps
alg = D3PDMakerCoreComps.MakerAlg(JetTagD3PDKeys.D3PDAlgName(), topSequence,
                                  file = JetTagD3PDKeys.D3PDFileName(),
                                  D3PDSvc = 'D3PD::RootD3PDSvc',
                                  tuplename=JetTagD3PDKeys.D3PDTupleName())

from JetTagD3PDMaker.BTaggingD3PD import BTaggingD3PD
BTaggingD3PD(alg, **(JetTagD3PDFlags.D3PDPropDict()))

