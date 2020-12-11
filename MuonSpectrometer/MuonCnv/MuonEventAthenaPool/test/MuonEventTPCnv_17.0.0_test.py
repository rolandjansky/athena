#!/usr/bin/env python
"""
Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""

from AthenaPoolUtilities.TPCnvTestConfig import TPCnvTest

if __name__ == "__main__":
    infile = 'esd/ESD-17.0.0.pool.root'

    keys = [
        #Muon::CscPrepDataContainer_tlp1
        'CSC_Clusters',

        #Muon::CscStripPrepDataContainer_tlp1
        'CSC_Measurements',

        #Muon::MuonPRD_Container_p2<Muon::RpcCoinData_p1>
        'RPC_triggerHits',

        #Muon::MuonPRD_Container_p2<Muon::MdtPrepData_p2>
        'MDT_DriftCircles',

        #Muon::RpcPrepDataContainer_tlp1
        'RPC_Measurements',

        #Muon::TgcPrepDataContainer_tlp1
        'TGC_MeasurementsNextBC',
        'TGC_MeasurementsPriorBC',
        'TGC_Measurements',

        #Muon::TgcCoinDataContainer_tlp2
        'TrigT1CoinDataCollectionNextBC',
        'TrigT1CoinDataCollectionPriorBC',
        'TrigT1CoinDataCollection',

        #Muon::ChamberT0s_p1
        'MboyMuonChamberT0s',
        'MooreMuonChamberT0s',
    ]

    TPCnvTest(infile, keys, useGeoModelSvc=True, doMuon=True)
