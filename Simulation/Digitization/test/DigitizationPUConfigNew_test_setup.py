"""Large setup for PileUp (PU) Digitization configuration test

Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""

test_HighPtMinbiasHitsFile = "/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/mc16_13TeV.361239.Pythia8EvtGen_A3NNPDF23LO_minbias_inelastic_high.merge.HITS.e4981_s3087_s3089/*"
test_LowPtMinbiasHitsFile = "/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/mc16_13TeV.361238.Pythia8EvtGen_A3NNPDF23LO_minbias_inelastic_low.merge.HITS.e4981_s3087_s3089/*"


# lifted from preInlcude.PileUpBunchTrainsMC16c_2017_Config1.py
test_BeamIntensityPattern = [0.0, # bunch crossing zero is always empty
0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,
0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,
0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,
0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,
0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,
0.0, # Starting BCID:    0, Gap: 2025 ns
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID:   82, Colliding: 48 BCIDs
0.0,0.0,0.0,0.0,0.0,0.0,0.0, # Starting BCID:  130, Gap: 175 ns
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID:  137, Colliding: 48 BCIDs
0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,
0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0, # Starting BCID:  185, Gap: 775 ns
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID:  216, Colliding: 48 BCIDs
0.0,0.0,0.0,0.0,0.0,0.0,0.0, # Starting BCID:  264, Gap: 175 ns
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID:  271, Colliding: 48 BCIDs
0.0,0.0,0.0,0.0,0.0,0.0,0.0, # Starting BCID:  319, Gap: 175 ns
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID:  326, Colliding: 48 BCIDs
0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,
0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0, # Starting BCID:  374, Gap: 775 ns
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID:  405, Colliding: 48 BCIDs
0.0,0.0,0.0,0.0,0.0,0.0,0.0, # Starting BCID:  453, Gap: 175 ns
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID:  460, Colliding: 48 BCIDs
0.0,0.0,0.0,0.0,0.0,0.0,0.0, # Starting BCID:  508, Gap: 175 ns
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID:  515, Colliding: 48 BCIDs
0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,
0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0, # Starting BCID:  563, Gap: 775 ns
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID:  594, Colliding: 48 BCIDs
0.0,0.0,0.0,0.0,0.0,0.0,0.0, # Starting BCID:  642, Gap: 175 ns
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID:  649, Colliding: 48 BCIDs
0.0,0.0,0.0,0.0,0.0,0.0,0.0, # Starting BCID:  697, Gap: 175 ns
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID:  704, Colliding: 48 BCIDs
0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,
0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,
0.0,0.0,0.0, # Starting BCID:  752, Gap: 875 ns
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID:  787, Colliding: 48 BCIDs
0.0,0.0,0.0,0.0,0.0,0.0,0.0, # Starting BCID:  835, Gap: 175 ns
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID:  842, Colliding: 48 BCIDs
0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,
0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0, # Starting BCID:  890, Gap: 775 ns
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID:  921, Colliding: 48 BCIDs
0.0,0.0,0.0,0.0,0.0,0.0,0.0, # Starting BCID:  969, Gap: 175 ns
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID:  976, Colliding: 48 BCIDs
0.0,0.0,0.0,0.0,0.0,0.0,0.0, # Starting BCID: 1024, Gap: 175 ns
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 1031, Colliding: 48 BCIDs
0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,
0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0, # Starting BCID: 1079, Gap: 775 ns
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 1110, Colliding: 48 BCIDs
0.0,0.0,0.0,0.0,0.0,0.0,0.0, # Starting BCID: 1158, Gap: 175 ns
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 1165, Colliding: 48 BCIDs
0.0,0.0,0.0,0.0,0.0,0.0,0.0, # Starting BCID: 1213, Gap: 175 ns
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 1220, Colliding: 48 BCIDs
0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,
0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0, # Starting BCID: 1268, Gap: 775 ns
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 1299, Colliding: 48 BCIDs
0.0,0.0,0.0,0.0,0.0,0.0,0.0, # Starting BCID: 1347, Gap: 175 ns
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 1354, Colliding: 48 BCIDs
0.0,0.0,0.0,0.0,0.0,0.0,0.0, # Starting BCID: 1402, Gap: 175 ns
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 1409, Colliding: 48 BCIDs
0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,
0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0, # Starting BCID: 1457, Gap: 775 ns
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 1488, Colliding: 48 BCIDs
0.0,0.0,0.0,0.0,0.0,0.0,0.0, # Starting BCID: 1536, Gap: 175 ns
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 1543, Colliding: 48 BCIDs
0.0,0.0,0.0,0.0,0.0,0.0,0.0, # Starting BCID: 1591, Gap: 175 ns
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 1598, Colliding: 48 BCIDs
0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,
0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,
0.0,0.0,0.0, # Starting BCID: 1646, Gap: 875 ns
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 1681, Colliding: 48 BCIDs
0.0,0.0,0.0,0.0,0.0,0.0,0.0, # Starting BCID: 1729, Gap: 175 ns
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 1736, Colliding: 48 BCIDs
0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,
0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0, # Starting BCID: 1784, Gap: 775 ns
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 1815, Colliding: 48 BCIDs
0.0,0.0,0.0,0.0,0.0,0.0,0.0, # Starting BCID: 1863, Gap: 175 ns
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 1870, Colliding: 48 BCIDs
0.0,0.0,0.0,0.0,0.0,0.0,0.0, # Starting BCID: 1918, Gap: 175 ns
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 1925, Colliding: 48 BCIDs
0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,
0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0, # Starting BCID: 1973, Gap: 775 ns
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 2004, Colliding: 48 BCIDs
0.0,0.0,0.0,0.0,0.0,0.0,0.0, # Starting BCID: 2052, Gap: 175 ns
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 2059, Colliding: 48 BCIDs
0.0,0.0,0.0,0.0,0.0,0.0,0.0, # Starting BCID: 2107, Gap: 175 ns
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 2114, Colliding: 48 BCIDs
0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,
0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0, # Starting BCID: 2162, Gap: 775 ns
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 2193, Colliding: 48 BCIDs
0.0,0.0,0.0,0.0,0.0,0.0,0.0, # Starting BCID: 2241, Gap: 175 ns
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 2248, Colliding: 48 BCIDs
0.0,0.0,0.0,0.0,0.0,0.0,0.0, # Starting BCID: 2296, Gap: 175 ns
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 2303, Colliding: 48 BCIDs
0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,
0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0, # Starting BCID: 2351, Gap: 775 ns
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 2382, Colliding: 48 BCIDs
0.0,0.0,0.0,0.0,0.0,0.0,0.0, # Starting BCID: 2430, Gap: 175 ns
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 2437, Colliding: 48 BCIDs
0.0,0.0,0.0,0.0,0.0,0.0,0.0, # Starting BCID: 2485, Gap: 175 ns
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 2492, Colliding: 48 BCIDs
0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,
0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,
0.0,0.0,0.0, # Starting BCID: 2540, Gap: 875 ns
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 2575, Colliding: 48 BCIDs
0.0,0.0,0.0,0.0,0.0,0.0,0.0, # Starting BCID: 2623, Gap: 175 ns
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 2630, Colliding: 48 BCIDs
0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,
0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0, # Starting BCID: 2678, Gap: 775 ns
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 2709, Colliding: 48 BCIDs
0.0,0.0,0.0,0.0,0.0,0.0,0.0, # Starting BCID: 2757, Gap: 175 ns
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 2764, Colliding: 48 BCIDs
0.0,0.0,0.0,0.0,0.0,0.0,0.0, # Starting BCID: 2812, Gap: 175 ns
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 2819, Colliding: 48 BCIDs
0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,
0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0, # Starting BCID: 2867, Gap: 775 ns
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 2898, Colliding: 48 BCIDs
0.0,0.0,0.0,0.0,0.0,0.0,0.0, # Starting BCID: 2946, Gap: 175 ns
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 2953, Colliding: 48 BCIDs
0.0,0.0,0.0,0.0,0.0,0.0,0.0, # Starting BCID: 3001, Gap: 175 ns
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 3008, Colliding: 48 BCIDs
0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,
0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0, # Starting BCID: 3056, Gap: 775 ns
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 3087, Colliding: 48 BCIDs
0.0,0.0,0.0,0.0,0.0,0.0,0.0, # Starting BCID: 3135, Gap: 175 ns
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 3142, Colliding: 48 BCIDs
0.0,0.0,0.0,0.0,0.0,0.0,0.0, # Starting BCID: 3190, Gap: 175 ns
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 3197, Colliding: 48 BCIDs
0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,
0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0, # Starting BCID: 3245, Gap: 775 ns
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 3276, Colliding: 48 BCIDs
0.0,0.0,0.0,0.0,0.0,0.0,0.0, # Starting BCID: 3324, Gap: 175 ns
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 3331, Colliding: 48 BCIDs
0.0,0.0,0.0,0.0,0.0,0.0,0.0, # Starting BCID: 3379, Gap: 175 ns
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,
1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 3386, Colliding: 48 BCIDs
0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,
0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,
0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,
0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,
0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,
0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,
0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,
0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,
0.0,0.0] # Starting BCID: 3434 - Abort gap





# based on configLumi_run300000_mc16d.py
_evts = lambda x: int(1*x)

JobMaker=[
    {"run":300000, "lb":9, "starttstamp":1500000480, "dt":0.000, "evts":_evts(1), "mu":8.5, "force_new":False},
    {"run":300000, "lb":10, "starttstamp":1500000540, "dt":0.000, "evts":_evts(1), "mu":9.5, "force_new":False},
    {"run":300000, "lb":11, "starttstamp":1500000600, "dt":0.000, "evts":_evts(1), "mu":10.5, "force_new":False},
    {"run":300000, "lb":12, "starttstamp":1500000660, "dt":0.000, "evts":_evts(2), "mu":11.5, "force_new":False},
    {"run":300000, "lb":13, "starttstamp":1500000720, "dt":0.000, "evts":_evts(3), "mu":12.5, "force_new":False},
    {"run":300000, "lb":14, "starttstamp":1500000780, "dt":0.000, "evts":_evts(5), "mu":13.5, "force_new":False},
    {"run":300000, "lb":15, "starttstamp":1500000840, "dt":0.000, "evts":_evts(6), "mu":14.5, "force_new":False},
    {"run":300000, "lb":16, "starttstamp":1500000900, "dt":0.000, "evts":_evts(9), "mu":15.5, "force_new":False},
    {"run":300000, "lb":17, "starttstamp":1500000960, "dt":0.000, "evts":_evts(12), "mu":16.5, "force_new":False},
    {"run":300000, "lb":18, "starttstamp":1500001020, "dt":0.000, "evts":_evts(16), "mu":17.5, "force_new":False},
    {"run":300000, "lb":19, "starttstamp":1500001080, "dt":0.000, "evts":_evts(20), "mu":18.5, "force_new":False},
    {"run":300000, "lb":20, "starttstamp":1500001140, "dt":0.000, "evts":_evts(26), "mu":19.5, "force_new":False},
    {"run":300000, "lb":21, "starttstamp":1500001200, "dt":0.000, "evts":_evts(31), "mu":20.5, "force_new":False},
    {"run":300000, "lb":22, "starttstamp":1500001260, "dt":0.000, "evts":_evts(37), "mu":21.5, "force_new":False},
    {"run":300000, "lb":23, "starttstamp":1500001320, "dt":0.000, "evts":_evts(43), "mu":22.5, "force_new":False},
    {"run":300000, "lb":24, "starttstamp":1500001380, "dt":0.000, "evts":_evts(46), "mu":23.5, "force_new":False},
    {"run":300000, "lb":25, "starttstamp":1500001440, "dt":0.000, "evts":_evts(49), "mu":24.5, "force_new":False},
    {"run":300000, "lb":26, "starttstamp":1500001500, "dt":0.000, "evts":_evts(51), "mu":25.5, "force_new":False},
    {"run":300000, "lb":27, "starttstamp":1500001560, "dt":0.000, "evts":_evts(54), "mu":26.5, "force_new":False},
    {"run":300000, "lb":28, "starttstamp":1500001620, "dt":0.000, "evts":_evts(57), "mu":27.5, "force_new":False},
    {"run":300000, "lb":29, "starttstamp":1500001680, "dt":0.000, "evts":_evts(59), "mu":28.5, "force_new":False},
    {"run":300000, "lb":30, "starttstamp":1500001740, "dt":0.000, "evts":_evts(61), "mu":29.5, "force_new":False},
    {"run":300000, "lb":31, "starttstamp":1500001800, "dt":0.000, "evts":_evts(62), "mu":30.5, "force_new":False},
    {"run":300000, "lb":32, "starttstamp":1500001860, "dt":0.000, "evts":_evts(62), "mu":31.5, "force_new":False},
    {"run":300000, "lb":33, "starttstamp":1500001920, "dt":0.000, "evts":_evts(62), "mu":32.5, "force_new":False},
    {"run":300000, "lb":34, "starttstamp":1500001980, "dt":0.000, "evts":_evts(62), "mu":33.5, "force_new":False},
    {"run":300000, "lb":35, "starttstamp":1500002040, "dt":0.000, "evts":_evts(61), "mu":34.5, "force_new":False},
    {"run":300000, "lb":36, "starttstamp":1500002100, "dt":0.000, "evts":_evts(59), "mu":35.5, "force_new":False},
    {"run":300000, "lb":37, "starttstamp":1500002160, "dt":0.000, "evts":_evts(58), "mu":36.5, "force_new":False},
    {"run":300000, "lb":38, "starttstamp":1500002220, "dt":0.000, "evts":_evts(57), "mu":37.5, "force_new":False},
    {"run":300000, "lb":39, "starttstamp":1500002280, "dt":0.000, "evts":_evts(56), "mu":38.5, "force_new":False},
    {"run":300000, "lb":40, "starttstamp":1500002340, "dt":0.000, "evts":_evts(52), "mu":39.5, "force_new":False},
    {"run":300000, "lb":41, "starttstamp":1500002400, "dt":0.000, "evts":_evts(48), "mu":40.5, "force_new":False},
    {"run":300000, "lb":42, "starttstamp":1500002460, "dt":0.000, "evts":_evts(45), "mu":41.5, "force_new":False},
    {"run":300000, "lb":43, "starttstamp":1500002520, "dt":0.000, "evts":_evts(42), "mu":42.5, "force_new":False},
    {"run":300000, "lb":44, "starttstamp":1500002580, "dt":0.000, "evts":_evts(39), "mu":43.5, "force_new":False},
    {"run":300000, "lb":45, "starttstamp":1500002640, "dt":0.000, "evts":_evts(37), "mu":44.5, "force_new":False},
    {"run":300000, "lb":46, "starttstamp":1500002700, "dt":0.000, "evts":_evts(35), "mu":45.5, "force_new":False},
    {"run":300000, "lb":47, "starttstamp":1500002760, "dt":0.000, "evts":_evts(33), "mu":46.5, "force_new":False},
    {"run":300000, "lb":48, "starttstamp":1500002820, "dt":0.000, "evts":_evts(32), "mu":47.5, "force_new":False},
    {"run":300000, "lb":49, "starttstamp":1500002880, "dt":0.000, "evts":_evts(31), "mu":48.5, "force_new":False},
    {"run":300000, "lb":50, "starttstamp":1500002940, "dt":0.000, "evts":_evts(31), "mu":49.5, "force_new":False},
    {"run":300000, "lb":51, "starttstamp":1500003000, "dt":0.000, "evts":_evts(30), "mu":50.5, "force_new":False},
    {"run":300000, "lb":52, "starttstamp":1500003060, "dt":0.000, "evts":_evts(30), "mu":51.5, "force_new":False},
    {"run":300000, "lb":53, "starttstamp":1500003120, "dt":0.000, "evts":_evts(29), "mu":52.5, "force_new":False},
    {"run":300000, "lb":54, "starttstamp":1500003180, "dt":0.000, "evts":_evts(28), "mu":53.5, "force_new":False},
    {"run":300000, "lb":55, "starttstamp":1500003240, "dt":0.000, "evts":_evts(27), "mu":54.5, "force_new":False},
    {"run":300000, "lb":56, "starttstamp":1500003300, "dt":0.000, "evts":_evts(26), "mu":55.5, "force_new":False},
    {"run":300000, "lb":57, "starttstamp":1500003360, "dt":0.000, "evts":_evts(26), "mu":56.5, "force_new":False},
    {"run":300000, "lb":58, "starttstamp":1500003420, "dt":0.000, "evts":_evts(25), "mu":57.5, "force_new":False},
    {"run":300000, "lb":59, "starttstamp":1500003480, "dt":0.000, "evts":_evts(25), "mu":58.5, "force_new":False},
    {"run":300000, "lb":60, "starttstamp":1500003540, "dt":0.000, "evts":_evts(24), "mu":59.5, "force_new":False},
    {"run":300000, "lb":61, "starttstamp":1500003600, "dt":0.000, "evts":_evts(24), "mu":60.5, "force_new":False},
    {"run":300000, "lb":62, "starttstamp":1500003660, "dt":0.000, "evts":_evts(23), "mu":61.5, "force_new":False},
    {"run":300000, "lb":63, "starttstamp":1500003720, "dt":0.000, "evts":_evts(21), "mu":62.5, "force_new":False},
    {"run":300000, "lb":64, "starttstamp":1500003780, "dt":0.000, "evts":_evts(19), "mu":63.5, "force_new":False},
    {"run":300000, "lb":65, "starttstamp":1500003840, "dt":0.000, "evts":_evts(18), "mu":64.5, "force_new":False},
    {"run":300000, "lb":66, "starttstamp":1500003900, "dt":0.000, "evts":_evts(15), "mu":65.5, "force_new":False},
    {"run":300000, "lb":67, "starttstamp":1500003960, "dt":0.000, "evts":_evts(12), "mu":66.5, "force_new":False},
    {"run":300000, "lb":68, "starttstamp":1500004020, "dt":0.000, "evts":_evts(10), "mu":67.5, "force_new":False},
    {"run":300000, "lb":69, "starttstamp":1500004080, "dt":0.000, "evts":_evts(8), "mu":68.5, "force_new":False},
    {"run":300000, "lb":70, "starttstamp":1500004140, "dt":0.000, "evts":_evts(7), "mu":69.5, "force_new":False},
    {"run":300000, "lb":71, "starttstamp":1500004200, "dt":0.000, "evts":_evts(5), "mu":70.5, "force_new":False},
    {"run":300000, "lb":72, "starttstamp":1500004260, "dt":0.000, "evts":_evts(3), "mu":71.5, "force_new":False},
    {"run":300000, "lb":73, "starttstamp":1500004320, "dt":0.000, "evts":_evts(2), "mu":72.5, "force_new":False},
    {"run":300000, "lb":74, "starttstamp":1500004380, "dt":0.000, "evts":_evts(2), "mu":73.5, "force_new":False},
    {"run":300000, "lb":75, "starttstamp":1500004440, "dt":0.000, "evts":_evts(1), "mu":74.5, "force_new":False},
    {"run":300000, "lb":76, "starttstamp":1500004500, "dt":0.000, "evts":_evts(1), "mu":75.5, "force_new":False},
    {"run":300000, "lb":77, "starttstamp":1500004560, "dt":0.000, "evts":_evts(1), "mu":76.5, "force_new":False},
    {"run":300000, "lb":78, "starttstamp":1500004620, "dt":0.000, "evts":_evts(1), "mu":77.5, "force_new":False},
    {"run":300000, "lb":79, "starttstamp":1500004680, "dt":0.000, "evts":_evts(1), "mu":78.5, "force_new":False},
    {"run":300000, "lb":80, "starttstamp":1500004740, "dt":0.000, "evts":_evts(1), "mu":79.5, "force_new":False},
    {"run":300000, "lb":81, "starttstamp":1500004800, "dt":0.000, "evts":_evts(1), "mu":80.5, "force_new":False}
    #--> end
]

import math
trfJobNumber = 1
trfMaxEvents = 25
corrMaxEvents = float(trfMaxEvents)

#We may need to repeat this run for long production jobs.
#NB: unlike vanilla variable-mu jobs, it"s possible to waste
#  up to trfMaxEvents-1 events per complete run in prodsys if
#  the number of events specified by this run is not evenly
#  divisible by trfMaxEvents.
runMaxEvents = sum(lb["evts"] for lb in JobMaker)
print("There are %d events in this run." % runMaxEvents)
jobsPerRun = int(math.ceil(float(runMaxEvents)/corrMaxEvents))
print("Assuming there are usually %d events per job. (Based on %d events in this job.)" % (corrMaxEvents, trfMaxEvents))
print("There must be %d jobs per run." % jobsPerRun)

sequentialEventNumbers = False

# Random mu sampling
randomMuSampling = False

# Load needed tools
from Digitization.RunDependentMCTaskIterator import getRunLumiInfoFragment
test_fragment = getRunLumiInfoFragment(jobnumber=(trfJobNumber-1), task=JobMaker, maxEvents=trfMaxEvents, sequentialEventNumbers=sequentialEventNumbers)


from RunDependentSimComps.RunLumiConfigTools import condenseRunLumiInfoFragment
print("Writing RunDMC trigger configuration fragment to file.  listOfRunsEvents = %s" %
      condenseRunLumiInfoFragment(test_fragment, "RunDMCTriggerRunsInfo.py"))


# migrated from SimTransformUtils.py
# TODO clean up, relocate
def pileUpCalc(nSignalEvts, refreshRate, nSubEvtPerBunch, nBunches):
    totalSubEvts  = nBunches*nSubEvtPerBunch
    totalSubEvts += totalSubEvts*refreshRate*nSignalEvts
    return totalSubEvts


import math
def makeBkgInputCol(flags, initialList, nBkgEvtsPerCrossing, correctForEmptyBunchCrossings):
    refreshrate = 1.0
    nSignalEvts = 1000
    if flags.Exec.MaxEvents > 0:
        nSignalEvts = int(flags.Exec.MaxEvents)
        print("Number of signal events (from athenaCommonFlags.EvtMax) = %s." % nSignalEvts)
    else:
        nSignalEvts = 0
        from PyUtils.MetaReader import read_metadata
        for inFile in list(flags.Input.Files):
            try:
                metadata = read_metadata(inFile)
                metadata = metadata[inFile]  # promote all keys one level up
                nSignalEvts += int(metadata["nentries"])
                print("{} -> __Test__001__:\n{}".format(__file__, nSignalEvts))
            except:
                logger.warning("Unable to open file [%s]"%inFile)
                logger.warning("caught:\n%s",err)
                import traceback
                traceback.print_exc()
        print("Number of signal events (read from files) = %s." % nSignalEvts)

    nBkgEventsPerFile = 5000
    try:
        from PyUtils.MetaReader import read_metadata
        metadata = read_metadata(initialList[0])
        metadata = metadata[initialList[0]]  # promote all keys one level up
        nBkgEventsPerFile = int(metadata["nentries"])
        print("{} -> __Test__001__:\n{}".format(__file__, nBkgEventsPerFile))
        print("Number of background events per file (read from file) = %s." % nBkgEventsPerFile)
    except:
        import traceback
        traceback.print_exc()
        print("Warning: Failed to count the number of background events in %s. "
              "Assuming 5000 - if this is an overestimate the job may die." % initialList[0])

    Nbunches = 1 + flags.Digitization.PU.FinalBunchCrossing - flags.Digitization.PU.InitialBunchCrossing
    nbunches = int(Nbunches)
    if correctForEmptyBunchCrossings:
        nbunches = int(math.ceil(float(nbunches)*float(flags.Digitization.PU.BunchSpacing)/float(flags.Beam.BunchSpacing)))
    print("Simulating a maximum of %s colliding-bunch crossings (%s colliding+non-colliding total) per signal event" % (nbunches, Nbunches))
    nBkgEventsForJob = pileUpCalc(float(nSignalEvts), 1.0, float(nBkgEvtsPerCrossing), nbunches)
    print("Number of background events required: %s. "
          "Number of background events in input files: %s" % (nBkgEventsForJob, nBkgEventsPerFile*len(initialList)))
    numberOfRepetitionsRequired = float(nBkgEventsForJob)/float(nBkgEventsPerFile*len(initialList))
    NumberOfRepetitionsRequired = 1 + int(math.ceil(numberOfRepetitionsRequired))
    uberList = []
    # FIXME many copies of this string seems rather inefficient
    for i in range(0, NumberOfRepetitionsRequired):
        uberList += initialList
    print("Expanding input list from %s to %s" % (len(initialList), len(uberList)))
    return uberList
