
# Declare the package name:
atlas_subdir( ActsGeometry )

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
                          Control/AthenaBaseComps
                          AthenaKernel
                          DetectorDescription/GeoModel/GeoModelUtilities
                          DetectorDescription/GeoPrimitives
                          Event/EventInfo
                          GaudiKernel
                          MagFieldElements MagFieldConditions
                          Calorimeter/CaloDetDescr
                          Tracking/Acts/ActsGeometryInterfaces
                          Tracking/Acts/ActsInterop
                          Tracking/TrkDetDescr/TrkGeometry)

# External dependencies:
find_package( CLHEP )
find_package( Eigen )
find_package( Boost )
find_package( nlohmann_json )

find_package( Acts COMPONENTS Core JsonPlugin)

# Component(s) in the package:

atlas_add_library( ActsGeometryLib
                   src/ActsAlignmentStore.cxx
                   src/ActsDetectorElement.cxx
                   src/ActsLayerBuilder.cxx
                   src/ActsStrawLayerBuilder.cxx
                   src/ActsTrackingGeometrySvc.cxx
                   src/util/*.cxx
                   PUBLIC_HEADERS ActsGeometry
                   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS} ${BOOST_INCLUDE_DIRS}
                   LINK_LIBRARIES ${CLHEP_LIBRARIES} ${EIGEN_LIBRARIES}
                   AthenaKernel
                   ActsInteropLib
                   ActsGeometryInterfacesLib
                   ActsCore
                   ActsJsonPlugin
                   TrkGeometry
                   PixelReadoutGeometry
                   SCT_ReadoutGeometry
                   TRT_ReadoutGeometry
                   MagFieldElements MagFieldConditions)

atlas_add_component( ActsGeometry
                     src/ActsExtrapolationAlg.cxx
                     src/ActsWriteTrackingGeometry.cxx
                     src/ActsWriteTrackingGeometryTransforms.cxx
                     src/ActsExtrapolationTool.cxx
                     src/ActsMaterialMapping.cxx
                     src/ActsSurfaceMappingTool.cxx
                     #src/ActsVolumeMappingTool.cxx
                     src/ActsObjWriterTool.cxx
                     #src/ActsExCellWriterSvc.cxx
                     src/ActsMaterialStepConverterTool.cxx
                     src/ActsMaterialJsonWriterTool.cxx
                     src/ActsMaterialTrackWriterSvc.cxx
                     #src/GeomShiftCondAlg.cxx
                     src/ActsAlignmentCondAlg.cxx
                     src/NominalAlignmentCondAlg.cxx
                     src/ActsTrackingGeometryTool.cxx
                     src/ActsPropStepRootWriterSvc.cxx
                     src/ActsCaloTrackingVolumeBuilder.cxx
                     src/components/*.cxx
                     PUBLIC_HEADERS ActsGeometry
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS} ${BOOST_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} ${EIGEN_LIBRARIES}
                     EventInfo
                     CaloDetDescrLib
                     ActsInteropLib
                     ActsGeometryLib
                     ActsGeometryInterfacesLib
                     ActsCore
                     ActsJsonPlugin)

# Install files from the package:
atlas_install_joboptions( share/*.py )
atlas_install_python_modules( python/*.py )
