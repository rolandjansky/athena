################################################################################
# Package: egammaEvent
################################################################################

# Declare the package name:
atlas_subdir( egammaEvent )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Calorimeter/CaloEvent
                          Calorimeter/CaloGeoHelpers
                          Calorimeter/CaloIdentifier
                          Control/CLIDSvc
                          Control/DataModel
                          Control/Navigation
                          Control/SGTools
                          Event/EventKernel
                          Event/FourMom
                          Event/NavFourMom
                          PhysicsAnalysis/AnalysisCommon/ParticleEvent
                          Reconstruction/Jet/JetEvent
                          Reconstruction/Particle
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkEvent/VxVertex
                          PRIVATE
                          GaudiKernel
                          Reconstruction/egamma/egammaConditions
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkMaterialOnTrack
                          Tracking/TrkEvent/TrkNeutralParameters
                          Tracking/TrkEvent/TrkParametersBase
                          Tracking/TrkEvent/TrkParticleBase
                          Tracking/TrkEvent/TrkTrack )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( egammaEvent
                   src/*.cxx
                   PUBLIC_HEADERS egammaEvent
                   INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${Boost_LIBRARIES} CaloEvent CaloGeoHelpers CaloIdentifier DataModel Navigation SGTools EventKernel FourMom NavFourMom ParticleEvent JetEvent Particle TrkParameters VxVertex
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} GaudiKernel egammaConditions TrkSurfaces TrkMaterialOnTrack TrkNeutralParameters TrkParametersBase TrkParticleBase TrkTrack )

atlas_add_dictionary( egammaEventDict
                      egammaEvent/egammaEventDict.h
                      egammaEvent/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} ${Boost_LIBRARIES} ${CLHEP_LIBRARIES} CaloEvent CaloGeoHelpers CaloIdentifier DataModel Navigation SGTools EventKernel FourMom NavFourMom ParticleEvent JetEvent Particle TrkParameters VxVertex GaudiKernel egammaConditions TrkSurfaces TrkMaterialOnTrack TrkNeutralParameters TrkParametersBase TrkParticleBase TrkTrack egammaEvent
                      NAVIGABLES egammaContainer ElectronContainer PhotonContainer EMShowerMinimalContainer
                      ELEMENT_LINKS egDetailContainer CaloRingsContainer )

atlas_add_dictionary( egammaEnumsDict
                      egammaEvent/egammaEnumsDict.h
                      egammaEvent/selectionEnums.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} ${Boost_LIBRARIES} ${CLHEP_LIBRARIES} CaloEvent CaloGeoHelpers CaloIdentifier DataModel Navigation SGTools EventKernel FourMom NavFourMom ParticleEvent JetEvent Particle TrkParameters VxVertex GaudiKernel egammaConditions TrkSurfaces TrkMaterialOnTrack TrkNeutralParameters TrkParametersBase TrkParticleBase TrkTrack egammaEvent )

