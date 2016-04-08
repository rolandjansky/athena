################################################################################
# Package: MuonRdoToPrepData
################################################################################

# Declare the package name:
atlas_subdir( MuonRdoToPrepData )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel
                          MuonSpectrometer/MuonCnv/MuonCnvToolInterfaces
                          PRIVATE
                          Control/StoreGate
                          DetectorDescription/Identifier )

# Component(s) in the package:
atlas_add_component( MuonRdoToPrepData
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel StoreGateLib SGtests Identifier )

# Install files from the package:
atlas_install_headers( MuonRdoToPrepData )

