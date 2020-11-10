# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrkNeuralNetworkUtils )

# External dependencies:
find_package( Boost )
find_package( ROOT COMPONENTS Matrix Core Tree MathCore Hist )

# Component(s) in the package:
atlas_add_root_dictionary( TrkNeuralNetworkUtilsLib
                           TrkNeuralNetworkUtilsLibDictSource
                           ROOT_HEADERS TrkNeuralNetworkUtils/TTrainedNetwork.h TrkNeuralNetworkUtils/LinkDef.h
                           EXTERNAL_PACKAGES ROOT Boost )

atlas_add_library( TrkNeuralNetworkUtilsLib
                   src/*.cxx
                   ${TrkNeuralNetworkUtilsLibDictSource}
                   PUBLIC_HEADERS TrkNeuralNetworkUtils
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps
                   PRIVATE_LINK_LIBRARIES ${Boost_LIBRARIES} GaudiKernel )

atlas_add_component( TrkNeuralNetworkUtils
                     src/components/*.cxx
                     LINK_LIBRARIES TrkNeuralNetworkUtilsLib )

atlas_add_test( test_buffer
        SOURCES test/test_TargetBuffer_t.cxx  )
