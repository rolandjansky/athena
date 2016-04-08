################################################################################
# Package: Qoot
################################################################################

# Declare the package name:
atlas_subdir( Qoot )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          graphics/Qat/QatDataAnalysis )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# this line failed automatic conversion in cmt2cmake :
# path_remove DYLD_LIBRARY_PATH "" host-darwin "$(<package>_cmtpath)/InstallArea/${CMTCONFIG}/lib/qatplugins" ;  path_append DYLD_LIBRARY_PATH "" host-darwin "$(<package>_cmtpath)/InstallArea/${CMTCONFIG}/lib/qatplugins"

# Component(s) in the package:
atlas_add_library( Qoot
                   src/*.cpp
                   NO_PUBLIC_HEADERS
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} QatDataAnalysis )

