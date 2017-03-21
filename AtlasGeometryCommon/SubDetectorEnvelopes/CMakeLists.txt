################################################################################
# Package: SubDetectorEnvelopes
################################################################################

# Declare the package name:
atlas_subdir( SubDetectorEnvelopes )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          DetectorDescription/AtlasDetDescr
                          GaudiKernel
                          PRIVATE
                          Control/AthenaBaseComps
                          Database/AthenaPOOL/RDBAccessSvc
                          DetectorDescription/GeoModel/GeoModelInterfaces )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( CLHEP )
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )

# Component(s) in the package:
atlas_add_component( SubDetectorEnvelopes
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} ${CORAL_LIBRARIES} ${CLHEP_LIBRARIES} AtlasDetDescr GaudiKernel AthenaBaseComps )

# Install files from the package:
atlas_install_headers( SubDetectorEnvelopes )
atlas_install_python_modules( python/*.py )

