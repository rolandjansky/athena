################################################################################
# Package: TrigTRTHighTHitCounter
################################################################################

# Declare the package name:
atlas_subdir( TrigTRTHighTHitCounter )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          PRIVATE
                          DetectorDescription/GeoPrimitives
                          DetectorDescription/Identifier
                          Event/xAOD/xAODTrigRinger
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          InnerDetector/InDetRecEvent/InDetPrepRawData
                          Trigger/TrigEvent/TrigCaloEvent
                          Trigger/TrigEvent/TrigSteeringEvent
                          Trigger/TrigSteer/TrigInterfaces
                          Trigger/TrigTools/TrigInDetToolInterfaces )

# External dependencies:
find_package( Eigen )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

include_directories(src)

# Component(s) in the package:
atlas_add_component( TrigTRTHighTHitCounter
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${EIGEN_LIBRARIES} GaudiKernel GeoPrimitives Identifier xAODTrigRinger InDetIdentifier InDetPrepRawData TrigCaloEvent TrigSteeringEvent TrigInterfacesLib EventContainers)

