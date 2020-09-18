################################################################################
# Package: MuonPrepRawDataToxAOD
################################################################################

# Declare the package name:
atlas_subdir( MuonPrepRawDataToxAOD )

# External dependencies:
find_package( Eigen )

# Component(s) in the package:
atlas_add_component( MuonPrepRawDataToxAOD
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS} 
                     LINK_LIBRARIES ${EIGEN_LIBRARIES} AtlasHepMCLib GaudiKernel AthenaBaseComps GeoPrimitives Identifier EventPrimitives xAODTracking MuonIdHelpersLib MuonRDO MuonPrepRawData MuonRIO_OnTrack MuonRecToolInterfaces MuonSimData TrkEventPrimitives TrkToolInterfaces )

