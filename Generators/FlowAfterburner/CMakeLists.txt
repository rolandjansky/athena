# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( FlowAfterburner )

# External dependencies:
find_package( CLHEP )
find_package( GSL )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( FlowAfterburnerLib
                   src/*.cxx
                   PUBLIC_HEADERS FlowAfterburner
                   INCLUDE_DIRS ${GSL_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS 
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${GSL_LIBRARIES} ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AthenaBaseComps AthenaKernel AtlasHepMCLib GaudiKernel TruthHelper GeneratorObjects
                   PRIVATE_LINK_LIBRARIES StoreGateLib TruthUtils )

atlas_add_component( FlowAfterburner
                     src/components/*.cxx
                     LINK_LIBRARIES FlowAfterburnerLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )

