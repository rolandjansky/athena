#!/usr/bin/env python
"""
Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""

from AthenaPoolUtilities.TPCnvTestConfig import TPCnvTest

if __name__ == "__main__":
    infile = 'esd/ESD-20.1.7.2.pool.root'
    keys = [
        #TrigPassBitsCollection_p1
        'TrigPassBitsCollection#HLT_TrigPassBitsCollection_passbits',

        #TrigPassFlagsCollection_p1
        'TrigPassFlagsCollection#HLT_TrigPassFlagsCollection_isEM',
        'TrigPassFlagsCollection#HLT_TrigPassFlagsCollection_passflags',

        #TrigRoiDescriptorCollection_p3
        'TrigRoiDescriptorCollection#HLT_TrigRoiDescriptorCollection_TrigT2CaloJet',
        'TrigRoiDescriptorCollection#HLT_TrigRoiDescriptorCollection_TrigT2CosmicJet',
        'TrigRoiDescriptorCollection#HLT_TrigRoiDescriptorCollection_secondaryRoI_EF',
        'TrigRoiDescriptorCollection#HLT_TrigRoiDescriptorCollection_secondaryRoI_L2',
        'TrigRoiDescriptorCollection#HLT_TrigRoiDescriptorCollection_forMS',
        'TrigRoiDescriptorCollection#HLT_TrigRoiDescriptorCollection_secondaryRoI_HLT',
        'TrigRoiDescriptorCollection#HLT_TrigRoiDescriptorCollection_forID2',
        'TrigRoiDescriptorCollection#HLT_TrigRoiDescriptorCollection_forID3',
        'TrigRoiDescriptorCollection#HLT_TrigRoiDescriptorCollection_SuperRoi',
        'TrigRoiDescriptorCollection#HLT_TrigRoiDescriptorCollection_SplitJet',
        'TrigRoiDescriptorCollection#HLT_TrigRoiDescriptorCollection_forID1',
        'TrigRoiDescriptorCollection#HLT_TrigRoiDescriptorCollection_TrigJetRec',
        'TrigRoiDescriptorCollection#HLT_TrigRoiDescriptorCollection_initialRoI',
        'TrigRoiDescriptorCollection#HLT_TrigRoiDescriptorCollection',
    ]

    TPCnvTest(infile, keys)
