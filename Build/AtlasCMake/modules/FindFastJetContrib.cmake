# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: FindFastJetContrib.cmake 734662 2016-04-07 17:42:50Z krasznaa $
#
# - Locate FastJetContrib package
# Defines:
#
#  FASTJETCONTRIB_FOUND
#  FASTJETCONTRIB_INCLUDE_DIR
#  FASTJETCONTRIB_INCLUDE_DIRS
#  FASTJETCONTRIB_<component>_FOUND
#  FASTJETCONTRIB_<component>_LIBRARY
#  FASTJETCONTRIB_LIBRARIES
#  FASTJETCONTRIB_LIBRARY_DIRS
#
# Can be steered by FASTJETCONTRIB_ROOT.
#

# Include the helper code:
include( AtlasInternals )

# Declare the module:
atlas_external_module( NAME FastJetContrib
   INCLUDE_SUFFIXES include
   INCLUDE_NAMES fastjet/contrib/AxesFinder.hh
                 fastjet/contrib/AxesDefinition.hh
                 fastjet/contrib/SoftKiller.hh
   LIBRARY_SUFFIXES lib
   DEFAULT_COMPONENTS ConstituentSubtractor JetFFMoments ScJet EnergyCorrelator
   JetsWithoutJets SubjetCounting GenericSubtractor Nsubjettiness VariableR
   JetCleanser RecursiveTools )

# Handle the standard find_package arguments:
include( FindPackageHandleStandardArgs )
find_package_handle_standard_args( FastJetContrib DEFAULT_MSG
   FASTJETCONTRIB_INCLUDE_DIR FASTJETCONTRIB_LIBRARIES )
mark_as_advanced( FASTJETCONTRIB_FOUND FASTJETCONTRIB_INCLUDE_DIR
   FASTJETCONTRIB_INCLUDE_DIRS FASTJETCONTRIB_LIBRARIES
   FASTJETCONTRIB_LIBRARY_DIRS )
