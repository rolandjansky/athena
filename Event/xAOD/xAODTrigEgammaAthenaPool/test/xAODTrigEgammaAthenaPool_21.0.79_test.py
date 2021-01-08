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
        #xAOD::ElectronTrigAuxContainer_v1
        'HLT_xAOD__ElectronContainer_egamma_Electrons',
        'HLT_xAOD__ElectronContainer_egamma_Iso_Electrons',
        'HLT_xAOD__ElectronContainer_egamma_SC_Electrons',
        'HLT_xAOD__ElectronContainer_egamma_SC_Iso_Electrons',
        'HLT_xAOD__MuonContainer_MuonEFInfo',

        #xAOD::PhotonTrigAuxContainer_v1
        'HLT_xAOD__PhotonContainer_egamma_Iso_Photons',
        'HLT_xAOD__PhotonContainer_egamma_Photons',
        'HLT_xAOD__PhotonContainer_egamma_SC_Iso_Photons',
        'HLT_xAOD__PhotonContainer_egamma_SC_Photons',

        #xAOD::TrigElectronAuxContainer_v1
        'HLT_xAOD__TrigElectronContainer_L2ElectronFex',
             ]

    TPCnvTest(infile, keys)
