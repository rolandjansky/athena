# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TBMonitoring )

# External dependencies:
find_package( AIDA )
find_package( CLHEP )

# Component(s) in the package:
atlas_add_library( TBMonitoringLib
                   src/*.cxx
                   PUBLIC_HEADERS TBMonitoring
                   INCLUDE_DIRS ${AIDA_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${AIDA_LIBRARIES} CaloGeoHelpers CaloIdentifier Identifier GaudiKernel TBEvent AthenaMonitoringLib StoreGateLib TBUtilsLib
                   PRIVATE_LINK_LIBRARIES ${CLHEP_LIBRARIES} AthenaKernel CaloEvent CaloUtilsLib EventInfo LArCablingLib LArIdentifier )

atlas_add_component( TBMonitoring
                     src/components/*.cxx
                     LINK_LIBRARIES TBMonitoringLib )

# Install files from the package:
atlas_install_joboptions( share/*.txt share/*.py )

