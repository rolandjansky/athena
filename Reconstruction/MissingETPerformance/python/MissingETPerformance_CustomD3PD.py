# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


import D3PDMakerCoreComps
from EventCommonD3PDMaker.EventInfoD3PDObject import EventInfoD3PDObject
from egammaD3PDMaker.ElectronD3PDObject       import ElectronD3PDObject
from egammaD3PDMaker.PhotonD3PDObject         import PhotonD3PDObject
from MuonD3PDMaker.MuonD3PDObject             import MuonD3PDObject
from CaloD3PDMaker.ClusterD3PDObject          import ClusterD3PDObject
##from CaloD3PDMaker.CellD3PDObject          import CellD3PDObject ##available in CaloD3PDMaker-00-01-06 onward
from D3PDMakerConfig.JetD3PDObject            import JetD3PDObject ##co JetD3PDMaker and change this to get more options
from MissingETD3PDMaker.MissingETD3PD_PhysicsModule import METPhysicsD3PD ##Might have to get package to do this
	
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
	
	
def metPerformanceD3PD (file,
	                level = 3,
	                tuplename = 'metPerfD3PD',
	                seq = topSequence,
	                D3PDSvc = 'D3PD::RootD3PDSvc'):

    from AthenaCommon.AlgSequence import AlgSequence
    alg = AlgSequence()
	
    subAlg = D3PDMakerCoreComps.MakerAlg(tuplename, seq,
                                      file = file, D3PDSvc = D3PDSvc)
    subAlg += EventInfoD3PDObject (0)
    subAlg += ElectronD3PDObject (0)
    subAlg += PhotonD3PDObject (0)
    subAlg += MuonD3PDObject (0)
    subAlg += JetD3PDObject (level)
     ##subAlg += JetD3PDObject (level, sgkey='AntiKt4H1TopoJets',prefix='AntiKt4H1Topo_') example to change container
    subAlg += ClusterD3PDObject(3)
    ##subAlg += CellD3PDObject(3) don't turn on unless needed. filesize explodes
    
    alg += subAlg
    alg += METPhysicsD3PD(file, tuplename=tuplename, level=level) ##might need MissingETD3PDMaker-00-00-24 for this to work
    
    return alg
