# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#
# File: CaloClusterCorrection/python/CaloSwCorrections.py
# Created: Nov 2006, sss
# Purpose: Top-level configuration file for EM cluster corrections.
#
# The main entry point here is make_CaloSwCorrectionsCfg, which returns
# ComponentAccumulator with
# a list of correction tools.  It can be used either `le menu' --- selecting
# a predefined list of correction tools, or `a la carte' --- explicitly
# specifying the list of tools you want to run.
#
# The simplest way of using it is like this:
#
#   ca = make_CaloSwCorrectionsCfg (flags, key)
#
# Here, `key' is a string that denotes the type of cluster which you
# are correcting.  It should be something like `ele55' for 5x5 electrons,
# `gam35' for 3x5 photons, etc.  If this argument is defaulted, `ele55'
# will be used.
#
# Tools should have unique names.  If you are using the same corrections
# multiple times (in offline and in trigger, for example), you should
# specify the `suffix' argument to make them unique.  This is a string
# that will be added to the end of the tool names.
#
# By default, the latest version of the corrections appropriate to the
# geometry being used will be chosen.  You can override this selection
# by supplying the `version' argument.  It may also be overridden
# with Calo.ClusterCorrection.CaloSwWhichCorrection.
# For the list of known version
# names, see the variable `CaloSwCorrection_versions' below.
# In particular, using version `none' turns off the corrections.
#
# Each individual correction has a version as well, independent
# of the version of the overall correction list.  You can override
# the version to be used for individual correction by adding
# parameters of the form CORRECTION_version.  For example:
#
#   make_CaloSwCorrectionsCfg (flags ,key, version='v4', lwc_version='v3')
#
# uses the `v4' corrections, except that we use version `v3' of the
# layer weight corrections.  You can also specify CORRECTION_key
# to override the key setting for an individual correction,
# and CORRECTION_XXX to override any individual parameter
# of a correction.  For example:
#
#   make_CaloSwCorrectionsCfg (flags, key, lwc_degree=2)
#
# makes the full standard set of corrections, except that the interpolation
# degree for the layer weights correction is changed to 2.
#
# You can explicitly specify a list of corrections to run with the
# `corrlist' argument.  For example,
#
#   make_CaloSwCorrectionsCfg (flags, key, corrlist=[[layers], [update], [gap]])
#
# runs only the `layers', `update', and `gap' corrections.  The names
# used here, like `layers' should be functions defined with this
# signature:
#
#    def layers (flags, cells_name, suffix, version, key, source, **kw):
#
# Definitions for all the standard corrections are contained in this file.
# Note that the names of these functions are what are used to recognize
# the CORRNAME in additional arguments of the form CORRNAME_XXX.
#
# If you just give the corrections without an explicit version
# (as in the example above), then what we do is this.  We first
# find the correction list we would use if the corrlist argument
# had not been specified (the `vcorrlist').  For each correction
# in the corrlist with no specified version, we look in the vcorrlist
# for a matching correction.  If we find one, the version from the
# vcorrlist is used.  Otherwise, the most recent version of the
# correction is used.
#
# The version of a correction in the corrlist may be explicitly
# specified as the second element in the list.  Additional parameter
# settings may be specified as following (name, value) tuples
# in the list.  For example:
#
#   make_CaloSwCorrectionsCfg (flags,
#                              key, corrlist=[[layers],
#                                             [update],
#                                             [gap, 'v3', ('degree', 2)]])
#
# says to use version `v3' of the gap correction, and in addition,
# to override the interpolation degree to be 2.
#
# If the parameter `weighting' is set to True, then the standard correction
# list will be augmented so that cells will be weighted to avoid
# double-counting the energy of cells that are in more than one cluster.
# This may also be enabled by adding the string `_wt' to the end
# of the correction version name, or by setting the
# Calo.ClusterCorrections.doSlidingWindowCellWeights flag.
#
# Similarly, if the parameter `remdup' is set to True, then the standard
# correction list will be augmented so that if there are multiple clusters
# too close together, all but one will be removed.
# This may also be enabled by adding the string `_remdup' to the end
# of the correction version name, or by setting the
# Calo.ClusterCorrection.doSlidingWindowRemoveDuplicates flag.
#
# Also, if the parameter `rembad' is set to True, then the standard
# correction list will be augmented so that clusters deemed to be
# bad (below an energy threshold) will be removed.
# This may also be enabled by adding the string `_rembad' to the end
# of the correction version name, or by setting the
# Calo.ClusterCorrection.doSlidingWindowRemoveBad flag.
#
# There are three ways in which a correction can get configured: from
# job options, from pool, or from cool.  You can specify which ones
# to use by adding the `source' argument to make_CaloSwCorrectionsCfg.
# This is one of the following (defined in constants.py):
#
#  - CALOCORR_JO: Configure from job options
#  - CALOCORR_POOL: Configure from a pool file
#  - CALOCORR_COOL: Configure from cool.
#
# The name of a pool file defined in the dictionary of poolfiles.py
# may also be used.
#
# This parameter may be specified as a list of any of these values; in that
# case, all the possibilities will be tried in order.  The default is taken
# from Calo.ClusterCorrection.defaultSource
# if it hasn't been explicitly set.  The usual setting is to try first
# cool, then pool, then job options.  (Note that there are a couple
# corrections which are not saved to pool/cool and can thus only
# be initialized from job options.)
#
# You may also override the source for an individual correction
# by adding an argument with a name of the form CORRECTION_source.
# The value of this may be anything that could be specified for source;
# in addition, you may explicitly specify a job options parameter class to use
# with a string of the form `MODULE.NAME'.
#

# Need to be sure that we always get run3 configurables in the imported
# steering modules.
from AthenaCommon.Configurable import Configurable

