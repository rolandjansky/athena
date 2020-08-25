################################################################################
# Package: InDetCaloClusterROIBuilder
################################################################################

# Declare the package name:
atlas_subdir( InDetCaloClusterROIBuilder )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Calorimeter/CaloDetDescr
                          Calorimeter/CaloTrackingGeometry
                          Control/AthenaBaseComps
                          Event/xAOD/xAODCaloEvent
                          Event/xAOD/xAODEgamma
                          GaudiKernel
                          InnerDetector/InDetRecTools/InDetRecToolInterfaces
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkCaloClusterROI
                          Tracking/TrkEvent/TrkEventPrimitives )

# Component(s) in the package:
atlas_add_component( InDetCaloClusterROIBuilder
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES CaloDetDescrLib AthenaBaseComps xAODCaloEvent xAODEgamma GaudiKernel InDetRecToolInterfaces TrkSurfaces TrkCaloClusterROI TrkEventPrimitives CaloTrackingGeometryLib )
