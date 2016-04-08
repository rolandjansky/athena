################################################################################
# Package: RDBAccessTest
################################################################################

# Declare the package name:
atlas_subdir( RDBAccessTest )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/MinimalRunTime
                          GaudiKernel
                          TestPolicy
                          PRIVATE
                          Database/AthenaPOOL/RDBAccessSvc )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )

# Component(s) in the package:
atlas_add_component( RDBAccessTest
                     src/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} ${CORAL_LIBRARIES} AthenaBaseComps GaudiKernel )

# Install files from the package:
atlas_install_headers( RDBAccessTest )
atlas_install_joboptions( share/*.py )