try:
    _wasRun3 = Configurable.configurableRun3Behavior
    Configurable.configurableRun3Behavior = True

    import re
    from CaloClusterCorrection.CaloComputeSWcellWeights \
         import make_CaloComputeSWcellWeights
    from CaloClusterCorrection.CaloSwLayers      import make_CaloSwLayers
    from CaloClusterCorrection.CaloSwRfac        import make_CaloSwRfac
    from CaloClusterCorrection.CaloSwEtaoff      import make_CaloSwEtaoff
    from CaloClusterCorrection.CaloSwPhioff      import make_CaloSwPhioff
    from CaloClusterCorrection.CaloSwEtamod      import make_CaloSwEtamod
    from CaloClusterCorrection.CaloSwPhimod      import make_CaloSwPhimod
    from CaloClusterCorrection.CaloSwClusterUpdate \
         import make_CaloSwClusterUpdate
    from CaloClusterCorrection.CaloSwGap         import make_CaloSwGap
    from CaloClusterCorrection.CaloSwLongWeights import make_CaloSwLongWeights
    from CaloClusterCorrection.CaloSwCalibHitsCalibration import make_CaloSwCalibHitsCalibration
    from CaloClusterCorrection.CaloSwDeadOTX_ps import make_CaloSwDeadOTX_ps
    from CaloClusterCorrection.CaloSwDeadOTX_back import make_CaloSwDeadOTX_back
    from CaloClusterCorrection.CaloSwTransitionRegionsCorr import make_CaloSwTransitionRegionsCorr
    from CaloClusterCorrection.CaloClusterListBadChannel   import make_CaloClusterListBadChannel
    from CaloClusterCorrection.CaloSwClcon       import make_CaloSwClcon
    from CaloClusterCorrection.CaloSwTime        import make_CaloSwTime
    from CaloClusterCorrection.constants         import \
         CALOCORR_SW, EMB1, EME1, EMB2, EME2
    from CaloClusterCorrection.common            import CaloClusterCorrSetup
    from CaloClusterCorrection.compat            import makeFlags, unpackCA
    
    from CaloClusterCorrection.CaloClusterRemoveDuplicates import make_CaloClusterRemoveDuplicates
    from CaloClusterCorrection.CaloClusterRemoveBad import make_CaloClusterRemoveBad

finally:
    Configurable.configurableRun3Behavior = _wasRun3


##############################################################################
# Here we define wrapper functions to set up all of the standard corrections.
# In the case where a correction has multiple versions for different
# samplings, we define multiple wrappers here.
# These are the names to use in the correction list and in the
# arguments to make_CaloSwCorrectionCfg.
#

def layers (flags, cells_name, *args, **kw):
    return make_CaloSwLayers (flags, None, cells_name=cells_name, *args, **kw)

def weight (flags, cells_name, *args, **kw):
    return make_CaloComputeSWcellWeights (flags, None, *args, **kw)

def rfac (flags, cells_name, *args, **kw):
    return make_CaloSwRfac (flags, None, *args, **kw)

def etaoff_b1 (flags, cells_name, *args, **kw):
    return make_CaloSwEtaoff (flags, EMB1, None, *args, **kw)

def etaoff_b2 (flags, cells_name, *args, **kw):
    return make_CaloSwEtaoff (flags, EMB2, None, *args, **kw)

def etaoff_e1 (flags, cells_name, *args, **kw):
    return make_CaloSwEtaoff (flags, EME1, None, *args, **kw)

def etaoff_e2 (flags, cells_name, *args, **kw):
    return make_CaloSwEtaoff (flags, EME2, None, *args, **kw)

def phioff_b2 (flags, cells_name, *args, **kw):
    return make_CaloSwPhioff (flags, EMB2, None, *args, **kw)

def phioff_e2 (flags, cells_name, *args, **kw):
    return make_CaloSwPhioff (flags, EME2, None, *args, **kw)

def phimod (flags, cells_name, *args, **kw):
    return make_CaloSwPhimod (flags, None, *args, **kw)

def etamod (flags, cells_name, *args, **kw):
    return make_CaloSwEtamod (flags, None, *args, **kw)

def update (flags, cells_name, *args, **kw):
    return make_CaloSwClusterUpdate (flags, None, *args, **kw)

def gap (flags, cells_name, *args, **kw):
    return make_CaloSwGap (flags, None, cells_name=cells_name, *args, **kw)

def lwc (flags, cells_name, *args, **kw):
    return make_CaloSwLongWeights (flags, None, *args, **kw)

def calhits (flags, cells_name, *args, **kw):
    return make_CaloSwCalibHitsCalibration (flags, None, *args, **kw)

def trcorr (flags, cells_name, *args, **kw):
    return make_CaloSwTransitionRegionsCorr (flags, None, *args, **kw)

def deadOTXps (flags, cells_name, *args, **kw):
    return make_CaloSwDeadOTX_ps (flags, None, *args, **kw )

def deadOTXback (flags, cells_name, *args, **kw):
    return make_CaloSwDeadOTX_back (flags, None, *args, **kw )

def clcon (flags, cells_name, *args, **kw):
    return make_CaloSwClcon (flags, None, *args, **kw)

def removeduplicates (flags, cells_name, *args, **kw):
    return make_CaloClusterRemoveDuplicates (flags, None, *args, **kw)

def removebad (flags, cells_name, *args, **kw):
    return make_CaloClusterRemoveBad (flags, None, *args, **kw)

def listBadChannel (flags, cells_name, *args, **kw):
    return make_CaloClusterListBadChannel (flags, CALOCORR_SW, None, *args, **kw)

def time (flags, cells_name, *args, **kw):
    return make_CaloSwTime (flags, None, *args, **kw)


##############################################################################
# The class responsible for making the corrections.
#

def _version_match (version, fragment):
    pat = '(_' + fragment + ')(_|$)'
    m = re.search (pat, version)
    ret = False
    if m:
        ret = True
        version = version[:m.start(1)] + version[m.end(1):]
    return (ret, version)


