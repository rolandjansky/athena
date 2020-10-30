# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#
# File: CaloClusterCorrection/python/CaloSwCorrections.py
# Created: Mar 2007, sss
# Purpose: Top-level configuration file for EM topo cluster corrections.
#
# The main entry point here is make_CaloTopoEMCorrectionsCfg, which returns
# a list of correction tools.  It can be used either `le menu' --- selecting
# a predefined list of correction tools, or `a la carte' --- explicitly
# specifying the list of tools you want to run.
#
# The simplest way of using it is like this:
#
#   make_CaloTopoEMCorrectionsCfg (flags, key)
#
# Here, `key' is a string that denotes the type of cluster which you
# are correcting.  It should be something like `ele420' for 4-2-0 electrons,
# `gam630' for 6-3-0 photons, etc.  
#
# Tools should have unique names.  If you are using the same corrections
# multiple times (in offline and in trigger, for example), you should
# specify the `suffix' argument to make them unique.  This is a string
# that will be added to the end of the tool names.
#
# By default, the latest version of the corrections appropriate to the
# geometry being used will be chosen.  You can override this selection
# by supplying the `version' argument.  It may also be overridden
# with Calo.ClusterCorrection.CaloTopoEMWhichCorrection.
# For the list of known version
# names, see the variable `CaloTopoEMCorrection_versions' below.
# In particular, using version `none' turns off the corrections.
#
# Each individual correction has a version as well, independent
# of the version of the overall correction list.  You can override
# the version to be used for individual correction by adding
# parameters of the form CORRECTION_version.  For example:
#
#   make_CaloTopoEMCorrectionsCfg (flags, key, version='v4', lwc_version='v3')
#
# uses the `v4' corrections, except that we use version `v3' of the
# layer weight corrections.  You can also specify CORRECTION_key
# to override the key setting for an individual correction,
# and CORRECTION_XXX to override any individual parameter
# of a correction.  For example:
#
#   make_CaloTopoEMCorrectionsCfg (flags, key, lwc_degree=2)
#
# makes the full standard set of corrections, except that the interpolation
# degree for the layer weights correction is changed to 2.
#
# You can explicitly specify a list of corrections to run with the
# `corrlist' argument.  For example,
#
#   make_CaloTopoEMCorrectionsCfg (flags, key, corrlist=[[layers], [update], [gap]])
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
#   make_CaloTopoEMCorrectionsCfg (flags,
#                                  key, corrlist=[[layers],
#                                                 [update],
#                                                 [gap, 'v3', ('degree', 2)]])
#
# says to use version `v3' of the gap correction, and in addition,
# to override the interpolation degree to be 2.
#
# There are three ways in which a correction can get configured: from
# job options, from pool, or from cool.  You can specify which ones
# to use by adding the `source' argument to make_CaloTopoEMCorrectionsCfg.
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

    from CaloClusterCorrection.CaloTopoEMmoments  import make_CaloTopoEMmoments
    from CaloClusterCorrection.CaloTopoEMlayers   import make_CaloTopoEMlayers
    from CaloClusterCorrection.CaloTopoEMetaoff   import make_CaloTopoEMetaoff
    from CaloClusterCorrection.CaloTopoEMetaoffSW import make_CaloTopoEMetaoffSW
    from CaloClusterCorrection.CaloTopoEMphioff   import make_CaloTopoEMphioff
    from CaloClusterCorrection.CaloTopoEMphimod   import make_CaloTopoEMphimod
    from CaloClusterCorrection.CaloTopoEMClusterUpdate \
         import make_CaloTopoEMClusterUpdate
    from CaloClusterCorrection.CaloTopoEMgap      import make_CaloTopoEMgap
    from CaloClusterCorrection.CaloClusterListBadChannel   import make_CaloClusterListBadChannel
    from CaloClusterCorrection.CaloTopoEMlongWeights \
         import make_CaloTopoEMlongWeights
    from CaloClusterCorrection.constants         import CALOCORR_EMTOPO, EMB1, EME1
    from CaloClusterCorrection.common            import CaloClusterCorrSetup
    from CaloClusterCorrection.compat            import makeFlags, unpackCA

finally:
    Configurable.configurableRun3Behavior = _wasRun3
    
     


##############################################################################
# Here we define wrapper functions to set up all of the standard corrections.
# In the case where a correction has multiple versions for different
# samplings, we define multiple wrappers here.
# These are the names to use in the correction list and in the
# arguments to make_CaloTopoEMCorrectionCfg.
#

def moments (flags, cells_name, *args, **kw):
    return make_CaloTopoEMmoments (flags, None, *args, **kw)

def layers (flags, cells_name, *args, **kw):
    return make_CaloTopoEMlayers (flags, None, cells_name=cells_name, *args, **kw)

def etaoff_sw_b1 (flags, cells_name, *args, **kw):
    return make_CaloTopoEMetaoffSW (flags, EMB1, None, *args, **kw)

def etaoff_sw_e1 (flags, cells_name, *args, **kw):
    return make_CaloTopoEMetaoffSW (flags, EME1, None, *args, **kw)

# Eta offset in sampling 2.
def etaoff_2 (flags, cells_name, *args, **kw):
    return make_CaloTopoEMetaoff (flags, None, *args, **kw)

# Phi offset in sampling 2.
def phioff (flags, cells_name, *args, **kw):
    return make_CaloTopoEMphioff (flags, None, *args, **kw)

def phimod (flags, cells_name, *args, **kw):
    return make_CaloTopoEMphimod (flags, None, *args, **kw)

