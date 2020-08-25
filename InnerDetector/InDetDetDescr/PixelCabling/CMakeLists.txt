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
   Control/StoreGate
   Database/AthenaPOOL/AthenaPoolUtilities
   DetectorDescription/Identifier
   InnerDetector/InDetDetDescr/InDetReadoutGeometry
   InnerDetector/InDetDetDescr/PixelReadoutGeometry
   InnerDetector/InDetDetDescr/InDetIdentifier
   InnerDetector/InDetConditions/PixelConditionsData
   Tools/PathResolver )

# Component(s) in the package:
atlas_add_library( PixelCablingLib
   PixelCabling/*.h
   INTERFACE
   PUBLIC_HEADERS PixelCabling
   LINK_LIBRARIES GaudiKernel AthenaKernel )

atlas_add_component( PixelCabling
   src/*.h src/*.cxx src/components/*.cxx
   LINK_LIBRARIES GaudiKernel AthenaBaseComps SGTools StoreGateLib
   AthenaPoolUtilities Identifier InDetIdentifier InDetReadoutGeometry PixelReadoutGeometry PixelConditionsData PathResolver PixelCablingLib )

atlas_add_test( PixelCablingConfigNew_test
                SCRIPT test/PixelCablingConfigNew_test.py
                PROPERTIES TIMEOUT 300 )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
