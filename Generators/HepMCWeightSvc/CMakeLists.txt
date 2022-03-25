# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( HepMCWeightSvc )

# External dependencies:
find_package( Boost )
find_package( CORAL COMPONENTS CoralBase )

# Component(s) in the package:
atlas_add_component( HepMCWeightSvc
   src/*.cxx src/components/*.cxx
   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
   LINK_LIBRARIES ${CORAL_LIBRARIES} GaudiKernel AthenaBaseComps
   AthenaPoolUtilities IOVDbDataModel EventInfo GenInterfacesLib IOVDbMetaDataToolsLib xAODEventInfo )


atlas_install_joboptions( share/*.txt )


atlas_add_test( HepMCWeightSvc_test
  SOURCES test/HepMCWeightSvc_test.cxx
  LINK_LIBRARIES GaudiKernel GenInterfacesLib TestTools AthenaKernel SGTools
  IOVDbDataModel IOVDbMetaDataToolsLib AthenaPoolUtilities StoreGateLib
  xAODEventInfo )
