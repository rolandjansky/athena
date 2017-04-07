################################################################################
# Package: ISF_FatrasTools
################################################################################

# Declare the package name:
atlas_subdir( ISF_FatrasTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          DetectorDescription/GeoPrimitives
                          Event/EventPrimitives
                          GaudiKernel
                          Simulation/Barcode/BarcodeEvent
                          Simulation/ISF/ISF_Core/ISF_Event
                          Simulation/ISF/ISF_Core/ISF_Interfaces
                          Simulation/ISF/ISF_Fatras/ISF_FatrasInterfaces
                          Tracking/TrkDetDescr/TrkDetDescrUtils
                          Tracking/TrkDetDescr/TrkGeometry
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkExtrapolation/TrkExInterfaces
                          Tracking/TrkExtrapolation/TrkExUtils
                          PRIVATE
                          Control/StoreGate
                          DetectorDescription/AtlasDetDescr
                          Simulation/FastSimulation/FastSimulationEvent
                          Simulation/Tools/AtlasCLHEP_RandomGenerators
                          Tracking/TrkDetDescr/TrkDetDescrInterfaces
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkDetDescr/TrkVolumes
                          Tracking/TrkEvent/TrkMaterialOnTrack
                          Tracking/TrkEvent/TrkNeutralParameters
                          Tracking/TrkEvent/TrkTrack )

# External dependencies:
find_package( CLHEP )
find_package( Eigen )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( ISF_FatrasTools
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} ${EIGEN_LIBRARIES} AthenaBaseComps AthenaKernel GeoPrimitives EventPrimitives GaudiKernel ISF_Event ISF_Interfaces ISF_FatrasInterfaces TrkDetDescrUtils TrkGeometry TrkEventPrimitives TrkParameters TrkExInterfaces TrkExUtils StoreGateLib SGtests AtlasDetDescr AtlasCLHEP_RandomGenerators TrkDetDescrInterfaces TrkSurfaces TrkVolumes TrkMaterialOnTrack TrkNeutralParameters TrkTrack )

# Install files from the package:
atlas_install_headers( ISF_FatrasTools )

