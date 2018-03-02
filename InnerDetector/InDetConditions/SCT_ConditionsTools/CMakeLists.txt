###############################################################################
# Package: SCT_ConditionsTools
################################################################################

# Declare the package name:
atlas_subdir( SCT_ConditionsTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaKernel
                          DetectorDescription/Identifier
                          GaudiKernel
                          PRIVATE
                          Control/AthenaBaseComps
                          Control/StoreGate
                          InnerDetector/InDetConditions/SCT_ConditionsData
                          InnerDetector/InDetConditions/SiPropertiesSvc
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry
                          InnerDetector/InDetDetDescr/SCT_Cabling )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component ( SCT_ConditionsTools
                      src/components/*.cxx
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AthenaKernel Identifier GaudiKernel AthenaBaseComps StoreGateLib SGtests SCT_ConditionsData InDetIdentifier InDetReadoutGeometry SCT_CablingLib SiPropertiesSvcLib SCT_ConditionsToolsLib )


atlas_add_library( SCT_ConditionsToolsLib
                   src/*.cxx
                   src/components/*.cxx
                   PUBLIC_HEADERS SCT_ConditionsTools
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AthenaKernel Identifier GaudiKernel AthenaBaseComps StoreGateLib SGtests SCT_ConditionsData SiPropertiesSvcLib InDetIdentifier InDetReadoutGeometry SCT_CablingLib )

# Add unit tests

# Install files from the package:
atlas_install_headers( SCT_ConditionsTools )
atlas_install_python_modules( python/*.py )
