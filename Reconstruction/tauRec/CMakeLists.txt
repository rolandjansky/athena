################################################################################
# Package: tauRec
################################################################################

# Declare the package name:
atlas_subdir( tauRec )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          GaudiKernel
                          PhysicsAnalysis/AnalysisCommon/ParticleEvent
                          Reconstruction/tauRecTools
                          PRIVATE
                          Reconstruction/tauEvent )

# External dependencies:
find_package( ROOT COMPONENTS TMVA Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( tauRecLib
                   src/*.cxx
                   PUBLIC_HEADERS tauRec
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps AthenaKernel GaudiKernel ParticleEvent tauRecToolsLib
                   PRIVATE_LINK_LIBRARIES tauEvent )

atlas_add_component( tauRec
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps AthenaKernel GaudiKernel ParticleEvent tauRecToolsLib tauEvent tauRecLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

