# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#
# File: CaloClusterCorrection/python/CaloSwGap_v2.py
# Created: Nov 2006, sss
# Purpose: Gap correction, v2.
#
# This correction was derived by Scott Snyder using DC2 MC files,
# reconstructed with 8.6.0.  Electrons at 50, 100, and 200 GeV were used.
#
# This was added in LArClusterRec-02-05-27, in 9.4.0.
#
# More details:
#
# \def\brocket#1{\left\langle #1 \right\rangle}
# This correction attempts to correct for the energy lost in the gap
# between the cryostats.  To improve the energy resolution in this
# region, we look in the tile calorimeter scintillator
# (TileGap3), and sum the energy in a rectangular region
# of $\Delta\eta=\pm0.2$, $\Delta\phi=\pm2\pi/64$ around the EM cluster.
# We form a corrected energy as a weighted sum:
#
# \begin{equation}
#   E' = aE_c + bE_s,
# \end{equation}
#
# where $E_c$ and $E_s$ are the cluster and scintillator energies,
# respectively.  It is convenient to redefine the weights like this:
#
# \begin{equation}
#  E' = A(E_c + \alpha E_s),
# \end{equation}
#
# The weights are defined as a function of $|\eta|$.
#
#  To begin with, we need to define what we want the result of this
# correction to be.  We'd like to avoid having to use detailed MC
# information about energy deposited in the dead material, both for
# simplicity and also so that the same method may be used on test beam
# (or even collider) data.  We do this by fitting a function to the plot
# of $R\equiv\Emeas/\Etrue$ over the range outside of the gap,
# and interpolating it over the gap.
# To be specific, we fit a quartic polynomial, over the regions
# 0.1--0.75, 0.85--1.3, and 1.6--2.4.  The result of this fit will
# be denoted $\mu(|\eta|)$.
#
# To derive $A$ and $\alpha$ for a given sample, we can then write
# (for each $|\eta|$ bin):
#
# \begin{equation}
# A = {\mu \over \brocket{E_c} + \alpha\brocket{E_s}}
# \end{equation}
#
# Then we can fix $\alpha$ by demanding that the uncertainty in
# the corrected energy be a minimum:
#
# \begin{eqnarray}
# s^2_{E'} &=& \brocket{{E'}^2} - \mu^2\\
#          &=& A^2\left[\brocket{E_c^2} + 2\alpha\brocket{E_cE_s} +
#                      \alpha^2\brocket{E_s^2}\right] -\\
#          & &
#             A^2\left[\brocket{E_c}^2 + 2\alpha\brocket{E_c}\brocket{E_s} +
#                      \alpha^2\brocket{E_s}^2\right].
# \end{eqnarray}
#
# Note that $s^2_c = \brocket{E_c^2} - \brocket{E_c}^2$, etc., so:
#
# \begin{equation}
# s^2_{E'} = A^2\left[s^2_c + \alpha^2s^2_s + 2\alpha\gamma\right],
# \end{equation}
#
# where $\gamma = \brocket{E_cE_s} - \brocket{E_c}\brocket{E_s}$.
#
# Then,
#
# \begin{equation}
# {\partial s^2\over\partial\alpha} =
#   2A^2\left[ -{\brocket{E_s}\over\brocket{E_c} + \alpha\brocket{E_s}}
#              \left(s^2_c + \alpha^2s^2_s + 2\alpha\gamma\right)
#              + \alpha s^2_s + \gamma \right].
# \end{equation}
#
# Setting this to zero yields
#
# \begin{equation}
# \alpha = {\brocket{E_s}s^2_c - \brocket{E_c}\gamma \over
#           \brocket{E_c}s^2_s - \brocket{E_s}\gamma}.
# \end{equation}
#
# In the region $|\eta|<1.35$,
# there very little scintillator energy, so the large variation
# of $\alpha$ here doesn't make much difference.  Other than that,
# $A$ and $\alpha$ are pretty much consistent across the three samples
# except in the very center of the gap, $1.45<|\eta|<1.5$.  Therefore,
# we will average over all the energies and derive a single gap
# correction.  Any residual energy dependence will be folded into the
# next correction.
#
# Specifically, we do this averaging by setting
# %
# \begin{equation}
# A = \sum_i{\mu_i \over \brocket{E_{ci}} + \alpha\brocket{E_{si}}}
# \end{equation}
# %
# and then
# %
# \begin{equation}
# s^2_{E'} = A^2\sum_i\left[s^2_{ci} + \alpha^2s^2_{si} + 2\alpha\gamma_i\right].
# \end{equation}
#


from CaloClusterCorrection.constants import CALOCORR_CLUSTER


#############################################################################
# Gap energy correction up to version 11.x.x

CaloSwGap_v2 = [
             # escale    wgapSci  offset
    [1.303750, 1.016806, 1.683127, 0.0],
    [1.311250, 1.015786, 4.305477, 0.0],
    [1.318750, 1.015267, 5.610438, 0.0],
    [1.326250, 1.022010, 0.616439, 0.0],
    [1.333750, 1.022342, 1.376083, 0.0],
    [1.341250, 1.024709, 1.997833, 0.0],
    [1.348750, 1.029334, 0.765594, 0.0],
    [1.356250, 1.033014, 1.683585, 0.0],
    [1.363750, 1.039184, 1.455713, 0.0],
    [1.371250, 1.050692, 0.756170, 0.0],
    [1.378750, 1.045414, 0.812620, 0.0],
    [1.386250, 1.037649, 1.444074, 0.0],
    [1.393750, 1.039127, 1.336271, 0.0],
    [1.401250, 1.046041, 1.215431, 0.0],
    [1.408750, 1.037376, 1.142844, 0.0],
    [1.416250, 1.023183, 1.481310, 0.0],
    [1.423750, 1.026816, 1.354458, 0.0],
    [1.431250, 1.034325, 1.342707, 0.0],
    [1.438750, 1.045756, 1.275463, 0.0],
    [1.446250, 1.077950, 1.187051, 0.0],
    [1.453750, 1.112825, 1.081919, 0.0],
    [1.461250, 1.139116, 1.144534, 0.0],
    [1.468750, 1.132502, 1.423852, 0.0],
    [1.476250, 0.980396, 2.118332, 0.0],
    [1.483750, 0.971699, 2.144619, 0.0],
    [1.491250, 0.832878, 2.890122, 0.0],
    [1.498750, 0.976646, 1.659909, 0.0],
    [1.506250, 1.009499, 1.281173, 0.0],
    [1.513750, 0.963686, 1.395732, 0.0],
    [1.521250, 0.920715, 1.648667, 0.0],
    [1.528750, 0.904699, 1.641731, 0.0],
    [1.536250, 0.928021, 1.305559, 0.0],
    [1.543750, 0.938568, 1.163320, 0.0],
    [1.551250, 0.944110, 1.127526, 0.0],
    [1.558750, 0.953431, 1.108617, 0.0],
    [1.566250, 0.949993, 1.408784, 0.0],
    [1.573750, 0.943903, 1.644857, 0.0],
    [1.581250, 0.950200, 1.379264, 0.0],
    [1.588750, 0.947081, 1.684241, 0.0],
    [1.596250, 0.951890, 1.474423, 0.0]
    ]


#############################################################################


class CaloSwGap_v2_parms:
    region = CALOCORR_CLUSTER
    etamin_crack = 1.3
    etamax_crack = 1.6
    degree = 3
    use_raw_eta = False
    correction = CaloSwGap_v2
