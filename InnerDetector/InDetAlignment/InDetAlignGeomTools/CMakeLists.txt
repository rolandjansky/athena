# $Id: CMakeLists.txt 769450 2016-08-23 11:00:12Z krasznaa $
################################################################################
# Package: InDetAlignGeomTools
################################################################################

# Declare the package name:
atlas_subdir( InDetAlignGeomTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Control/AthenaBaseComps
   Control/AthContainers
   Tracking/TrkAlignment/TrkAlignEvent
   Tracking/TrkAlignment/TrkAlignGenTools
   Tracking/TrkAlignment/TrkAlignInterfaces
   PRIVATE
   Database/AthenaPOOL/RDBAccessSvc
   DetectorDescription/GeoModel/GeoModelInterfaces
   DetectorDescription/GeoModel/GeoModelUtilities
   GaudiKernel
   InnerDetector/InDetDetDescr/InDetIdentifier
   InnerDetector/InDetDetDescr/InDetReadoutGeometry
   Tracking/TrkEvent/TrkRIO_OnTrack )

# External dependencies:
find_package( ROOT COMPONENTS Core Geom )

# Component(s) in the package:
atlas_add_library( InDetAlignGeomToolsLib
   InDetAlignGeomTools/*.h
   INTERFACE
   PUBLIC_HEADERS InDetAlignGeomTools
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps AthContainers
   TrkAlignEvent TrkAlignGenToolsLib )

atlas_add_component( InDetAlignGeomTools
   src/*.cxx src/components/*.cxx
   LINK_LIBRARIES GeoModelUtilities GaudiKernel InDetIdentifier
   InDetReadoutGeometry TrkRIO_OnTrack InDetAlignGeomToolsLib )
