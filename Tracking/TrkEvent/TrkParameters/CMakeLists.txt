################################################################################
# Package: TrkParameters
################################################################################

# Declare the package name:
atlas_subdir( TrkParameters )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkParametersBase )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

#Component(s) in the package:
atlas_add_library( TrkParameters
                   src/*.cxx
                   PUBLIC_HEADERS TrkParameters
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES TrkSurfaces TrkParametersBase
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} )

atlas_add_dictionary( TrkParametersDict
                      TrkParameters/TrkParametersDict.h
                      TrkParameters/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} TrkSurfaces TrkParametersBase TrkParameters )


#Executables for tests
atlas_add_executable( TrkParameters_testConstExprMethods
	              test/testConstExpr.cxx 
		      LINK_LIBRARIES TrkParameters)
#Executables for tests
atlas_add_test( ut_TrkParameters_testConstExprMethods
	        SCRIPT TrkParameters_testConstExprMethods)
