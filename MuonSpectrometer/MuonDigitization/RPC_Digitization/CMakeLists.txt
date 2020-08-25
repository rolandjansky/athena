################################################################################
# Package: RPC_Digitization
################################################################################

# Declare the package name:
atlas_subdir( RPC_Digitization )

# External dependencies:
find_package( AIDA )
find_package( Boost COMPONENTS filesystem thread system )
find_package( CLHEP )
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )

# Component(s) in the package:
atlas_add_component( RPC_Digitization
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${AIDA_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} 
                     LINK_LIBRARIES ${Boost_LIBRARIES} ${CORAL_LIBRARIES} ${CLHEP_LIBRARIES} AtlasHepMCLib AthenaBaseComps PileUpToolsLib xAODEventInfo GaudiKernel MuonSimData MuonSimEvent HitManagement AthenaKernel EventInfo GeneratorObjects MuonCondInterface MuonCondData MuonReadoutGeometry MuonDigitContainer MuonIdHelpersLib PathResolver GeometryDBSvcLib EventInfoMgtLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/RPC_Digitization_jobOptions.txt share/*.py )
atlas_install_runtime( share/G4RPC_Digitizer.txt share/PermanentDeadPanels.txt share/PermanentGoodPanels.txt )

