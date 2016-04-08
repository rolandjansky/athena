################################################################################
# Package: PixelCabling
################################################################################

# Declare the package name:
atlas_subdir( PixelCabling )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaKernel
                          GaudiKernel
                          PRIVATE
                          Control/AthenaBaseComps
                          Control/CLIDSvc
                          Control/SGTools
                          Control/StoreGate
                          Database/AthenaPOOL/AthenaPoolUtilities
                          DetectorDescription/Identifier
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          InnerDetector/InDetDetDescr/PixelGeoModel
                          Tools/PathResolver )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( COOL COMPONENTS CoolKernel )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )
find_package( tdaq-common )

# Component(s) in the package:
atlas_add_component( PixelCabling
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${COOL_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${TDAQ-COMMON_INCLUDE_DIRS}
                     LINK_LIBRARIES ${COOL_LIBRARIES} ${Boost_LIBRARIES} ${ROOT_LIBRARIES} ${TDAQ-COMMON_LIBRARIES} AthenaKernel GaudiKernel AthenaBaseComps SGTools StoreGateLib SGtests AthenaPoolUtilities Identifier InDetIdentifier PathResolver )

# Install files from the package:
atlas_install_headers( PixelCabling )
atlas_install_joboptions( share/*.py )
atlas_install_runtime( share/*.dat )

