################################################################################
# Package: InDetZVTOPVxFinder
################################################################################

# Declare the package name:
atlas_subdir( InDetZVTOPVxFinder )

# declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel
                          InnerDetector/InDetRecTools/InDetRecToolInterfaces
                          Tracking/TrkEvent/VxVertex
                          Tracking/TrkEvent/TrkTrack
                          Reconstruction/Particle
                          Tracking/TrkEvent/TrkParticleBase
                          Tracking/TrkEvent/TrkParameters
                          Event/xAOD/xAODTracking
                          PRIVATE
                          External/AtlasCLHEP
                          Control/DataModel
                          InnerDetector/InDetConditions/InDetBeamSpotService
                          Tracking/TrkExtrapolation/TrkExInterfaces
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkVertexFitter/TrkVertexFitterInterfaces
                          Tracking/TrkEvent/VxSecVertex
                          Event/EventPrimitives
                          Tracking/TrkVertexFitter/TrkVxEdmCnv )

# External dependencies
find_package( HepMC )

# Component(s) in the package:
atlas_add_component( InDetZVTOPVxFinder
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${HEPMC_INCLUDE_DIRS}
                     LINK_LIBRARIES ${HEPMC_LIBRARIES} AthenaBaseComps GaudiKernel InDetRecToolInterfaces VxVertex Particle TrkParticleBase TrkParameters xAODTracking DataModel TrkExInterfaces TrkSurfaces TrkVertexFitterInterfaces VxSecVertex EventPrimitives TrkVxEdmCnvLib )

# Install files from the package:
atlas_install_headers( InDetZVTOPVxFinder )
