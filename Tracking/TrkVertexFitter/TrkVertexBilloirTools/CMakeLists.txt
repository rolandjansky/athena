################################################################################
# Package: TrkVertexBilloirTools
################################################################################

# Declare the package name:
atlas_subdir( TrkVertexBilloirTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Event/xAOD/xAODTracking
                          GaudiKernel
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkVertexFitter/TrkVertexFitterInterfaces
                          PRIVATE
                          Event/EventPrimitives
                          Tracking/TrkDetDescr/TrkDetDescrUtils
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkLinks
                          Tracking/TrkEvent/TrkParticleBase
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkEvent/VxVertex
                          Tracking/TrkExtrapolation/TrkExInterfaces
			  Control/CxxUtils
			  )


# Component(s) in the package:
atlas_add_component( TrkVertexBilloirTools
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps xAODTracking GaudiKernel TrkParameters TrkVertexFitterInterfaces EventPrimitives TrkDetDescrUtils 
		     TrkSurfaces TrkLinks TrkParticleBase TrkTrack VxVertex TrkExInterfaces CxxUtils)

# Install files from the package:
atlas_install_headers( TrkVertexBilloirTools )

