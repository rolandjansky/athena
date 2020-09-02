################################################################################
# Package: CaloCalibHitRec
################################################################################

# Declare the package name:
atlas_subdir( CaloCalibHitRec )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Calorimeter/CaloDmDetDescr
                          Calorimeter/CaloGeoHelpers
                          Calorimeter/CaloIdentifier
                          Calorimeter/CaloRec
                          Calorimeter/CaloSimEvent
                          Control/AthenaBaseComps
                          Control/StoreGate
                          DetectorDescription/Identifier
                          Event/xAOD/xAODCaloEvent
			  Event/xAOD/xAODTruth
                          Control/CxxUtils
                          GaudiKernel
                          PRIVATE
                          Calorimeter/CaloDetDescr
                          Calorimeter/CaloEvent
                          Control/AthenaKernel
                          DetectorDescription/AtlasDetDescr
                          LArCalorimeter/LArRecEvent
                          PhysicsAnalysis/TruthParticleID/McParticleEvent
                          TileCalorimeter/TileEvent
                          Tools/PathResolver )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_library( CaloCalibHitRecLib
                     src/*.cxx
		     PUBLIC_HEADERS CaloCalibHitRec
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} CaloDmDetDescr CaloGeoHelpers CaloIdentifier CaloRecLib CaloSimEvent AthenaBaseComps StoreGateLib SGtests Identifier xAODCaloEvent xAODTruth GaudiKernel CaloDetDescrLib CaloEvent AthenaKernel AtlasDetDescr LArRecEvent McParticleEvent TileEvent PathResolver CxxUtils )

# Component(s) in the package:
atlas_add_component( CaloCalibHitRec
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} CaloDmDetDescr CaloGeoHelpers CaloIdentifier CaloRecLib CaloSimEvent AthenaBaseComps StoreGateLib SGtests Identifier xAODCaloEvent xAODTruth GaudiKernel CaloDetDescrLib CaloEvent AthenaKernel AtlasDetDescr LArRecEvent McParticleEvent TileEvent PathResolver CxxUtils )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

