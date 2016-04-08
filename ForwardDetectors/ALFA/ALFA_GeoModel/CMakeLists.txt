################################################################################
# Package: ALFA_GeoModel
################################################################################

# Declare the package name:
atlas_subdir( ALFA_GeoModel )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaKernel
                          Control/CLIDSvc
                          Control/SGTools
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Database/AthenaPOOL/RDBAccessSvc
                          DetectorDescription/GeoModel/GeoModelInterfaces
                          DetectorDescription/GeoModel/GeoModelKernel
                          DetectorDescription/GeoModel/GeoModelUtilities
                          ForwardDetectors/ALFA/ALFA_Geometry
                          PRIVATE
                          Control/StoreGate
                          GaudiKernel )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )

# Component(s) in the package:
atlas_add_component( ALFA_GeoModel
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} ${CORAL_LIBRARIES} AthenaKernel SGTools AthenaPoolUtilities GeoModelKernel GeoModelUtilities ALFA_Geometry StoreGateLib SGtests GaudiKernel )

# Install files from the package:
atlas_install_headers( ALFA_GeoModel )
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

