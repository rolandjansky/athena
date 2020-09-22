################################################################################
# Package: xAODMuonCnv
################################################################################

# Declare the package name:
atlas_subdir( xAODMuonCnv )

atlas_add_library( xAODMuonCnvLib
                   xAODMuonCnv/*.h
                   INTERFACE
                   PUBLIC_HEADERS xAODMuonCnv
                   LINK_LIBRARIES GaudiKernel xAODMuon MuonSegment TrkSegment AthLinks )

# Component(s) in the package:
atlas_add_component( xAODMuonCnv
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES xAODMuonCnvLib AthenaBaseComps AthenaKernel xAODPrimitives muonEvent Particle xAODTruth)


