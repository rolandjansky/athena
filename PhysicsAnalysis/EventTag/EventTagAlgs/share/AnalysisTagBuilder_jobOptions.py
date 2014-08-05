from EventTagAlgs.EventTagGlobal import EventTagGlobal

if not EventTagGlobal.InitEventTagBuilder:
    raise RunTimeError('EventTagBuilder has not been initialized')

# Combined Performance Groups - Tag Fragments
include ( "ElectronPhotonTagTools/EgammaTagTool_jobOptions.py" )
include ( "MuonTagTools/CombinedMuonTagTool_jobOptions.py" )
include ( "TauTagTools/TauIdentificationTagTool_jobOptions.py" )
include ( "JetMissingEtTagTools/JetMissingEtIdentificationTagTool_jobOptions.py" )
include ( "JetTaggingTagTools/JetTaggingTagTool_jobOptions.py" )

# Physics Groups - Tag Fragments
include ( "BPhysTagTools/BPhysTagTool_jobOptions.py" )
include ( "ExoticPhysTagTools/ExoticPhysTagTool_jobOptions.py" )
include ( "HeavyIonPhysTagTools/HeavyIonPhysTagTool_jobOptions.py" )
include ( "HiggsPhysTagTools/HiggsPhysTagTool_jobOptions.py" )
include ( "SUSYPhysTagTools/SUSYPhysTagTool_jobOptions.py" )
include ( "StandardModelPhysTagTools/StandardModelPhysTagTool_jobOptions.py" )
include ( "TopPhysTagTools/TopPhysTagTool_jobOptions.py" )
include ( "DPDUtils/DPDTagTool_jobOptions.py" )

from EventTagAlgs.EventTagAlgsConf import AnalysisTagBuilder

AnalysisTagBuilder = AnalysisTagBuilder(
  AttributeList=EventTagGlobal.AttributeList,
  EgammaTagTool=EgammaTagTool,
  CombinedMuonTagTool=CombinedMuonTagTool,
  TauIdentificationTagTool=TauIdentificationTagTool,
  JetMissingEtIdentificationTagTool=JetMissingEtIdentificationTagTool,
  JetTaggingTagTool=JetTaggingTagTool,
  BPhysTagTool=BPhysTagTool,
  ExoticPhysTagTool=ExoticPhysTagTool,
  HeavyIonPhysTagTool=HeavyIonPhysTagTool,
  HiggsPhysTagTool=HiggsPhysTagTool,
  SUSYPhysTagTool=SUSYPhysTagTool,
  StandardModelPhysTagTool=StandardModelPhysTagTool,
  TopPhysTagTool=TopPhysTagTool,
  DPDTagTool=DPDTagTool,
  NumEgamma=1,
  NumMuon=1,
  NumTau=1,
  NumJetMissinEt=1,
  NumJetTagging=1,
  NumBPhys=1,
  NumExoticPhys=1,
  NumHeavyIonPhys=1,
  NumHiggsPhys=1,
  NumSusyPhys=1,
  NumSMPhys=1,
  NumTopPhys=1,
  NumDpd=1,
  CheckAttribute=True)

topSequence += AnalysisTagBuilder
