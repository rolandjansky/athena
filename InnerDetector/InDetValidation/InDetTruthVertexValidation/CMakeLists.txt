# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( InDetTruthVertexValidation )

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
atlas_add_root_dictionary( InDetTruthVertexValidationLib _cintDictSource
   ROOT_HEADERS InDetTruthVertexValidation/InDetVertexTruthMatchUtils.h
   InDetTruthVertexValidation/InDetVertexTruthMatchTool.h Root/LinkDef.h
   EXTERNAL_PACKAGES ROOT )

# Component(s) in the package:
atlas_add_library( InDetTruthVertexValidationLib
                   Root/*.cxx ${_cintDictSource}
                   PUBLIC_HEADERS InDetTruthVertexValidation
                   PRIVATE_LINK_LIBRARIES ${extra_libs}
                   LINK_LIBRARIES AsgTools xAODTracking xAODTruth )

if( NOT XAOD_STANDALONE )
    atlas_add_component( InDetTruthVertexValidation
                         src/*.cxx src/*.h
                         src/components/*.cxx
                         LINK_LIBRARIES ${extra_libs} GaudiKernel xAODTracking AthenaBaseComps AsgTools InDetTruthVertexValidationLib )
endif()

# Install files from the package:
atlas_install_joboptions( share/*.py )

if( XAOD_STANDALONE )
    atlas_add_executable( VertexTruthMatchTest
                      util/VertexTruthMatchTest.cxx
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES InDetTruthVertexValidationLib xAODTracking xAODEventInfo xAODRootAccess ${ROOT_LIBRARIES} ${extra_libs} )
endif()

