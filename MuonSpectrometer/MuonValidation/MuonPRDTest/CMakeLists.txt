################################################################################
# Package: MuonPRDTest
################################################################################

# Declare the package name:
atlas_subdir( MuonPRDTest )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( MuonPRDTest
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} 
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AtlasHepMCLib GaudiKernel AthenaBaseComps AthenaKernel StoreGateLib SGtests Identifier 
		     xAODEventInfo GeneratorObjects MuonAGDDDescription MuonReadoutGeometry MuonDigitContainer MuonRDO MuonIdHelpersLib MuonPrepRawData MuonRIO_OnTrack 
		     MuonRecToolInterfaces MuonSimData MuonSimEvent TrkGeometry TrkSurfaces TrkParameters TrkExInterfaces TrackRecordLib)

# Install files from the package:
atlas_install_joboptions( share/*.py )
atlas_install_scripts( scripts/*.py )

