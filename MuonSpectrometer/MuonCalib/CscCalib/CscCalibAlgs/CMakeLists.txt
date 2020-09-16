################################################################################
# Package: CscCalibAlgs
################################################################################

# Declare the package name:
atlas_subdir( CscCalibAlgs )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( CscCalibAlgs
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps AthContainers StoreGateLib SGtests GaudiKernel CscCalibData CscCalibToolsLib MuonCSC_CnvToolsLib MuonCondInterface MuonCondData MuonReadoutGeometry MuonIdHelpersLib MuonRDO )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

