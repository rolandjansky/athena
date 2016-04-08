################################################################################
# Package: HiggsD3PDMaker
################################################################################

# Declare the package name:
atlas_subdir( HiggsD3PDMaker )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Control/AthenaKernel
                          GaudiKernel
                          PhysicsAnalysis/AnalysisCommon/ParticleEvent
                          PhysicsAnalysis/D3PDMaker/D3PDMakerUtils
                          PhysicsAnalysis/HiggsPhys/HSG2/HSG2Event
                          Tracking/TrkEvent/VxVertex )

# Component(s) in the package:
atlas_add_component( HiggsD3PDMaker
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaKernel GaudiKernel ParticleEvent D3PDMakerUtils HSG2Event VxVertex )

# Install files from the package:
atlas_install_python_modules( python/*.py )

