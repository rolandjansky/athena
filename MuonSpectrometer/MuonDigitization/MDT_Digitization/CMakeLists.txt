################################################################################
# Package: MDT_Digitization
################################################################################

# Declare the package name:
atlas_subdir( MDT_Digitization )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_component( MDT_Digitization
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} AtlasHepMCLib AthenaBaseComps AthenaKernel PileUpToolsLib StoreGateLib SGtests Identifier xAODEventInfo GaudiKernel GeneratorObjects MdtCalibData MdtCalibSvcLib MuonCondInterface MuonCondData MuonReadoutGeometry MuonDigitContainer MDT_Response MuonIdHelpersLib MuonSimData MuonSimEvent HitManagement PathResolver TrkDetDescrUtils EventPrimitives )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
atlas_install_runtime( share/*.rt )

