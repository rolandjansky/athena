################################################################################
# Package: TrkGeometryAlpine
################################################################################

# Declare the package name:
atlas_subdir( TrkGeometryAlpine )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Tracking/TrkDetDescr/TrkGeometry
                          PRIVATE
                          Tracking/TrkDetDescr/TrkDetElementBase
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkParameters )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( TrkGeometryAlpine
	             src/*.cxx
		     PUBLIC_HEADERS TrkGeometryAlpine
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} TrkGeometry
		     PRIVATE_LINK_LIBRARIES TrkDetElementBase TrkSurfaces TrkParameters )

