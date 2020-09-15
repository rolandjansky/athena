# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""! Common Standard Model parameters for Powheg.

@author James Robinson  <james.robinson@cern.ch>
"""

## Import python dictionary with particle parameters and EW parameters. Particle parameters in there are extracted from PDG API.
## When using the PDF values, the lepton masses are a bit more precise compared to the old ones, the quark masses are very different from the old ones, the W and H mass are also different, same holds for the width of the top quark and the H boson.
from EvgenProdTools.offline_dict import parameters
from ..decorators import Singleton

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

class EW_parameters(object):
    """! EW parameters."""

    ## EM coupling at 0
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
    
def usePDGvalues():
    ## electron mass
    mass.e = parameters['particles']['11']['mass']

    ## muon mass
    mass.mu = parameters['particles']['13']['mass']

    ## tau mass
    mass.tau = parameters['particles']['13']['mass']

    ## down quark mass
    mass.d = parameters['particles']['1']['mass']

    ## up quark mass
    mass.u = parameters['particles']['2']['mass']

    ## strange quark mass
    mass.s = parameters['particles']['3']['mass']

    ## charm quark mass
    mass.c = parameters['particles']['4']['mass']

    ## bottom quark mass
    mass.b = parameters['particles']['5']['mass']

    ## top quark mass
    mass.t = parameters['particles']['6']['mass']

    ## W boson mass
    mass.W = parameters['particles']['24']['mass']

    ## Z boson mass
    mass.Z = parameters['particles']['23']['mass']

    ## Higgs boson mass
    mass.H = parameters['particles']['25']['mass']

    ## top quark width
    width.t = parameters['particles']['6']['width']

    ## W boson width
    width.W = parameters['particles']['24']['width']

    ## Z boson width
    width.Z = parameters['particles']['23']['width']

    ## Higgs boson width
    width.H = parameters['particles']['25']['width']

    ## BR W -> ev
    branching_ratio.W_to_enu = parameters['EW_parameters']['W_to_enu']

    ## BR W -> lv (sum of e, mu, tau)
    branching_ratio.W_to_leptons = parameters['EW_parameters']['W_to_leptons']

    ## BR W -> qq
    branching_ratio.W_to_hadrons = parameters['EW_parameters']['W_to_hadrons']

    ## BR t -> Wb
    branching_ratio.t_to_Wb = parameters['EW_parameters']['t_to_Wb']

    ## BR t -> Ws
    branching_ratio.t_to_Ws = parameters['EW_parameters']['t_to_Wd']

    ## BR t -> Wd
    branching_ratio.t_to_Wd = parameters['EW_parameters']['t_to_Ws']

    ## CKM Vud
    CKM.Vud = parameters['EW_parameters']['Vud']

    ## CKM Vus
    CKM.Vus = parameters['EW_parameters']['Vus']

    ## CKM Vub
    CKM.Vub = parameters['EW_parameters']['Vub']

    ## CKM Vcd
    CKM.Vcd = parameters['EW_parameters']['Vcd']

    ## CKM Vcs
    CKM.Vcs = parameters['EW_parameters']['Vcs']

    ## CKM Vcb
    CKM.Vcb = parameters['EW_parameters']['Vcb']

    ## CKM Vtd
    CKM.Vtd = parameters['EW_parameters']['Vtd']

    ## CKM Vts
    CKM.Vts = parameters['EW_parameters']['Vts']

    ## CKM Vtb
    CKM.Vtb = parameters['EW_parameters']['Vtb']

    ## EM coupling at 0
    EW_parameters.alphaem_0 = parameters['EW_parameters']['alphaem_0']

    ## EM coupling
    EW_parameters.alphaem = parameters['EW_parameters']['alphaem']

    ## Strong coupling
    EW_parameters.alphaqcd = parameters['EW_parameters']['alphaqcd']

    ## Fermi constant
    EW_parameters.G_F = parameters['EW_parameters']['G_F']

    ## sin(theta_W) ^ 2
    EW_parameters.sin2thetaW = parameters['EW_parameters'][('SIN2THETAW', 'sin2thetaW', 'Sin2ThetaW')]

    ## sin(theta_Cabibbo) ^ 2
    EW_parameters.sin2cabibbo = parameters['EW_parameters']['sin2cabibbo']
