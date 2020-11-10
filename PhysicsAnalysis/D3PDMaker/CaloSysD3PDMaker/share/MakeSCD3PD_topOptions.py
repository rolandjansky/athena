# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

if "doSimpleSC" not in dir() : 
     doSimpleSC = False


from RecExConfig.RecFlags  import rec
from AthenaCommon.BeamFlags import jobproperties
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from AthenaCommon.GlobalFlags import globalflags
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
import math

bdir = "/usatlas/u/flanni/bnl_local/"
if "inputFile" not in dir():
     inputFile = bdir + "mc12_14TeV.159000.ParticleGenerator_nu_E50.recon.ESD.e1564_s1499_s1504_r4327_tid01204497_00/ESD.01204497._005853.pool.root.1"

athenaCommonFlags.FilesInput= [inputFile]
athenaCommonFlags.EvtMax = -1

from RecExConfig.RecFlags import rec
rec.readESD.set_Value_and_Lock(True)
if "outputFile" not in dir():
     tuple_name = "test.scd3pd.root"
else:
    tuple_name = outputFile

rec.doHist.set_Value_and_Lock(False)
rec.doCBNT.set_Value_and_Lock(False)
rec.doWriteTAGCOM.set_Value_and_Lock(False)
rec.doWriteTAG.set_Value_and_Lock(False)
rec.doWriteAOD.set_Value_and_Lock(False)
rec.doAOD.set_Value_and_Lock(False)
rec.doMonitoring.set_Value_and_Lock(False)
rec.readAOD.set_Value_and_Lock(False)
rec.doTruth.set_Value_and_Lock( True )
rec.doTrigger.set_Value_and_Lock( True )

#rec.AutoConfiguration.set_Value_and_Lock(['FieldAndGeo','BeamType', 'ConditionsTag', 'DoTruth', 'InputType']) 

from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags
D3PDMakerFlags.TruthSGKey = 'GEN_EVENT,GEN_AOD,TruthEvent'
D3PDMakerFlags.DoTrigger = True
D3PDMakerFlags.TruthWriteGeant.set_Value_and_Lock(True)
D3PDMakerFlags.TruthWriteHadrons.set_Value_and_Lock(True)

# RecExCommon
include ("RecExCommon/RecExCommon_topOptions.py")

if doSimpleSC:
     from LArL1Sim.LArL1SimConf import LArSCSimpleMaker
     topSequence+=LArSCSimpleMaker()
     

# D3PDMaker calo block

from D3PDMakerCoreComps.MakerAlg import *
from CaloSysD3PDMaker.ClusterD3PDObject import *
from CaloD3PDMaker.CaloCellD3PDObject import *
from CaloSysD3PDMaker.CaloInfoD3PDObject import *
from EventCommonD3PDMaker.EventInfoD3PDObject import *

### L1Calo

include("TileRecAlgs/TileCellToTTL1_jobOptions.py")

from L1CaloD3PDMaker.L1CaloCPMHits_D3PDObject  import L1CaloCPMHits_D3PDObject
from L1CaloD3PDMaker.L1CaloCPMRoI_D3PDObject   import L1CaloCPMRoI_D3PDObject
from L1CaloD3PDMaker.L1CaloCPMTower_D3PDObject import L1CaloCPMTower_D3PDObject

### L1Calo JEM
from L1CaloD3PDMaker.L1CaloJEMEtSums_D3PDObject  import L1CaloJEMEtSums_D3PDObject
from L1CaloD3PDMaker.L1CaloJEMHits_D3PDObject    import L1CaloJEMHits_D3PDObject
from L1CaloD3PDMaker.L1CaloJEMRoI_D3PDObject     import L1CaloJEMRoI_D3PDObject
from L1CaloD3PDMaker.L1CaloJetElement_D3PDObject import L1CaloJetElement_D3PDObject

### L1Calo CMM
from L1CaloD3PDMaker.L1CaloCMMCPHits_D3PDObject  import L1CaloCMMCPHits_D3PDObject
from L1CaloD3PDMaker.L1CaloCMMEtSums_D3PDObject  import L1CaloCMMEtSums_D3PDObject
from L1CaloD3PDMaker.L1CaloCMMJetHits_D3PDObject import L1CaloCMMJetHits_D3PDObject
from L1CaloD3PDMaker.L1CaloCMMRoI_D3PDObject     import L1CaloCMMRoI_D3PDObject

