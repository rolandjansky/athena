
# Declare the package name:
atlas_subdir( ActsPriVtxFinder )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          PRIVATE
                          Control/StoreGate
                          DetectorDescription/Identifier
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry
                          InnerDetector/InDetDetDescr/PixelReadoutGeometry
                          InnerDetector/InDetDetDescr/SCT_ReadoutGeometry
                          InnerDetector/InDetDetDescr/TRT_ReadoutGeometry
                          InnerDetector/InDetConditions/BeamSpotConditionsData
                          InnerDetector/InDetRecTools/InDetTrackSelectionTool
                          Control/AthenaBaseComps
                          AthenaKernel
                          DetectorDescription/GeoModel/GeoModelUtilities
                          DetectorDescription/GeoPrimitives
                          Event/EventInfo
                          GaudiKernel
                          MagFieldElements MagFieldConditions
                          Calorimeter/CaloDetDescr
                          Tracking/Acts/ActsInterop
                          Tracking/TrkEvent/TrkTrackLink
                          Tracking/TrkEvent/TrkLinks 
                          Event/xAOD/xAODCore
                          Event/xAOD/xAODTracking
                          Reconstruction/RecoTools/ITrackToVertex
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkParticleBase
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkEvent/VxVertex
                          Tracking/TrkVertexFitter/TrkVertexFitterInterfaces
                          InnerDetector/InDetRecTools/InDetRecToolInterfaces )

# External dependencies:
find_package( CLHEP )
find_package( Eigen )
find_package( Boost )

find_package( Acts COMPONENTS Core )

# Component(s) in the package:

atlas_add_library( ActsPriVtxFinderLib
                   PUBLIC_HEADERS ActsPriVtxFinder
                   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS} ${BOOST_INCLUDE_DIRS}
                   LINK_LIBRARIES ${CLHEP_LIBRARIES} ${EIGEN_LIBRARIES}
                   AthenaKernel
                   ActsInteropLib
                   ActsGeometryLib
                   ActsGeometryInterfacesLib
                   ActsCore
                   PixelReadoutGeometry
                   SCT_ReadoutGeometry
                   TRT_ReadoutGeometry
                   BeamSpotConditionsData
                   InDetTrackSelectionToolLib
                   TrkLinks
                   xAODCore 
                   xAODTracking 
                   ITrackToVertex 
                   TrkSurfaces 
                   TrkParticleBase 
                   TrkTrack 
                   VxVertex
                   TrkVertexFitterInterfaces
                   AthenaMonitoringKernelLib
                   InDetRecToolInterfaces 
                   MagFieldElements MagFieldConditions)

atlas_add_component( ActsPriVtxFinder
                     src/*.cxx
                     src/components/*.cxx
                     PUBLIC_HEADERS ActsPriVtxFinder
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS} ${BOOST_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} ${EIGEN_LIBRARIES} 
                     EventInfo
                     CaloDetDescrLib
                     ActsInteropLib 
                     ActsPriVtxFinderLib
                     ActsGeometryLib
                     ActsGeometryInterfacesLib
                     ActsCore
                     BeamSpotConditionsData
                     TrkLinks
                     xAODCore 
                     xAODTracking 
                     ITrackToVertex 
                     TrkSurfaces 
                     TrkParticleBase 
                     TrkTrack 
                     VxVertex
                     TrkVertexFitterInterfaces 
                     AthenaMonitoringKernelLib
                     InDetRecToolInterfaces )
