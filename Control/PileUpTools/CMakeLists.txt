################################################################################
# Package: PileUpTools
################################################################################

# Declare the package name:
atlas_subdir( PileUpTools )

# External dependencies:
find_package( GTest )

# Component(s) in the package:
atlas_add_library( PileUpToolsLib
                   src/*.cxx
                   PUBLIC_HEADERS PileUpTools
                   LINK_LIBRARIES AthenaBaseComps AthenaKernel
                   AthLinks SGTools
                   EventInfo xAODEventInfo xAODCnvInterfaces
                   GaudiKernel StoreGateLib CxxUtils )

atlas_add_component( PileUpTools
                     src/components/*.cxx
                     LINK_LIBRARIES PileUpToolsLib )


atlas_add_test( PileUpHashHelper_test
                SOURCES test/PileUpHashHelper_test.cxx src/PileUpHashHelper.cxx
                INCLUDE_DIRS src ${GTEST_INCLUDE_DIRS}
                LINK_LIBRARIES xAODEventInfo ${GTEST_LIBRARIES}
                POST_EXEC_SCRIPT nopost.sh )