from L1CaloD3PDMaker.L1CaloPPM_D3PDObject import L1CaloPPM_D3PDObject
dbTTreeName = 'L1CaloDB'
from L1CaloD3PDMaker.L1CaloPPMdbMC_D3PDObject import L1CaloPPMdbMC_D3PDObject

from CaloSysD3PDMaker.EMClusterD3PDObject           import EMClusterD3PDObject
from CaloSysD3PDMaker.TowerD3PDObject               import TowerD3PDObject
from egammaD3PDMaker.ElectronD3PDObject import *
from egammaD3PDMaker.PhotonD3PDObject import *
from TruthD3PDMaker.TruthParticleD3PDObject import *

alg = MakerAlg("scD3PD", topSequence, file = tuple_name , D3PDSvc = 'D3PD::RootD3PDSvc')
alg += EventInfoD3PDObject (10)
alg += CaloInfoD3PDObject (10)
alg += ElectronD3PDObject(1)
alg += PhotonD3PDObject(1)
alg += ClusterD3PDObject (10,sgkey="CaloCalTopoCluster")
#alg += EMClusterD3PDObject (10)
alg += TruthParticleD3PDObject (10)

alg += L1CaloCPMHits_D3PDObject(level = 0, allowMissing = True)
alg += L1CaloCPMRoI_D3PDObject(level = 0, allowMissing = True)
alg += L1CaloCPMTower_D3PDObject( level=0, allowMissing = True  )

alg += L1CaloJEMEtSums_D3PDObject(level = 0, allowMissing = True)
alg += L1CaloJEMHits_D3PDObject(level = 0, allowMissing = True)
alg += L1CaloJEMRoI_D3PDObject(level = 0, allowMissing = True)
alg += L1CaloJetElement_D3PDObject(level = 0, allowMissing = True)

alg += L1CaloCMMCPHits_D3PDObject(level = 0, allowMissing = True)
alg += L1CaloCMMEtSums_D3PDObject(level = 0, allowMissing = True)
alg += L1CaloCMMJetHits_D3PDObject(level = 0, allowMissing = True)
alg += L1CaloCMMRoI_D3PDObject(level = 0, allowMissing = True)

alg += L1CaloPPM_D3PDObject( 0 )

myEtaCut = [0.4,0.5]
myPhiCut = [-math.pi,math.pi]
mySamplings = [1,2]
myCalo=[1]

from CaloSysD3PDMaker.SCD3PDObject import *

#alg += AllSCD3PDObject(0, prefix='sc_')
# alg += SCD3PDObject(0, CaloEtaCut=myEtaCut, CaloPhiCut=myPhiCut, CaloLayers=mySamplings, CaloDetectors=myCalo )
#alg += SCD3PDObject(0, prefix='sc_ntld_') # equivalent to TileDLayerOption=False
alg += SCD3PDObject(0, prefix='sc_', TileDLayerOption=True)
alg += TowerD3PDObject (10)

include('TrigEgammaD3PDMaker/Setup_RoILinksCnvTool_IBv2.py')
from TriggerD3PDMaker.EmTauROID3PDObject import *
from TrigEgammaD3PDMaker.TrigEgammaD3PD import *
#alg += InitTrigD3PDObject(1)
#if not hasattr( alg, "trig_L1_emtau_Filler" ):
#     alg += EmTauROID3PDObject( 0, prefix="trig_L1_emtau", sgkey="LVL1_EMTAU")
TrigEgammaD3PDObjects( alg, 1)
from TrigJetD3PDMaker.TrigJetD3PD import *
TrigJetD3PDObjects( alg, 1)
from TrigMissingETD3PDMaker.TrigMETD3PD import TrigMETD3PDObjects
TrigMETD3PDObjects(alg, 1)
from TrigTauD3PDMaker.TrigTauD3PD import TrigTauD3PDObjects
TrigTauD3PDObjects(alg, 0, False, False)
from EventCommonD3PDMaker.LBMetadataConfig           import LBMetadataConfig
from EventCommonD3PDMaker.CutFlowMetadataConfig        import CutFlowMetadataConfig
from TriggerD3PDMaker.BunchStructureMetadata         import addBunchStructureMetadata
alg.MetadataTools += [LBMetadataConfig()]
alg.MetadataTools += [CutFlowMetadataConfig(alg)]
addBunchStructureMetadata(alg)


