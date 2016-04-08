################################################################################
# Package: InDetAlignGeomTools
################################################################################

# Declare the package name:
atlas_subdir( InDetAlignGeomTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/DataModel
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
find_package( Boost COMPONENTS filesystem thread system )
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( InDetAlignGeomTools
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} ${CORAL_LIBRARIES} AthenaBaseComps DataModel TrkAlignEvent TrkAlignGenToolsLib GeoModelUtilities GaudiKernel InDetIdentifier InDetReadoutGeometry TrkRIO_OnTrack )

# Install files from the package:
atlas_install_headers( InDetAlignGeomTools )

