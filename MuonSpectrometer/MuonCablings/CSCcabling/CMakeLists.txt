################################################################################
# Package: CSCcabling
################################################################################

# Declare the package name:
atlas_subdir( CSCcabling )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )

# Component(s) in the package:
atlas_add_library( CSCcablingLib
                   src/*.cxx
                   PUBLIC_HEADERS CSCcabling
                   PRIVATE_INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
                   LINK_LIBRARIES AthenaBaseComps GaudiKernel
                   PRIVATE_LINK_LIBRARIES ${Boost_LIBRARIES} ${CORAL_LIBRARIES} GeoModelInterfaces GeometryDBSvcLib )

atlas_add_component( CSCcabling
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} ${CORAL_LIBRARIES} AthenaBaseComps GaudiKernel CSCcablingLib )