#-------------------------------------------------------------------------------
def _args (level, name, kwin, **kw):
    kw = kw.copy()
    kw['level'] = level
    for (k, v) in list(kwin.items()):
        if k.startswith (name + '_'):
            kw[k[len(name)+1:]] = v
    return kw

# MET objects
from MissingETD3PDMaker.MissingETD3PDMakerFlags      import MissingETD3PDMakerFlags
from MissingETD3PDMaker.MissingETD3PDObject          import MissingETD3PDObject
from MissingETD3PDMaker.MissingETD3PDObject          import MissingETCompositionD3PDObject
from MissingETD3PDMaker.MissingETGoodnessD3PDObject  import MissingETGoodnessD3PDObject
from MissingETD3PDMaker.MissingETD3PDTriggerBitsObject import *
MissingETD3PDMakerFlags.METDefaultJetPrefix = 'jet_AntiKt6LCTopo_MET_'
MissingETD3PDMakerFlags.doMissingETRegions = True
METIncludes =      ['MET_RefEle_Comps', 'MET_RefJet_Comps', 'MET_RefMuon_Comps', 'MET_SoftJets_Comps', 'MET_MuonBoy_Comps','MET_MuonBoy_Regions', 'MET_RefMuon_Staco_Comps', 'MET_RefMuon_Muid_Comps', 'MET_RefTau_Comps', 'MET_CellOut_Comps', 'MET_CellOut_Eflow_Comps', 'MET_LocHadTopo_Comps','MET_LocHadTopo_Regions', 'MET_Muon_Isol_Staco_Comps', 'MET_Muon_NonIsol_Staco_Comps', 'MET_Muon_Total_Staco_Comps', 'MET_Final_Comps', 'MET_Muon_Isol_Muid_Comps', 'MET_Muon_NonIsol_Muid_Comps', 'MET_Muon_Total_Muid_Comps', 'MET_Truth_Comps',  'MET_RefMuon_Track_Staco_Comps','MET_RefMuon_Track_Staco_Regions', 'MET_RefMuon_Track_Muid_Comps', 'MET_RefMuon_Track_Muid_Regions','MET_Track_Comps', 'MET_TopoObj_Comps', 'MET_RefFinal_em_Comps', 'MET_RefJet_em_Comps','MET_RefEle_em_Comps','MET_RefGamma_em_Comps', 'MET_RefTau_em_Comps', 'MET_CellOut_em' 'MET_RefMuon_Track_em_Comps', 'MET_SoftJets_em_Comps', 'MET_RefMuon_em_Comps', 'MET_Truth_NonInt', 'MET_Truth_NonInt_SumEt', 'MET_Truth_Int', 'MET_Truth_Int_SumEt', 'MET_RefEle_SumEt', 'MET_RefJet_SumEt', 'MET_RefMuon_SumEt', 'MET_SoftJets_SumEt', 'MET_MuonBoy_SumEt', 'MET_RefMuon_Staco_SumEt', 'MET_RefMuon_Muid_SumEt', 'MET_RefTau_SumEt', 'MET_CellOut_SumEt', 'MET_CellOut_Eflow_SumEt', 'MET_LocHadTopo_SumEt', 'MET_Muon_Isol_Staco_SumEt', 'MET_Muon_NonIsol_Staco_SumEt', 'MET_Muon_Total_Staco_SumEt', 'MET_Final_SumEt', 'MET_Muon_Isol_Muid_SumEt', 'MET_Muon_NonIsol_Muid_SumEt', 'MET_Muon_Total_Muid_SumEt', 'MET_Truth_SumEt',  'MET_RefMuon_Track_Staco_SumEt', 'MET_RefMuon_Track_Muid_SumEt', 'MET_Track_SumEt', 'MET_TopoObj_SumEt', 'MET_RefFinal_em_SumEt', 'MET_RefJet_em_SumEt','MET_RefEle_em_SumEt','MET_RefGamma_em_SumEt', 'MET_RefTau_em_SumEt', 'MET_CellOut_em' 'MET_RefMuon_Track_em_SumEt', 'MET_SoftJets_em_SumEt', 'MET_RefMuon_em_SumEt', 'MET_RefMuon_Track_Comps', 'MET_RefMuon_Track_SumEt', 'MET_Muon_Isol_Comps', 'MET_Muon_NonIsol_Comps', 'MET_Muon_Total_Comps','MET_Muon_Isol_SumEt', 'MET_Muon_NonIsol_SumEt', 'MET_Muon_Total_SumEt',  'MET_CellOut_Regions']

