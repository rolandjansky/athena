################################################################################
# Package: AmdcMGM
################################################################################

# Declare the package name:
atlas_subdir( AmdcMGM )

# Component(s) in the package:
atlas_add_library( AmdcMGMLib
                   src/*.cxx
                   PUBLIC_HEADERS AmdcMGM
                   LINK_LIBRARIES AthenaBaseComps GeoPrimitives GaudiKernel MuonReadoutGeometry AmdcAthLib MuonIdHelpersLib
                   PRIVATE_LINK_LIBRARIES Identifier )

atlas_add_component( AmdcMGM
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GeoPrimitives GaudiKernel AmdcAthLib MuonReadoutGeometry MuonIdHelpersLib Identifier AmdcMGMLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )

