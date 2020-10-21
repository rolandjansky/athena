# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( InDetIdentifier )

# External dependencies:
find_package( Boost COMPONENTS unit_test_framework )

# Component(s) in the package:
atlas_add_library( InDetIdentifier
                   src/PixelID.cxx
                   src/SCT_ID.cxx
                   src/TRT_ID.cxx
                   src/SiliconID.cxx
                   PUBLIC_HEADERS InDetIdentifier
                   LINK_LIBRARIES AthenaKernel AtlasDetDescr IdDict Identifier
                   PRIVATE_LINK_LIBRARIES GaudiKernel )

atlas_add_dictionary( InDetIdentifierDict
                      InDetIdentifier/InDetIdentifierDict.h
                      InDetIdentifier/selection.xml
                      LINK_LIBRARIES InDetIdentifier )

# Tests in the package:
atlas_add_test( SCT_ID_test                
                SOURCES
                test/SCT_ID_test.cxx
                INCLUDE_DIRS ${Boost_INCLUDE_DIRS}  
                LINK_LIBRARIES ${Boost_LIBRARIES} IdDictParser InDetIdentifier
                POST_EXEC_SCRIPT "nopost.sh"                
                )
set_target_properties( InDetIdentifier_SCT_ID_test  PROPERTIES ENABLE_EXPORTS True )
