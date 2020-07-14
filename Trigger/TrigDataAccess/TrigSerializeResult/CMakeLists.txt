# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigSerializeResult )

# External dependencies:
find_package( ROOT COMPONENTS RIO Core Tree MathCore Hist pthread )
find_package( Boost )

# Component(s) in the package:
atlas_add_library( TrigSerializeResultLib
                   src/*.cxx
                   PUBLIC_HEADERS TrigSerializeResult
                   PRIVATE_INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES AthenaBaseComps AthenaKernel GaudiKernel
                   PRIVATE_LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} CxxUtils DataModelRoot PathResolver )

atlas_add_component( TrigSerializeResult
                     src/components/*.cxx
                     LINK_LIBRARIES TrigSerializeResultLib )

atlas_add_dictionary( TrigSerializeResultDict
                      TrigSerializeResult/TrigSerializeResultDict.h
                      TrigSerializeResult/selection.xml
                      LINK_LIBRARIES TrigSerializeResultLib )

# Install files from the package:
atlas_install_runtime( share/bs-streamerinfos*.root )

# Tests in the package:
atlas_add_test( remap_test
                SOURCES
                test/remap_test.cxx
                LINK_LIBRARIES TrigSerializeResultLib
                POST_EXEC_SCRIPT nopost.sh )
