################################################################################
# Package: CscOverlay
################################################################################

# Declare the package name:
atlas_subdir( CscOverlay )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaKernel
                          GaudiKernel
                          MuonSpectrometer/MuonCalib/CscCalib/CscCalibTools
                          MuonSpectrometer/MuonCnv/MuonCSC_CnvTools
                          MuonSpectrometer/MuonOverlay/MuonOverlayBase
                          MuonSpectrometer/MuonRDO
                          PRIVATE
                          Control/StoreGate
                          MuonSpectrometer/MuonDigitization/MuonDigToolInterfaces
                          MuonSpectrometer/MuonIdHelpers )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_component( CscOverlay
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} AthenaKernel GaudiKernel MuonOverlayBase MuonRDO StoreGateLib SGtests MuonIdHelpersLib )

# Install files from the package:
atlas_install_headers( CscOverlay )
atlas_install_joboptions( share/*.py )
