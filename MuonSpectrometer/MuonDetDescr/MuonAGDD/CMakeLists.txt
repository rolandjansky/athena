################################################################################
# Package: MuonAGDD
################################################################################

# Declare the package name:
atlas_subdir( MuonAGDD )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          DetectorDescription/AGDD/AGDDControl
                          GaudiKernel
                          PRIVATE
                          Control/StoreGate
                          Database/AthenaPOOL/RDBAccessSvc
                          DetectorDescription/AGDD/AGDD2GeoSvc
                          DetectorDescription/AGDD/AGDDKernel
                          DetectorDescription/AGDD/AGDDModel
                          DetectorDescription/GeoModel/GeoModelInterfaces
                          Event/EventInfo
                          Event/EventInfoMgt
                          MuonSpectrometer/MuonDetDescr/MuonAGDDBase
                          MuonSpectrometer/MuonDetDescr/MuonReadoutGeometry )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )

# Component(s) in the package:
atlas_add_component( MuonAGDD
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} ${CORAL_LIBRARIES} AGDDControl GaudiKernel StoreGateLib SGtests AGDDKernel AGDDModel EventInfo MuonAGDDBase MuonReadoutGeometry )

# Install files from the package:
atlas_install_headers( MuonAGDD )
atlas_install_python_modules( python/*.py )

