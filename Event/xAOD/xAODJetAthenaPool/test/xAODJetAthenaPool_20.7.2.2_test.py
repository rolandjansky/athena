#!/usr/bin/env python
"""
Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""

import ROOT
from AthenaPoolUtilities.TPCnvTestConfig import TPCnvTest

if __name__ == "__main__":

    # Force-load some dictionaries. Needed to work around ROOT-10940.
    ROOT.xAOD.CaloCluster()

    infile = 'aod/AOD-20.7.2.2/AOD-20.7.2.2-full.pool.root'
    keys = [
        #xAOD::JetTrigAuxContainer_v2
        'HLT_xAOD__JetContainer_EFJet',
        'HLT_xAOD__JetContainer_FarawayJet',
        'HLT_xAOD__JetContainer_SplitJet',
        'HLT_xAOD__JetContainer_SuperRoi',
        'HLT_xAOD__JetContainer_TrigTauJet',
        #'HLT_xAOD__JetContainer_a10r_tcemsubjesISFS',
        'HLT_xAOD__JetContainer_a10tcemnojcalibFS',
        'HLT_xAOD__JetContainer_a10tcemsubjesFS',
        'HLT_xAOD__JetContainer_a3ionemsubjesFS',
        'HLT_xAOD__JetContainer_a4ionemsubjesFS',
        'HLT_xAOD__JetContainer_a4tcemnojcalibFS',
        #'HLT_xAOD__JetContainer_a4tcemsubjesISFS',

        #xAOD::JetAuxContainer_v1
        'AntiKt10LCTopoJets',
        'AntiKt10TruthJets',
        'AntiKt10TruthWZJets',
        'AntiKt2PV0TrackJets',
        'AntiKt3PV0TrackJets',
        'AntiKt4EMPFlowJets',
        'AntiKt4EMTopoJets',
        'AntiKt4LCTopoJets',
        'AntiKt4PV0TrackJets',
        'AntiKt4TruthJets',
        'AntiKt4TruthWZJets',
        'CamKt12LCTopoJets',
        'CamKt12TruthJets',
        'CamKt12TruthWZJets',
             ]

    TPCnvTest(infile, keys)
