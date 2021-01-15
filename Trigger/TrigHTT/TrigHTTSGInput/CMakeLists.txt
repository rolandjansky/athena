################################################################################
# Package: TrigHTTSGInput
################################################################################

# Declare the package name:
atlas_subdir( TrigHTTSGInput )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Generators/GeneratorObjects
                          InnerDetector/InDetConditions/InDetConditionsSummaryService
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          InnerDetector/InDetRecEvent/InDetPrepRawData
                          InnerDetector/InDetRecEvent/InDetRIO_OnTrack                          
                          Tracking/TrkExtrapolation/TrkExInterfaces
                          Tracking/TrkTools/TrkToolInterfaces
                          Tracking/TrkTools/TrkTrackSummaryTool
			  Tracking/TrkFitter/TrkFitterInterfaces
                          Tracking/TrkEvent/TrkRIO_OnTrack
                          Tracking/TrkEvent/TrkSpacePoint
                          Tracking/TrkEvent/VxVertex
			  Tracking/TrkEvent/TrkTrack
                          Tracking/TrkEvent/TrkTruthData
                          PRIVATE
			  Trigger/TrigHTT/TrigHTTObjects
			  Control/StoreGate
                          DetectorDescription/AtlasDetDescr
                          DetectorDescription/IdDict
                          DetectorDescription/IdDictDetDescr
                          DetectorDescription/Identifier
                          DetectorDescription/ReadoutGeometryBase
                          Event/EventInfo
                          Event/EventPrimitives
                          GaudiKernel
                          InnerDetector/InDetConditions/InDetBeamSpotService
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry
                          InnerDetector/InDetDetDescr/PixelCabling
                          InnerDetector/InDetDetDescr/SCT_Cabling
                          InnerDetector/InDetRawEvent/InDetRawData
                          InnerDetector/InDetRawEvent/InDetSimData
			 )

# External dependencies:
find_package( Boost COMPONENTS iostreams filesystem thread system )
find_package( HepMC )
find_package( HepPDT )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )


# Component(s) in the package:
atlas_add_library( TrigHTTSGInputLib
                   src/*.cxx TrigHTTSGInput/*.h ${TrigHTTSGInputLibDictSource}
                   PUBLIC_HEADERS TrigHTTSGInput
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${HEPPDT_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} ${Boost_LIBRARIES}
		   AthenaBaseComps GeneratorObjects InDetIdentifier
		   InDetPrepRawData TrkTrack TrkTruthData
		   TrkToolInterfaces TrigCaloEvent
		   TrigHTTObjectsLib 
		   TrigDecisionToolLib StoreGateLib SGtests SCT_CablingLib TrkExInterfaces
                   PRIVATE_LINK_LIBRARIES ${HEPPDT_LIBRARIES}
		   ${HEPMC_LIBRARIES} AtlasDetDescr IdDict
		   IdDictDetDescr Identifier
		   EventInfo EventPrimitives GaudiKernel
		   InDetReadoutGeometry InDetRawData InDetSimData
		   TileIdentifier TrkRIO_OnTrack ReadoutGeometryBase
		   TrkSpacePoint VxVertex TrkFitterInterfaces PixelCablingLib InDetRIO_OnTrack )

atlas_add_component( TrigHTTSGInput
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${HEPPDT_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES}
                     ${Boost_LIBRARIES} ${HEPPDT_LIBRARIES}
                     ${HEPMC_LIBRARIES} AthenaBaseComps
                     GeneratorObjects InDetIdentifier InDetPrepRawData
                     TrkTrack TrkTruthData TrkToolInterfaces
                     TrigDecisionToolLib TrigCaloEvent
		     TrigHTTObjectsLib 
		     StoreGateLib
                     SGtests AtlasDetDescr IdDict IdDictDetDescr
                     Identifier EventInfo
		     EventPrimitives GaudiKernel InDetReadoutGeometry
                     SCT_CablingLib InDetRawData InDetSimData
		     TileIdentifier TrkRIO_OnTrack TrkSpacePoint
                     VxVertex ReadoutGeometryBase
		     TrkFitterInterfaces TrkExInterfaces TrigHTTSGInputLib )

# Install files from the package:
atlas_install_joboptions( share/*jobOptions*.py )

# unit tests

