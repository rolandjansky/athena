################################################################################
# Package: DataModel
################################################################################

# Declare the package name:
atlas_subdir( DataModel )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthAllocators
                          Control/AthContainers
                          Control/AthLinks
                          Control/CxxUtils
                          Control/SGTools )

# Component(s) in the package:
atlas_add_library( DataModel
                   PUBLIC_HEADERS DataModel
                   LINK_LIBRARIES AthAllocators AthContainers AthLinks CxxUtils SGTools )

