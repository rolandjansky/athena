################################################################################
# Package: MuonCalibStandAloneBase
################################################################################

# Declare the package name:
atlas_subdir( MuonCalibStandAloneBase )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( MuonCalibStandAloneBaseLib
                   src/*.cxx
                   PUBLIC_HEADERS MuonCalibStandAloneBase
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES AthenaBaseComps GaudiKernel MdtCalibData MdtCalibFitters MuonCalibEventBase MuonCalibITools MuonCalibIdentifier StoreGateLib SGtests MuonCalibToolsLib MuonIdHelpersLib
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} Identifier MuonCalibMath MuonReadoutGeometry )

atlas_add_component( MuonCalibStandAloneBase
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps GaudiKernel MdtCalibData MdtCalibFitters MuonCalibEventBase MuonCalibITools MuonCalibIdentifier StoreGateLib SGtests Identifier MuonCalibToolsLib MuonCalibMath MuonReadoutGeometry MuonIdHelpersLib MuonCalibStandAloneBaseLib )

