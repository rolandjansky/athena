################################################################################
# Package: CaloCalibHitRec
################################################################################
 
# Declare the package name:
atlas_subdir( CaloCalibHitRec )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Calorimeter/CaloDmDetDescr
                          Calorimeter/CaloGeoHelpers
                          Calorimeter/CaloIdentifier
                          Calorimeter/CaloRec
                          Calorimeter/CaloSimEvent
			  Calorimeter/CaloUtils
                          Control/StoreGate
                          DetectorDescription/Identifier
                          Event/xAOD/xAODCaloEvent
			  Event/xAOD/xAODEgamma/xAODEgamma
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
find_package( ROOT COMPONENTS Core )

atlas_add_library ( CaloCalibHitRecLib
    		     src/*.cxx
		     PUBLIC_HEADERS CaloCalibHitRec
		     PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
		     LINK_LIBRARIES
		     PRIVATE_LINK_LIBRARIES  ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES}
		     AthenaBaseComps CaloDmDetDescr
		     CaloGeoHelpers CaloIdentifier CaloRecLib
		     CaloSimEvent CaloUtils CxxUtils StoreGateLib
    		     SGtests Identifier xAODCaloEvent GaudiKernel
    		     CaloDetDescrLib CaloEvent xAODEgamma AthenaKernel
    		     AtlasDetDescr LArRecEvent McParticleEvent
    		     TileEvent PathResolver  )
		     
atlas_add_component( CaloCalibHitRec
				    #src/*.cxx	
				    src/components/*.cxx
				    INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
		     		    LINK_LIBRARIES ${ROOT_LIBRARIES}
				    ${CLHEP_LIBRARIES} AthenaBaseComps
				    CaloDmDetDescr CaloGeoHelpers
				    CaloIdentifier CaloRecLib
				    CaloSimEvent CaloUtils CxxUtils
				    StoreGateLib SGtests Identifier
				    xAODCaloEvent GaudiKernel
				    CaloDetDescrLib CaloEvent
				    xAODEgamma AthenaKernel
				    AtlasDetDescr LArRecEvent
				    McParticleEvent TileEvent
				    PathResolver
				    CaloCalibHitRecLib )

# Install files from the package:
atlas_install_headers( CaloCalibHitRec )
atlas_install_joboptions( share/*.py )

