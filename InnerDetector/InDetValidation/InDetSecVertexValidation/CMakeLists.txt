# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( InDetSecVertexValidation )

# Extra dependencies, based on the build environment:
set( extra_libs )
if( XAOD_STANDALONE )
   set( extra_libs xAODRootAccess EventLoop )
else()
   set( extra_libs AthAnalysisBaseCompsLib )
endif()

# External dependencies:
find_package( ROOT COMPONENTS Core Tree Hist RIO )

# Generate a CINT dictionary source file:
atlas_add_root_dictionary( InDetSecVertexValidationLib _cintDictSource
   ROOT_HEADERS InDetSecVertexValidation/InDetSecVertexTruthMatchTool.h Root/LinkDef.h
   EXTERNAL_PACKAGES ROOT )

# Component(s) in the package:
atlas_add_library( InDetSecVertexValidationLib
                   Root/*.cxx ${_cintDictSource}
                   PUBLIC_HEADERS InDetSecVertexValidation
                   PRIVATE_LINK_LIBRARIES ${extra_libs}
                   LINK_LIBRARIES AsgTools xAODTracking xAODTruth )

if( NOT XAOD_STANDALONE )
    atlas_add_component( InDetSecVertexValidation
                         src/*.cxx src/*.h
                         src/components/*.cxx
                         LINK_LIBRARIES ${extra_libs} GaudiKernel xAODTracking AthenaBaseComps AsgTools InDetSecVertexValidationLib )
endif()

# Install files from the package:
atlas_install_joboptions( share/*.py )

if( XAOD_STANDALONE )
    atlas_add_executable( SecVertexTruthMatchTest
                      util/SecVertexTruthMatchTest.cxx
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES InDetSecVertexValidationLib xAODTracking xAODEventInfo xAODRootAccess ${ROOT_LIBRARIES} ${extra_libs} )
endif()

