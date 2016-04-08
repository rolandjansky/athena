################################################################################
# Package: TrkVxEdmCnv
################################################################################

# Declare the package name:
atlas_subdir( TrkVxEdmCnv )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Event/xAOD/xAODTracking
                          GaudiKernel
                          PRIVATE
                          Control/StoreGate
                          Tracking/TrkEvent/TrkLinks
                          Tracking/TrkEvent/TrkTrackLink
                          Tracking/TrkEvent/VxVertex
                          Tracking/TrkVertexFitter/TrkVertexFitterInterfaces )

# Component(s) in the package:
atlas_add_library( TrkVxEdmCnvLib
                   src/*.cxx
                   src/components/*.cxx
                   PUBLIC_HEADERS TrkVxEdmCnv
                   LINK_LIBRARIES AthenaBaseComps xAODTracking GaudiKernel StoreGateLib SGtests
                   PRIVATE_LINK_LIBRARIES TrkLinks VxVertex TrkVertexFitterInterfaces )

atlas_add_component( TrkVxEdmCnv
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps xAODTracking GaudiKernel StoreGateLib SGtests TrkLinks VxVertex TrkVertexFitterInterfaces TrkVxEdmCnvLib )

