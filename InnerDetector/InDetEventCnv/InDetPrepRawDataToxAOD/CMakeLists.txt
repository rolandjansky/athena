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
                          InnerDetector/InDetConditions/PixelConditionsServices
                          InnerDetector/InDetConditions/PixelConditionsTools
                          InnerDetector/InDetConditions/PixelConditionsData
                          InnerDetector/InDetConditions/InDetCondServices
                          InnerDetector/InDetConditions/TRT_ConditionsServices
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry
                          InnerDetector/InDetRawEvent/InDetRawData
                          InnerDetector/InDetRawEvent/InDetSimData
                          InnerDetector/InDetRecEvent/InDetPrepRawData
                          InnerDetector/InDetRecTools/TRT_DriftFunctionTool
                          InnerDetector/InDetSimEvent
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkTruthData )

# External dependencies:
find_package( CLHEP )
find_package( HepMC )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( InDetPrepRawDataToxAOD
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} ${HEPMC_LIBRARIES} GaudiKernel AthenaBaseComps StoreGateLib SGtests Identifier xAODTracking TRT_ConditionsServicesLib InDetIdentifier InDetReadoutGeometry InDetRawData InDetSimData InDetPrepRawData PixelConditionsData InDetSimEvent TrkSurfaces TrkTruthData )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

