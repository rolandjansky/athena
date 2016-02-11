################################################################################
# Package: CoralDB
################################################################################
# Author: Juerg Beringer <JBeringer@lbl.gov>
################################################################################

# Declare the package name:
atlas_subdir( CoralDB )

# External dependencies:
find_package( Boost COMPONENTS system thread program_options )
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )

# Component(s) in the package:
atlas_add_library( CoralDB
   src/Alias.cxx src/CompoundTag.cxx src/Connection.cxx src/Encodable.cxx
   src/CoralDB.cxx
   PUBLIC_HEADERS CoralDB
   INCLUDE_DIRS ${CORAL_INCLUDE_DIRS}
   LINK_LIBRARIES ${CORAL_LIBRARIES}
   PRIVATE_LINK_LIBRARIES ${Boost_SYSTEM_LIBRARY} ${Boost_THREAD_LIBRARY} )

atlas_add_executable( ccdb
   src/ccdb.cxx
   INCLUDE_DIRS ${CORAL_INCLUDE_DIRS}
   LINK_LIBRARIES ${CORAL_LIBRARIES} ${Boost_LIBRARIES} CoralDB )

atlas_add_executable( ccdb-migrate
   src/ccdb-migrate.cxx
   INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
   LINK_LIBRARIES ${Boost_LIBRARIES} CoralDB )
