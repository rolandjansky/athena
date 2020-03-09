# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

infile = 'aod/AOD-20.1.7.2/AOD-20.1.7.2-full.pool.root'
keys = [
    #EmTauRoI
    'HLT_xAOD__EmTauRoIContainer_L1TopoEM',
    'HLT_xAOD__EmTauRoIContainer_L1TopoTau',
    'LVL1EmTauRoIs',

    #EnergySumRoI
    'LVL1EnergySumRoI',

    #JetRoI
    'HLT_xAOD__JetRoIContainer_L1TopoJet',
    'LVL1JetRoIs',

    #TrigComposite
    'HLT_xAOD__TrigCompositeContainer_L1TopoComposite',
    'HLT_xAOD__TrigCompositeContainer_L1TopoMET',

    #TrigNavigation
    'TrigNavigation',
         ]

include ('AthenaPoolUtilities/TPCnvTest.py')
