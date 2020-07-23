################################################################################
# Package: RecBackgroundAlgs
################################################################################

# Declare the package name:
atlas_subdir( RecBackgroundAlgs )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthLinks
                          Control/AthenaBaseComps
                          Event/xAOD/xAODCaloEvent
                          Event/xAOD/xAODEventInfo
                          Event/xAOD/xAODJet
                          GaudiKernel
                          MuonSpectrometer/MuonIdHelpers
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonSegment
                          MuonSpectrometer/MuonReconstruction/MuonRecTools/MuonRecHelperTools
                          Reconstruction/RecBackground/RecBackgroundEvent
                          Tracking/TrkEvent/TrkSegment
                          PRIVATE
                          Calorimeter/CaloGeoHelpers
                          Control/AthenaKernel
                          Control/StoreGate
                          DetectorDescription/GeoPrimitives
                          ForwardDetectors/LUCID/LUCID_RawEvent
                          InnerDetector/InDetRecEvent/BCM_CollisionTime
                          LArCalorimeter/LArRecEvent
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonPrepRawData
                          PhysicsAnalysis/EventTag/TagEvent
                          TileCalorimeter/TileEvent )

# External dependencies:
find_package( Eigen )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( RecBackgroundAlgs
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${EIGEN_LIBRARIES} AthLinks AthenaBaseComps xAODCaloEvent xAODEventInfo xAODJet GaudiKernel MuonIdHelpersLib MuonSegment MuonRecHelperToolsLib RecBackgroundEvent TrkSegment CaloGeoHelpers AthenaKernel StoreGateLib SGtests GeoPrimitives LUCID_RawEvent LArRecEvent MuonPrepRawData TagEvent TileEvent BCM_CollisionTimeLib )

# Install files from the package:
atlas_install_headers( RecBackgroundAlgs )
atlas_install_joboptions( share/*.py )

