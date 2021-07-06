################################################################################
# Package: MuonCalibEvent
################################################################################

# Declare the package name:
atlas_subdir( MuonCalibEvent )

# Component(s) in the package:
atlas_add_library( MuonCalibEvent
                   src/MdtCalibHit.cxx
                   PUBLIC_HEADERS MuonCalibEvent
                   LINK_LIBRARIES AthContainers GeoPrimitives MuonCalibEventBase MuonPrepRawData
                   PRIVATE_LINK_LIBRARIES GaudiKernel MuonCalibITools )

