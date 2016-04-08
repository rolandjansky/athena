################################################################################
# Package: MuGirl
################################################################################

# Declare the package name:
atlas_subdir( MuGirl )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          PRIVATE
                          Control/AthenaBaseComps
                          Event/xAOD/xAODCaloEvent
                          MagneticField/MagFieldInterfaces
                          MuonSpectrometer/MuonIdHelpers
                          MuonSpectrometer/MuonReconstruction/MuonPatternFinders/MuonPatternFinderTools/MuonHoughPatternTools
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonSegment
                          MuonSpectrometer/MuonReconstruction/MuonRecUtils/MuonLayerHough
                          Reconstruction/MuonIdentification/ICaloTrkMuIdTools
                          Reconstruction/MuonIdentification/MuGirlInterfaces
                          Reconstruction/MuonIdentification/MuonCombinedEvent
                          Reconstruction/RecoTools/RecoToolInterfaces
                          Tracking/TrkEvent/TrkCaloExtension )

# Component(s) in the package:
atlas_add_component( MuGirl
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES GaudiKernel AthenaBaseComps xAODCaloEvent MagFieldInterfaces MuonIdHelpersLib MuonHoughPatternToolsLib MuonSegment MuonLayerHough ICaloTrkMuIdTools MuGirlInterfaces MuonCombinedEvent RecoToolInterfaces TrkCaloExtension )

# Install files from the package:
atlas_install_headers( MuGirl )
atlas_install_python_modules( python/*.py )

