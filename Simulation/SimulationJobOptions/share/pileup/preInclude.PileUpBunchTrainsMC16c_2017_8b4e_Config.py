####################################################################################
## This configuration replicates the beam configuration in the
## following file: 25ns_1916b_1909_1042_1560_112bpi_20i8b4e.csv.
## Any of the filled bunch crossings can be chosen as the central
## bunch crossing, so the timing of the out-of-time pile-up will vary
## significantly from event to event.  Cavern Background is
## independent of the bunch pattern. (Compatible with 25ns cavern
## background.)
####################################################################################

from Digitization.DigitizationFlags import digitizationFlags

digitizationFlags.cavernIgnoresBeamInt = True

digitizationFlags.bunchSpacing = 25 # This now sets the bunch slot length.


digitizationFlags.BeamIntensityPattern = [0.0, # bunch crossing zero is always empty
0.0,0.0,0.0,0.0,0.0,0.0,         # Starting BCID:    0, Gap: 150 ns

1.0,1.0,1.0,1.0,1.0,             # Starting BCID:    7, Colliding:  5 BCIDs
0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,
0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,
0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,
0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,
0.0,0.0,0.0,0.0,0.0,0.0,         # Starting BCID:   12, Gap: 950 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID:   50, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID:   58, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID:   62, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID:   70, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID:   74, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID:   82, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID:   86, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID:   94, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID:   98, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID:  106, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID:  110, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID:  118, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID:  122, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,
0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0, # Starting BCID:  130, Gap: 775 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID:  161, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID:  169, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID:  173, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID:  181, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID:  185, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID:  193, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID:  197, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID:  205, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID:  209, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID:  217, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID:  221, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID:  229, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID:  233, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0,0.0,0.0,0.0, # Starting BCID:  241, Gap: 175 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID:  248, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID:  256, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID:  260, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID:  268, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID:  272, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID:  280, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID:  284, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID:  292, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID:  296, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID:  304, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID:  308, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID:  316, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID:  320, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,
0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0, # Starting BCID:  328, Gap: 775 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID:  359, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID:  367, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID:  371, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID:  379, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID:  383, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID:  391, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID:  395, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID:  403, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID:  407, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID:  415, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID:  419, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID:  427, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID:  431, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0,0.0,0.0,0.0, # Starting BCID:  439, Gap: 175 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID:  446, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID:  454, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID:  458, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID:  466, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID:  470, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID:  478, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID:  482, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID:  490, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID:  494, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID:  502, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID:  506, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID:  514, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID:  518, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,
0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0, # Starting BCID:  526, Gap: 775 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID:  557, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID:  565, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID:  569, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID:  577, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID:  581, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID:  589, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID:  593, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID:  601, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID:  605, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID:  613, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID:  617, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID:  625, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID:  629, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0,0.0,0.0,0.0, # Starting BCID:  637, Gap: 175 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID:  644, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID:  652, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID:  656, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID:  664, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID:  668, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID:  676, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID:  680, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID:  688, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID:  692, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID:  700, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID:  704, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID:  712, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID:  716, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,
0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,
0.0,0.0, # Starting BCID:  724, Gap: 850 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID:  758, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID:  766, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID:  770, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID:  778, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID:  782, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID:  790, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID:  794, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID:  802, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID:  806, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID:  814, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID:  818, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID:  826, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID:  830, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,
0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0, # Starting BCID:  838, Gap: 775 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID:  869, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID:  877, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID:  881, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID:  889, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID:  893, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID:  901, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID:  905, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID:  913, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID:  917, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID:  925, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID:  929, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID:  937, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID:  941, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0,0.0,0.0,0.0, # Starting BCID:  949, Gap: 175 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID:  956, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID:  964, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID:  968, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID:  976, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID:  980, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID:  988, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID:  992, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID: 1000, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 1004, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID: 1012, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 1016, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID: 1024, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 1028, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,
0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0, # Starting BCID: 1036, Gap: 775 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 1067, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID: 1075, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 1079, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID: 1087, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 1091, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID: 1099, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 1103, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID: 1111, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 1115, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID: 1123, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 1127, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID: 1135, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 1139, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0,0.0,0.0,0.0, # Starting BCID: 1147, Gap: 175 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 1154, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID: 1162, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 1166, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID: 1174, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 1178, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID: 1186, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 1190, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID: 1198, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 1202, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID: 1210, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 1214, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID: 1222, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 1226, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,
0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0, # Starting BCID: 1234, Gap: 775 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 1265, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID: 1273, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 1277, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID: 1285, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 1289, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID: 1297, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 1301, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID: 1309, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 1313, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID: 1321, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 1325, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID: 1333, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 1337, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0,0.0,0.0,0.0, # Starting BCID: 1345, Gap: 175 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 1352, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID: 1360, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 1364, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID: 1372, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 1376, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID: 1384, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 1388, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID: 1396, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 1400, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID: 1408, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 1412, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID: 1420, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 1424, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,
0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0, # Starting BCID: 1432, Gap: 775 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 1463, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID: 1471, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 1475, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID: 1483, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 1487, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID: 1495, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 1499, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID: 1507, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 1511, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID: 1519, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 1523, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID: 1531, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 1535, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0,0.0,0.0,0.0, # Starting BCID: 1543, Gap: 175 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 1550, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID: 1558, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 1562, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID: 1570, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 1574, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID: 1582, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 1586, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID: 1594, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 1598, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID: 1606, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 1610, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID: 1618, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 1622, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,
0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,
0.0,0.0, # Starting BCID: 1630, Gap: 850 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 1664, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID: 1672, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 1676, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID: 1684, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 1688, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID: 1696, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 1700, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID: 1708, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 1712, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID: 1720, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 1724, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID: 1732, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 1736, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,
0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0, # Starting BCID: 1744, Gap: 775 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 1775, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID: 1783, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 1787, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID: 1795, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 1799, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID: 1807, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 1811, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID: 1819, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 1823, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID: 1831, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 1835, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID: 1843, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 1847, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0,0.0,0.0,0.0, # Starting BCID: 1855, Gap: 175 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 1862, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID: 1870, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 1874, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID: 1882, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 1886, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID: 1894, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 1898, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID: 1906, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 1910, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID: 1918, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 1922, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID: 1930, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 1934, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,
0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0, # Starting BCID: 1942, Gap: 775 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 1973, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID: 1981, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 1985, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID: 1993, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 1997, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID: 2005, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 2009, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID: 2017, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 2021, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID: 2029, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 2033, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID: 2041, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 2045, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0,0.0,0.0,0.0, # Starting BCID: 2053, Gap: 175 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 2060, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID: 2068, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 2072, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID: 2080, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 2084, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID: 2092, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 2096, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID: 2104, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 2108, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID: 2116, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 2120, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID: 2128, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 2132, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,
0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0, # Starting BCID: 2140, Gap: 775 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 2171, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID: 2179, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 2183, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID: 2191, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 2195, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID: 2203, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 2207, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID: 2215, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 2219, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID: 2227, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 2231, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID: 2239, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 2243, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0,0.0,0.0,0.0, # Starting BCID: 2251, Gap: 175 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 2258, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID: 2266, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 2270, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID: 2278, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 2282, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID: 2290, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 2294, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID: 2302, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 2306, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID: 2314, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 2318, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID: 2326, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 2330, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,
0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0, # Starting BCID: 2338, Gap: 775 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 2369, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID: 2377, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 2381, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID: 2389, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 2393, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID: 2401, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 2405, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID: 2413, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 2417, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID: 2425, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 2429, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID: 2437, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 2441, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0,0.0,0.0,0.0, # Starting BCID: 2449, Gap: 175 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 2456, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID: 2464, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 2468, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID: 2476, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 2480, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID: 2488, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 2492, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID: 2500, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 2504, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID: 2512, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 2516, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID: 2524, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 2528, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,
0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,
0.0,0.0, # Starting BCID: 2536, Gap: 850 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 2570, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID: 2578, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 2582, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID: 2590, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 2594, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID: 2602, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 2606, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID: 2614, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 2618, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID: 2626, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 2630, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID: 2638, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 2642, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,
0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0, # Starting BCID: 2650, Gap: 775 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 2681, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID: 2689, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 2693, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID: 2701, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 2705, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID: 2713, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 2717, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID: 2725, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 2729, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID: 2737, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 2741, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID: 2749, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 2753, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0,0.0,0.0,0.0, # Starting BCID: 2761, Gap: 175 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 2768, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID: 2776, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 2780, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID: 2788, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 2792, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID: 2800, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 2804, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID: 2812, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 2816, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID: 2824, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 2828, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID: 2836, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 2840, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,
0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0, # Starting BCID: 2848, Gap: 775 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 2879, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID: 2887, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 2891, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID: 2899, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 2903, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID: 2911, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 2915, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID: 2923, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 2927, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID: 2935, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 2939, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID: 2947, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 2951, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0,0.0,0.0,0.0, # Starting BCID: 2959, Gap: 175 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 2966, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID: 2974, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 2978, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID: 2986, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 2990, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID: 2998, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 3002, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID: 3010, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 3014, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID: 3022, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 3026, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID: 3034, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 3038, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,
0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0, # Starting BCID: 3046, Gap: 775 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 3077, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID: 3085, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 3089, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID: 3097, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 3101, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID: 3109, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 3113, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID: 3121, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 3125, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID: 3133, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 3137, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID: 3145, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 3149, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0,0.0,0.0,0.0, # Starting BCID: 3157, Gap: 175 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 3164, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID: 3172, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 3176, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID: 3184, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 3188, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID: 3196, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 3200, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID: 3208, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 3212, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID: 3220, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 3224, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID: 3232, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 3236, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,
0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0, # Starting BCID: 3244, Gap: 775 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 3275, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID: 3283, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 3287, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID: 3295, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 3299, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID: 3307, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 3311, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID: 3319, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 3323, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID: 3331, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 3335, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID: 3343, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 3347, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0,0.0,0.0,0.0, # Starting BCID: 3355, Gap: 175 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 3362, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID: 3370, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 3374, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID: 3382, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 3386, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID: 3394, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 3398, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID: 3406, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 3410, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID: 3418, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 3422, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0, # Starting BCID: 3430, Gap: 100 ns

1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0, # Starting BCID: 3434, Colliding:  8 BCIDs
0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,
0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,
0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,
0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,
0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,
0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,
0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,
0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0] # Staring BCID: 3442 - Abort gap

from AthenaCommon.BeamFlags import jobproperties
jobproperties.Beam.bunchSpacing = 25 # Set this to the spacing between filled bunch-crossings within the train.
