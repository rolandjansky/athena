################################################################################
# Package: DBReplicaSvc
################################################################################

# Declare the package name:
atlas_subdir( DBReplicaSvc )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          PRIVATE
                          Control/AthenaBaseComps
                          Tools/PathResolver )

# External dependencies:
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )

atlas_add_library( DBReplicaSvcLib
                   INTERFACE
                   PUBLIC_HEADERS DBReplicaSvc
                   INCLUDE_DIRS ${CORAL_INCLUDE_DIRS}
                   LINK_LIBRARIES GaudiKernel )

# Component(s) in the package:
atlas_add_component( DBReplicaSvc
                     src/DBReplicaSvc.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${CORAL_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CORAL_LIBRARIES} GaudiKernel AthenaBaseComps PathResolver )

