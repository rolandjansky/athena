# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( CaloSamplingFractionAnalysis )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Tree )

# Component(s) in the package:
atlas_add_component( CaloSamplingFractionAnalysis
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} ${ROOT_LIBRARIES} AthenaBaseComps AtlasHepMCLib CaloDetDescrLib CaloEvent CaloIdentifier CaloSimEvent EventInfo GaudiKernel GeneratorObjects GeoAdaptors Identifier LArElecCalib LArG4RunControl LArReadoutGeometry LArSimEvent StoreGateLib TileConditionsLib TileSimEvent )

# Install files from the package:
atlas_install_joboptions( share/*.py )
atlas_install_runtime( share/*.C share/HEC_SF_analysis share/run*.sh )

