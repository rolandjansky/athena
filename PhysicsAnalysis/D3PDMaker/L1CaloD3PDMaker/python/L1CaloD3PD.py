# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id$
#
# @file L1CaloD3PDMaker/python/L1CaloD3PD.py
# @author John Morris <john.morris@cern.ch>
# @brief Construct a L1Calo D3PD

from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags
D3PDMakerFlags.DoTrigger = False

import L1CaloD3PDMaker
from EventCommonD3PDMaker.EventInfoD3PDObject import EventInfoD3PDObject
from L1CaloD3PDMaker.L1CaloPPM_D3PDObject     import L1CaloPPM_D3PDObject

TTreeName = 'physics'
dbTTreeName = 'L1CaloDB'

from RecExConfig.RecFlags import rec
isMC = rec.doTruth.get_Value()

if isMC:
  from EventCommonD3PDMaker.GenEventD3PDObject import GenEventD3PDObject
  from TruthD3PDMaker.TruthParticleD3PDObject  import TruthParticleD3PDObject
  from TauD3PDMaker.TauD3PDMakerConf import TruthTausToSG
  from L1CaloD3PDMaker.L1CaloPPMdbMC_D3PDObject import L1CaloPPMdbMC_D3PDObject

if not isMC:
  from L1CaloD3PDMaker.L1CaloPPMdbData_D3PDObject import L1CaloPPMdbData_D3PDObject
  

METIncludes = ['MET_RefFinal','MET_Final','MET_Topo','MET_LocHadTopo','MET_Base','MET_TopoObj','MET_Base_Samplings']
JetIncludes = ['Kinematics','KineMoments','DQMoments','JetQual','EMFraction','JESMoments','EMScale','Samplings']
JetHLTIncludes = ['Kinematics','EMScale','JetRoIword']

if isMC:
  METIncludes.append('MET_Truth_NonInt')


