################################################################################
# Package: TileDetDescr
################################################################################

# Declare the package name:
atlas_subdir( TileDetDescr )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Calorimeter/CaloIdentifier
                          Control/CLIDSvc
                          Control/SGTools
                          Database/AthenaPOOL/RDBAccessSvc
                          DetectorDescription/GeoModel/GeoModelKernel
                          DetectorDescription/Identifier
                          PRIVATE
                          Calorimeter/CaloDetDescr
                          DetectorDescription/AtlasDetDescr
                          GaudiKernel
                          TileCalorimeter/TileIdentifier )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )

# Component(s) in the package:
atlas_add_library( TileDetDescr
                   src/Tile*.cxx
                   PUBLIC_HEADERS TileDetDescr
                   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
                   LINK_LIBRARIES ${Boost_LIBRARIES} ${CORAL_LIBRARIES} CaloIdentifier SGTools GeoModelKernel Identifier CaloDetDescrLib
                   PRIVATE_LINK_LIBRARIES AtlasDetDescr GaudiKernel TileIdentifier )

