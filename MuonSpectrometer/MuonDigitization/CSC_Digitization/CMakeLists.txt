################################################################################
# Package: CSC_Digitization
################################################################################

# Declare the package name:
atlas_subdir( CSC_Digitization )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_component( CSC_Digitization
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} AtlasHepMCLib AthenaBaseComps AthenaKernel PileUpToolsLib GaudiKernel MuonReadoutGeometry MuonDigitContainer MuonIdHelpersLib MuonSimData MuonSimEvent HitManagement StoreGateLib SGtests GeneratorObjects CscCalibToolsLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

