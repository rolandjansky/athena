################################################################################
# Package: JetEventAthenaPool
################################################################################

# Declare the package name:
atlas_subdir( JetEventAthenaPool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Database/AtlasSealCLHEP
                          GaudiKernel
                          PRIVATE
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          PhysicsAnalysis/JetTagging/JetTagInfoAthenaPool
                          Reconstruction/Jet/JetEvent
                          Reconstruction/Jet/JetEventTPCnv )

# Component(s) in the package:
atlas_add_poolcnv_library( JetEventAthenaPoolPoolCnv
                           src/*.cxx
                           FILES JetEvent/JetMomentMap.h JetEvent/JetSamplingCollection.h JetEvent/JetCollection.h JetEvent/JetKeyDescriptor.h
                           TYPES_WITH_NAMESPACE 
                           LINK_LIBRARIES AthenaPoolUtilities GaudiKernel AthenaPoolCnvSvcLib JetEvent JetEventTPCnv )

# Install files from the package:
atlas_install_headers( JetEventAthenaPool )
atlas_install_joboptions( share/*.py )

# Set up (a) test(s) for the converter(s):
if( IS_DIRECTORY ${CMAKE_SOURCE_DIR}/Database/AthenaPOOL/AthenaPoolUtilities )
   set( AthenaPoolUtilitiesTest_DIR
      ${CMAKE_SOURCE_DIR}/Database/AthenaPOOL/AthenaPoolUtilities/cmake )
endif()
find_package( AthenaPoolUtilitiesTest )

if( ATHENAPOOLUTILITIESTEST_FOUND )
  set( JETEVENTATHENAPOOL_REFERENCE_TAG
       JetEventAthenaPoolReference-01-00-00 )
  run_tpcnv_test( JetTPCnv_15.9.0.5   AOD-15.9.0.5-full
                   REFERENCE_TAG ${JETEVENTATHENAPOOL_REFERENCE_TAG} )
  run_tpcnv_test( JetTPCnv_17.0.0     AOD-17.0.0-full
                   REFERENCE_TAG ${JETEVENTATHENAPOOL_REFERENCE_TAG} )
  run_tpcnv_test( JetTPCnv_17.2.8     AOD-17.2.8-full
                   REFERENCE_TAG ${JETEVENTATHENAPOOL_REFERENCE_TAG} )
else()
   message( WARNING "Couldn't find AthenaPoolUtilitiesTest. No test(s) set up." )
endif()   
