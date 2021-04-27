################################################################################
# Package: ICaloTrkMuIdTools
################################################################################

# Declare the package name:
atlas_subdir( ICaloTrkMuIdTools )

# Component(s) in the package:
atlas_add_library( ICaloTrkMuIdTools
                   PUBLIC_HEADERS ICaloTrkMuIdTools
                   LINK_LIBRARIES CaloEvent CaloIdentifier xAODCaloEvent xAODTracking GaudiKernel muonEvent TrkSurfaces TrkEventPrimitives TrkParameters TrkTrack CaloDetDescrLib TrkCaloExtension )
