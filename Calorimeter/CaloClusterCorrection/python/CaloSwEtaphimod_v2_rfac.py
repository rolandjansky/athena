# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#
# File: CaloClusterCorrection/python/CaloSwEtaphimod_v2_rfac.py
# Created: Nov 2006, sss
# Purpose: Crude energy response correction used internally by the v2
#          eta and phi modulation corrections.
#
# In v2 of the corrections, the eta and phi modulation corrections were
# done before the gross energy corrections.  The modulation corrections
# are, however, energy-dependent, and this was handled by doing
# an interpolation in energy.  What one has, however, is the uncorrected
# reconstructed energy, while what one would like to use for the interpolation
# is the true energy.  For the modulation corrections, the difference
# between these two made an observable difference.  This was dealt with
# by first scaling the energy by a crude, eta-dependent factor to get
# the energy approximately correct to use for the interpolation.
#
# In the v3 corrections, the modulation corrections were moved to after
# the gross energy corrections, so this was no longer an issue for those
# corrections.  (It is, however, still an issue for the position corrections.
# The same strategy is still used there, though the implementation is
# a bit different.)
#

#############################################################################
# Crude containment correction used internally by the eta and phi modulation
# corrections.
#

CaloSwEtaphimod_v2_rfac = [
[0.0125,   9.158837e-01],
[0.0375,   9.549339e-01],
[0.0625,   9.574016e-01],
[0.0875,   9.579815e-01],
[0.1125,   9.579382e-01],
[0.1375,   9.580596e-01],
[0.1625,   9.577885e-01],
[0.1875,   9.584260e-01],
[0.2125,   9.573891e-01],
[0.2375,   9.583148e-01],
[0.2625,   9.583855e-01],
[0.2875,   9.586575e-01],
[0.3125,   9.586427e-01],
[0.3375,   9.585706e-01],
[0.3625,   9.593939e-01],
[0.3875,   9.587972e-01],
[0.4125,   9.592283e-01],
[0.4375,   9.592296e-01],
[0.4625,   9.598244e-01],
[0.4875,   9.592557e-01],
[0.5125,   9.591028e-01],
[0.5375,   9.581793e-01],
[0.5625,   9.584898e-01],
[0.5875,   9.587682e-01],
[0.6125,   9.588540e-01],
[0.6375,   9.569409e-01],
[0.6625,   9.532247e-01],
[0.6875,   9.514746e-01],
[0.7125,   9.508389e-01],
[0.7375,   9.488401e-01],
[0.7625,   8.092602e-01],
[0.7875,   7.399155e-01],
[0.8125,   7.738033e-01],
[0.8375,   9.371027e-01],
[0.8625,   9.370135e-01],
[0.8875,   9.347815e-01],
[0.9125,   9.346408e-01],
[0.9375,   9.346482e-01],
[0.9625,   9.324597e-01],
[0.9875,   9.313113e-01],
[1.0125,   9.298394e-01],
[1.0375,   9.270200e-01],
[1.0625,   9.269487e-01],
[1.0875,   9.244406e-01],
[1.1125,   9.221041e-01],
[1.1375,   9.223799e-01],
[1.1625,   9.202957e-01],
[1.1875,   9.186866e-01],
[1.2125,   9.129674e-01],
[1.2375,   9.126045e-01],
[1.2625,   9.096150e-01],
[1.2875,   9.070148e-01],
[1.3125,   9.008394e-01],
[1.3375,   8.925832e-01],
[1.3625,   8.763337e-01],
[1.3875,   8.656485e-01],
[1.4125,   8.609927e-01],
[1.4375,   7.805832e-01],
[1.4625,   6.015924e-01],
[1.4875,   5.502519e-01],
[1.5125,   6.945754e-01],
[1.5375,   8.477056e-01],
[1.5625,   9.254098e-01],
[1.5875,   9.287047e-01],
[1.6125,   9.302695e-01],
[1.6375,   9.154825e-01],
[1.6625,   9.115917e-01],
[1.6875,   9.014728e-01],
[1.7125,   9.374421e-01],
[1.7375,   9.427920e-01],
[1.7625,   9.382549e-01],
[1.7875,   9.351722e-01],
[1.8125,   9.160857e-01],
[1.8375,   9.065793e-01],
[1.8625,   9.099748e-01],
[1.8875,   9.145431e-01],
[1.9125,   9.176964e-01],
[1.9375,   9.209157e-01],
[1.9625,   9.224258e-01],
[1.9875,   9.223443e-01],
[2.0125,   9.215440e-01],
[2.0375,   9.190382e-01],
[2.0625,   9.199215e-01],
[2.0875,   9.187429e-01],
[2.1125,   9.182841e-01],
[2.1375,   9.171402e-01],
[2.1625,   9.178473e-01],
[2.1875,   9.173785e-01],
[2.2125,   9.170678e-01],
[2.2375,   9.172518e-01],
[2.2625,   9.178104e-01],
[2.2875,   9.188930e-01],
[2.3125,   9.184640e-01],
[2.3375,   9.196976e-01],
[2.3625,   9.200801e-01],
[2.3875,   9.234573e-01],
[2.4125,   9.244256e-01],
[2.4375,   9.254043e-01],
[2.4625,   9.163888e-01],
[2.4875,   8.225253e-01],
    ]


