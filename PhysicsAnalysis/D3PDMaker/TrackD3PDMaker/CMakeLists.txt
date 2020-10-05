################################################################################
# Package: TrackD3PDMaker
################################################################################

# Declare the package name:
atlas_subdir( TrackD3PDMaker )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Tracking/TrkEvent/TrkParameters
                          PRIVATE
                          Control/AthenaKernel
                          Control/CxxUtils
                          DetectorDescription/Identifier
                          Event/EventPrimitives
                          Event/xAOD/xAODBase
                          Event/xAOD/xAODPrimitives
                          Event/xAOD/xAODTracking
                          GaudiKernel
                          InnerDetector/InDetConditions/InDetBeamSpotService
                          InnerDetector/InDetConditions/InDetConditionsSummaryService
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry
                          InnerDetector/InDetRecTools/InDetTestBLayer
                          PhysicsAnalysis/AnalysisCommon/ParticleEvent
                          PhysicsAnalysis/D3PDMaker/D3PDMakerInterfaces
                          PhysicsAnalysis/D3PDMaker/D3PDMakerUtils
                          Reconstruction/Particle
                          Reconstruction/RecoTools/ITrackToVertex
                          Reconstruction/RecoTools/RecoToolInterfaces
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkParticleBase
                          Tracking/TrkEvent/VxVertex 
                          Generators/AtlasHepMC )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( HepPDT )

# Component(s) in the package:
atlas_add_component( TrackD3PDMaker
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${HEPPDT_INCLUDE_DIRS} 
                     LINK_LIBRARIES ${Boost_LIBRARIES} ${HEPPDT_LIBRARIES} AtlasHepMCLib TrkParameters AthenaKernel CxxUtils SGtests Identifier EventPrimitives xAODBase xAODPrimitives xAODTracking GaudiKernel  InDetIdentifier InDetReadoutGeometry  InDetTestBLayerLib ParticleEvent D3PDMakerUtils Particle ITrackToVertex RecoToolInterfaces TrkEventPrimitives TrkParticleBase VxVertex InDetBeamSpotServiceLib )

# Install files from the package:
atlas_install_headers( TrackD3PDMaker )
atlas_install_python_modules( python/*.py )
