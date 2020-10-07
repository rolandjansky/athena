# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( LArL1Sim )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_component( LArL1Sim
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} AthAllocators AthenaBaseComps GaudiKernel GeoModelInterfaces LArDigitizationLib CaloDetDescrLib CaloEvent CaloIdentifier CaloTriggerToolLib AthenaKernel StoreGateLib EventInfo LArCablingLib LArElecCalib LArIdentifier LArRawEvent LArSimEvent PathResolver )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py )
atlas_install_runtime( share/Fcal_ptweights_table7.data )

