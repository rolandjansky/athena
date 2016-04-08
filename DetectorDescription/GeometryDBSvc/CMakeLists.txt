################################################################################
# Package: GeometryDBSvc
################################################################################

# Declare the package name:
atlas_subdir( GeometryDBSvc )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Database/AthenaPOOL/RDBAccessSvc
                          GaudiKernel
                          PRIVATE
                          Control/AthenaBaseComps )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )

# Component(s) in the package:
atlas_add_component( GeometryDBSvc
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} ${CORAL_LIBRARIES} GaudiKernel AthenaBaseComps )

# Install files from the package:
atlas_install_headers( GeometryDBSvc )

