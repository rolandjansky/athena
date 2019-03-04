################################################################################
# Package: SiPropertiesTool
################################################################################

# Declare the package name:
atlas_subdir( SiPropertiesTool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          GaudiKernel
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry
                          PRIVATE
                          Control/StoreGate
                          DetectorDescription/Identifier
                          InnerDetector/InDetConditions/InDetConditionsSummaryService
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          InnerDetector/InDetConditions/PixelConditionsData
                          InnerDetector/InDetConditions/PixelConditionsTools
                          InnerDetector/InDetConditions/SCT_ConditionsData )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_library( SiPropertiesToolLib
                   src/*.cxx
                   PUBLIC_HEADERS SiPropertiesTool
                   PRIVATE_INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES AthenaBaseComps AthenaKernel GaudiKernel InDetReadoutGeometry PixelConditionsData SCT_ConditionsData StoreGateLib SGtests
                   PRIVATE_LINK_LIBRARIES ${CLHEP_LIBRARIES} Identifier InDetIdentifier )

atlas_add_component( SiPropertiesTool
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} AthenaBaseComps AthenaKernel GaudiKernel InDetReadoutGeometry PixelConditionsData SCT_ConditionsData StoreGateLib SGtests Identifier InDetIdentifier SiPropertiesToolLib )

# Run tests:
atlas_add_test( TestSCTProperties
                SCRIPT athena.py --threads=5 SiPropertiesTool/testSCTProperties.py
                PROPERTIES TIMEOUT 300
                ENVIRONMENT THREADS=5 )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
