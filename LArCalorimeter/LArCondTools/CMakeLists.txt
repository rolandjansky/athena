################################################################################
# Package: LArCondTools
################################################################################

# Declare the package name:
atlas_subdir( LArCondTools )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( LArCondTools
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${Boost_LIBRARIES} ${CORAL_LIBRARIES} AthenaBaseComps StoreGateLib SGtests AthenaPoolUtilities Identifier GaudiKernel CaloDetDescrLib CaloIdentifier CaloGeoHelpers AthenaKernel LArCablingLib LArHV LArReadoutGeometry LArIdentifier LArRawConditions LArCOOLConditions LArRecConditions )

# Install files from the package:
atlas_install_joboptions( share/*.py )

