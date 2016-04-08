################################################################################
# Package: CoraCool
################################################################################

# Declare the package name:
atlas_subdir( CoraCool )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( COOL COMPONENTS CoolKernel CoolApplication )
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# tag NEEDS_CORAL_RELATIONAL_ACCESS was not recognized in automatic conversion in cmt2cmake

# Component(s) in the package:
atlas_add_library( CoraCool
                   src/CoraCoolDatabaseSvcFactory.cxx
                   src/CoraCoolDatabaseSvc.cxx
                   src/CoraCoolDatabase.cxx
                   src/CoraCoolSequence.cxx
                   src/CoraCoolFolder.cxx
                   src/CoraCoolObject.cxx
                   src/CoraCoolObjectIter.cxx
                   PUBLIC_HEADERS CoraCool
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${COOL_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} ${Boost_LIBRARIES} ${COOL_LIBRARIES} ${CORAL_LIBRARIES} )

