#!/usr/bin/env python
"""
Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""

from AthenaPoolUtilities.TPCnvTestConfig import TPCnvTest

if __name__ == "__main__":
    infile = 'aod/AOD-18.0.0/AOD-18.0.0-full.pool.root'

    keys = [
         #MissingETComposition_p2
        'MET_RefComposition',

        #MissingET_p3
        'MET_RefGamma',
        'MET_RefMuons',
        'MET_RefTau',
        'MET_Muons',
        'MET_Muons_Track',
        'MET_RefMuons_Track',
        'MET_Muons_Spectro',
        'MET_RefEle',
        'MET_Cryo',
        'MET_MuonMuons',
        'MET_RefJet',
        'MET_RefJet_JVF',
        'MET_RefJet_JVFCut',
        'MET_Track',
        'MET_RefFinal',
        'MET_CellOut_Eflow',
        'MET_RefFinal_STVF',
        'MET_CellOut_Eflow_JetArea',
        'MET_CellOut_Eflow_STVF',
        'MET_CellOut_Eflow_JetAreaJVF',
        'MET_CellOut_Eflow_JetAreaRhoEta5JVF',

        #MissingEtCalo_p3
        'MET_Base0',
        'MET_Topo',
        'MET_LocHadTopo',

        #MissingEtTruth_p3
        'MET_Truth_PileUp',
        'MET_Truth',

        #MuonCaloEnergyContainer_p1
        'MuonCaloEnergyCollection',

        #MuonContainer_p6
        'MuGirlLowBetaCollection',
        'CaloMuonCollection',
        'Muons',

        #MuonSpShowerContainer_p1
        'MuonSpShowers',
             ]

    TPCnvTest(infile, keys, useGeoModelSvc=True, doPixel=True, doSCT=True, doTRT=True, doMuon=True, doTracks=True)
