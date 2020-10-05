################################################################################
# Package: ICaloTrkMuIdTools
################################################################################

# Declare the package name:
atlas_subdir( ICaloTrkMuIdTools )

find_package( onnxruntime )

# Component(s) in the package:
atlas_add_library( ICaloTrkMuIdTools
                   PUBLIC_HEADERS ICaloTrkMuIdTools
		   INCLUDE_DIRS ${ONNXRUNTIME_INCLUDE_DIRS}
		   LINK_LIBRARIES ${ONNXRUNTIME_LIBRARIES} CaloEvent CaloIdentifier xAODCaloEvent xAODTracking GaudiKernel muonEvent TrkSurfaces TrkEventPrimitives TrkParameters TrkTrack CaloDetDescrLib )
