################################################################################
# Package: MuonFastDigitization
################################################################################

# Declare the package name:
atlas_subdir( MuonFastDigitization )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( MuonFastDigitization
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AthenaBaseComps AthenaKernel StoreGateLib SGtests Identifier GaudiKernel MuonAGDDDescription MuonReadoutGeometry MuonIdHelpersLib MuonPrepRawData MuonRecToolInterfaces MuonSimData MuonSimEvent PathResolver TrkSurfaces TrkEventPrimitives )

