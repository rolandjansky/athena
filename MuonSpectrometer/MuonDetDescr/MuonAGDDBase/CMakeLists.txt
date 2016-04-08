################################################################################
# Package: MuonAGDDBase
################################################################################

# Declare the package name:
atlas_subdir( MuonAGDDBase )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          DetectorDescription/AGDD/AGDDControl
                          DetectorDescription/AGDD/AGDDKernel
                          GaudiKernel
                          MuonSpectrometer/MuonDetDescr/MuonAGDDDescription
                          PRIVATE
                          Control/StoreGate
                          DetectorDescription/AGDD/AGDDModel
                          DetectorDescription/GeoModel/GeoModelInterfaces
                          DetectorDescription/GeoModel/GeoModelKernel
                          MuonSpectrometer/MuonGeoModel )

# Component(s) in the package:
atlas_add_library( MuonAGDDBase
                   src/*.cxx
                   PUBLIC_HEADERS MuonAGDDBase
                   LINK_LIBRARIES AGDDControl AGDDKernel GaudiKernel MuonAGDDDescription StoreGateLib SGtests MuonGeoModelLib
                   PRIVATE_LINK_LIBRARIES AGDDModel GeoModelKernel )