class CaloSwCorrectionsSetup (CaloClusterCorrSetup):

    name = "EM sliding-window"
    version_override_flag_name = 'caloSwWhichCorrection'
    correction_generation_flag_name = 'caloSwGeneration'
    correction_generation_default = "00-02-13"

    ##########################################################################
    # This is the list of all known correction versions.
    versions = {

        # A special case: disable all corrections except those
        # technically required.
        'none' : [[layers,               100],
                  [update,               300]
                  ],

        # The original Geant3-based corrections, translated from
        # the original Fortran reconstruction code.
        'g3' : [[layers,                     100],
                [etaoff_b2, 'g3',            201],
                [etaoff_e2, 'g3',            202],
                [phioff_b2, 'g3',            203],
                [phioff_e2, 'g3',            204],
                [etaoff_b1, 'g3',            205],
                [etaoff_e1, 'g3',            206],
                [phimod,    'g3',            250],
                [etamod,    'g3',            251],
                [update,    'dont_update_e', 300],
                [clcon,     'g3',            410],
                [gap,       'g3',            420],
                [time,                       801]],

        # This is the same as what was in 10.0.0.
        # This uses the old layer weights correction, from release 9,
        # which were for a single cluster size only.
        # An additional eta- and E-dependent correction is made
        # before the layer weights correction; this correction was
        # tuned to make the Ereco/Etrue ratios come out to 1.
        # It was tuned on DC2 single-electron samples at energies
        # of 50, 100, and 200 GeV; simple Gaussian fits were used
        # to extract the peak positions.
        'DC2' : [[layers,                     100],
                 [etaoff_b2, 'v2',            201],
                 [etaoff_e2, 'v2',            202],
                 [phioff_b2, 'v2',            203],
                 [phioff_e2, 'v2',            204],
                 [etaoff_b1, 'v2',            205],
                 [etaoff_e1, 'v2',            206],
                 [phimod,    'v2',            250],
                 [etamod,    'v2',            251],
                 [update,    'dont_update_e', 300],
                 [gap,       'v2',            401],
                 [clcon,     'dc2',           410],
                 [lwc,       'old',           450],
                 [time,                       801]],

        # This scheme uses the new layer weight corrections from Stathes,
        # based on 9.0.4 MC.  These are now evaluated for multiple
        # cluster sizes.  In addition, there is a final
        # eta- and E-dependent correction made after the layer weights
        # correction, tuned to make the Ereco/Etrue ratios come out to 1.
        # It was tuned on DC2 single-electron samples at energies
        # of 20, 50, 100, 200, 500, and 1000 GeV.  Double Gaussian fits
        # were used to extract the peak positions.
        'DC2new' : [[layers,                     100],
                    [etaoff_b2, 'v2',            201],
                    [etaoff_e2, 'v2',            202],
                    [phioff_b2, 'v2',            203],
                    [phioff_e2, 'v2',            204],
                    [etaoff_b1, 'v2',            205],
                    [etaoff_e1, 'v2',            206],
                    [phimod,    'v2',            250],
                    [etamod,    'v2',            251],
                    [update,    'dont_update_e', 300],
                    [gap,       'v2',            401],
                    [lwc,       '904',           450],
                    [clcon,     'dc2new',        480],
                    [time,                       801]],

        # This scheme uses the new layer weight corrections from Stathes,
        # based on 9.0.4 MC.  No additional correction were applied.
        # Between the DC2 and 9.0.4 samples, there is approximately a 1%
        # difference observed in the endcap; there may also be a several
        # percent difference in the gap region.  That's why we have separate
        # options for DC2 and for Rome.
        'Rome' : [[layers,                     100],
                  [etaoff_b2, 'v2',            201],
                  [etaoff_e2, 'v2',            202],
                  [phioff_b2, 'v2',            203],
                  [phioff_e2, 'v2',            204],
                  [etaoff_b1, 'v2',            205],
                  [etaoff_e1, 'v2',            206],
                  [phimod,    'v2',            250],
                  [etamod,    'v2',            251],
                  [update,    'dont_update_e', 300],
                  [gap,       'v2',            401],
                  [lwc,       '904gap',        450],
                  [time,                       801]],

        # This scheme uses the new layer weight corrections from Stathes,
        # based on 11.0.41 MC.  No additional correction were applied.
        'DC3-02' : [[layers,                     100],
                    [etaoff_b2, 'v2',            201],
                    [etaoff_e2, 'v2',            202],
                    [phioff_b2, 'v2',            203],
                    [phioff_e2, 'v2',            204],
                    [etaoff_b1, 'v2',            205],
                    [etaoff_e1, 'v2',            206],
                    [phimod,    'v2',            250],
                    [etamod,    'v2',            251],
                    [update,    'dont_update_e', 300],
                    [gap,       'v3',            401],
                    [lwc,       'v3',            450],
                    [time,                       801]],

        # This scheme has new position corrections, which are now evaluated
        # for multiple cluster sizes and separately for electrons and photons.
        # The energy corrections for electrons are the same as DC3-02,
        # but new corrections have been added for photons.
        # The modulation corrections are the same as before, except that
        # they have been moved to the end.
        'DC3-02-V3' : [[layers,            100],
                       [rfac,      'v3',   150],
                       [etaoff_b1, 'v3',   201],
                       [etaoff_e1, 'v3',   202],
                       [etaoff_b2, 'v3',   203],
                       [etaoff_e2, 'v3',   204],
                       [phioff_b2, 'v3',   205],
                       [phioff_e2, 'v3',   206],
                       [update,            300],
                       [gap,       'v3_1', 401],
                       [lwc,       'v3_1', 450],
                       [phimod,    'v3',   501],
                       [etamod,    'v3',   502],
                       [time,              801]],

        # For 12.0.4.
        # Corrections have been rederived using 12.0.3 simulation
        # and reconstruction.  Modulation corrections have not yet
        # been rederived.
        'v4' : [[layers,             100],
                [rfac,      'v4',    150],
                [etaoff_b1, 'v4',    201],
                [etaoff_e1, 'v4',    202],
                [etaoff_b2, 'v4',    203],
                [etaoff_e2, 'v4',    204],
                [phioff_b2, 'v4',    205],
                [phioff_e2, 'v4',    206],
                [update,             300],
                [gap,       'v4',    401],
                [lwc,       'v4',    450],
                [phimod,    'v4',    501],
                [etamod,    'v4',    502],
                [time,               801],
                [listBadChannel,     820]],

        # Same as v4, except for the eta offset; the depth used
        # to derive this correction is replaced by the optimized one,
        # and large-statistics samples are used for the 50 and 100 GeV points.
        'v4_1' : [[layers,             100],
                  [rfac,      'v4',    150],
                  [etaoff_b1, 'v4_1',  201],
                  [etaoff_e1, 'v4_1',  202],
                  [etaoff_b2, 'v4_1',  203],
                  [etaoff_e2, 'v4_1',  204],
                  [phioff_b2, 'v4',    205],
                  [phioff_e2, 'v4',    206],
                  [update,             300],
                  [gap,       'v4',    401],
                  [lwc,       'v4',    450],
                  [phimod,    'v4',    501],
                  [etamod,    'v4',    502],
                  [time,               801],
                  [listBadChannel,     820]],

        # Same as v4, except that the layer weights correction is replaced
        # by an energy correction derived from calibration hits.
        'v4_calh' : [[layers,          100],
                     [rfac,      'v4', 150],
                     [etaoff_b1, 'v4', 201],
                     [etaoff_e1, 'v4', 202],
                     [etaoff_b2, 'v4', 203],
                     [etaoff_e2, 'v4', 204],
                     [phioff_b2, 'v4', 205],
                     [phioff_e2, 'v4', 206],
                     [update,          300],
                     [gap,       'v4', 401],
                     [calhits,   'v1', 450],
                     [phimod,    'v4', 501],
                     [etamod,    'v4', 502],
                     [time,            801],
                     [listBadChannel,  820]],

        # Includes updated calhits correction, the v4_1 etaoff correction,
        # and the new trcorr correction.
        'v4_1_calh' : [[layers,            100],
                       [rfac,      'v4',   150],
                       [etaoff_b1, 'v4_1', 201],
                       [etaoff_e1, 'v4_1', 202],
                       [etaoff_b2, 'v4_1', 203],
                       [etaoff_e2, 'v4_1', 204],
                       [phioff_b2, 'v4',   205],
                       [phioff_e2, 'v4',   206],
                       [update,            300],
                       [gap,       'v4',   401],
                       [calhits,   'v2',   450],
                       [trcorr,    'v1',   451],   
                       [phimod,    'v4',   501],
                       [etamod,    'v4',   502],
                       [time,              801],
                       [listBadChannel,    820]],

        # Layer weights correction updated for release 14.0.0 geometry
        # (ATLAS-CSC-05-00-00).  Other corrections still based on 12.
        'v5_lwc' : [[layers,                   100],
                    [rfac,      'v4',          150],
                    [etaoff_b1, 'v4_1',        201],
                    [etaoff_e1, 'v4_1',        202],
                    [etaoff_b2, 'v4_1',        203],
                    [etaoff_e2, 'v4_1',        204],
                    [phioff_b2, 'v4',          205],
                    [phioff_e2, 'v4',          206],
                    [update,                   300],
                    [gap,       'v4',          401],
                    [lwc,       'v5',          450],
                    [phimod,    'v4',          501],
                    [etamod,    'v4',          502],
                    [time,                     801],
                    [listBadChannel,           820]],

        # Layer weights correction updated for geometry
        # (ATLAS-CSC-05-00-00).  Other corrections still based on 12.
        'v6_lwc' : [[layers,                   100],
                    [rfac,      'v4',          150],
                    [etaoff_b1, 'v4_1',        201],
                    [etaoff_e1, 'v4_1',        202],
                    [etaoff_b2, 'v4_1',        203],
                    [etaoff_e2, 'v4_1',        204],
                    [phioff_b2, 'v4',          205],
                    [phioff_e2, 'v4',          206],
                    [update,                   300],
                    [gap,       'v4',          401],
                    [lwc,       'v6',          450],
                    [phimod,    'v4',          501],
                    [etamod,    'v4',          502],
                    [time,                     801],
                    [listBadChannel,           820]],
                      
        
        # Calibration hits and transition region corrections updated
        # for release 14.0.0 geometry (ATLAS-CSC-05-00-00).
        # Other corrections still based on 12.
        'v5_calh' : [[layers,                  100],
                     [rfac,      'v4',         150],
                     [etaoff_b1, 'v4_1',       201],
                     [etaoff_e1, 'v4_1',       202],
                     [etaoff_b2, 'v4_1',       203],
                     [etaoff_e2, 'v4_1',       204],
                     [phioff_b2, 'v4',         205],
                     [phioff_e2, 'v4',         206],
                     [update,                  300],
                     [gap,       'v4',         401],
                     [calhits,   'v5',         450],
                     [trcorr,    'v5',         451],
                     [phimod,    'v4',         501],
                     [etamod,    'v4',         502],
                     [time,                    801],
                     [listBadChannel,          820]],

        # Same as v5_calh, except for some fixes at eta=0 and eta=0.8.
        'v5_1_calh' : [[layers,                100],
                       [rfac,      'v4',       150],
                       [etaoff_b1, 'v4_1',     201],
                       [etaoff_e1, 'v4_1',     202],
                       [etaoff_b2, 'v4_1',     203],
                       [etaoff_e2, 'v4_1',     204],
                       [phioff_b2, 'v4',       205],
                       [phioff_e2, 'v4',       206],
                       [update,                300],
                       [gap,       'v4',       401],
                       [calhits,   'v5_1',     450],
                       [trcorr,    'v5_1',     451],
                       [phimod,    'v4',       501],
                       [etamod,    'v4',       502],
                       [time,                  801],
                       [listBadChannel,        820]],

        # Calibration hits correction using release 14 geometry
        # (ATLAS-CSC-05-00-00).
        # Now have separate calibration hits corrections for
        # converted and unconverted photons.
        # Other corrections still based on 12.
        'v6_calh' : [[layers,                100],
                       [rfac,      'v4',     150],
                       [etaoff_b1, 'v4_1',   201],
                       [etaoff_e1, 'v4_1',   202],
                       [etaoff_b2, 'v4_1',   203],
                       [etaoff_e2, 'v4_1',   204],
                       [phioff_b2, 'v4',     205],
                       [phioff_e2, 'v4',     206],
                       [update,              300],
                       [gap,       'v4',     401],
                       [calhits,   'v6',     450],
                       [trcorr,    'v5_1',   451],
                       [phimod,    'v4',     501],
                       [etamod,    'v4',     502],
                       [time,                801],
                       [listBadChannel,      820]],

        # Same as v6_calh, except that the signs of the phi offset correction
        # and the phase terms in the phi modulation correction are flipped
        # in the barrel.  This because the orientation of the accordions
        # is backwards in the MC in that region.
        'v6data_calh' : [[layers,                100],
                         [rfac,      'v4',     150],
                         [etaoff_b1, 'v4_1',   201],
                         [etaoff_e1, 'v4_1',   202],
                         [etaoff_b2, 'v4_1',   203],
                         [etaoff_e2, 'v4_1',   204],
                         [phioff_b2, 'v4data', 205],
                         [phioff_e2, 'v4data', 206],
                         [update,              300],
                         [gap,       'v4',     401],
                         [calhits,   'v6',     450],
                         [trcorr,    'v5_1',   451],
                         [phimod,    'v4data', 501],
                         [etamod,    'v4',     502],
                         [time,                801],
                         [listBadChannel,      820]],

        # Same as v6_calh, except that the signs of the phi offset correction
        # and the phase terms in the phi modulation correction are flipped 
        # in the barrel.  This because the orientation of the accordions
        # is backwards in the MC in that region. 
        # Also, the out-of-cone correction for calibration hits
        # is increased by the DATA-MC difference as estimated with W events
        'v6dataleak_calh' : [[layers,                100],
                         [rfac,      'v4',     150],   
                         [etaoff_b1, 'v4_1',   201],   
                         [etaoff_e1, 'v4_1',   202],   
                         [etaoff_b2, 'v4_1',   203],   
                         [etaoff_e2, 'v4_1',   204],   
                         [phioff_b2, 'v4data', 205],
                         [phioff_e2, 'v4data', 206],
                         [update,              300],   
                         [gap,       'v4',     401],   
                         [calhits,   'v6leakdata', 450],   
                         [trcorr,    'v5_1',   451],   
                         [phimod,    'v4data', 501],
                         [etamod,    'v4',     502],   
                         [time,                801],
                         [listBadChannel,      820]],  

        # Calibration hits correction using release 14 geometry
        # (ATLAS-CSC-05-00-00).
        # Corrections for dead OTX added
        # Other corrections still based on 12.
        'v7_calh' : [[layers,                100],
                       [rfac,       'v4',     150],
                       [etaoff_b1,  'v4_1',   201],
                       [etaoff_e1,  'v4_1',   202],
                       [etaoff_b2,  'v4_1',   203],
                       [etaoff_e2,  'v4_1',   204],
                       [phioff_b2,  'v4',     205],
                       [phioff_e2,  'v4',     206],
                       [update,               300],
                       [gap,        'v4',     401],
                       [calhits,    'v6',     450],
                       [trcorr,     'v5_1',   451],
                       [deadOTXps,  'v1'  ,   460],
                       [deadOTXback,'v1'  ,   461],
                       [phimod,     'v4',     501],
                       [etamod,     'v4',     502],
                       [time,                 801],
                       [listBadChannel,       820]],

        # Same as v7_calh, except that the signs of the phi offset correction
        # and the phase terms in the phi modulation correction are flipped
        # in the barrel.  This because the orientation of the accordions
        # is backwards in the MC in that region.
        'v7data_calh' : [[layers,                100],
                         [rfac,       'v4',     150],
                         [etaoff_b1,  'v4_1',   201],
                         [etaoff_e1,  'v4_1',   202],
                         [etaoff_b2,  'v4_1',   203],
                         [etaoff_e2,  'v4_1',   204],
                         [phioff_b2,  'v4data', 205],
                         [phioff_e2,  'v4data', 206],
                         [update,               300],
                         [gap,        'v4',     401],
                         [calhits,    'v6',     450],
                         [trcorr,     'v5_1',   451],
                         [deadOTXps,  'v1'  ,   460],
                         [deadOTXback,'v1'  ,   461],
                         [phimod,     'v4data', 501],
                         [etamod,     'v4',     502],
                         [time,                 801],
                         [listBadChannel,       820]],

        # Same as v7_calh, except that the signs of the phi offset correction
        # and the phase terms in the phi modulation correction are flipped 
        # in the barrel.  This because the orientation of the accordions
        # is backwards in the MC in that region. 
        # Also, the out-of-cone correction for calibration hits
        # is increased by the DATA-MC difference as estimated with W events
        'v7dataleak_calh' : [[layers,                100],   
                         [rfac,       'v4',     150],   
                         [etaoff_b1,  'v4_1',   201],   
                         [etaoff_e1,  'v4_1',   202],   
                         [etaoff_b2,  'v4_1',   203],   
                         [etaoff_e2,  'v4_1',   204],   
                         [phioff_b2,  'v4data', 205],
                         [phioff_e2,  'v4data', 206],
                         [update,               300],
                         [gap,        'v4',     401],   
                         [calhits,    'v6leakdata',  450],   
                         [trcorr,     'v5_1',   451],   
                         [deadOTXps,  'v1'  ,   460],   
                         [deadOTXback,'v1'  ,   461],   
                         [phimod,     'v4data', 501],
                         [etamod,     'v4',     502],   
                         [time,                 801],
                         [listBadChannel,       820]],

        # Same as v7_calh + Update of the calibration hit correction from v6 to v8 (derived from ATLAS-GEO-16) for MC
        'v8_calh' :    [[layers,                100],   
                         [rfac,       'v4',     150],   
                         [etaoff_b1,  'v4_1',   201],   
                         [etaoff_e1,  'v4_1',   202],   
                         [etaoff_b2,  'v4_1',   203],   
                         [etaoff_e2,  'v4_1',   204],   
                         [phioff_b2,  'v4',     205],
                         [phioff_e2,  'v4',     206],
                         [update,               300],   
                         [gap,        'v4',     401],   
                         [calhits,    'v8',     450],   
                         [trcorr,     'v5_1',   451],   
                         [deadOTXps,  'v1'  ,   460],   
                         [deadOTXback,'v1'  ,   461],   
                         [phimod,     'v4'  ,   501],
                         [etamod,     'v4',     502],   
                         [time,                 801],   
                         [listBadChannel,       820]],


        # Same as v7dataleak_calh + Update of the calibration hit correction from v6 to v8 (derived from ATLAS-GEO-16) for MC
        'v8dataleak_calh' :    [[layers,                100],   
                         [rfac,       'v4',     150],   
                         [etaoff_b1,  'v4_1',   201],   
                         [etaoff_e1,  'v4_1',   202],   
                         [etaoff_b2,  'v4_1',   203],   
                         [etaoff_e2,  'v4_1',   204],   
                         [phioff_b2,  'v4data', 205],
                         [phioff_e2,  'v4data', 206],
                         [update,               300],   
                         [gap,        'v4',     401],   
                         [calhits,    'v8leakdata',  450],   
                         [trcorr,     'v5_1',   451],   
                         [deadOTXps,  'v1'  ,   460],   
                         [deadOTXback,'v1'  ,   461],   
                         [phimod,     'v4data', 501],
                         [etamod,     'v4',     502],   
                         [time,                 801],   
                         [listBadChannel,       820]],

        # Same as v8_calh + Update of the calibration hit correction from v8 to v9 (derived from ATLAS-GEO-18-01) for MC
        'v9_calh' :    [[layers,                100],
                         [rfac,       'v4',     150],   
                         [etaoff_b1,  'v4_1',   201],   
                         [etaoff_e1,  'v4_1',   202],   
                         [etaoff_b2,  'v4_1',   203],   
                         [etaoff_e2,  'v4_1',   204],   
                         [phioff_b2,  'v4',     205],   
                         [phioff_e2,  'v4',     206],   
                         [update,               300],
                         [gap,        'v4',     401],   
                         [calhits,    'v9',     450],   
                         [trcorr,     'v5_1',   451],   
                         [deadOTXps,  'v1'  ,   460],   
                         [deadOTXback,'v1'  ,   461],   
                         [phimod,     'v4'  ,   501],   
                         [etamod,     'v4',     502],   
                         [time,                 801],
                         [listBadChannel,       820]],


        # Same as v8dataleak_calh + Update of the calibration hit correction from v8 to v9 (derived from ATLAS-GEO-18-01) for MC
        #   Out-of-cone leakage correction still tuned to data
        'v9dataleak_calh' :    [[layers,                100],
                         [rfac,       'v4',     150],   
                         [etaoff_b1,  'v4_1',   201],   
                         [etaoff_e1,  'v4_1',   202],   
                         [etaoff_b2,  'v4_1',   203],   
                         [etaoff_e2,  'v4_1',   204],   
                         [phioff_b2,  'v4data', 205],
                         [phioff_e2,  'v4data', 206],
                         [update,               300],   
                         [gap,        'v4',     401],   
                         [calhits,    'v9leakdata',  450],   
                         [trcorr,     'v5_1',   451],   
                         [deadOTXps,  'v1'  ,   460],   
                         [deadOTXback,'v1'  ,   461],   
                         [phimod,     'v4data', 501],
                         [etamod,     'v4',     502],   
                         [time,                 801],
                         [listBadChannel,       820]],

        # v9_calh + new gap correction.
        'v10_calh' :    [[layers,               100],
                         [rfac,       'v4',     150],   
                         [etaoff_b1,  'v4_1',   201],   
                         [etaoff_e1,  'v4_1',   202],   
                         [etaoff_b2,  'v4_1',   203],   
                         [etaoff_e2,  'v4_1',   204],   
                         [phioff_b2,  'v4',     205],   
                         [phioff_e2,  'v4',     206],   
                         [update,               300],
                         [gap,        'v5',     401],   
                         [calhits,    'v9',     450],   
                         [trcorr,     'v5_1',   451],   
                         [deadOTXps,  'v1'  ,   460],   
                         [deadOTXback,'v1'  ,   461],   
                         [phimod,     'v4'  ,   501],   
                         [etamod,     'v4',     502],   
                         [time,                 801],
                         [listBadChannel,       820]],

        # v9dataleak_calh + new gap correction.
        'v10dataleak_calh' :    [[layers,               100],
                                 [rfac,       'v4',     150],   
                                 [etaoff_b1,  'v4_1',   201],   
                                 [etaoff_e1,  'v4_1',   202],   
                                 [etaoff_b2,  'v4_1',   203],   
                                 [etaoff_e2,  'v4_1',   204],   
                                 [phioff_b2,  'v4data', 205],
                                 [phioff_e2,  'v4data', 206],
                                 [update,               300],   
                                 [gap,        'v5',     401],   
                                 [calhits,    'v9leakdata',  450],   
                                 [trcorr,     'v5_1',   451],   
                                 [deadOTXps,  'v1'  ,   460],   
                                 [deadOTXback,'v1'  ,   461],   
                                 [phimod,     'v4data', 501],
                                 [etamod,     'v4',     502],   
                                 [time,                 801],
                                 [listBadChannel,       820]],

        # v9_calh + new phi-dependent gap correction.
        'v11_calh' :    [[layers,               100],
                         [rfac,       'v4',     150],   
                         [etaoff_b1,  'v4_1',   201],   
                         [etaoff_e1,  'v4_1',   202],   
                         [etaoff_b2,  'v4_1',   203],   
                         [etaoff_e2,  'v4_1',   204],   
                         [phioff_b2,  'v4',     205],   
                         [phioff_e2,  'v4',     206],   
                         [update,               300],
                         [gap,        'v6',     401],   
                         [calhits,    'v9',     450],   
                         [trcorr,     'v5_1',   451],   
                         [deadOTXps,  'v1'  ,   460],   
                         [deadOTXback,'v1'  ,   461],   
                         [phimod,     'v4'  ,   501],   
                         [etamod,     'v4',     502],   
                         [time,                 801],
                         [listBadChannel,       820]],

        # v9dataleak_calh + new phi-dependent gap correction.
        'v11dataleak_calh' :    [[layers,               100],
                                 [rfac,       'v4',     150],   
                                 [etaoff_b1,  'v4_1',   201],   
                                 [etaoff_e1,  'v4_1',   202],   
                                 [etaoff_b2,  'v4_1',   203],   
                                 [etaoff_e2,  'v4_1',   204],   
                                 [phioff_b2,  'v4data', 205],
                                 [phioff_e2,  'v4data', 206],
                                 [update,               300],   
                                 [gap,        'v6',     401],   
                                 [calhits,    'v9leakdata',  450],   
                                 [trcorr,     'v5_1',   451],   
                                 [deadOTXps,  'v1'  ,   460],   
                                 [deadOTXback,'v1'  ,   461],   
                                 [phimod,     'v4data', 501],
                                 [etamod,     'v4',     502],   
                                 [time,                 801],
                                 [listBadChannel,       820]],


        # No energy corrections, except for gap.
        'v11_noecorr' : [[layers,               100],
                         [rfac,       'v4',     150],   
                         [etaoff_b1,  'v4_1',   201],   
                         [etaoff_e1,  'v4_1',   202],   
                         [etaoff_b2,  'v4_1',   203],   
                         [etaoff_e2,  'v4_1',   204],   
                         [phioff_b2,  'v4',     205],   
                         [phioff_e2,  'v4',     206],   
                         [update,               300],
                         [gap,        'v6',     401],   
                         [time,                 801],
                         [listBadChannel,       820]],

        # No energy corrections, except for gap.
        # phi offset correction sense flipped for data.
        'v11phiflip_noecorr' : [[layers,               100],
                             [rfac,       'v4',     150],   
                             [etaoff_b1,  'v4_1',   201],   
                             [etaoff_e1,  'v4_1',   202],   
                             [etaoff_b2,  'v4_1',   203],   
                             [etaoff_e2,  'v4_1',   204],   
                             [phioff_b2,  'v4data', 205],
                             [phioff_e2,  'v4data', 206],
                             [update,               300],   
                             [gap,        'v6',     401],   
                             [time,                 801],
                             [listBadChannel,       820]],

        # Updated with new position corrections from Ewan Hill.
        # This version was derived from mc12, and so has the negative
        # part of the calorimeter in the wrong phi direction.
        # v11_calh + updated rfac, phioff, etaoff: v5.
        'v12_calh' :    [[layers,               100],
                         [rfac,       'v5',     150],   
                         [etaoff_b1,  'v5',     201],   
                         [etaoff_e1,  'v5',     202],   
                         [etaoff_b2,  'v5',     203],   
                         [etaoff_e2,  'v5',     204],   
                         [phioff_b2,  'v5',     205],   
                         [phioff_e2,  'v5',     206],   
                         [update,               300],
                         [gap,        'v6',     401],   
                         [calhits,    'v9',     450],   
                         [trcorr,     'v5_1',   451],   
                         [deadOTXps,  'v1'  ,   460],   
                         [deadOTXback,'v1'  ,   461],   
                         [phimod,     'v4'  ,   501],   
                         [etamod,     'v4',     502],   
                         [time,                 801],
                         [listBadChannel,       820]],

        # Same as v12_calh, but with the sense of phi flipped
        # in the negative calorimeter.  This could be used for mc14.
        'v12phiflip_calh' : [
                         [layers,               100],
                         [rfac,       'v5',     150],   
                         [etaoff_b1,  'v5',     201],   
                         [etaoff_e1,  'v5',     202],   
                         [etaoff_b2,  'v5',     203],   
                         [etaoff_e2,  'v5',     204],   
                         [phioff_b2,  'v5data', 205],   
                         [phioff_e2,  'v5data', 206],   
                         [update,               300],
                         [gap,        'v6',     401],   
                         [calhits,    'v9',     450],   
                         [trcorr,     'v5_1',   451],   
                         [deadOTXps,  'v1'  ,   460],   
                         [deadOTXback,'v1'  ,   461],   
                         [phimod,     'v4'  ,   501],   
                         [etamod,     'v4',     502],   
                         [time,                 801],
                         [listBadChannel,       820]],

        # Same as v12_calh, but with the sense of phi flipped
        # in the negative calorimeter and with the OOC adjusted
        # for the DATA-MC difference as estimated with W events.
        # This is for data.
        'v12dataleak_calh' : [
                         [layers,               100],
                         [rfac,       'v5',     150],   
                         [etaoff_b1,  'v5',     201],   
                         [etaoff_e1,  'v5',     202],   
                         [etaoff_b2,  'v5',     203],   
                         [etaoff_e2,  'v5',     204],   
                         [phioff_b2,  'v5data', 205],   
                         [phioff_e2,  'v5data', 206],   
                         [update,               300],
                         [gap,        'v6',     401],   
                         [calhits,    'v9leakdata', 450],   
                         [trcorr,     'v5_1',   451],   
                         [deadOTXps,  'v1'  ,   460],   
                         [deadOTXback,'v1'  ,   461],   
                         [phimod,     'v4'  ,   501],   
                         [etamod,     'v4',     502],   
                         [time,                 801],
                         [listBadChannel,       820]],


        # Updated with new position corrections from Ewan Hill.
        # No energy corrections except for gap.
        # This version was derived from mc12, and so has the negative
        # part of the calorimeter in the wrong phi direction.
        'v12_noecorr' : [[layers,               100],
                         [rfac,       'v5',     150],   
                         [etaoff_b1,  'v5',     201],   
                         [etaoff_e1,  'v5',     202],   
                         [etaoff_b2,  'v5',     203],   
                         [etaoff_e2,  'v5',     204],   
                         [phioff_b2,  'v5',     205],   
                         [phioff_e2,  'v5',     206],   
                         [update,               300],
                         [gap,        'v6',     401],   
                         [time,                 801],
                         [listBadChannel,       820]],


        # Same as v12_noecorr, but with the sense of phi flipped
        # in the negative calorimeter.  This could be used for mc14
        # or for data.
        'v12phiflip_noecorr' : [
                         [layers,               100],
                         [rfac,       'v5',     150],   
                         [etaoff_b1,  'v5',     201],   
                         [etaoff_e1,  'v5',     202],   
                         [etaoff_b2,  'v5',     203],   
                         [etaoff_e2,  'v5',     204],   
                         [phioff_b2,  'v5data', 205],   
                         [phioff_e2,  'v5data', 206],   
                         [update,               300],
                         [gap,        'v6',     401],   
                         [time,                 801],
                         [listBadChannel,       820]],

        # Same as v12phiflip_noecorr but remove also the gap correction, i.e only position corrections are applied
        'v12phiflip_noecorrnogap' : [
                         [layers,               100],
                         [rfac,       'v5',     150],   
                         [etaoff_b1,  'v5',     201],   
                         [etaoff_e1,  'v5',     202],   
                         [etaoff_b2,  'v5',     203],   
                         [etaoff_e2,  'v5',     204],   
                         [phioff_b2,  'v5data', 205],   
                         [phioff_e2,  'v5data', 206],   
                         [update,               300],
                         [time,                 801],
                         [listBadChannel,       820]],

        # Layer weights correction for Atlfast2 simulation
        # with ATLAS-CSC-05-00-00 geometry (14.0.0).
        'v1_atlfast' : [[layers,                    100],
                        [rfac,      'v4',           150],
                        [etaoff_b1, 'v4_1',         201],
                        [etaoff_e1, 'v4_1',         202],
                        [etaoff_b2, 'v4_1',         203],
                        [etaoff_e2, 'v4_1',         204],
                        [phioff_b2, 'v4',           205],
                        [phioff_e2, 'v4',           206],
                        [update,                    300],
                        [lwc,       'atlfast_v1',   450],
                        [phimod,    'v4',           501],
                        [etamod,    'v4',           502],
                        [time,                      801],
                        [listBadChannel,            820]],

        # Layer weights correction for low energy photons from pi0, to apply to 3x5 clusters seeded by Emtopo
        'pi0_v1_lwc' : [[layers,                   100],
                        [update,                   300],
                        [lwc,       'pi0_v1',       450],
                        [time,                     801],
                        [listBadChannel,           820]],

        'pi0_v2_noc' : [[layers,                   100],
                        [update,                   300],
                        [time,                     801],
                        [listBadChannel,           820]],


        # Define the list of correction tools to run when
        # hierarchical tagging is used.
        # All tools that may possibly be used for these corrections
        # should be listed here.  Tools that are not configured
        # from the database should have the proper order and
        # version (if any); tools that are configured from the
        # database should not have order set and should have the
        # version set to `@'.
        # The ordering of tools in this list has no significance.
        '@' :         [[layers,                     100],
                       [rfac,       '@'],
                       [etaoff_b1,  '@'],
                       [etaoff_e1,  '@'],
                       [etaoff_b2,  '@'],
                       [etaoff_e2,  '@'],
                       [phioff_b2,  '@'],
                       [phioff_e2,  '@'],
                       [update,     '@'],
                       [gap,        '@'],
                       [lwc,        '@'],
                       [calhits,    '@'],
                       [deadOTXps,  '@'],
                       [deadOTXback,'@'],
                       [clcon,      '@'],
                       [trcorr,     '@'],
                       [phimod,     '@'],
                       [etamod,     '@'],
                       [time,                       801],
                       [listBadChannel,             820],
                       ],
        }

    # Alias for the newest correction.
    newest_version = '@GLOBAL'


    ##########################################################################
    # Mapping between detector description version and the appropriate
    # correction version.  Anything not listed here gets the most recent
    # correction.
    #
    # These are first matched against a string of the form
    # DATASOURCE-GEOM; if that doesn't match, we try just GEOM.
    #
    # Note: By default, these will be converted to hierarchical tags
    # if we read from cool.  To disable this, start the version name
    # with `NOHIER#'.
    #
    geom_versions = [
        # Use global tag by default for real data.
        ['data-*'                , '@GLOBAL'],
        
        ['DC1*'                  , 'DC2new'],
        ['DC2*'                  , 'DC2new'],
        ['ATLAS-00*'             , 'DC2new'],
        ['ATLAS-DC2*'            , 'DC2new'],
        ['ATLAS-01*'             , 'Rome'],
        ['Rome*'                 , 'Rome'],
        ['ATLAS-Rome*'           , 'Rome'],
        ['DC3*'                  , 'DC3-02-V3'],
        ['ATLAS-DC3*'            , 'DC3-02-V3'],
        ['CSC*'                  , 'v4_1_calh'],
        ['ATLAS-CSC-00*'         , 'v4_1_calh'],
        ['ATLAS-CSC-01*'         , 'v4_1_calh'],
        ['ATLAS-CSC-02*'         , 'v4_1_calh'],
        ['ATLAS-CSC-03*'         , 'v4_1_calh'],
        ['ATLAS-CSC-04*'         , 'v4_1_calh'],
        ['ATLAS-CSC-05*'         , 'v6_calh'],
        #['ATLAS-GEO-*'           , '@GLOBAL'],
        ['ATLAS-GEO-*'           , newest_version],
        #['ATLAS-GEO-*'           , 'v11_calh'],
        ['ATLAS-GEONF-*'         , '@GLOBAL'],
        ['ATLAS-GEONSF-*'        , '@GLOBAL'],
        ['ATLAS-Comm-*'          , newest_version],
        ['ATLAS-CommNF-*'        , newest_version],
        ['ATLAS-Commissioning-*' , newest_version],
        ]


    ##########################################################################
    # Code to handle cell weighting.
    #
    def make_corrections (self, flags, **kw_in):
        kw = flags.Calo.ClusterCorrection.caloSwCorrectionArgs
        kw.update (kw_in)

        for kk in ['weighting', 'remdup', 'rembad']:
            if kk in kw:
                setattr (self, kk, kw[kk])
                del kw[kk]
        
        return CaloClusterCorrSetup.make_corrections (self, flags, **kw)


    def lookup_version (self, flags, version, corrclass):
        # Special cases:
        #  If the version name includes `_wt', turn on weighting.
        (self.weighting, version) = _version_match (version, 'wt')

        #  If the version name includes `_remdup',
        #  turn on duplicate removal.
        (self.remdup, version) = _version_match (version, 'remdup')

        #  If the version name includes `_rembad',
        #  turn on bad cluster removal.
        (self.rembad, version) = _version_match (version, 'rembad')

        # Now, find the standard list of corrections to use.
        (vcorrlist, version) = CaloClusterCorrSetup.lookup_version (self,
                                                                    flags,
                                                                    version,
                                                                    corrclass)

        # Check global flags.
        if flags.Calo.ClusterCorrection.doSlidingWindowCellWeights:
            self.weighting = 1
        if flags.Calo.ClusterCorrection.doSlidingWindowRemoveDuplicates:
            self.remdup = 1
        if flags.Calo.ClusterCorrection.doSlidingWindowRemoveBad:
            self.rembad = 1

        # If weighting or rembad is turned on, make the appropriate additions
        # to the list.
        if self.weighting or self.rembad:
            ilayers = None
            for (i, c) in enumerate (vcorrlist):
                if c[0] == layers:
                    ilayers = i
                    break
            if ilayers is not None:
                vcorrlist = vcorrlist[:]
                if self.rembad:
                    vcorrlist.insert (ilayers + 1, [removebad, 121])
                if self.weighting:
                    vcorrlist.insert (ilayers + 1, [layers, 'wt', 102])
                    vcorrlist.insert (ilayers + 1, [weight, 101])

        # If remdup is turned on, make the appropriate additions
        # to the list.
        if self.remdup:
            vcorrlist = vcorrlist[:]
            vcorrlist.append ([removeduplicates, 901])

        return (vcorrlist, version)


    def __init__ (self):
        self.weighting = False
        self.remdup = False
        self.rembad = False
        return

    
