################################################################################
# Package: InDetTruthVertexValidation
################################################################################

# Declare the package name:
atlas_subdir( InDetTruthVertexValidation )

# Extra dependencies, based on the build environment:
set( extra_deps )
set( extra_libs )
if( XAOD_STANDALONE )
   set( extra_deps Control/xAODRootAccess
                   PhysicsAnalysis/D3PDTools/EventLoop )
   set( extra_libs xAODRootAccess EventLoop )
else()
   set( extra_deps PRIVATE Control/AthenaBaseComps PhysicsAnalysis/POOLRootAccess GaudiKernel )
   set( extra_libs AthAnalysisBaseCompsLib )
endif()


# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthToolSupport/AsgTools
                          Event/xAOD/xAODTracking
                          Event/xAOD/xAODTruth
                          Event/EventPrimitives
                          Event/xAOD/xAODEventInfo
                          ${extra_deps} )

# External dependencies:
find_package( Eigen )
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
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS} 
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} ${EIGEN_LIBRARIES} EventPrimitives xAODTracking AsgTools xAODEventInfo xAODTruth ${extra_libs})

if( NOT XAOD_STANDALONE )
    atlas_add_component( InDetTruthVertexValidation
                     src/*.cxx src/*.h
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${EIGEN_LIBRARIES} ${extra_libs} EventPrimitives xAODTracking AthenaBaseComps AsgTools xAODEventInfo xAODTruth InDetTruthVertexValidationLib )
endif()

# Install files from the package:
atlas_install_joboptions( share/*.py )

if( XAOD_STANDALONE )
    atlas_add_executable( VertexTruthMatchTest
                      util/VertexTruthMatchTest.cxx
                      LINK_LIBRARIES InDetTruthVertexValidationLib xAODTracking xAODEventInfo xAODRootAccess ${ROOT_LIBRARIES} ${EIGEN_LIBRARIES} ${extra_libs})
endif()

