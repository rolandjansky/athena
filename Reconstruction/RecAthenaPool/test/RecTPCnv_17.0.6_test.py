#!/usr/bin/env python
"""
Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""

from AthenaPoolUtilities.TPCnvTestConfig import TPCnvTest

if __name__ == "__main__":
    infile = 'aod/AOD-17.0.6/AOD-17.0.6-full.pool.root'

    keys = [
        #MissingETComposition_p1
        'MET_RefComposition',
        'MET_RefComposition_em',

        #MissingET_p3
        'MET_RefGamma',
        'MET_RefGamma_em',
        'MET_RefMuon',
        'MET_RefMuon_em',
        'MET_RefMuon_Muid',
        'MET_Muid',
        'MET_RefTau',
        'MET_MuonBoy',
        'MET_MuonBoy_Spectro',
        'MET_RefTau_em',
        'MET_Muid_Track',
        'MET_RefMuon_Track',
        'MET_MuonBoy_Track',
        'MET_RefMuon_Track_em',
        'MET_RefMuon_Track_Muid',
        'MET_Muid_Spectro',
        'MET_RefEle',
        'MET_Track',
        'MET_Cryo',
        'MET_DM_All',
        'MET_RefEle_em',
        'MET_TopoObj',
        'MET_DM_Crack2',
        'MET_DM_Cryo',
        'MET_CryoCone',
        'MET_DM_Crack1',
        'MET_LocHadTopoObj',
        'MET_Muon',
        'MET_MuonMuid',
        'MET_SoftJets_em',
        'MET_Final',
        'MET_SoftJets',
        'MET_RefJet',
        'MET_RefJet_em',
        'MET_RefFinal',
        'MET_CellOut_em',
        'MET_RefFinal_em',
        'MET_CellOut_Eflow',
        'MET_CellOut_Eflow_Muid',

        #MissingEtCalo_p3
        'MET_Base0',
        'MET_Calib',
        'MET_Base',
        'MET_Topo',
        'MET_CorrTopo',
        'MET_LocHadTopo',

        #MissingEtTruth_p3
        'MET_Truth',
        'MET_Truth_PileUp',

        #MuonCaloEnergyContainer_tlp1
        'MuonCaloEnergyCollection',

        #MuonContainer_p6
        'MuGirlLowBetaCollection',
        'CaloMuonCollection',
        'StacoMuonCollection',
        'MuidMuonCollection',

        #MuonSpShowerContainer_p1
        'MuonSpShowers',
             ]

    TPCnvTest(infile, keys, useGeoModelSvc=True, doPixel=True, doSCT=True, doTRT=True, doMuon=True, doTracks=True)