METIncludes += [ 'MET_RefFinal_STVF','MET_CellOut_Eflow_STVF','MET_CellOut_Eflow_JetArea','MET_RefJet_JVF','MET_RefJet_JVFCut', 'MET_CellOut_Eflow_JetAreaJVF','MET_CellOut_Eflow_JetAreaRhoEta5JVF', 'MET_RefFinal_STVF_Comps' , 'MET_RefFinal_STVF_SumEt', ' MET_RefFinal_STVF_Regions']

kw={}
alg += MissingETD3PDObject(**_args (0, 'MissingEt', kw, allowMissing=True, include = METIncludes, exclude = ["MET_Cluster_Weights"]))

#####Jet objects


from JetD3PDMaker.JetD3PDObject                      import JetD3PDObject
from JetD3PDMaker.JetD3PDMakerConf                   import *
import JetD3PDMaker
from JetRecTools.JetRecToolsConf                     import *
from JetRec.JetMomentGetter                          import make_JetMomentGetter
jetIncludes =      ['Kinematics','DQMoments','EMFraction','JESMoments','EMScale','JVtx','BTagDefault','BTag',
                    'TrueFlavorComponents','Samplings','BTagComponents', 'Constituents', 'ConstituentScale','CaloQual',
                    'AssocTrackCont']

from QcdD3PDMaker.QcdD3PDMakerFlags  import QcdD3PDMakerFlags
QcdD3PDMakerFlags.doHR.set_Value_and_Lock(False)
QcdD3PDMakerFlags.doMuon.set_Value_and_Lock(False)
QcdD3PDMakerFlags.doTau.set_Value_and_Lock(False)
QcdD3PDMakerFlags.doElectron.set_Value_and_Lock(False)
QcdD3PDMakerFlags.doPhoton.set_Value_and_Lock(False)
QcdD3PDMakerFlags.doPhoton.set_Value_and_Lock(False)
QcdD3PDMakerFlags.doTowerJet.set_Value_and_Lock(False)
QcdD3PDMakerFlags.doTrackJet.set_Value_and_Lock(False)
QcdD3PDMakerFlags.doGSCJet.set_Value_and_Lock(False)
QcdD3PDMakerFlags.doTruthJet.set_Value_and_Lock(False)
QcdD3PDMakerFlags.doMoreTrig.set_Value_and_Lock(False)
QcdD3PDMakerFlags.doTrack.set_Value_and_Lock(False)
QcdD3PDMakerFlags.doMET.set_Value_and_Lock(False)
QcdD3PDMakerFlags.doClusterHad.set_Value_and_Lock(False)
QcdD3PDMakerFlags.doMuonSpShower2.set_Value_and_Lock(False)
QcdD3PDMakerFlags.doRecJet.set_Value_and_Lock(False)
QcdD3PDMakerFlags.doRecJetOnlyAdditional.set_Value_and_Lock(False)
#QcdD3PDMakerFlags.useSPESD.set_Value_and_Lock(False)
QcdD3PDMakerFlags.doTopoJet.set_Value_and_Lock(True)
QcdD3PDMakerFlags.doLCTopoJet.set_Value_and_Lock(True)

# https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/D3PDVariablesForPileup

flags=QcdD3PDMakerFlags
jetInclude=flags.jetInclude()
#jetInclude+=['Samplings','BTagComponents','BTagDefault','BTag']  # for slim
jetInclude += jetIncludes
jetIncludeNew=flags.jetInclude()
jetIncludeNew+=['BTagComponents','BTagDefault','BTag'] # for full
jetExclude=flags.jetExclude()

alg += JetD3PDObject(5, prefix = 'jet_AntiKt4TopoEM_',
                     sgkey = 'AntiKt4TopoEMJets' )  
alg += JetD3PDObject(5, prefix = 'jet_AntiKt4LCTopo_',
                     sgkey = 'AntiKt4LCTopoJets' )  
alg += JetD3PDObject(0, prefix = 'jet_AntiKt4Truth_',
                     sgkey = 'AntiKt4TruthJets')  

from QcdD3PDMaker.QcdEventShapeD3PDObject import EventShapeD3PDObject
alg += EventShapeD3PDObject(**_args(0, 'rho', kw))


#, include = jetInclude, exclude = jetExclude))

        

                                        
