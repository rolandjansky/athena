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
                          DetectorDescription/GeoModel/GeoModelKernel
                          DetectorDescription/GeoModel/GeoModelUtilities
                          GaudiKernel
                          MuonSpectrometer/MuonConditions/MuonCondGeneral/MuonCondInterface
                          MuonSpectrometer/MuonDetDescr/MuonReadoutGeometry
                          MuonSpectrometer/MuonGMdbObjects
                          PRIVATE
                          Control/CLIDSvc
                          Control/SGTools
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Database/AthenaPOOL/RDBAccessSvc
                          DetectorDescription/AGDD/AGDDKernel
                          DetectorDescription/IdDictDetDescr
                          MuonSpectrometer/MuonDetDescr/MuonAGDDDescription
                          MuonSpectrometer/MuonDetDescr/MuonDetDescrUtils
                          MuonSpectrometer/MuonIdHelpers )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( CLHEP )
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )

# Component(s) in the package:
atlas_add_library( MuonGeoModelLib
                   src/*.cxx
                   PUBLIC_HEADERS MuonGeoModel
                   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${CLHEP_LIBRARIES} AthenaKernel GeoModelKernel GeoModelUtilities GaudiKernel MuonCondInterface MuonReadoutGeometry MuonGMdbObjects StoreGateLib SGtests MuonIdHelpersLib
                   PRIVATE_LINK_LIBRARIES ${Boost_LIBRARIES} ${CORAL_LIBRARIES} SGTools AthenaPoolUtilities AGDDKernel IdDictDetDescr MuonAGDDDescription MuonDetDescrUtils )

atlas_add_component( MuonGeoModel
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} ${CORAL_LIBRARIES} ${CLHEP_LIBRARIES} AthenaKernel StoreGateLib SGtests GeoModelKernel GeoModelUtilities GaudiKernel MuonCondInterface MuonReadoutGeometry MuonGMdbObjects SGTools AthenaPoolUtilities AGDDKernel IdDictDetDescr MuonAGDDDescription MuonDetDescrUtils MuonIdHelpersLib MuonGeoModelLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )

