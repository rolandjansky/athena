################################################################################
# Package: TrackCaloClusterRecValidation
################################################################################


# Declare the package name:
atlas_subdir( TrackCaloClusterRecValidation )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/AthenaMonitoring
                          Event/xAOD/xAODCore
                          Event/xAOD/xAODJet
                          Event/xAOD/xAODTracking
                          Event/xAOD/xAODTruth
                          Event/xAOD/xAODEventInfo
                          Event/xAOD/xAODPFlow
                          GaudiKernel
                          InnerDetector/InDetValidation/InDetPhysValMonitoring
                          Tracking/TrkValidation/TrkValHistUtils
                          PRIVATE
                          Control/AthToolSupport/AsgTools
                          Control/AthenaKernel
                          DetectorDescription/GeoPrimitives
                          Event/EventPrimitives
                          Event/xAOD/xAODBase
                          Event/xAOD/xAODParticleEvent
                          Reconstruction/Jet/JetCalibTools
                          Tools/PathResolver
                          )
                          

# External dependencies:
find_package( Eigen )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread EG )
find_package( XercesC )
find_package( Boost )

# Component(s) in the package:
atlas_add_component( TrackCaloClusterRecValidation
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${XERCESC_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}  ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${XERCESC_LIBRARIES} ${Boost_LIBRARIES} ${EIGEN_LIBRARIES} 
                     AthenaBaseComps AthenaMonitoringLib xAODCore xAODTracking xAODTruth xAODPFlow GaudiKernel TrkValHistUtils InDetPhysValMonitoringLib
                     AsgTools AthenaKernel GeoPrimitives EventPrimitives xAODBase xAODParticleEvent xAODEventInfo xAODJet PathResolver)
# Install files from the package:
atlas_install_runtime( share/*.xml  share/*.xsl )