CaloSwCorrections = CaloSwCorrectionsSetup()


##############################################################################
# Main entry point to create a list of correction tools
#

#
# Create and return a CA of correction tools.
# FLAGS are the configuration flags.
# KEY is a string that specifies the correction type.
# SUFFIX is a string to add to the end of each tool name.
# VERSION specifies which version of corrections to use.
# CORRLIST can be used to explicitly specify which corrections to run.
# CELLS_NAME is the SG key to use to find the calorimeter cells,
# for those corrections that require it.
# SOURCE specifies the source(s) from which tools are configured.
# See above for details.
# None means to use the default.
# If WEIGHTING is true, then cells are weighted to avoid
# double-counting energy for cells that are shared between clusters.
# If REMDUP is true, then in groups of clusters that are very close
# together, we drop all but one.
#
# For more detailed information, see the comments at the start of this file.
#
def make_CaloSwCorrectionsCfg (flags,
                               key = None,
                               suffix = '',
                               version = None,
                               corrlist = None,
                               cells_name = None,
                               source = None,
                               weighting = False,
                               remdup = False,
                               rembad = False,
                               **kw):
    return CaloSwCorrections.make_corrections (flags,
                                               corrclass = CALOCORR_SW,
                                               key = key,
                                               suffix = suffix,
                                               version = version,
                                               corrlist = corrlist,
                                               cells_name = cells_name,
                                               source = source,
                                               weighting = weighting,
                                               remdup = remdup,
                                               rembad = rembad,
                                               **kw)



