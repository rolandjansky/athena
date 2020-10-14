# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#
# File: CaloClusterCorrection/python/constants.py
# Created: Nov 2009, sss
# Purpose: Some constants used for the cluster correction configuration.
#
# Broken out from common.py so that they can be used in contexts where
# it's too early to import common.py.
#

# Correction class strings.
# In cool, we make folders with paths like
#   /CALO/<classname>/<correction name>
# We want to group together, i.e, all SW corrections and topo corrections.
CALOCORR_SW     = 'CaloSwClusterCorrections'
CALOCORR_EMTOPO = 'EMTopoClusterCorrections'

# Lists of implemented cluster type keys.  Need to be here rather than
# in the top-level steering modules to avoid circular imports.
sw_valid_keys = ['ele55', 'ele35', 'ele37',
                 'gam55', 'gam35', 'gam37']
topoem_valid_keys = ['ele633', 'gam633', 'ele420']

# Region codes from CaloClusterCorrectionCommon.h.
CALOCORR_EMB1  = 0
CALOCORR_EMB2  = 1
CALOCORR_EME1 = 2
CALOCORR_EME2 = 3
CALOCORR_COMBINED2 = 4
CALOCORR_CLUSTER = 5

# Origin codes.  For reading from job options, pool, or cool.
CALOCORR_JO = 'jo'
CALOCORR_POOL = 'pool'
CALOCORR_COOL = 'cool'

# Used to mark a tool that shouldn't be saved to pool.
CALOCORR_NOPOOL = 'nopool'

# Special case for constructing a tool to write constants to pool.
# Acts like CALOCORR_JO, except that if the tool is marked
# with CALOCORR_NOPOOL,
# we don't make the tool.  Also, we set the name appropriately for writing
# to pool.
CALOCORR_TOPOOL = 'topool'

# Some default settings.
CALOCORR_DEFAULT_KEY = 'ele55'

# Symbolic names for calorimeter samplings,
# and string representations for them.
EMB1 = 1
EMB2 = 2
EME1 = 5
EME2 = 6

sampnames = { EMB1 : 'b1',
              EMB2 : 'b2',
              EME1 : 'e1',
              EME2 : 'e2',
              None : '',
              }
