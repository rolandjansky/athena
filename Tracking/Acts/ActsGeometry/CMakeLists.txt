
# Declare the package name:
atlas_subdir( ActsGeometry )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          PRIVATE
                          Control/StoreGate
                          DetectorDescription/Identifier
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry
                          Control/AthenaBaseComps
                          AthenaKernel
                          DetectorDescription/GeoModel/GeoModelUtilities
                          DetectorDescription/GeoPrimitives
                          Event/EventInfo
                          GaudiKernel
                          MagneticField/MagFieldInterfaces
                          Tracking/Acts/ActsGeometryInterfaces
                          Tracking/Acts/ActsInterop )

# External dependencies:
find_package( CLHEP )
find_package( Eigen )
find_package( Boost )

find_package( Acts COMPONENTS Core )

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
                   ActsCore)

atlas_add_component( ActsGeometry
                     src/ActsExtrapolationAlg.cxx
                     src/ActsWriteTrackingGeometry.cxx
                     src/ActsExtrapolationTool.cxx
                     src/ActsObjWriterTool.cxx
                     #src/ActsExCellWriterSvc.cxx
                     #src/ActsMaterialTrackWriterSvc.cxx
                     #src/GeomShiftCondAlg.cxx
                     src/NominalAlignmentCondAlg.cxx
                     src/ActsTrackingGeometryTool.cxx
                     src/ActsPropStepRootWriterSvc.cxx
                     src/components/*.cxx
                     PUBLIC_HEADERS ActsGeometry
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS} ${BOOST_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} ${EIGEN_LIBRARIES} 
                     EventInfo
                     ActsInteropLib 
                     ActsGeometryLib
                     ActsGeometryInterfacesLib
                     ActsCore)

# Install files from the package:
atlas_install_headers( ActsGeometry )
atlas_install_joboptions( share/*.py )
atlas_install_python_modules( python/*.py )

