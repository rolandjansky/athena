################################################################################
# Package: TrackRecord
################################################################################

# Declare the package name:
atlas_subdir( TrackRecord )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

atlas_add_library( TrackRecordLib
                   TrackRecord/*.h
                   INTERFACE
                   PUBLIC_HEADERS TrackRecord
                   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   LINK_LIBRARIES HitManagement AthenaKernel ${CLHEP_LIBRARIES} )

# Component(s) in the package:
atlas_add_dictionary( TrackRecordDict
                      TrackRecord/TrackRecordDict.h
                      TrackRecord/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AthenaKernel HitManagement TrackRecordLib )