def L1CaloD3PD(filename,
               level = 0,
               Jets = False,
               Egamma = False,
               MET = False,
               Taus = False,
               Database = False,
               Collision = False,
               Calibration = False,
               Reprocess = False,
               ReprocessList = 0):

  if(Calibration == True):
    alg = MSMgr.NewRootStream( 'StreamNTUP_L1Calo', filename , 'L1Calo' )
    ppm_blockargs = { 'L1CaloPPM' : {'DoCalibration' : True}}

    alg += EventInfoD3PDObject(level = 0)

    ### L1Calo PPM
    alg += L1CaloPPM_D3PDObject(level = 9,blockargs = ppm_blockargs,prefix='')
    from L1CaloD3PDMaker.L1CaloRODHeader_D3PDObject import L1CaloRODHeader_D3PDObject

    ### L1Calo CPM
    from L1CaloD3PDMaker.L1CaloCPMHits_D3PDObject  import L1CaloCPMHits_D3PDObject
    from L1CaloD3PDMaker.L1CaloCPMRoI_D3PDObject   import L1CaloCPMRoI_D3PDObject
    from L1CaloD3PDMaker.L1CaloCPMTower_D3PDObject import L1CaloCPMTower_D3PDObject
    alg += L1CaloCPMHits_D3PDObject(level = 0, allowMissing = True)
    alg += L1CaloCPMRoI_D3PDObject(level = 0, allowMissing = True)
    alg += L1CaloCPMTower_D3PDObject(level = 0, allowMissing = True)    

    ### L1Calo JEM
    from L1CaloD3PDMaker.L1CaloJEMEtSums_D3PDObject  import L1CaloJEMEtSums_D3PDObject
    from L1CaloD3PDMaker.L1CaloJEMHits_D3PDObject    import L1CaloJEMHits_D3PDObject
    from L1CaloD3PDMaker.L1CaloJEMRoI_D3PDObject     import L1CaloJEMRoI_D3PDObject
    from L1CaloD3PDMaker.L1CaloJetElement_D3PDObject import L1CaloJetElement_D3PDObject
    alg += L1CaloJEMEtSums_D3PDObject(level = 0, allowMissing = True)
    alg += L1CaloJEMHits_D3PDObject(level = 0, allowMissing = True)
    alg += L1CaloJEMRoI_D3PDObject(level = 0, allowMissing = True)
    alg += L1CaloJetElement_D3PDObject(level = 0, allowMissing = True)

    ### L1Calo CMM
    from L1CaloD3PDMaker.L1CaloCMMCPHits_D3PDObject  import L1CaloCMMCPHits_D3PDObject
    from L1CaloD3PDMaker.L1CaloCMMEtSums_D3PDObject  import L1CaloCMMEtSums_D3PDObject
    from L1CaloD3PDMaker.L1CaloCMMJetHits_D3PDObject import L1CaloCMMJetHits_D3PDObject
    from L1CaloD3PDMaker.L1CaloCMMRoI_D3PDObject     import L1CaloCMMRoI_D3PDObject
    alg += L1CaloCMMCPHits_D3PDObject(level = 0, allowMissing = True)
    alg += L1CaloCMMEtSums_D3PDObject(level = 0, allowMissing = True)
    alg += L1CaloCMMJetHits_D3PDObject(level = 0, allowMissing = True)
    alg += L1CaloCMMRoI_D3PDObject(level = 0, allowMissing = True)

    ## L1Calo ROD Headers
    alg += L1CaloRODHeader_D3PDObject(level = 0, allowMissing = True)
    

  if(Database == True):
    dbAlg = MSMgr.NewRootStream( 'StreamNTUP_L1CaloDB', filename,  dbTTreeName )
    if isMC:
      dbAlg += L1CaloPPMdbMC_D3PDObject(level = 0)
    if not isMC:
      dbAlg += L1CaloPPMdbData_D3PDObject(level = 0)

  if(Calibration == False):
    alg = MSMgr.NewRootStream( 'StreamNTUP_L1CaloPhysics', filename, TTreeName )

    ### Event and Collision Info
    if(Collision == True):
      from TriggerD3PDMaker.TrigDecisionD3PDObject   import TrigDecisionD3PDObject
      from TriggerD3PDMaker.TrigDBKeysD3PDObject     import TrigDBKeysD3PDObject
      from TriggerD3PDMaker.TrigConfMetadata         import addTrigConfMetadata
      from TrackD3PDMaker.VertexD3PDObject           import PrimaryVertexD3PDObject
      from TrackD3PDMaker.TrackD3PDObject            import TrackParticleD3PDObject
      from CaloD3PDMaker.LArCollisionTimeD3PDObject  import LArCollisionTimeD3PDObject
      from CaloD3PDMaker.CollisionDecisionD3PDObject import CollisionDecisionD3PDObject
      from CaloD3PDMaker.MBTSTimeD3PDObject          import MBTSTimeD3PDObject
      alg += EventInfoD3PDObject(level = 0)
      alg += PrimaryVertexD3PDObject (0, 'PrimaryVertex', allowMissing = True, sgkey = D3PDMakerFlags.VertexSGKey(), prefix = 'vxp_')
      alg += TrigDecisionD3PDObject(level = 2)
      alg += TrigDBKeysD3PDObject(level = 0)
      addTrigConfMetadata( alg )
      alg += LArCollisionTimeD3PDObject(level = 1)
      alg += CollisionDecisionD3PDObject (level = 1)
      alg += MBTSTimeD3PDObject (level = 1)
      

    ### L1Calo PPM
    if(Reprocess == False):
      alg += L1CaloPPM_D3PDObject(level = 0)
    if(Reprocess == True):
      ppm_blockargs = { 'L1CaloPPM' : {'DoCalibration' : True}}
      alg += L1CaloPPM_D3PDObject(level = 0,blockargs = ppm_blockargs)
    
    ### L1Calo CPM
    from L1CaloD3PDMaker.L1CaloCPMHits_D3PDObject  import L1CaloCPMHits_D3PDObject
    from L1CaloD3PDMaker.L1CaloCPMRoI_D3PDObject   import L1CaloCPMRoI_D3PDObject
    from L1CaloD3PDMaker.L1CaloCPMTower_D3PDObject import L1CaloCPMTower_D3PDObject
    alg += L1CaloCPMHits_D3PDObject(level = 0, allowMissing = True)
    alg += L1CaloCPMRoI_D3PDObject(level = 0, allowMissing = True)
    alg += L1CaloCPMTower_D3PDObject(level = 0, allowMissing = True)
    
    ### L1Calo JEM
    from L1CaloD3PDMaker.L1CaloJEMEtSums_D3PDObject  import L1CaloJEMEtSums_D3PDObject
    from L1CaloD3PDMaker.L1CaloJEMHits_D3PDObject    import L1CaloJEMHits_D3PDObject
    from L1CaloD3PDMaker.L1CaloJEMRoI_D3PDObject     import L1CaloJEMRoI_D3PDObject
    from L1CaloD3PDMaker.L1CaloJetElement_D3PDObject import L1CaloJetElement_D3PDObject
    alg += L1CaloJEMEtSums_D3PDObject(level = 0, allowMissing = True)
    alg += L1CaloJEMHits_D3PDObject(level = 0, allowMissing = True)
    alg += L1CaloJEMRoI_D3PDObject(level = 0, allowMissing = True)
    alg += L1CaloJetElement_D3PDObject(level = 0, allowMissing = True)
    
    ### L1Calo CMM
    from L1CaloD3PDMaker.L1CaloCMMCPHits_D3PDObject  import L1CaloCMMCPHits_D3PDObject
    from L1CaloD3PDMaker.L1CaloCMMEtSums_D3PDObject  import L1CaloCMMEtSums_D3PDObject
    from L1CaloD3PDMaker.L1CaloCMMJetHits_D3PDObject import L1CaloCMMJetHits_D3PDObject
    from L1CaloD3PDMaker.L1CaloCMMRoI_D3PDObject     import L1CaloCMMRoI_D3PDObject
    alg += L1CaloCMMCPHits_D3PDObject(level = 0, allowMissing = True)
    alg += L1CaloCMMEtSums_D3PDObject(level = 0, allowMissing = True)    
    alg += L1CaloCMMJetHits_D3PDObject(level = 0, allowMissing = True)
    alg += L1CaloCMMRoI_D3PDObject(level = 0, allowMissing = True)

    if not isMC:
      from L1CaloD3PDMaker.L1CaloRODHeader_D3PDObject import L1CaloRODHeader_D3PDObject
      alg += L1CaloRODHeader_D3PDObject(level = 0, allowMissing = True) ### ROD Headers are not in mc10

    ### L1Calo RoIs
    from TriggerD3PDMaker.JetROID3PDObject       import JetROID3PDObject
    from TriggerD3PDMaker.EmTauROID3PDObject     import EmTauROID3PDObject
    from TriggerD3PDMaker.EnergySumROID3PDObject import EnergySumROID3PDObject
    from TriggerD3PDMaker.JetETROID3PDObject     import JetETROID3PDObject
    alg += JetROID3PDObject(level = 3, prefix='trig_L1_jet_')
    alg += EmTauROID3PDObject(level = 3,prefix='trig_L1_emtau_',exclude=['Triggers'])
    alg += JetETROID3PDObject(level = 2,prefix='trig_L1_jetEt_')
    alg += EnergySumROID3PDObject(level = 2,prefix='trig_L1_energySum_', allowMissing = True)

    ### Jets
    if(Jets == True):
      from TrigJetD3PDMaker.TrigJetD3PDObject import TrigJetD3PDObject
      from JetD3PDMaker.JetD3PDObject         import JetD3PDObject
      alg += TrigJetD3PDObject(level = 2,prefix = 'trig_L2_jet_',exclude=['Decision'])
      alg += JetD3PDObject (level = 0,include = JetHLTIncludes, prefix='trig_EF_jet_', sgkey='HLT_TrigJetRec',name='HLT_TrigJetRec_Filler')


      alg += JetD3PDObject (level = 0,include = JetIncludes, prefix='AntiKt4TopoEMJet_', sgkey='AntiKt4TopoEMJets',name='AntiKt4TopoEMJets_Filler')
      alg += JetD3PDObject (level = 0,include = JetIncludes, prefix='AntiKt6TopoEMJet_', sgkey='AntiKt6TopoEMJets',name='AntiKt6TopoEMJets_Filler')

      alg += JetD3PDObject (level = 0,include = JetIncludes, prefix='AntiKt4LCTopoJet_', sgkey='AntiKt4LCTopoJets',name='AntiKt4LCTopoJets_Filler')
      alg += JetD3PDObject (level = 0,include = JetIncludes, prefix='AntiKt6LCTopoJet_', sgkey='AntiKt6LCTopoJets',name='AntiKt6LCTopoJets_Filler')

      if isMC:
        alg += JetD3PDObject (0, prefix='AntiKt4TruthJets_',   sgkey='AntiKt4TruthJets')
        alg += JetD3PDObject (0, prefix='AntiKt6TruthJets_',   sgkey='AntiKt6TruthJets')

    ### Egamma
    if(Egamma == True):
      from TrigEgammaD3PDMaker.TrigElectronD3PDObject import TrigElectronD3PDObject
      from TrigEgammaD3PDMaker.EFElectronD3PDObject   import EFElectronD3PDObject
      from egammaD3PDMaker.ElectronD3PDObject         import ElectronD3PDObject
      alg += TrigElectronD3PDObject(level = 0)
      alg += EFElectronD3PDObject(level = 0)
      alg += ElectronD3PDObject(level = 2)

      from TrigEgammaD3PDMaker.TrigPhotonD3PDObject import TrigPhotonD3PDObject
      from TrigEgammaD3PDMaker.EFPhotonD3PDObject   import EFPhotonD3PDObject
      from egammaD3PDMaker.PhotonD3PDObject         import PhotonD3PDObject
      alg += TrigPhotonD3PDObject(level =0)
      alg += EFPhotonD3PDObject(level = 0)
      alg += PhotonD3PDObject(level = 2)

    ### Missing Et
    if(MET == True):
      from TrigMissingETD3PDMaker.TrigMETD3PDObject import TrigMETD3PDObject
      from MissingETD3PDMaker.MissingETD3PDObject   import MissingETD3PDObject
      alg += TrigMETD3PDObject( 0 )
      alg += TrigMETD3PDObject( 0, prefix = "trig_EF_met_", sgkey = "HLT_TrigEFMissingET" )
      alg += MissingETD3PDObject(level=-1,include=METIncludes,allowMissing = True)

    ### Taus
    if(Taus == True):
      from TrigTauD3PDMaker.TrigL2TauD3PDObject import TrigL2TauD3PDObject
      from TrigTauD3PDMaker.TrigEFTauD3PDObject import TrigEFTauD3PDObject
      from TauD3PDMaker.TauD3PDObject           import TauD3PDObject
      alg += TrigL2TauD3PDObject(level = 0)
      alg += TrigEFTauD3PDObject(level = 0)
      alg += TauD3PDObject(level = 1,prefix='Tau_')

    ### MonteCarlo extras
    if isMC:
      alg += GenEventD3PDObject (1)
      alg += TruthParticleD3PDObject (1, prefix='mc_')


    ### Reprocess L1Calo from ADC to RoI
    if(Reprocess == True):
      from L1CaloD3PDMaker.ReprocessL1Calo_D3PDObject import ReprocessL1Calo_D3PDObject
      for i in ReprocessList:
        Reprocess_blockargs = { 'ReprocessL1Calo' : {'ReprocessName' : i} }
        alg += ReprocessL1Calo_D3PDObject(level = 0,blockargs = Reprocess_blockargs,
                                          prefix = 'L1Calo_rtt_trig_L1_' + i + '_',
                                          name = 'L1Calo_rtt_trig_L1_' + i + '_')






        
