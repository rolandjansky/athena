###############################################################################
# Package: SCT_ConditionsAlgorithms
################################################################################

# Declare the package name:
atlas_subdir( SCT_ConditionsAlgorithms )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/StoreGate
                          Database/AthenaPOOL/AthenaPoolUtilities
                          DetectorDescription/Identifier
                          GaudiKernel
                          InnerDetector/InDetConditions/InDetConditionsSummaryService
                          InnerDetector/InDetConditions/SCT_ConditionsData
                          InnerDetector/InDetConditions/SCT_ConditionsTools
                          InnerDetector/InDetDetDescr/SCT_Cabling
                          Event/EventInfo
                          Event/xAOD/xAODEventInfo
                          PRIVATE
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry 
                          InnerDetector/InDetConditions/SCT_ConditionsServices )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )

# Component(s) in the package:
atlas_add_component( SCT_ConditionsAlgorithms
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} AthenaBaseComps AthenaKernel StoreGateLib SGtests Identifier GaudiKernel SCT_ConditionsData SCT_CablingLib EventInfo xAODEventInfo AthenaPoolUtilities InDetIdentifier InDetReadoutGeometry SCT_ConditionsServicesLib SCT_ConditionsToolsLib )

# Add unit tests

# Install files from the package:
