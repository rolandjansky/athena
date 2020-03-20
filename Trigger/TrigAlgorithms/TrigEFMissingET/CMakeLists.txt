# $Id: CMakeLists.txt 800798 2017-03-16 17:29:50Z jburr $
################################################################################
# Package: TrigEFMissingET
################################################################################

# Declare the package name:
atlas_subdir( TrigEFMissingET )

# External dependencies:
find_package( ROOT COMPONENTS Core Hist GenVector )
find_package( tdaq-common COMPONENTS eformat )
find_package( Eigen )

# Component(s) in the package:
atlas_add_library( TrigEFMissingETLib
  src/METComponent.cxx
  src/METMonitor.cxx
  src/StatusFlags.cxx
  src/SignedKinematics.cxx
  src/CVFTool.cxx
  src/PufitGrid.cxx
  src/PufitUtils.cxx
  PUBLIC_HEADERS TrigEFMissingET
  INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${Eigen_INCLUDE_DIRS}
  LINK_LIBRARIES xAODTrigMissingET AsgTools xAODCaloEvent xAODTracking
  TrigInterfacesLib AthenaBaseComps GaudiKernel xAODBase
  PRIVATE_LINK_LIBRARIES AthLinks
)

atlas_add_component( TrigEFMissingET
   src/FexBase.cxx
   src/CellFex.cxx
   src/TCFex.cxx
   src/MHTFex.cxx
   src/TrkMHTFex.cxx
   src/TrkTCFex.cxx
   src/TCPufitFex.cxx
   src/TCPufitTrkFex.cxx
   src/CellPufitFex.cxx
   src/TCPuetaFex.cxx
   src/MuonFex.cxx
   src/FexSum.cxx
   src/components/*.cxx
   LINK_LIBRARIES GaudiKernel TrigEFMissingETLib ${TDAQ-COMMON_LIBRARIES}
   xAODJet CaloGeoHelpers xAODMuon PathResolver AthContainers CaloEvent
   LArRecEvent TileEvent TrigT2CaloCommonLib TrigTimeAlgsLib
   InDetTrackSelectionToolLib JetInterface
   # These next ones are needed for the trigger navigation to link feature
   # accessors properly
   TrigCaloEvent TrigMissingEtEvent TrigMuonEvent TrigParticle
)

# Install files from the package:
atlas_install_python_modules( python/*.py )
