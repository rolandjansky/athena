################################################################################
# Package: ISF_FatrasTools
################################################################################

# Declare the package name:
atlas_subdir( ISF_FatrasTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          PRIVATE
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/StoreGate
                          DetectorDescription/AtlasDetDescr
                          DetectorDescription/GeoPrimitives
                          Event/EventPrimitives
                          Simulation/Barcode/BarcodeEvent
                          Simulation/FastSimulation/FastSimulationEvent
                          Simulation/ISF/ISF_Core/ISF_Event
                          Simulation/ISF/ISF_Core/ISF_Interfaces
                          Simulation/ISF/ISF_Fatras/ISF_FatrasInterfaces
                          Tracking/TrkDetDescr/TrkDetDescrInterfaces
                          Tracking/TrkDetDescr/TrkDetDescrUtils
                          Tracking/TrkDetDescr/TrkGeometry
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkDetDescr/TrkVolumes
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkMaterialOnTrack
                          Tracking/TrkEvent/TrkNeutralParameters
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkExtrapolation/TrkExInterfaces
                          Tracking/TrkExtrapolation/TrkExUtils )

# External dependencies:
find_package( CLHEP )
find_package( Eigen )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( ISF_FatrasTools
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} ${EIGEN_LIBRARIES} AthenaBaseComps AthenaKernel GeoPrimitives EventPrimitives FastSimulationEventLib GaudiKernel ISF_Event ISF_InterfacesLib ISF_FatrasInterfaces TrkDetDescrUtils TrkGeometry TrkEventPrimitives TrkParameters TrkExInterfaces TrkExUtils StoreGateLib SGtests AtlasDetDescr TrkDetDescrInterfaces TrkSurfaces TrkVolumes TrkMaterialOnTrack TrkNeutralParameters TrkTrack )
