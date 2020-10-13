# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#
# File: CaloClusterCorrection/python/CaloSwPhimod.py
# Created: Nov 2006, sss
# Purpose: Steering module for phi modulation corrections.
#
# The response of the calorimeter varies slightly depending on the
# position of the particle impact in phi relative to the accordion
# absorber structure.  This correction attempts to remove this variation.
#
# Note, however, that the inherent uncertainties in energy and phi
# measurement --- including the phi smearing due to bremsstrahlung
# and the 1/\sqrt{E} behavior of the energy resolution --- will tend
# to smear this effect out, especially for lower energies.  We can only
# correct for the portion that's visible after the smearing.
#
# The correction is derived in the following way.
# First the calorimeter is binned in eta, to separate regions of the
# calorimeter that have different energy response behaviors.
# In each of these bins, for each particle, we take the measured phi
# position and convert it to an offset within the absorber structure,
# phi_cell.  (This is done by taking the modulus with respect to the phi size
# of the absorbers, which is 2pi/1024 in the barrel and 2pi/768
# in the endcap, corresponding to four and three absorbers per cell,
# respectively.)  If eta is negative, the sign of phi is flipped.
# We then plot R = E_meas/E_true versus phi_cell.
# This plot is binned in phi_cell.  In each of these bins, we project
# the R values to a 1-d histogram and fit a Gaussian.  The range for
# this fit is either 0.8 or 0.9 to 1.0, depending on the |eta| position.
# Finally, the mean from this fit and its uncertainty are plotted versus
# phi)cell to produce the phi-modulation plot.
#
# These plots are then fit with this functional form:
#
#  1 + A[alpha cos(N phi+C) + (1-alpha)cos(2N phi+D)],
#
# where alpha = atan B/pi + 1/2, and N is the total number of
# absorbers in 2pi (either 1024 or 768, depending on |eta|).
#


from AthenaConfiguration.ComponentFactory import CompFactory
from CaloClusterCorrection.constants import \
     CALOCORR_COOL, CALOCORR_DEFAULT_KEY, CALOCORR_SW
from CaloClusterCorrection.common import makecorr

#
# This table lists all available versions of this correction.
# See common.py for a description of the contents.
#
from CaloClusterCorrection.constants import sw_valid_keys as keys
cls_g3 = CompFactory.CaloSwPhimod_g3  # CaloClusterCorrection
cls_v2 = CompFactory.CaloSwPhimod_v2  # CaloClusterCorrection
CaloSwPhimod_versions = [

    # The original G3-based correction, translated from the
    # original fortran version.
    ['g3', cls_g3, ['CaloSwPhimod_g3.CaloSwPhimod_g3_parms',
                    'caloswcorr_pool', CALOCORR_COOL], keys],

    # DC2 simulation and 8.x.0 reconstruction.
    # Electrons at 50, 100, and 200 GeV were used.
    # Derived for 5x5 clusters only.
    # This was added in LArClusterRec-02-05-25, in 9.0.3.
    # This version runs before the gross energy corrections.
    ['v2', cls_v2, ['CaloSwPhimod_v2.CaloSwPhimod_v2_parms',
                    'caloswcorr_pool', CALOCORR_COOL], keys],

    # This is the same as v2, except that it runs after the
    # gross energy corrections.
    # It was added in CaloClusterCorrection-00-02-58, in 12.0.3.
    ['v3', cls_v2, ['CaloSwPhimod_v2.CaloSwPhimod_v3_parms',
                    'caloswcorr_pool', CALOCORR_COOL], keys],

    # This was derived using 12.0.3 simulation and reconstruction,
    # for 5x5, 3x5, and 3x7 clusters, for both electrons and photons.
    # It was added in CaloClusterCorrection-00-02-65, in 12.0.4.
    ['v4', cls_v2, ['CaloSwPhimod_v4.CaloSwPhimod_v4_parms',
                    'caloswcorr_pool', CALOCORR_COOL], keys],

    # Same as v4, but with the signs of the phase terms flipped in the barrel.
    # This because the orientation of the accordions is backwards in the MC.
    ['v4data', cls_v2, ['CaloSwPhimod_v4data.CaloSwPhimod_v4data_parms',
                        'caloswcorr_pool', CALOCORR_COOL], keys],
    ]


#
# Create a new tool instance.
#  FLAGS is the configuration flags instance.
#  NAME is the base name for this tool.  If defaulted, a name will
#   be constructed from the name of the correction, the version, and the key.
#  If SUFFIX is not None, it will be added onto the end of the tool name.
#  VERSION is a string specifying which of several versions of the correction
#   to choose.  If defaulted, the latest version of the correction is chosen.
#  KEY is a string to specify the type of cluster to which the correction
#   applies.  The convention is to use `ele55' for 5x5 electron clusters,
#   `gam35' for 3x5 photon clusters, and so on.
#  SOURCE tells from where we should read the calibration constants.
#   See common.py for a description of the possibilities.
#   None means to use the default.
#  CONFCLASS gives the Configurable class to use to create the tool.
#   It may (and usually should) be defaulted.
#
# Additional keyword arguments may be passed to override any tool
# parameters/constants.
#
def make_CaloSwPhimod (flags,
                       name = None,
                       suffix = None,
                       version = None,
                       key = CALOCORR_DEFAULT_KEY,
                       source = None,
                       confclass = None,
                       **kw):
    # Make the tool.
    return makecorr (flags,
                     versions  = CaloSwPhimod_versions,
                     name      = name,
                     basename  = 'phimod',
                     suffix    = suffix,
                     version   = version,
                     key       = key,
                     sampling  = None,
                     source    = source,
                     confclass = confclass,
                     corrclass = CALOCORR_SW,
                     **kw)

