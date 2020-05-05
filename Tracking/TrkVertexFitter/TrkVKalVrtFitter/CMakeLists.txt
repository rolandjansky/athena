# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name.
atlas_subdir( TrkVKalVrtFitter )

# External dependencies.
find_package( CLHEP )

# Component(s) in the package.
atlas_add_library( TrkVKalVrtFitterLib
   TrkVKalVrtFitter/*.h src/*.cxx
   PUBLIC_HEADERS TrkVKalVrtFitter
   PRIVATE_INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
   LINK_LIBRARIES AthenaBaseComps GeoPrimitives EventPrimitives xAODTracking
   GaudiKernel MagFieldInterfaces MagFieldElements MagFieldConditions
   TrkNeutralParameters TrkParameters
   TrkParticleBase VxSecVertex VxVertex TrkExInterfaces TrkVKalVrtCore
   TrkVertexFitterInterfaces
   PRIVATE_LINK_LIBRARIES ${CLHEP_LIBRARIES} TrkSurfaces TestTools )

atlas_add_component( TrkVKalVrtFitter
   src/components/*.cxx
   LINK_LIBRARIES TrkVKalVrtFitterLib )

# Install files from the package.
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

# Test(s) in the package.
atlas_add_test( TrkVKalVrtFitter_test
   SCRIPT athena.py TrkVKalVrtFitter/TrkVKalVrtFitter_test.py
   PROPERTIES TIMEOUT 300
   LOG_IGNORE_PATTERN " INFO |WARNING |found service|Adding private|^ +[+]|HepPDT Version|frontier.c" )
