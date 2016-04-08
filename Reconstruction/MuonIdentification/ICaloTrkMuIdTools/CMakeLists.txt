################################################################################
# Package: ICaloTrkMuIdTools
################################################################################

# Declare the package name:
atlas_subdir( ICaloTrkMuIdTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Calorimeter/CaloDetDescr
                          Calorimeter/CaloEvent
                          Calorimeter/CaloIdentifier
                          Event/xAOD/xAODCaloEvent
                          Event/xAOD/xAODTracking
                          GaudiKernel
                          Reconstruction/MuonIdentification/muonEvent
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkEvent/TrkTrack )

# Component(s) in the package:
atlas_add_library( ICaloTrkMuIdTools
                   PUBLIC_HEADERS ICaloTrkMuIdTools
                   LINK_LIBRARIES CaloEvent CaloIdentifier xAODCaloEvent xAODTracking GaudiKernel muonEvent TrkSurfaces TrkEventPrimitives TrkParameters TrkTrack CaloDetDescrLib )

