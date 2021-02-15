#!/usr/bin/env python
"""
Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""

from AthenaPoolUtilities.TPCnvTestConfig import TPCnvTest

if __name__ == "__main__":
    infile = 'aod/AOD-18.0.0/AOD-18.0.0-full.pool.root'
    keys = [
        #TrigPassBitsCollection_p1
        'TrigPassBitsCollection#HLT_passbits',

        #TrigPassFlagsCollection_p1
        'TrigPassFlagsCollection#HLT_passflags',
        'TrigPassFlagsCollection#HLT_isEM',

        #TrigRoiDescriptorCollection_p2
        'TrigRoiDescriptorCollection#HLT',
        'TrigRoiDescriptorCollection#HLT_T2TauFinal',
        'TrigRoiDescriptorCollection#HLT_TrigT2CaloTau',
        'TrigRoiDescriptorCollection#HLT_TrigT2CosmicJet',
        'TrigRoiDescriptorCollection#HLT_secondaryRoI_EF',
        'TrigRoiDescriptorCollection#HLT_forMS',
        'TrigRoiDescriptorCollection#HLT_forID',
        'TrigRoiDescriptorCollection#HLT_secondaryRoI_L2',
        'TrigRoiDescriptorCollection#HLT_TrigT2CaloEgamma',
        'TrigRoiDescriptorCollection#HLT_initialRoI',
        'TrigRoiDescriptorCollection#HLT_TrigT2CaloJet',
    ]

    TPCnvTest(infile, keys)
