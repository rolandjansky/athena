################################################################################
# Package: MuonGMdbObjects
################################################################################

# Declare the package name:
atlas_subdir( MuonGMdbObjects )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )

# Component(s) in the package:
atlas_add_library( MuonGMdbObjects
                   src/*.cxx
                   PUBLIC_HEADERS MuonGMdbObjects
                   LINK_LIBRARIES RDBAccessSvcLib AmdcDbLib
                   PRIVATE_INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
                   PRIVATE_LINK_LIBRARIES ${Boost_LIBRARIES} ${CORAL_LIBRARIES} )

