################################################################################
# Package: InDetSecVxFinderTool
################################################################################

# Declare the package name:
atlas_subdir( InDetSecVxFinderTool )


# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          DetectorDescription/GeoPrimitives
                          GaudiKernel
                          InnerDetector/InDetRecTools/InDetRecToolInterfaces
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkEvent/TrkParticleBase
                          Tracking/TrkEvent/TrkTrackLink
                          PRIVATE
                          Event/EventPrimitives
                          Event/xAOD/xAODTracking
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkLinks
                          Tracking/TrkEvent/TrkNeutralParameters
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkEvent/VxJetVertex
                          Tracking/TrkEvent/VxSecVertex
                          Tracking/TrkEvent/VxVertex
                          Tracking/TrkExtrapolation/TrkExInterfaces
                          Tracking/TrkTools/TrkToolInterfaces
                          Tracking/TrkVertexFitter/TrkJetVxFitter
                          Tracking/TrkVertexFitter/TrkVertexFitterInterfaces
                          Tracking/TrkVertexFitter/TrkVertexFitters
                          Tracking/TrkVertexFitter/TrkVertexSeedFinderTools
                          Tracking/TrkVertexFitter/TrkVertexSeedFinderUtils
                          Tracking/TrkVertexFitter/TrkVxEdmCnv )

# External dependencies:
find_package( CLHEP )
find_package( Eigen )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( InDetSecVxFinderToolLib
                   src/*.cxx
                   PUBLIC_HEADERS InDetSecVxFinderTool
                   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${CLHEP_LIBRARIES} ${EIGEN_LIBRARIES} AthenaBaseComps GeoPrimitives GaudiKernel InDetRecToolInterfaces TrkParameters TrkParticleBase TrkJetVxFitterLib TrkVertexFittersLib TrkVxEdmCnvLib
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} EventPrimitives xAODTracking TrkSurfaces TrkEventPrimitives TrkLinks TrkNeutralParameters TrkTrack VxJetVertex VxSecVertex VxVertex TrkExInterfaces TrkToolInterfaces TrkVertexFitterInterfaces )

atlas_add_component( InDetSecVxFinderTool
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} ${EIGEN_LIBRARIES} AthenaBaseComps GeoPrimitives GaudiKernel InDetRecToolInterfaces TrkParameters TrkParticleBase EventPrimitives xAODTracking TrkSurfaces TrkEventPrimitives TrkLinks TrkNeutralParameters TrkTrack VxJetVertex VxSecVertex VxVertex TrkExInterfaces TrkToolInterfaces TrkJetVxFitterLib TrkVertexFitterInterfaces TrkVertexFittersLib TrkVxEdmCnvLib InDetSecVxFinderToolLib )

