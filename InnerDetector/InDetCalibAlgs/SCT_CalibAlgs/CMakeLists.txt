################################################################################
# Package: SCT_CalibAlgs
################################################################################

# Declare the package name:
atlas_subdir( SCT_CalibAlgs )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/StoreGate
                          Database/AthenaPOOL/AthenaPoolUtilities
                          DetectorDescription/Identifier
                          Event/EventInfo
                          GaudiKernel
                          InnerDetector/InDetConditions/InDetConditionsSummaryService
                          InnerDetector/InDetConditions/SCT_ConditionsData
                          InnerDetector/InDetConditions/SCT_ConditionsTools
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          InnerDetector/InDetDetDescr/SCT_Cabling
                          InnerDetector/InDetRawEvent/InDetRawData
                          PRIVATE
                          Database/RegistrationServices
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry
			  InnerDetector/InDetDetDescr/SCT_ReadoutGeometry
                          Tools/PathResolver
                          Tools/PyJobTransformsCore )

# External dependencies:
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )
find_package( ROOT COMPONENTS MathCore Core Tree Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( SCT_CalibAlgs
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${CORAL_LIBRARIES} AthenaBaseComps AthenaKernel StoreGateLib SGtests AthenaPoolUtilities Identifier EventInfo GaudiKernel SCT_ConditionsData InDetIdentifier SCT_CablingLib InDetRawData InDetReadoutGeometry SCT_ReadoutGeometry PathResolver InDetConditionsSummaryService SCT_ConditionsToolsLib RegistrationServicesLib )

# Install files from the package:
atlas_install_headers( SCT_CalibAlgs )
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

