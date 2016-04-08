################################################################################
# Package: TrigJPsiTools
################################################################################

# Declare the package name:
atlas_subdir( TrigJPsiTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Calorimeter/CaloEvent
                          GaudiKernel
                          Reconstruction/MuonIdentification/muonEvent
                          Reconstruction/Particle
                          Reconstruction/egamma/egammaEvent
                          Trigger/TrigAnalysis/TrigEffJpsiTools
                          Trigger/TrigAnalysis/TrigInDetAnalysisUtils
                          Trigger/TrigEvent/TrigInDetEvent
                          Trigger/TrigEvent/TrigMuonEvent )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread MathMore Minuit Minuit2 Matrix Physics HistPainter Rint )

# Component(s) in the package:
atlas_add_library( TrigJPsiTools
                   src/*.cxx
                   PUBLIC_HEADERS TrigJPsiTools
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES CaloEvent GaudiKernel muonEvent Particle egammaEvent TrigInDetAnalysisUtils TrigInDetEvent TrigMuonEvent
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} )

