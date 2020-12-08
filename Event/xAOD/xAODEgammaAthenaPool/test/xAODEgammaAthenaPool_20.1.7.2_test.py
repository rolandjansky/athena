#!/usr/bin/env python
"""
Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""

from AthenaPoolUtilities.TPCnvTestConfig import TPCnvTest

if __name__ == "__main__":

    infile = 'aod/AOD-20.1.7.2/AOD-20.1.7.2-full.pool.root'
    keys = [
        #xAOD::ElectronAuxContainer
        'Electrons',
        'ForwardElectrons',
        'HLT_xAOD__ElectronContainer_egamma_Electrons',

        #xAOD::PhotonAuxContainer
        'Photons',
        'HLT_xAOD__PhotonContainer_egamma_Photons',
             ]

    TPCnvTest(infile, keys)
