################################################################################
# Package: ALFA_Geometry
################################################################################

# Declare the package name:
atlas_subdir( ALFA_Geometry )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaKernel
                          Control/CLIDSvc
                          Control/SGTools
                          Database/AthenaPOOL/RDBAccessSvc
                          DetectorDescription/GeoModel/GeoModelInterfaces
                          DetectorDescription/GeoModel/GeoModelKernel
                          GaudiKernel
                          PRIVATE
                          Control/StoreGate
                          Tools/PathResolver )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( CLHEP )
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )

# Component(s) in the package:
atlas_add_library( ALFA_Geometry
                   src/*.cxx
                   PUBLIC_HEADERS ALFA_Geometry
                   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${Boost_LIBRARIES} ${CORAL_LIBRARIES} ${CLHEP_LIBRARIES} AthenaKernel SGTools GeoModelKernel GaudiKernel StoreGateLib SGtests
                   PRIVATE_LINK_LIBRARIES PathResolver )

# Install files from the package:
atlas_install_runtime( share/metrology/*.dat )

