#!/usr/bin/env python
"""
Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""

import ROOT
from AthenaPoolUtilities.TPCnvTestConfig import TPCnvTest

if __name__ == "__main__":

    # Force-load some dictionaries. Needed to work around ROOT-10940.
    ROOT.xAOD.CaloCluster()

    infile = 'aod/AOD-21.0.79/AOD-21.0.79-full.pool.root'
    keys = [
        #xAOD::MissingETAuxAssociationMap_v2
        'METAssoc_AntiKt4EMPFlow',
        'METAssoc_AntiKt4EMTopo',
        'METAssoc_AntiKt4LCTopo',

        #xAOD::MissingETAuxComponentMap_v1
        'METMap_Truth',

        # xAOD::MissingETAuxContainer_v1
        'MET_Calo',
        'MET_Core_AntiKt4EMPFlow',
        'MET_Core_AntiKt4EMTopo',
        'MET_Core_AntiKt4LCTopo',
        'MET_EMTopo',
        'MET_EMTopoRegions',
        'MET_LocHadTopo',
        'MET_LocHadTopoRegions',
        'MET_Reference_AntiKt4EMPFlow',
        'MET_Reference_AntiKt4EMTopo',
        'MET_Reference_AntiKt4LCTopo',
        'MET_Track',
        'MET_Truth',
        'MET_TruthRegions',
             ]

    TPCnvTest(infile, keys)
