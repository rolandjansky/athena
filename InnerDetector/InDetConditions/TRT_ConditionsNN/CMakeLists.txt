# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TRT_ConditionsNN )

# External dependencies:
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )
find_package( COOL COMPONENTS CoolKernel CoolApplication )
find_package( lwtnn )

# Component(s) in the package:
atlas_add_library( TRT_ConditionsNNLib
                   src/TRTPIDNN.cxx
                   PUBLIC_HEADERS TRT_ConditionsNN
                   INCLUDE_DIRS ${LWTNN_INCLUDE_DIRS}
                   LINK_LIBRARIES ${LWTNN_LIBRARIES} AthenaKernel AthenaPoolUtilities Identifier GaudiKernel )


atlas_add_component( TRT_ConditionsNN
                     src/*Alg.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${CORAL_INCLUDE_DIRS} ${COOL_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CORAL_LIBRARIES} ${COOL_LIBRARIES} AthenaBaseComps AthenaKernel AthenaPoolUtilities GaudiKernel StoreGateLib TRT_ConditionsNNLib )