def update (flags, cells_name, *args, **kw):
    return make_CaloTopoEMClusterUpdate (flags, None, *args, **kw)

def lwc (flags, cells_name, *args, **kw):
    return make_CaloTopoEMlongWeights (flags, None, *args, **kw)

def gap (flags, cells_name, *args, **kw):
    return make_CaloTopoEMgap (flags, None, *args, **kw)

def listBadChannel (flags, cells_name, *args, **kw):
    return make_CaloClusterListBadChannel (flags, CALOCORR_EMTOPO, None, *args, **kw)


##############################################################################
# The class responsible for making the corrections.
#


class CaloTopoEMCorrectionsSetup (CaloClusterCorrSetup):

    name = "EM topo"
    version_override_flag_name = 'caloTopoEMWhichCorrection'
    correction_generation_flag_name = 'caloTopoEMGeneration'
    correction_generation_default = "00-02-00"

    ##########################################################################
    # This is the list of all known correction versions.
    versions = {

        # A special case: disable all corrections except those
        # technically required.
        'none' : [[moments,     100],
                  [listBadChannel,                 820]], 

        # From the original job options file.
        # I think this was for 6-3-3 clusters.
        'v1' : [[moments,                        100],
                [layers,                         101],
                [phioff,       'v1',             201],
                [etaoff_2,     'v1',             202],
                # In sampling 1, use v2 of the SW corrns
                [etaoff_sw_b1, 'v2',             203], 
                [etaoff_sw_e1, 'v2',             204],
                [phimod,       'v1',             250],
                [update,       'dont_update_e',  300],
                ],

        # From the original DC3 job options file.
        # I think this was for 6-3-3 clusters.
        'DC3' : [[moments,                        100],
                 [layers,                         101],
                 [phioff,       'v1',             201],
                 [etaoff_2,     'v1',             202],
                 # In sampling 1, use v2 of the SW corrns
                 [etaoff_sw_b1, 'v2',             203],
                 [etaoff_sw_e1, 'v2',             204],
                 [phimod,       'v1',             250],
                 [update,       'dont_update_e',  300],
                 [lwc,          'v1',             401],
                 [gap,          'v1',             450],
                 ],

        # Same as DC3, except that the phi offset in the endcap is removed.
        'DC3-v2' : [[moments,                       100],
                    [layers,                        101],
                    [phioff,       'v2',            201],
                    [etaoff_2,     'v1',            202],
                    # In samp 1, use v2 of the SW corrns
                    [etaoff_sw_b1, 'v2',            203], 
                    [etaoff_sw_e1, 'v2',            204],
                    [phimod,       'v1',            250],
                    [update,       'dont_update_e', 300],
                    [lwc,          'v1',            401],
                    [gap,          'v1',            450],
                    [listBadChannel,                820],
                    ],

        # Define the list of correction tools to run when
        # hierarchical tagging is used.
        # All tools that may possibly be used for these corrections
        # should be listed here.  Tools that are not configured
        # from the database should have the proper order and
        # version (if any); tools that are configured from the
        # database should not have order set and should have the
        # version set to `@'.
        # The ordering of tools in this list has no significance.
        '@'      : [[moments,                        100],
                    [layers,                         101],
                    [phioff,       '@'],
                    [etaoff_2,     '@'],
                    [etaoff_sw_b1, '@'],
                    [etaoff_sw_e1, '@'],
                    [phimod,       '@'],
                    [update,       '@'],
                    [lwc,          '@'],
                    [gap,          '@'],
                    [listBadChannel,                 820]
                    ],
}

    # Alias for the newest correction.
    newest_version = 'DC3-v2'


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

        ['*'                     , 'DC3-v2' ],
        ]


    # Handle CaloTopoEMCorrectionArgs.
    def make_corrections (self, flags, **kw_in):
        kw = {}
        kw = flags.Calo.ClusterCorrection.caloTopoEMCorrectionArgs
        kw.update (kw_in)

        return CaloClusterCorrSetup.make_corrections (self, flags, **kw)


CaloTopoEMCorrections = CaloTopoEMCorrectionsSetup()


##############################################################################
# Main entry point to create a list of correction tools.
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
#
# For more detailed information, see the comments at the start of this file.
#
def make_CaloTopoEMCorrectionsCfg (flags,
                                   key = None,
                                   suffix = '',
                                   version = None,
                                   corrlist = None,
                                   cells_name = None,
                                   source = None,
                                   **kw):
    return CaloTopoEMCorrections.make_corrections (flags,
                                                   corrclass = CALOCORR_EMTOPO,
                                                   key = key,
                                                   suffix = suffix,
                                                   version = version,
                                                   corrlist = corrlist,
                                                   cells_name = cells_name,
                                                   source = source,
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
#
# For more detailed information, see the comments at the start of this file.
#
def make_CaloTopoEMCorrections (key = None,
                                suffix = '',
                                version = None,
                                corrlist = None,
                                cells_name = None,
                                source = None,
                                **kw):
    try:
        wasRun3 = Configurable.configurableRun3Behavior
        Configurable.configurableRun3Behavior = True
        ca = CaloTopoEMCorrections.make_corrections (makeFlags(),
                                                     corrclass = CALOCORR_EMTOPO,
                                                     key = key,
                                                     suffix = suffix,
                                                     version = version,
                                                     corrlist = corrlist,
                                                     cells_name = cells_name,
                                                     source = source,
                                                     **kw)
    finally:
        Configurable.configurableRun3Behavior = wasRun3
    return unpackCA (ca)
