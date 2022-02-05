# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name: 
atlas_subdir( EnhancedBiasWeighter )

# External dependencies:
find_package( COOL COMPONENTS CoolKernel CoolApplication )
find_package( CORAL COMPONENTS CoralKernel )
find_package( ROOT COMPONENTS XMLIO XMLParser Core )

# Component(s) in the package:
atlas_add_library( EnhancedBiasWeighterLib
                   Root/*.cxx
                   PUBLIC_HEADERS EnhancedBiasWeighter
                   INCLUDE_DIRS ${COOL_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${CORAL_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${COOL_LIBRARIES} AsgTools DerivationFrameworkInterfaces GaudiKernel TrigAnalysisInterfaces xAODEventInfo LumiBlockData
                   PRIVATE_LINK_LIBRARIES ${CORAL_LIBRARIES} ${ROOT_LIBRARIES} PathResolver )

atlas_add_component( EnhancedBiasWeighter
                     src/components/*.cxx
                     LINK_LIBRARIES EnhancedBiasWeighterLib )

atlas_add_dictionary( EnhancedBiasWeighterDict
                      EnhancedBiasWeighter/EnhancedBiasWeighterDict.h
                      EnhancedBiasWeighter/selection.xml
                      LINK_LIBRARIES EnhancedBiasWeighterLib )
