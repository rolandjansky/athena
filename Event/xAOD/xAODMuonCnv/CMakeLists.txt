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
                          Control/CxxUtils
                          Event/xAOD/xAODPrimitives
                          GaudiKernel
                          Reconstruction/MuonIdentification/muonEvent
                          Reconstruction/Particle
			  Event/xAOD/xAODTruth )

# Component(s) in the package:
atlas_add_component( xAODMuonCnv
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthLinks xAODMuon MuonSegment TrkSegment AthenaBaseComps AthenaKernel xAODPrimitives GaudiKernel muonEvent Particle xAODTruth)

# Install files from the package:
atlas_install_headers( xAODMuonCnv )

