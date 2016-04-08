################################################################################
# Package: ExoticsD3PDMaker
################################################################################

# Declare the package name:
atlas_subdir( ExoticsD3PDMaker )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Calorimeter/CaloEvent
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          GaudiKernel
                          PhysicsAnalysis/AnalysisCommon/AnalysisTools
                          Reconstruction/MuonIdentification/muonEvent
                          Reconstruction/Particle
                          Reconstruction/egamma/egammaEvent
                          Tracking/TrkEvent/TrkTrackSummary )

# Component(s) in the package:
atlas_add_component( ExoticsD3PDMaker
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES CaloEvent AthenaBaseComps AthenaKernel GaudiKernel AthAnalysisToolsLib muonEvent Particle egammaEvent TrkTrackSummary )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

