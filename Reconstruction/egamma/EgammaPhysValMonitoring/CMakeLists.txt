################################################################################
# Package: EgammaPhysValMonitoring
################################################################################

# Declare the package name:
atlas_subdir( EgammaPhysValMonitoring )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          PRIVATE
                          Control/AthenaBaseComps
                          Control/AthenaMonitoring
                          Event/xAOD/xAODEgamma
                          Event/xAOD/xAODEventInfo
                          Event/xAOD/xAODPrimitives
                          Event/xAOD/xAODTruth
                          PhysicsAnalysis/MCTruthClassifier
                          Tracking/TrkValidation/TrkValHistUtils )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( EgammaPhysValMonitoring
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} GaudiKernel AthenaBaseComps AthenaMonitoringLib xAODEgamma xAODEventInfo xAODPrimitives xAODTruth MCTruthClassifierLib TrkValHistUtils )

