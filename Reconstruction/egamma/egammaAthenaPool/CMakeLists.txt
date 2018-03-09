################################################################################
# Package: egammaAthenaPool
################################################################################

# Declare the package name:
atlas_subdir( egammaAthenaPool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Database/AtlasSealCLHEP
                          PRIVATE
                          Control/AthenaBaseComps
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          GaudiKernel
                          Reconstruction/egamma/egammaEvent
                          Reconstruction/egamma/egammaEventTPCnv )

# Component(s) in the package:
atlas_add_poolcnv_library( egammaAthenaPoolPoolCnv
                           src/*.cxx
                           FILES egammaEvent/egammaContainer.h egammaEvent/ElectronContainer.h egammaEvent/PhotonContainer.h egammaEvent/egDetailContainer.h egammaEvent/EMShowerMinimalContainer.h egammaEvent/CaloRingsContainer.h
                           TYPES_WITH_NAMESPACE 
                           LINK_LIBRARIES AthenaPoolUtilities AthenaBaseComps AthenaPoolCnvSvcLib GaudiKernel egammaEvent egammaEventTPCnv )

# Install files from the package:
atlas_install_headers( egammaAthenaPool )
atlas_install_joboptions( share/*.py )

# Set up (a) test(s) for the converter(s):
if( IS_DIRECTORY ${CMAKE_SOURCE_DIR}/Database/AthenaPOOL/AthenaPoolUtilities )
   set( AthenaPoolUtilitiesTest_DIR
      ${CMAKE_SOURCE_DIR}/Database/AthenaPOOL/AthenaPoolUtilities/cmake )
endif()
find_package( AthenaPoolUtilitiesTest )

if( ATHENAPOOLUTILITIESTEST_FOUND )
   set( EGAMMAATHENAPOOL_REFERENCE_TAG
        egammaAthenaPoolReference-01-00-00 )
  run_tpcnv_test( egammaEventTPCnv_15.1.0    AOD-15.1.0-full
                   REFERENCE_TAG ${EGAMMAATHENAPOOL_REFERENCE_TAG} )
  run_tpcnv_test( egammaEventTPCnv_15.6.9    AOD-15.6.9-full
                   REFERENCE_TAG ${EGAMMAATHENAPOOL_REFERENCE_TAG} )
  run_tpcnv_test( egammaEventTPCnv_16.6.7.1  AOD-16.6.7.1-full
                   REFERENCE_TAG ${EGAMMAATHENAPOOL_REFERENCE_TAG} )
  run_tpcnv_test( egammaEventTPCnv_18.0.0    AOD-18.0.0-full
                   REFERENCE_TAG ${EGAMMAATHENAPOOL_REFERENCE_TAG} )
else()
   message( WARNING "Couldn't find AthenaPoolUtilitiesTest. No test(s) set up." )
endif()   
