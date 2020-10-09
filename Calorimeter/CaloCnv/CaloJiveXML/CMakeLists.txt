# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( CaloJiveXML )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_component( CaloJiveXML
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} AthenaBaseComps AthenaKernel CaloDetDescrLib CaloEvent CaloIdentifier EventContainers GaudiKernel Identifier JiveXMLLib LArCablingLib LArElecCalib LArIdentifier LArRawEvent TileCalibBlobObjs TileConditionsLib TileEvent TileIdentifier )

# Install files from the package:
atlas_install_joboptions( share/*.py )
