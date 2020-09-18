################################################################################
# Package: CscOverlay
################################################################################

# Declare the package name:
atlas_subdir( CscOverlay )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_component( CscOverlay
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} AthenaKernel GaudiKernel MuonRDO StoreGateLib SGtests MuonIdHelpersLib CscCalibToolsLib MuonCSC_CnvToolsLib )

# Install files from the package:
atlas_install_headers( CscOverlay )
atlas_install_python_modules( python/*.py )
