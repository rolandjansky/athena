################################################################################
# Package: MuonTGMeasAssocAlg
################################################################################

# Declare the package name:
atlas_subdir( MuonTGMeasAssocAlg )

# External dependencies:
find_package( Eigen )

# Component(s) in the package:
atlas_add_component( MuonTGMeasAssocAlg
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${EIGEN_LIBRARIES} AthenaBaseComps StoreGateLib SGtests GeoPrimitives GaudiKernel MuonReadoutGeometry MuonIdHelpersLib MuonPrepRawData TrkGeometry TrkSurfaces TrkPrepRawData TrkSegment TrkTrack TrkExInterfaces EventPrimitives MuonSegment TrkEventPrimitives TrkMeasurementBase TrkParameters MuonTGRecToolsLib )

# Install files from the package:
atlas_install_headers( MuonTGMeasAssocAlg )
atlas_install_joboptions( share/*.py )

