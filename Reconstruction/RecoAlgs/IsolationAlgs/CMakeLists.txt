################################################################################
# Package: IsolationAlgs
################################################################################

# Declare the package name:
atlas_subdir( IsolationAlgs )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          PRIVATE
                          Calorimeter/CaloEvent
                          Control/AthContainers
                          Control/AthenaBaseComps
                          Event/xAOD/xAODEgamma
                          Event/xAOD/xAODMuon
                          Event/xAOD/xAODPrimitives
                          Reconstruction/RecoTools/RecoToolInterfaces
                          Reconstruction/RecoTools/ParticlesInConeTools
			  PhysicsAnalysis/ElectronPhotonID/IsolationCorrections )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )

# Component(s) in the package:
atlas_add_component( IsolationAlgs
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} GaudiKernel CaloEvent AthContainers AthenaBaseComps xAODEgamma xAODMuon xAODPrimitives RecoToolInterfaces IsolationCorrectionsLib ParticlesInConeTools)

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

