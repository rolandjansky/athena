################################################################################
# Package: IdDictDetDescr
################################################################################

# Declare the package name:
atlas_subdir( IdDictDetDescr )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/CLIDSvc
                          Control/SGTools
                          DetectorDescription/IdDict
                          PRIVATE
                          DetectorDescription/Identifier )

# Component(s) in the package:
atlas_add_library( IdDictDetDescr
                   src/IdDictManager.cxx
                   PUBLIC_HEADERS IdDictDetDescr
                   LINK_LIBRARIES SGTools IdDict
                   PRIVATE_LINK_LIBRARIES Identifier )

