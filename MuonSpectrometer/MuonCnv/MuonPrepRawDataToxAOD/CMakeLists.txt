################################################################################
# Package: MuonPrepRawDataToxAOD
################################################################################

# Declare the package name:
atlas_subdir( MuonPrepRawDataToxAOD )

# Component(s) in the package:
atlas_add_component( MuonPrepRawDataToxAOD
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AtlasHepMCLib GaudiKernel AthenaBaseComps GeoPrimitives Identifier EventPrimitives xAODTracking MuonIdHelpersLib MuonRDO MuonPrepRawData MuonRIO_OnTrack MuonRecToolInterfaces MuonSimData TrkEventPrimitives TrkToolInterfaces )

