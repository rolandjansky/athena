# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id$
#
# @file L1CaloD3PDMaker/python/L1CaloD3PDUpgrade.py
# @author John Morris <john.morris@cern.ch>
# @brief Construct a L1Calo Upgrade D3PD

from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags
D3PDMakerFlags.DoTrigger = False

import L1CaloD3PDMaker
from EventCommonD3PDMaker.EventInfoD3PDObject import EventInfoD3PDObject

TTreeName = 'physics'

from RecExConfig.RecFlags import rec
isMC = rec.doTruth.get_Value()

def L1CaloD3PDUpgrade(filename,level = 0):
  alg = MSMgr.NewRootStream( 'StreamNTUP_L1CaloPhysics', filename, TTreeName )
  
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
    
  ### Trigger RoIs
  from TriggerD3PDMaker.JetROID3PDObject       import JetROID3PDObject
  from TriggerD3PDMaker.EmTauROID3PDObject     import EmTauROID3PDObject
  from TriggerD3PDMaker.EnergySumROID3PDObject import EnergySumROID3PDObject
  from TriggerD3PDMaker.JetETROID3PDObject     import JetETROID3PDObject
  from TriggerD3PDMaker.MuonROID3PDObject      import MuonROID3PDObject
  alg += JetROID3PDObject(level = 3, prefix='trig_L1_jet_')
  alg += EmTauROID3PDObject(level = 3,prefix='trig_L1_emtau_',exclude=['Triggers'])
  alg += JetETROID3PDObject(level = 2,prefix='trig_L1_jetEt_')
  alg += EnergySumROID3PDObject(level = 2,prefix='trig_L1_energySum_', allowMissing = True)
  alg += MuonROID3PDObject(level = 2,prefix='trig_L1_mu_')

  ### MonteCarlo extras
  if isMC:
    from EventCommonD3PDMaker.GenEventD3PDObject import GenEventD3PDObject
    from TruthD3PDMaker.TruthParticleD3PDObject  import TruthParticleD3PDObject
    alg += GenEventD3PDObject (1)
    alg += TruthParticleD3PDObject (1, prefix='mc_')        
