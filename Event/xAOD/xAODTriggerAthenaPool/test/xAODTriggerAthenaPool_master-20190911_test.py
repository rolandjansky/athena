#!/usr/bin/env python
"""
Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""

import ROOT
from AthenaPoolUtilities.TPCnvTestConfig import TPCnvTest

if __name__ == "__main__":

    # Force-load some dictionaries. Needed to work around ROOT-10940.
    ROOT.xAOD.CaloCluster()

    infile = 'aod/AOD-master-20190911/AOD-master-20190911-full.pool.root'
    keys = [
        #EmTauRoI_v2
        'HLT_xAOD__EmTauRoIContainer_L1TopoEM',
        'HLT_xAOD__EmTauRoIContainer_L1TopoTau',
        'LVL1EmTauRoIs',

        #EnergySumRoI_v2
        'LVL1EnergySumRoI',

        #JetRoI_v2
        'HLT_xAOD__JetRoIContainer_L1TopoJet',
        'LVL1JetRoIs',

        #TrigComposite_v2
        'HLT_xAOD__TrigCompositeContainer_ExpressInfo_HLT',
        'HLT_xAOD__TrigCompositeContainer_L1TopoComposite',
        'HLT_xAOD__TrigCompositeContainer_L1TopoMET',
        'HLT_xAOD__TrigCompositeContainer_MuonRoICluster',
        'HLT_xAOD__TrigCompositeContainer_TrigEFDielectronMassFex',

        #TrigNavigation_v1
        'TrigNavigation',

        #TrigPassBits_v1
        'HLT_xAOD__TrigPassBitsContainer_passbits',
             ]

    TPCnvTest(infile, keys)
