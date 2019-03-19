################################################################################
# Package: PixelCabling
################################################################################

# Declare the package name:
atlas_subdir( PixelCabling )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Control/AthenaKernel
   GaudiKernel
   PRIVATE
   Control/AthenaBaseComps
   Control/SGTools
   Control/StoreGate
   Database/AthenaPOOL/AthenaPoolUtilities
   DetectorDescription/Identifier
   InnerDetector/InDetDetDescr/InDetReadoutGeometry
   InnerDetector/InDetDetDescr/InDetIdentifier
   Tools/PathResolver )

# External dependencies:
find_package( Boost )
find_package( CORAL COMPONENTS CoralBase )
find_package( tdaq-common )

# Component(s) in the package:
atlas_add_library( PixelCablingLib
   PixelCabling/*.h
   INTERFACE
   PUBLIC_HEADERS PixelCabling
   LINK_LIBRARIES GaudiKernel AthenaKernel )

atlas_add_component( PixelCabling
   src/*.h src/*.cxx src/components/*.cxx
   INCLUDE_DIRS ${CORAL_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}
   ${TDAQ-COMMON_INCLUDE_DIRS}
   LINK_LIBRARIES ${CORAL_LIBRARIES} ${Boost_LIBRARIES}
   ${TDAQ-COMMON_LIBRARIES} GaudiKernel AthenaBaseComps SGTools StoreGateLib
   AthenaPoolUtilities Identifier InDetIdentifier InDetReadoutGeometry PathResolver PixelCablingLib )

atlas_add_test( PixelCablingConfigNew_test
                SCRIPT test/PixelCablingConfigNew_test.py
                PROPERTIES TIMEOUT 30 )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
atlas_install_runtime( share/*.dat )
