#!/usr/bin/env python
"""
Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""

from AthenaPoolUtilities.TPCnvTestConfig import TPCnvTest

if __name__ == "__main__":
    infile = 'esd/ESD-15.6.1.pool.root'
    keys = [
        #TrigOperationalInfoCollection_tlp1
        'TrigOperationalInfoCollection#HLT_OPI_L2',
        'TrigOperationalInfoCollection#HLT_OPI_EF',

        #TrigRoiDescriptorCollection_tlp1
        'TrigRoiDescriptorCollection#HLT',
        'TrigRoiDescriptorCollection#HLT_T2TauFinal',
        'TrigRoiDescriptorCollection#HLT_TrigT2CaloTau',
        'TrigRoiDescriptorCollection#HLT_TrigT2CosmicJet',
        'TrigRoiDescriptorCollection#HLT_forMS',
        'TrigRoiDescriptorCollection#HLT_forID',
        'TrigRoiDescriptorCollection#HLT_secondaryRoI_EF',
        'TrigRoiDescriptorCollection#HLT_secondaryRoI_L2',
        'TrigRoiDescriptorCollection#HLT_TrigT2CaloJet',
        'TrigRoiDescriptorCollection#HLT_initialRoI',
        'TrigRoiDescriptorCollection#HLT_TrigT2CaloEgamma',
    ]

    TPCnvTest(infile, keys)

