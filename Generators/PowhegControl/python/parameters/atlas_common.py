# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""! Common Standard Model parameters for Powheg.

@author James Robinson  <james.robinson@cern.ch>
"""


class mass(object):
    """! Particle masses."""

    ## electron mass
    e = 0.00051

    ## muon mass
    mu = 0.1057

    ## tau mass
    tau = 1.777

    ## down quark mass
    d = 0.32

    ## up quark mass
    u = 0.32

    ## strange quark mass
    s = 0.5

    ## charm quark mass
    c = 1.55

    ## bottom quark mass
    b = 4.95

    ## top quark mass
    t = 172.5

    ## W boson mass
    W = 80.399

    ## Z boson mass
    Z = 91.1876

    ## Higgs boson mass
    H = 125.0


class width(object):
    """! Particle widths."""

    ## top quark width
    t = 1.32

    ## W boson width
    W = 2.085

    ## Z boson width
    Z = 2.4952

    ## Higgs boson width
    H = 0.00407


class branching_ratio(object):
    """! Particle branching ratios."""

    ## BR W -> ev
    W_to_enu = 0.1082

    ## BR W -> lv (sum of e, mu, tau)
    W_to_leptons = 3 * W_to_enu

    ## BR W -> qq
    W_to_hadrons = 1.0 - W_to_leptons

    ## BR t -> Wb
    t_to_Wb = 1.0

    ## BR t -> Ws
    t_to_Ws = 0.0

    ## BR t -> Wd
    t_to_Wd = 0.0


class CKM(object):
    """! CKM matrix."""

    ## CKM Vud
    Vud = 0.97428

    ## CKM Vus
    Vus = 0.2253

    ## CKM Vub
    Vub = 0.00347

    ## CKM Vcd
    Vcd = 0.2252

    ## CKM Vcs
    Vcs = 0.97345

    ## CKM Vcb
    Vcb = 0.041

    ## CKM Vtd
    Vtd = 0.00862

    ## CKM Vts
    Vts = 0.0403

    ## CKM Vtb
    Vtb = 0.999152


## EM coupling
alphaem_0 = 0.00729735252  # 1/137.036

## EM coupling
alphaem = 0.00781653039  # 1/127.934

## Strong coupling
alphaqcd = 0.1185

## Fermi constant
G_F = 0.00001166397

## sin(theta_W) ^ 2
sin2thetaW = 0.23113

## sin(theta_Cabibbo) ^ 2
sin2cabibbo = 0.051
