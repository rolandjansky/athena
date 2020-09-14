################################################################################
# Package: muonEvent
################################################################################

# Declare the package name:
atlas_subdir( muonEvent )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( muonEvent
                   src/*.cxx
                   PUBLIC_HEADERS muonEvent
                   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${CLHEP_LIBRARIES} CaloEvent CaloIdentifier AthContainers AthLinks AthenaKernel FourMom NavFourMom ParticleEvent Particle TrkMaterialOnTrack TrkSegment
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} Navigation GaudiKernel VxVertex )

atlas_add_dictionary( muonEventDict
                      muonEvent/muonEventDict.h
                      muonEvent/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} CaloEvent CaloIdentifier AthContainers AthLinks AthenaKernel FourMom NavFourMom ParticleEvent Particle TrkMaterialOnTrack TrkSegment Navigation GaudiKernel VxVertex muonEvent
                      NAVIGABLES Analysis::MuonContainer 
                      ELEMENT_LINKS MuonCaloEnergyContainer )

