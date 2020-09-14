################################################################################
# Package: CscCalibTools
################################################################################

# Declare the package name:
atlas_subdir( CscCalibTools )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

atlas_add_library( CscCalibToolsLib
                   CscCalibTools/*.h
                   INTERFACE
                   PUBLIC_HEADERS CscCalibTools
                   LINK_LIBRARIES GaudiKernel )

# Component(s) in the package:
atlas_add_component( CscCalibTools
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} GaudiKernel AthenaBaseComps CxxUtils StoreGateLib SGtests MuonCondData MuonCondInterface CscCalibToolsLib )