##############################################################################
# Backwards compatibility:
# Main entry point to create a list of correction tools (old configuration)
#

#
# Create and return a list of correction tools.
# KEY is a string that specifies the correction type.
# SUFFIX is a string to add to the end of each tool name.
# VERSION specifies which version of corrections to use.
# CORRLIST can be used to explicitly specify which corrections to run.
# CELLS_NAME is the SG key to use to find the calorimeter cells,
# for those corrections that require it.
# SOURCE specifies the source(s) from which tools are configured.
# See above for details.
# None means to use the default.
# If WEIGHTING is true, then cells are weighted to avoid
# double-counting energy for cells that are shared between clusters.
# If REMDUP is true, then in groups of clusters that are very close
# together, we drop all but one.
#
# For more detailed information, see the comments at the start of this file.
#
def make_CaloSwCorrections (key = None,
                            suffix = '',
                            version = None,
                            corrlist = None,
                            cells_name = None,
                            source = None,
                            weighting = False,
                            remdup = False,
                            rembad = False,
                            **kw):
    try:
        wasRun3 = Configurable.configurableRun3Behavior
        Configurable.configurableRun3Behavior = True
        ca = CaloSwCorrections.make_corrections (makeFlags(),
                                                 corrclass = CALOCORR_SW,
                                                 key = key,
                                                 suffix = suffix,
                                                 version = version,
                                                 corrlist = corrlist,
                                                 cells_name = cells_name,
                                                 source = source,
                                                 weighting = weighting,
                                                 remdup = remdup,
                                                 rembad = rembad,
                                                 **kw)
    finally:
        Configurable.configurableRun3Behavior = wasRun3
    return unpackCA (ca)



#
# Creating an instance of this class gives you something which
# is roughly compatible with the old correction setup interface; i.e.,
# you can use it like
#
#   obj.set (alg, '37', corrlist = obj.corrlist37)
#
class CaloSwCorrections_compat:
    def __init__ (self, version = None):
        self.version = version
        return
    def set (self, alg, suffix='', corrlist ='ele55', key = None):
        if isinstance (corrlist, str):
            key = corrlist
            corrlist = None

        corr = make_CaloSwCorrections (key, suffix, self.version, corrlist)
        for c in corr:
            alg += c
            alg.ClusterCorrectionTools += [c.getFullName()]
            # Need to make sure that setup gets run on the tool;
            # otherwise, the parameters won't get set.
            c.setup()
        return
    def corrlist (self):
        return 'ele55'
    corrlist55 = 'ele55'
    corrlist35 = 'ele35'
    corrlist37 = 'ele37'
    corrlistele55 = 'ele55'
    corrlistele35 = 'ele35'
    corrlistele35 = 'ele35'
    corrlistgam   = 'gam55'
    corrlistgam55 = 'gam55'
    corrlistgam35 = 'gam35'
    corrlistgam37 = 'gam37'




