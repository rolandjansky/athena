################################################################################
# Package: FastTrackSimWrap
################################################################################

# Declare the package name:
atlas_subdir( FastTrackSimWrap )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Generators/GeneratorObjects
                          InnerDetector/InDetConditions/InDetConditionsSummaryService
                          InnerDetector/InDetConditions/InDetCondTools
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          InnerDetector/InDetRecEvent/InDetPrepRawData
                          InnerDetector/InDetRecEvent/InDetRIO_OnTrack
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkEvent/TrkTruthData
                          Tracking/TrkTools/TrkToolInterfaces
                          Tracking/TrkTools/TrkTrackSummaryTool
                          Trigger/TrigAnalysis/TrigDecisionTool
                          Trigger/TrigEvent/TrigCaloEvent
                          Trigger/TrigFTK/TrigFTKSim
                          Trigger/TrigTools/TrigFTKToolInterfaces
                          Trigger/TrigTools/TrigFTKTrackConverter
                          PRIVATE
                          Control/StoreGate
                          DetectorDescription/AtlasDetDescr
                          DetectorDescription/IdDict
                          DetectorDescription/IdDictDetDescr
                          DetectorDescription/Identifier
                          Event/EventInfo
                          Event/EventPrimitives
                          GaudiKernel
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry
                          InnerDetector/InDetDetDescr/PixelCabling
                          InnerDetector/InDetDetDescr/SCT_Cabling
                          InnerDetector/InDetRawEvent/InDetRawData
                          InnerDetector/InDetRawEvent/InDetSimData
                          TileCalorimeter/TileIdentifier
			  Tracking/TrkFitter/TrkFitterInterfaces
                          Tracking/TrkEvent/TrkRIO_OnTrack
                          Tracking/TrkEvent/TrkSpacePoint
                          Tracking/TrkEvent/VxVertex )

# External dependencies:
find_package( Boost COMPONENTS iostreams filesystem thread system )
find_package( HepMC )
find_package( HepPDT )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( FastTrackSimWrapLib
                   src/*.cxx
                   PUBLIC_HEADERS FastTrackSimWrap
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${HEPPDT_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} ${Boost_LIBRARIES} AthenaBaseComps GeneratorObjects InDetCondTools InDetIdentifier InDetPrepRawData TrkTrack TrkTruthData TrkToolInterfaces TrigCaloEvent TrigDecisionToolLib TrigFTKSimLib StoreGateLib SGtests SCT_CablingLib
                   PRIVATE_LINK_LIBRARIES ${HEPPDT_LIBRARIES} ${HEPMC_LIBRARIES} AtlasDetDescr IdDict IdDictDetDescr Identifier EventInfo EventPrimitives GaudiKernel InDetReadoutGeometry InDetRawData InDetSimData TileIdentifier TrkRIO_OnTrack TrkSpacePoint VxVertex TrkFitterInterfaces PixelCablingLib InDetRIO_OnTrack )

atlas_add_component( FastTrackSimWrap
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${HEPPDT_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${Boost_LIBRARIES} ${HEPPDT_LIBRARIES} ${HEPMC_LIBRARIES} AthenaBaseComps GeneratorObjects InDetCondTools InDetIdentifier InDetPrepRawData TrkTrack TrkTruthData TrkToolInterfaces TrigDecisionToolLib TrigCaloEvent TrigFTKSimLib StoreGateLib SGtests AtlasDetDescr IdDict IdDictDetDescr Identifier EventInfo EventPrimitives GaudiKernel InDetReadoutGeometry SCT_CablingLib InDetRawData InDetSimData TileIdentifier TrkRIO_OnTrack TrkSpacePoint VxVertex FastTrackSimWrapLib TrkFitterInterfaces )

# Install files from the package:
atlas_install_joboptions( share/*jobOptions*.py )

