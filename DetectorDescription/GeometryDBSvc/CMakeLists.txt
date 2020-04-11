################################################################################
# Package: GeometryDBSvc
################################################################################

# Declare the package name:
atlas_subdir( GeometryDBSvc )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Database/RDBAccessSvc
                          GaudiKernel
                          PRIVATE
                          Control/AthenaBaseComps )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )

atlas_add_library( GeometryDBSvcLib
                   INTERFACE
                   PUBLIC_HEADERS GeometryDBSvc
                   LINK_LIBRARIES RDBAccessSvcLib GaudiKernel )

# Component(s) in the package:
atlas_add_component( GeometryDBSvc
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} ${CORAL_LIBRARIES} GaudiKernel AthenaBaseComps GeometryDBSvcLib )

# Install files from the package:
atlas_install_headers( GeometryDBSvc )

