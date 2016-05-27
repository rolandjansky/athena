infile = 'aod/AOD-18.0.0/AOD-18.0.0-full.pool.root'
keys = [
    #ElectronContainer_p5
    'ElectronAODCollection',

    #PhotonContainer_p5
    'PhotonAODCollection',

    #egammaContainer_p5
    'HLT_egamma',
    'HLT_egamma_Photons',
    'HLT_egamma_Electrons',

    #egDetailContainer_p2
    # Dumped as part of the above.
         ]

include ('AthenaPoolUtilities/TPCnvTest.py')
