# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( ActsGeometry )

# External dependencies:
find_package( Acts COMPONENTS Core PluginJson )
find_package( Boost )
find_package( CLHEP )
find_package( GeoModelCore )
find_package( ROOT COMPONENTS Core Tree RIO )
find_package( TBB )

# Component(s) in the package:
atlas_add_library( ActsGeometryLib
                   src/ActsAlignmentStore.cxx
                   src/ActsDetectorElement.cxx
                   src/ActsLayerBuilder.cxx
                   src/ActsStrawLayerBuilder.cxx
                   src/ActsTrackingGeometrySvc.cxx
                   src/util/*.cxx
                   PUBLIC_HEADERS ActsGeometry
                   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${TBB_INCLUDE_DIRS}
                   LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} ${TBB_LIBRARIES}
                   ActsCore
                   ActsGeometryInterfacesLib
                   ActsInteropLib
                   ActsPluginJson
                   AthenaBaseComps
                   AthenaKernel
                   CaloDetDescrLib
                   EventInfo
                   GaudiKernel
                   GeoModelUtilities
                   GeoPrimitives
                   Identifier
                   InDetIdentifier
                   InDetReadoutGeometry
                   MagFieldConditions
                   PRIVATE_LINK_LIBRARIES
                   StoreGateLib
                   TRT_ReadoutGeometry
                   TrkGeometry
                   TrkSurfaces )

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
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${GEOMODELCORE_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} ${CLHEP_LIBRARIES} ${GEOMODELCORE_LIBRARIES} ${ROOT_LIBRARIES}
                     ActsGeometryInterfacesLib
                     ActsGeometryLib
                     ActsInteropLib
                     AthenaBaseComps
                     AthenaKernel
                     CaloDetDescrLib
                     EventInfo
                     GaudiKernel
                     StoreGateLib
                     TRT_ReadoutGeometry
                     TrkGeometry )

# Install files from the package:
atlas_install_joboptions( share/*.py )
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
