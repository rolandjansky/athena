# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( MuonCondTest )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( MuonCondTest
                     src/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} ${CORAL_LIBRARIES} ${ROOT_LIBRARIES} AthenaBaseComps AthenaKernel CoralUtilitiesLib GaudiKernel GeoPrimitives Identifier MuonAlignmentData MuonCondData MuonCondInterface MuonCondSvcLib MuonIdHelpersLib MuonReadoutGeometry StoreGateLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )

