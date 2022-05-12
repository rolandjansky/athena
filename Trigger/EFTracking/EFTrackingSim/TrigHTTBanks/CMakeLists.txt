################################################################################
# Package: TrigHTTBanks
################################################################################

# Declare the package name:
atlas_subdir( TrigHTTBanks )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( Eigen )
find_package( HepPDT )
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )
find_package( TBB )

# Create a library for non-Gaudi components
atlas_add_library(
    TrigHTTBanksLib         src/*.cxx TrigHTTBanks/*.h
    PUBLIC_HEADERS          TrigHTTBanks
    INCLUDE_DIRS            ${HEPPDT_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS} ${TBB_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}
    LINK_LIBRARIES          ${HEPPDT_LIBRARIES} ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} ${EIGEN_LIBRARIES} ${TBB_LIBRARIES} ${Boost_LIBRARIES}
                            AthenaBaseComps GaudiKernel TrigHTTMapsLib TrigHTTObjectsLib 
)

# Declare Gaudi component(s) in the package
atlas_add_component(
    TrigHTTBanks            src/components/*.cxx
    INCLUDE_DIRS            ${HEPPDT_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS} ${TBB_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}
    LINK_LIBRARIES          TrigHTTBanksLib
)

# Install files from the package and run flake8 test:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} --extend-select=ATL900,ATL901 )  


#atlas_add_test(             HTTFitConstantBank_test
#    SOURCES                 test/HTTFitConstantBank_test.cxx
#    LINK_LIBRARIES          TrigHTTBanksLib TrigHTTObjectsLib TrigHTTMapsLib
#)

#atlas_install_generic(  test_banks/*
#    DESTINATION share/htt_configuration/test_banks
#)
