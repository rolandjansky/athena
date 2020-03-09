# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

infile = 'aod/AOD-20.1.7.2/AOD-20.1.7.2-full.pool.root'
keys = [
    #xAOD::MissingETAssociationMap
    'METAssoc_AntiKt4EMPFlow',
    'METAssoc_AntiKt4EMTopo',
    'METAssoc_AntiKt4LCTopo',

    #xAOD::MissingETAuxComponentMap
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

include ('AthenaPoolUtilities/TPCnvTest.py')
