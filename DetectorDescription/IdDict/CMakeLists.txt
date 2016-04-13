################################################################################
# Package: IdDict
################################################################################

# Declare the package name:
atlas_subdir( IdDict )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          DetectorDescription/Identifier )

# Component(s) in the package:
atlas_add_library( IdDict
                   src/IdDictMgr.cxx
                   src/IdDictFieldImplementation.cxx
                   PUBLIC_HEADERS IdDict
                   LINK_LIBRARIES Identifier )

