# MET standalone reconstruction jobOptions

infile = "/atlas/data1/userdata/khoo/Data16/AOD_r21/mc16_13TeV.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.merge.AOD.e3698_s2997_r8903_r8906/AOD.10226638._000244.pool.root.1"
from AthenaCommon.AppMgr import ServiceMgr
import AthenaPoolCnvSvc.ReadAthenaPool
ServiceMgr.EventSelector.InputCollections = [infile]

theApp.EvtMax = 50

from METReconstruction.METAssocConfig import AssocConfig, getAssociator
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from AthenaCommon import CfgMgr
trkseltool=CfgMgr.InDet__InDetTrackSelectionTool("IDTrkSel_METAssoc",
                                                    CutLevel="TightPrimary",
                                                    maxZ0SinTheta=3,
                                                    maxD0=2)
from AthenaCommon.AppMgr import ToolSvc
ToolSvc += trkseltool


import cppyy
try: cppyy.loadDictionary('METReconstructionDict')
except: pass
from ROOT import met
assocList = [
    CfgMgr.met__METJetAssocTool('MET_EMJetAssocTool__AthenaMT',
                                InputCollection="AntiKt4EMTopoJets"),
    CfgMgr.met__METMuonAssociator('MET_MuonAssociator__AthenaMT',
                                  InputCollection="Muons"),
    CfgMgr.met__METElectronAssociator('MET_ElectronAssociator__AthenaMT',
                                      InputCollection="Electrons",
                                      TCMatchMethod=met.ClusterLink),
    CfgMgr.met__METPhotonAssociator('MET_PhotonAssociator__AthenaMT',
                                    InputCollection="Photons",
                                    TCMatchMethod=met.ClusterLink),
    CfgMgr.met__METTauAssociator('MET_TauAssociator__AthenaMT',
                                 InputCollection = "TauJets"),
    CfgMgr.met__METSoftAssociator('MET_SoftAssociator__AthenaMT',
                                  DecorateSoftConst = True,
                                  LCModClusterKey = "LCOriginTopoClusters",
                                  EMModClusterKey = "EMOriginTopoClusters")
    ]
for assoc in assocList:
    assoc.UseModifiedClus=True
    assoc.ClusColl = "EMOriginTopoClusters"
    assoc.TrackSelectorTool = trkseltool
    #assoc.TrackIsolationTool = trkisotool
    #assoc.CaloIsolationTool = caloisotool
    ToolSvc += assoc

EMAssocTool = CfgMgr.met__METAssociationTool('MET_AssociationTool_AntiKt4EMTopo',
                                             METAssociators = assocList,
                                             METSuffix = 'MTAntiKt4EMTopo',
                                             TCSignalState=0) # EM clusters
ToolSvc += EMAssocTool

METAssocAlg_MT = CfgMgr.met__METRecoAlg(name='METAssociation_MT',RecoTools=[EMAssocTool])
topSequence += METAssocAlg_MT

metMaker = CfgMgr.met__METMaker('METMaker_MTAntiKt4EMTopo',
                                DoPFlow=False,
                                DoSoftTruth=False,
                                JetSelection="Tier0",
                                );
ToolSvc += metMaker

makerAlg = CfgMgr.met__METMakerAlg('METMakerAlg_MTAntiKt4EMTopo',
                                   METMapName='METAssoc_MTAntiKt4EMTopo',
                                   METCoreName='MET_Core_MTAntiKt4EMTopo',
                                   METName='MET_Reference_MTAntiKt4EMTopo',
                                   InputJets="AntiKt4EMTopoJets",
                                   Maker=metMaker,
                                   MuonSelectionTool=None,
                                   ElectronLHSelectionTool=None,
                                   PhotonIsEMSelectionTool=None,
                                   TauSelectionTool=None,
                                   )
topSequence += makerAlg


write_xAOD = True
if write_xAOD:
    from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
    xaodStream = MSMgr.NewPoolRootStream( "StreamAOD", "xAOD.pool.root" )
    xaodStream.AddItem("xAOD::EventInfo#EventInfo")
    xaodStream.AddItem("xAOD::EventAuxInfo#EventInfoAux.")
    #
    xaodStream.AddItem("xAOD::MissingETAssociationMap#MET_Assoc_AntiKt4EMTopo")
    xaodStream.AddItem("xAOD::MissingETAuxAssociationMap#MET_Assoc_AntiKt4EMTopoAux.")
    xaodStream.AddItem("xAOD::MissingETAssociationMap#MET_Assoc_MTAntiKt4EMTopo")
    xaodStream.AddItem("xAOD::MissingETAuxAssociationMap#MET_Assoc_MTAntiKt4EMTopoAux.")
    #
    xaodStream.AddItem("xAOD::MissingETContainer#MET_Reference_AntiKt4EMTopo")
    xaodStream.AddItem("xAOD::MissingETAuxContainer#MET_Reference_AntiKt4EMTopoAux.")
    xaodStream.AddItem("xAOD::MissingETContainer#MET_Reference_MTAntiKt4EMTopo")
    xaodStream.AddItem("xAOD::MissingETAuxContainer#MET_Reference_MTAntiKt4EMTopoAux.")


