# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import D3PDMakerCoreComps
from D3PDMakerConfig.D3PDMakerFlags           import D3PDMakerFlags
from RecExConfig.RecFlags import rec
from MissingETD3PDMaker.MissingETD3PDMakerFlags import *
from MissingETD3PDMaker.MissingETD3PDObject   import *
from MissingETD3PDMaker.MissingETGoodnessD3PDObject import 	MissingETGoodnessD3PDObject
## from TrackD3PDMaker.TrackD3PDObject                  import TrackParticleD3PDObject
## from TrackD3PDMaker.TrackD3PDMakerFlags              import *
## from CaloD3PDMaker.ClusterD3PDObject                 import ClusterD3PDObject
## from egammaD3PDMaker.ElectronD3PDObject              import ElectronD3PDObject
## from egammaD3PDMaker.PhotonD3PDObject                import PhotonD3PDObject
## from MuonD3PDMaker.MuonD3PDObject                    import MuonD3PDObject
## from JetD3PDMaker.JetD3PDObject                      import JetD3PDObject
## from TauD3PDMaker.TauD3PDObject                      import TauD3PDObject

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
		
def MissingETD3PD (file,
                   level = 10,
                   tuplename = 'METD3PD',
                   seq = topSequence,
                   D3PDSvc = 'D3PD::RootD3PDSvc'):
	
    alg = D3PDMakerCoreComps.MakerAlg(tuplename, seq,
                                      file = file, D3PDSvc = D3PDSvc)

    alg += MissingETD3PDObject (level=level, allowMissing = True, exclude=['MET_Base', 'MET_Truth_Int', 'MET_RefFinal_Phi', 'MET_MuonBoy_Et', 'MET_RefJet_SumEt'])#, exclude = ['MET_Muon_Isol_Muid', 'MET_Muon_Total_Muid', 'MET_Muon_NonIsol_Muid','MET_Muon_Isol_Staco', 'MET_Muon_Total_Staco', 'MET_Muon_NonIsol_Staco', 'MET_SoftJets', 'MET_RefMuon_Muid', 'MET_RefMuon_Staco', 'MET_RefMuon_Track_Muid', 'MET_RefMuon_Track_Staco'])
    ##the terms in the exclude before this are specialized terms used by MET experts. They're not in regular AODs or ESDs [yet?], just specialized datasets used for the Pisa Hadronic Cal Workshop - so you may want to exclude them, otherwise they may show up as zeroed sets
  #  alg += MissingETGoodnessD3PDObject (level=level, allowMissing = True) 

    ##Example of a custom object
    #alg += MissingETD3PDObject (level=level, sgkey = MissingETD3PDMakerFlags.METRefEleSGKey(), prefix=MissingETD3PDMakerFlags.METRefEleSGKey(), allowMissing = True)

   ##  #More involved custom example - these are setup for custom MET D3PDs for the Pisa Hadronic Cal Workshop
##     customMETs_Staco = ['MET_RefFinal', 'MET_RefGamma', 'MET_RefEle', 'MET_RefTau', 'MET_RefJet', 'MET_RefMuon' ,'MET_RefMuon_Staco', 'MET_CellOut', 'MET_Cryo', 'MET_Muon_Isol_Staco', 'MET_Muon_NonIsol_Staco', 'MET_Muon_Total_Staco', 'MET_SoftJets', 'MET_RefMuon_Track', 'MET_RefMuon_Track_Staco']


##     customMETs_Muid = ['MET_RefFinal', 'MET_RefGamma', 'MET_RefEe', 'MET_RefTau', 'MET_RefJet', 'MET_RefMuon' ,'MET_RefMuon_Muid', 'MET_CellOut', 'MET_Cryo', 'MET_Muon_Isol_Muid', 'MET_Muon_NonIsol_Muid', 'MET_Muon_Total_Muid', 'MET_SoftJets', 'MET_RefMuon_Track', 'MET_RefMuon_Track_Muid']


##     for custom in customMETs_Staco:
##         alg += MissingETD3PDObject (level=0, sgkey = custom+'_LCW_pt20', prefix=custom+'_LCW_pt20', allowMissing = True)
##         alg += MissingETD3PDObject (level=0, sgkey = custom+'_GCW_pt20', prefix=custom+'_GCW_pt20', allowMissing = True)
##         alg += MissingETD3PDObject (level=0, sgkey = custom+'_LCW_NI_pt20_noSoftJets_eflow', prefix=custom+'_LCW_NI_pt20_noSoftJets_eflow', allowMissing = True)

##     for custom in customMETs_Muid: 
##         alg += MissingETD3PDObject (level=0, sgkey = custom+'_GCW_NI_pt20_Muid', prefix=custom+'_GCW_NI_pt20_Muid', allowMissing = True)
##         alg += MissingETD3PDObject (level=0, sgkey = custom+'_LCW_NI_pt20_Muid_eflow', prefix=custom+'_LCW_NI_pt20_Muid_eflow', allowMissing = True)
        
        
   ##  alg += ElectronD3PDObject(0)
##     alg += PhotonD3PDObject(0)
##     alg += TauD3PDObject(0)
##     alg += MuonD3PDObject(0, sgkey='StacoMuonCollection', prefix="mu_staco_")
##     alg += MuonD3PDObject(0, sgkey='MuidMuonCollection', prefix="mu_muid_")
##     alg += JetD3PDObject(0, sgkey=MissingETD3PDMakerFlags.METDefaultJetCollectionSGKey(), prefix="jet_"+MissingETD3PDMakerFlags.METDefaultJetCollectionSGKey()[:-4].lower()+"_")
##     alg += ClusterD3PDObject(0, prefix='cl_')
##     #TrackD3PDMakerFlags.stor    eTrackPredictionAtBLayer = False
    #alg += TrackParticleD3PDObject(0)
    ##If you want a custom objecto MissingEtCalo type use MissingETCaloD3PDObject
    ##Or TruthMETD3PDObject
    ##JetsInfoMETD3PDObject
    ##EtaRingsMETD3PDObject
    ##those and MissingETD3PDObject are the distinct types

    return alg
