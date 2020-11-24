# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the name of this package:
atlas_subdir( TopJetSubstructure )

# This package uses ROOT:
find_package( ROOT REQUIRED COMPONENTS Core Physics )

# This package uses FastJet:
find_package( FastJet )
find_package( FastJetContrib )

# Build a library that other components can link against:
atlas_add_library( TopJetSubstructure Root/*.cxx Root/*.h Root/*.icc
                   TopJetSubstructure/*.h TopJetSubstructure/*.icc TopJetSubstructure/*/*.h
                   TopJetSubstructure/*/*.icc 
                   PUBLIC_HEADERS TopJetSubstructure
                   INCLUDE_DIRS ${FASTJET_INCLUDE_DIRS}
                                ${FASTJETCONTRIB_INCLUDE_DIRS}
                                ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${FASTJET_LIBRARIES}
                                  ${FASTJETCONTRIB_LIBRARIES}
                                  ${ROOT_LIBRARIES}
                                  xAODEgamma
                                  xAODJet )
