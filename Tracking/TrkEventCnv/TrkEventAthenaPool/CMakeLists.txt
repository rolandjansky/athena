################################################################################
# Package: TrkEventAthenaPool
################################################################################

# Declare the package name:
atlas_subdir( TrkEventAthenaPool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Database/AthenaPOOL/AthenaPoolUtilities
                          PRIVATE
                          AtlasTest/TestTools
                          Control/StoreGate
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          DetectorDescription/AtlasDetDescr
                          GaudiKernel
                          Tracking/TrkDetDescr/TrkDetElementBase
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkMaterialOnTrack
                          Tracking/TrkEvent/TrkMeasurementBase
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkEvent/TrkParticleBase
                          Tracking/TrkEvent/TrkPseudoMeasurementOnTrack
                          Tracking/TrkEvent/TrkRIO_OnTrack
                          Tracking/TrkEvent/TrkSegment
                          Tracking/TrkEvent/TrkSpacePoint
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkEvent/TrkV0Vertex
                          Tracking/TrkEvent/VxMultiVertex
                          Tracking/TrkEvent/VxVertex
                          Tracking/TrkEventCnv/TrkEventCnvTools
                          Tracking/TrkEventCnv/TrkEventTPCnv )

# Component(s) in the package:
atlas_add_poolcnv_library( TrkEventAthenaPoolPoolCnv
                           src/*.cxx
                           FILES TrkSpacePoint/SpacePointCollection.h TrkSpacePoint/SpacePointContainer.h TrkTrack/TrackCollection.h VxMultiVertex/MVFVxContainer.h VxVertex/VxContainer.h TrkV0Vertex/V0Container.h TrkSegment/SegmentCollection.h
                           TYPES_WITH_NAMESPACE Trk::SegmentCollection Trk::TrackParticleBaseCollection
                           LINK_LIBRARIES AthenaPoolUtilities StoreGateLib SGtests AthenaPoolCnvSvcLib AtlasDetDescr GaudiKernel TrkDetElementBase TrkSurfaces TrkMaterialOnTrack TrkMeasurementBase TrkParameters TrkParticleBase TrkPseudoMeasurementOnTrack TrkRIO_OnTrack TrkSegment TrkSpacePoint TrkTrack TrkV0Vertex VxMultiVertex VxVertex TrkEventTPCnv )

# Install files from the package:
atlas_install_headers( TrkEventAthenaPool )
atlas_install_joboptions( share/*.py )

