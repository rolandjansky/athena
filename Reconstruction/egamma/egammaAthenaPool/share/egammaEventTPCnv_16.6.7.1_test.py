infile = 'aod/AOD-16.6.7.1/AOD-16.6.7.1-full.pool.root'
keys = [
    #ElectronContainer_p4
    'AtlfastIsoElectronCollection',
    'AtlfastElectronCollection',
    'ElectronAODCollection',

    #PhotonContainer_p4
    'AtlfastIsoPhotonCollection',
    'AtlfastPhotonCollection',
    'PhotonAODCollection',

    #egammaContainer_p4
    'HLT_egamma',
    'HLT_egamma_Photons',
    'HLT_egamma_Electrons',

    #egDetailContainer_p2
    # Dumped as part of the above.
         ]

include ('AthenaPoolUtilities/TPCnvTest.py')
