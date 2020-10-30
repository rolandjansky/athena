# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#
# File: CaloClusterCorrection/python/CaloSwPhimod_v4_data.py
# Created: Sep 2010, sss
# Purpose: Phi modulation corrections, v4, data version.
#
# It turns out that the direction of the accordions is backwards in the MC
# for the barrel.
# So for the data, we flip the sign of the phase arguments in this region.
# Recall that the correction has the form
#
#  p0 * (a*cos(N*phi + p1) + (1-a)*cos(2*N*phi + p3))
#
# where a = atan(p2)/pi + 0.5
#
# So here we flip the signs of p1 and p3; that corresponds to making
# this correction identical under a phi -> -phi transformation.
#

from CaloClusterCorrection.CaloSwPhimod_v4 import CaloSwPhimod_v4_parms


def _flip_phi3 (eta, p0, p1, p2, p3):
    if abs(eta) < 1.5:
        p1 = -p1
        p3 = -p3
    return [eta, p0, p1, p2, p3]
def _flip_phi2 (corr2):
    return [_flip_phi3(eta,p0,p1,p2,p3) for (eta,p0,p1,p2,p3) in corr2]
def _flip_phi1 (corr1):
    return [_flip_phi2(c) for c in corr1]
def _flip_phi (corr):
    return dict([[k, _flip_phi1(v)] for k,v in corr.items()])

    
def _copy_parms (src, dst):
    for (k, v) in src.__dict__.items():
        if k[0] == '_': continue
        if k == 'correction':
            v = _flip_phi (v)
        setattr (dst, k, v)
    return


class CaloSwPhimod_v4data_parms:
    pass

_copy_parms (CaloSwPhimod_v4_parms, CaloSwPhimod_v4data_parms)


