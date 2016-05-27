infile = 'aod/AOD-15.6.9/AOD-15.6.9-full.pool.root'
keys = [
    #ElectronContainer_p2
    'AtlfastIsoElectronCollection',
    'AtlfastElectronCollection',
    'ElectronAODCollection',

    #PhotonContainer_p2
    'AtlfastIsoPhotonCollection',
    'AtlfastPhotonCollection',
    'PhotonAODCollection',

    #egammaContainer_p2
    'HLT_egamma',
    'HLT_egamma_Photons',
    'HLT_egamma_Electrons',

    #egDetailContainer_p2
    # Dumped as part of the above.
         ]

include ('AthenaPoolUtilities/TPCnvTest.py')
