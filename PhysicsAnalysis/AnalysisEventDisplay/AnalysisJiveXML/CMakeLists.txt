################################################################################
# Package: AnalysisJiveXML
################################################################################

# Declare the package name:
atlas_subdir( AnalysisJiveXML )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel
                          Reconstruction/MuonIdentification/muonEvent
                          Reconstruction/egamma/egammaEvent
                          Reconstruction/tauEvent
                          Trigger/TrigEvent/TrigCaloEvent
                          graphics/JiveXML
                          PRIVATE
                          Calorimeter/CaloEvent
                          Calorimeter/CaloGeoHelpers
                          Control/AthLinks
                          Control/AthContainers
                          PhysicsAnalysis/AnalysisCommon/CompositeParticleEvent
                          PhysicsAnalysis/TruthParticleID/McParticleEvent
                          Reconstruction/Jet/JetEvent
                          Reconstruction/Particle
                          Tracking/TrkEvent/TrkEventPrimitives )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_component( AnalysisJiveXML
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} AthenaBaseComps GaudiKernel muonEvent egammaEvent tauEvent TrigCaloEvent JiveXMLLib CaloEvent CaloGeoHelpers AthLinks AthContainers McParticleEvent JetEvent Particle TrkEventPrimitives )

# Install files from the package:
atlas_install_headers( AnalysisJiveXML )
atlas_install_joboptions( share/*.py )

