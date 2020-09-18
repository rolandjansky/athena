################################################################################
# Package: ITrackToVertex
################################################################################

# Declare the package name:
atlas_subdir( ITrackToVertex )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( ITrackToVertex
                   PUBLIC_HEADERS ITrackToVertex
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
		   LINK_LIBRARIES ${ROOT_LIBRARIES} xAODTracking GaudiKernel TrkDetDescrUtils 
		   TrkNeutralParameters TrkParameters CxxUtils)

atlas_add_dictionary( ITrackToVertexDict
                      ITrackToVertex/ITrackToVertex.h
                      ITrackToVertex/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} xAODTracking GaudiKernel TrkDetDescrUtils 
		      TrkNeutralParameters TrkParameters ITrackToVertex )

