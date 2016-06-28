################################################################################
# Package: AssociationComps
################################################################################

# Declare the package name:
atlas_subdir( AssociationComps )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Calorimeter/CaloEvent
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/AthContainers
                          Control/AthLinks
                          Control/Navigation
                          Control/StoreGate
                          Event/EventKernel
                          Event/FourMomUtils
                          Event/NavFourMom
                          GaudiKernel
                          PhysicsAnalysis/AssociationBuilder/AssociationKernel
                          Reconstruction/Jet/JetEvent
                          Reconstruction/MuonIdentification/muonEvent
                          Reconstruction/egamma/egammaEvent
                          Reconstruction/tauEvent
                          Trigger/TrigAnalysis/TrigObjectMatching
                          Trigger/TrigEvent/TrigCaloEvent
                          Trigger/TrigEvent/TrigMuonEvent
                          Trigger/TrigEvent/TrigParticle )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( CLHEP )

# Component(s) in the package:
atlas_add_component( AssociationComps
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} ${CLHEP_LIBRARIES} CaloEvent AthenaKernel AthenaBaseComps AthContainers AthLinks Navigation StoreGateLib SGtests EventKernel FourMomUtils NavFourMom GaudiKernel AssociationKernel JetEvent muonEvent egammaEvent tauEvent TrigObjectMatchingLib TrigCaloEvent TrigMuonEvent TrigParticle )

# Install files from the package:
atlas_install_joboptions( share/*.py )

