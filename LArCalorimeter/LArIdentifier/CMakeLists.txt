# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( LArIdentifier )

# External dependencies:
find_package( Boost )

# Component(s) in the package:
atlas_add_library( LArIdentifier
                   src/*.cxx
                   PUBLIC_HEADERS LArIdentifier
                   INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                   LINK_LIBRARIES ${Boost_LIBRARIES} AthenaKernel AtlasDetDescr IdDict Identifier StoreGateLib
                   PRIVATE_LINK_LIBRARIES CaloIdentifier GaudiKernel )

atlas_add_dictionary( LArIdentifierDict
                      LArIdentifier/LArIdentifierDict.h
                      LArIdentifier/selection.xml
                      LINK_LIBRARIES LArIdentifier )

atlas_add_executable( test_laronlineid
                      test/test_laronlineid.cxx
                      LINK_LIBRARIES GaudiKernel IdDictParser LArIdentifier )

atlas_add_executable( test_larhvid
                      test/test_larhvid.cxx
                      LINK_LIBRARIES GaudiKernel IdDictParser LArIdentifier )
