# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigT1CaloCalibConditions )

# External dependencies:
find_package( CORAL COMPONENTS CoralBase )
find_package( nlohmann_json )

# Component(s) in the package:
atlas_add_library( TrigT1CaloCalibConditions
                   src/*.cxx
                   PUBLIC_HEADERS TrigT1CaloCalibConditions
                   INCLUDE_DIRS ${CORAL_INCLUDE_DIRS}
                   LINK_LIBRARIES ${CORAL_LIBRARIES} AthContainers AthenaBaseComps AthenaKernel AthenaPoolUtilities GaudiKernel
                   PRIVATE_LINK_LIBRARIES nlohmann_json::nlohmann_json )

atlas_add_dictionary( TrigT1CaloCalibConditionsDict
                      TrigT1CaloCalibConditions/TrigT1CaloCalibConditionsDict.h
                      TrigT1CaloCalibConditions/selection.xml
                      LINK_LIBRARIES TrigT1CaloCalibConditions )

# Install files from the package:
atlas_install_joboptions( share/*.py )

