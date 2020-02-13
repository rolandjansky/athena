################################################################################
# Package: MuonGeoModel
################################################################################

# Declare the package name:
atlas_subdir( MuonGeoModel )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaKernel
                          Control/StoreGate
                          DetectorDescription/GeoModel/GeoModelInterfaces
                          DetectorDescription/GeoModel/GeoModelUtilities
			              DetectorDescription/GeoPrimitives
                          GaudiKernel
                          MuonSpectrometer/MuonConditions/MuonCondGeneral/MuonCondInterface
                          MuonSpectrometer/MuonDetDescr/MuonReadoutGeometry
                          MuonSpectrometer/MuonGMdbObjects
                          PRIVATE
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Database/RDBAccessSvc
                          DetectorDescription/AGDD/AGDDKernel
                          DetectorDescription/IdDictDetDescr
                          MuonSpectrometer/MuonDetDescr/MuonAGDDDescription
                          MuonSpectrometer/MuonDetDescr/MuonDetDescrUtils
                          MuonSpectrometer/MuonIdHelpers )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )
find_package( Eigen )
find_package( GeoModelCore )

# Component(s) in the package:
atlas_add_library( MuonGeoModelLib
                   src/*.cxx
                   PUBLIC_HEADERS MuonGeoModel
                   PRIVATE_INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
                   LINK_LIBRARIES ${GEOMODELCORE_LIBRARIES} AthenaKernel GeoModelUtilities GaudiKernel MuonCondInterface MuonReadoutGeometry MuonGMdbObjects StoreGateLib SGtests MuonIdHelpersLib
                   PRIVATE_LINK_LIBRARIES ${Boost_LIBRARIES} ${CORAL_LIBRARIES} AthenaPoolUtilities AGDDKernel IdDictDetDescr MuonAGDDDescription MuonDetDescrUtils )

atlas_add_component( MuonGeoModel
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS} 
                     LINK_LIBRARIES ${Boost_LIBRARIES} ${CORAL_LIBRARIES} ${GEOMODELCORE_LIBRARIES} AthenaKernel StoreGateLib SGtests GeoModelUtilities GaudiKernel MuonCondInterface MuonReadoutGeometry MuonGMdbObjects AthenaPoolUtilities AGDDKernel IdDictDetDescr MuonAGDDDescription MuonDetDescrUtils MuonIdHelpersLib MuonGeoModelLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )

