################################################################################
# Package: MuonSimHitToPrdTest
################################################################################

# Declare the package name:
atlas_subdir( MuonSimHitToPrdTest )

# Component(s) in the package:
atlas_add_component( MuonSimHitToPrdTest
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps EventPrimitives GeoAdaptors Identifier GaudiKernel GeneratorObjects MuonReadoutGeometry MuonDigitContainer MuonIdHelpersLib MuonRDO MuonPrepRawData MuonSimEvent )

# Install files from the package:
atlas_install_headers( MuonSimHitToPrdTest )
atlas_install_joboptions( share/*.py )

