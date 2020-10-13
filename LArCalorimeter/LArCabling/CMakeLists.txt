# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( LArCabling )

# External dependencies:
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )

# Component(s) in the package:
atlas_add_library( LArCablingLib
                   src/*.cxx
                   PUBLIC_HEADERS LArCabling
                   PRIVATE_INCLUDE_DIRS ${CORAL_INCLUDE_DIRS}
                   LINK_LIBRARIES CaloIdentifier AthenaBaseComps AthenaKernel AthenaPoolUtilities CxxUtils Identifier LArIdentifier StoreGateLib
                   PRIVATE_LINK_LIBRARIES ${CORAL_LIBRARIES} PathResolver )

atlas_add_component( LArCabling
                     src/components/*.cxx
                     LINK_LIBRARIES LArCablingLib )

atlas_add_dictionary( LArCablingDict
                      LArCabling/LArCablingDict.h
                      LArCabling/selection.xml
                      LINK_LIBRARIES LArCablingLib )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} --extend-ignore=F401,F821 )
atlas_install_runtime( share/FEBtoRODfred_v10.data )

if( NOT SIMULATIONBASE AND NOT GENERATIONBASE )
  atlas_add_test( LArCablingConfig    SCRIPT python -m LArCabling.LArCablingConfig POST_EXEC_SCRIPT nopost.sh )
endif()
