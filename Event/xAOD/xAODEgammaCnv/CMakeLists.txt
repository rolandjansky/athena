################################################################################
# Package: xAODEgammaCnv
################################################################################

# Declare the package name:
atlas_subdir( xAODEgammaCnv )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthContainers
                          Event/xAOD/xAODEgamma
                          GaudiKernel
                          PRIVATE
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Event/FourMom
                          Event/xAOD/xAODTracking
                          Event/xAOD/xAODTruth
                          PhysicsAnalysis/AnalysisCommon/ParticleEvent
                          Reconstruction/Particle
                          Reconstruction/egamma/egammaEvent )

# Component(s) in the package:
atlas_add_library( xAODEgammaCnvLib
                   xAODEgammaCnv/*.h
                   INTERFACE
                   PUBLIC_HEADERS xAODEgammaCnv
                   LINK_LIBRARIES AthContainers GaudiKernel xAODEgamma )

atlas_add_component( xAODEgammaCnv
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthContainers AthenaBaseComps AthenaKernel FourMom GaudiKernel Particle ParticleEvent egammaEvent xAODEgamma xAODTracking xAODTruth )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

