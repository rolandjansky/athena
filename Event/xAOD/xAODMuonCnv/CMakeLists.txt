################################################################################
# Package: xAODMuonCnv
################################################################################

# Declare the package name:
atlas_subdir( xAODMuonCnv )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthLinks
                          Event/xAOD/xAODMuon
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonSegment
                          Tracking/TrkEvent/TrkSegment
                          PRIVATE
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Event/xAOD/xAODPrimitives
                          GaudiKernel
                          Reconstruction/MuonIdentification/muonEvent
                          Reconstruction/Particle
			  Event/xAOD/xAODTruth )

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


