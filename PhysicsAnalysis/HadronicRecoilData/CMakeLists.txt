################################################################################
# Package: HadronicRecoilData
################################################################################

# Declare the package name:
atlas_subdir( HadronicRecoilData )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Calorimeter/CaloEvent
                          Control/CLIDSvc
                          Control/DataModel
                          Control/SGTools
                          PhysicsAnalysis/TruthParticleID/McParticleEvent
                          Reconstruction/MuonIdentification/muonEvent
                          Reconstruction/Particle
                          Reconstruction/egamma/egammaEvent )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( HadronicRecoilData
                   src/*.cxx
                   PUBLIC_HEADERS HadronicRecoilData
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} CaloEvent DataModel SGTools McParticleEvent muonEvent Particle egammaEvent )

