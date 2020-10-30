# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( LArEventTest )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Tree )

# Component(s) in the package:
atlas_add_component( LArEventTest
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} ${ROOT_LIBRARIES} CaloIdentifier AthenaBaseComps StoreGateLib GaudiKernel LArElecCalib LArIdentifier LArRawConditions LArRawEvent LArCablingLib TBEvent CaloEvent xAODEventInfo )

# Install files from the package:
atlas_install_joboptions( share/*.py )
