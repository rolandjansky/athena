################################################################################
# Package: InDetPrepRawDataToxAOD
################################################################################

# Declare the package name:
atlas_subdir( InDetPrepRawDataToxAOD )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          PRIVATE
                          Control/AthenaBaseComps
                          Control/StoreGate
                          DetectorDescription/Identifier
                          Event/xAOD/xAODTracking
                          InnerDetector/InDetConditions/PixelConditionsTools
                          InnerDetector/InDetConditions/InDetByteStreamErrors
                          InnerDetector/InDetConditions/PixelConditionsData
                          InnerDetector/InDetConditions/InDetCondTools
                          InnerDetector/InDetConditions/TRT_ConditionsServices
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry
			  InnerDetector/InDetDetDescr/PixelReadoutGeometry
			  InnerDetector/InDetDetDescr/TRT_ReadoutGeometry
                          InnerDetector/InDetDetDescr/PixelCabling
                          InnerDetector/InDetRawEvent/InDetRawData
                          InnerDetector/InDetRawEvent/InDetSimData
                          InnerDetector/InDetRecEvent/InDetPrepRawData
                          InnerDetector/InDetRecTools/TRT_DriftFunctionTool
                          InnerDetector/InDetSimEvent
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkTruthData 
                          Generators/AtlasHepMC )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( InDetPrepRawDataToxAOD
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} 
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AtlasHepMCLib GaudiKernel AthenaBaseComps StoreGateLib SGtests Identifier InDetByteStreamErrors PixelConditionsData xAODTracking TRT_ConditionsServicesLib InDetIdentifier InDetReadoutGeometry PixelReadoutGeometry TRT_ReadoutGeometry InDetRawData InDetSimData InDetPrepRawData InDetSimEvent TrkSurfaces TrkTruthData TRT_DriftFunctionToolLib PixelCablingLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

