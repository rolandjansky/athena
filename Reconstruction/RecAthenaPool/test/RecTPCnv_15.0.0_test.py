#!/usr/bin/env python
"""
Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""

from AthenaPoolUtilities.TPCnvTestConfig import TPCnvTest

if __name__ == "__main__":
    infile = 'aod/AOD-15.0.0/AOD-15.0.0-full.pool.root'

    keys = [
        #MissingET_p1
        'MET_RefGamma',
        'MET_RefTau',
        'MET_RefEle',
        'ObjMET_TauJet',
        'MET_RefMuon',
        'MET_RefMuon_Track',
        'AtlfastMissingEt',
        'MET_CellOut',
        'MET_CellOut_MiniJet',
        'MET_Cryo',
        'MET_CryoCone',
        'MET_DM_All',
        'MET_DM_Crack1',
        'MET_DM_Crack2',
        'MET_DM_Cryo',
        'MET_Final',
        'MET_LocHadTopoObj',
        'MET_MuonBoy',
        'MET_RefFinal',
        'MET_RefJet',
        'MET_TopoObj',
        'ObjMET_IdTrk',
        'ObjMET_Elec',
        'MET_MuonBoy_Track',
        'MET_MuonBoy_Spectro',
        'ObjMET_Muon',
        'MET_Muon',
        'ObjMET_Jet',
        'ObjMET_MiniJet',
        'ObjMET_Rest',
        'ObjMET_Final',

        #MissingEtCalo_p1
        'MET_Base0',
        'MET_Base',
        'MET_Calib',
        'MET_LocHadTopo',
        'MET_Topo',
        'MET_CorrTopo',

        #MissingEtTruth_p1
        'MET_Truth',
        'MET_Truth_PileUp',

        #MuonCaloEnergyContainer_p1
        'MuonCaloEnergyCollection',

        #MuonContainer_p4
        'MuGirlLowBetaCollection',
        'AtlfastIsoMuonCollection',
        'AtlfastMuonCollection',
        'StacoMuonCollection',
        'CaloMuonCollection',
        'MuidMuonCollection',

        #MuonSpShowerContainer_p1
        'MuonSpShowers',
             ]

    TPCnvTest(infile, keys, useGeoModelSvc=True, doPixel=True, doSCT=True, doTRT=True, doMuon=True, doTracks=True)
