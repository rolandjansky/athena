# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( LArTools )

# External dependencies:
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )
find_package( ROOT COMPONENTS RIO Core Tree MathCore Hist pthread )

# Component(s) in the package:
atlas_add_library( LArToolsLib
                   src/*.cxx
                   PUBLIC_HEADERS LArTools
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
                   LINK_LIBRARIES AthenaBaseComps AthenaKernel CxxUtils Identifier GaudiKernel LArElecCalib
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} ${CORAL_LIBRARIES} CaloIdentifier AthenaPoolUtilities LArCablingLib LArRecConditions )

atlas_add_component( LArTools
                     src/components/*.cxx
                     LINK_LIBRARIES LArToolsLib )

atlas_add_dictionary( LArToolsDict
                      LArTools/LArToolsDict.h
                      LArTools/selection.xml
                      LINK_LIBRARIES LArToolsLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )

