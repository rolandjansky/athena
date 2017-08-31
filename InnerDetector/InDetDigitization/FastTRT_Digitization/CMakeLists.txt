################################################################################
# Package: FastTRT_Digitization
################################################################################

# Declare the package name:
atlas_subdir( FastTRT_Digitization )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/PileUpTools
                          Control/StoreGate
                          DetectorDescription/Identifier
                          Event/EventInfo
                          Event/xAOD/xAODEventInfo
                          GaudiKernel
                          InnerDetector/InDetConditions/TRT_ConditionsServices
                          InnerDetector/InDetRecEvent/InDetPrepRawData
                          InnerDetector/InDetRecTools/TRT_DriftFunctionTool
                          InnerDetector/InDetSimEvent
                          Simulation/HitManagement
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkEvent/TrkTruthData
                          Tracking/TrkTools/TrkToolInterfaces
                          Event/EventContainers
                          PRIVATE
                          Control/CxxUtils
                          DetectorDescription/GeoPrimitives
                          Generators/GeneratorObjects
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry
                          InnerDetector/InDetRawEvent/InDetSimData
                          Tracking/TrkDetDescr/TrkDetElementBase )

# External dependencies:
find_package( CLHEP )
find_package( Eigen )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( FastTRT_Digitization
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} ${EIGEN_LIBRARIES} AthenaBaseComps AthenaKernel PileUpToolsLib StoreGateLib SGtests Identifier EventInfo xAODEventInfo GaudiKernel TRT_ConditionsServicesLib InDetPrepRawData InDetSimEvent HitManagement TrkParameters TrkTruthData TrkToolInterfaces CxxUtils GeoPrimitives GeneratorObjects InDetIdentifier InDetReadoutGeometry InDetSimData TrkDetElementBase EventContainers)

# Install files from the package:
atlas_install_headers( FastTRT_Digitization )
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

