################################################################################
# Package: TrkNeutralParameters
################################################################################

# Declare the package name:
atlas_subdir( TrkNeutralParameters )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkParametersBase )

# installed library
atlas_add_library( TrkNeutralParameters
                   PUBLIC_HEADERS TrkNeutralParameters
                   LINK_LIBRARIES TrkSurfaces TrkParametersBase )

 #Executables for tests
atlas_add_executable( TrkNeutralParameters_testConstexprMethods
                      test/testChargeDefinition.cxx 
                      LINK_LIBRARIES TrkNeutralParameters)
