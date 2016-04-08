################################################################################
# Package: TrigMuonCoinHierarchy
################################################################################

# Declare the package name:
atlas_subdir( TrigMuonCoinHierarchy )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel
                          MuonSpectrometer/MuonRDO
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonPrepRawData
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonTrigCoinData
                          PhysicsAnalysis/AnalysisTrigger/AnalysisTriggerEvent
                          PRIVATE
                          Event/EventInfo
                          MuonSpectrometer/MuonCablings/TGCcablingInterface
                          MuonSpectrometer/MuonDetDescr/MuonReadoutGeometry
                          MuonSpectrometer/MuonIdHelpers
                          Reconstruction/MuonIdentification/muonEvent
                          Tools/PathResolver
                          Tracking/TrkEvent/TrkCompetingRIOsOnTrack
                          Tracking/TrkEvent/TrkEventUtils
                          Tracking/TrkEvent/TrkRIO_OnTrack
                          Tracking/TrkEvent/TrkTrack
                          Trigger/TrigT1/TrigT1Result )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( TrigMuonCoinHierarchyLib
                   src/*.cxx
                   PUBLIC_HEADERS TrigMuonCoinHierarchy
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES AthenaBaseComps GaudiKernel MuonRDO MuonPrepRawData MuonTrigCoinData AnalysisTriggerEvent TGCcablingInterfaceLib MuonIdHelpersLib
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} EventInfo MuonReadoutGeometry muonEvent PathResolver TrkCompetingRIOsOnTrack TrkEventUtils TrkRIO_OnTrack TrkTrack TrigT1Result )

atlas_add_component( TrigMuonCoinHierarchy
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps GaudiKernel MuonRDO MuonPrepRawData MuonTrigCoinData AnalysisTriggerEvent EventInfo TGCcablingInterfaceLib MuonReadoutGeometry MuonIdHelpersLib muonEvent PathResolver TrkCompetingRIOsOnTrack TrkEventUtils TrkRIO_OnTrack TrkTrack TrigT1Result TrigMuonCoinHierarchyLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )
atlas_install_runtime( share/mask2/*.txt )

