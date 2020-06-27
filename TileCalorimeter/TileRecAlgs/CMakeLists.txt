
################################################################################
# Package: TileRecAlgs
################################################################################

# Declare the package name:
atlas_subdir( TileRecAlgs )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Calorimeter/CaloIdentifier
                          Control/AthenaBaseComps
                          Control/StoreGate
                          GaudiKernel
                          TileCalorimeter/TileIdentifier
                          PRIVATE
                          AtlasTest/TestTools
                          Calorimeter/CaloDetDescr
                          Calorimeter/CaloEvent
                          Control/AthenaKernel
                          DetectorDescription/Identifier
                          DetectorDescription/IdDictParser
                          Event/EventContainers
                          Event/xAOD/xAODEventInfo
                          TileCalorimeter/TileCalib/TileCalibBlobObjs
                          TileCalorimeter/TileConditions
                          TileCalorimeter/TileDetDescr
                          TileCalorimeter/TileEvent
                          TileCalorimeter/TileRecUtils
                          TileCalorimeter/TileSimEvent )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( CLHEP )

# Component(s) in the package:
atlas_add_component( TileRecAlgs
                     src/Tile*.cxx
                     src/MBTSTimeDiffEventInfoAlg.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} ${CLHEP_LIBRARIES} CaloIdentifier AthenaBaseComps StoreGateLib SGtests GaudiKernel TileIdentifier CaloDetDescrLib CaloEvent AthenaKernel Identifier xAODEventInfo TileCalibBlobObjs TileConditionsLib TileDetDescr TileEvent TileRecUtilsLib TileSimEvent EventContainers )


atlas_add_test( TileDigitsThresholdFilter_test
                SOURCES test/TileDigitsThresholdFilter_test.cxx
                LINK_LIBRARIES TileEvent TileIdentifier TileConditionsLib AthenaBaseComps StoreGateLib IdDictParser TestTools GaudiKernel)
# Needed for the plugin service to see the test components
# defined in the test binary.
set_target_properties( TileRecAlgs_TileDigitsThresholdFilter_test PROPERTIES ENABLE_EXPORTS True )


# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py )

# Tests:
atlas_add_test( MBTSTimeDiffEventInfoAlgConfig_test
                SCRIPT python -m TileRecAlgs.MBTSTimeDiffEventInfoAlgConfig
                PROPERTIES TIMEOUT 300
                POST_EXEC_SCRIPT nopost.sh)

atlas_add_test( TileDigitsFilterConfig_test
                SCRIPT python -m TileRecAlgs.TileDigitsFilterConfig
                PROPERTIES TIMEOUT 300
                POST_EXEC_SCRIPT nopost.sh)
