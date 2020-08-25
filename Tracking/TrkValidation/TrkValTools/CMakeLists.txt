################################################################################
# Package: TrkValTools
################################################################################

# Declare the package name:
atlas_subdir( TrkValTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/AthenaMonitoring
                          Control/AthContainers
                          Control/AthenaKernel
                          Event/xAOD/xAODTracking
                          GaudiKernel
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkTools/TrkToolInterfaces
                          Tracking/TrkValidation/TrkValEvent
                          Tracking/TrkValidation/TrkValInterfaces
                          InnerDetector/InDetRecTools/InDetTrackSelectionTool
                          GaudiKernel
                          Event/xAOD/xAODCore
                          Event/xAOD/xAODJet
                          Event/xAOD/xAODTruth
                          Event/xAOD/xAODEventInfo
                          Event/xAOD/xAODPFlow
                          Event/xAOD/xAODCaloEvent
                          InnerDetector/InDetRecTools/TrackVertexAssociationTool
                          Commission/CommissionEvent
                          PRIVATE
                          Commission/CommissionEvent
                          DetectorDescription/AtlasDetDescr
                          Control/AthToolSupport/AsgTools
                          Control/AthenaKernel
                          Event/EventInfo
                          Event/EventPrimitives
                          Generators/GeneratorObjects
                          Generators/AtlasHepMC
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkCompetingRIOsOnTrack
                          Tracking/TrkEvent/TrkEventUtils
                          Tracking/TrkEvent/TrkMaterialOnTrack
                          Tracking/TrkEvent/TrkMeasurementBase
                          Tracking/TrkEvent/TrkParticleBase
                          Tracking/TrkEvent/TrkRIO_OnTrack
                          Tracking/TrkEvent/TrkTrackSummary
                          Tracking/TrkEvent/TrkTruthData
                          Tracking/TrkExtrapolation/TrkExInterfaces
                          Tracking/TrkFitter/TrkFitterUtils
                          Tools/PathResolver)

# External dependencies:
find_package( CLHEP )
find_package( HepPDT )
find_package( ROOT COMPONENTS Core EG Tree MathCore Hist RIO pthread Graf Graf3d Gpad Html Postscript Gui GX11TTF GX11 )
find_package( Eigen )
find_package( XercesC )
find_package( Boost )

atlas_add_library( TrkValToolsLib
                   TrkValTools/*.h
                   INTERFACE
                   INCLUDE_DIRS ${HEPPDT_INCLUDE_DIRS}
                   PUBLIC_HEADERS TrkValTools
                   LINK_LIBRARIES GaudiKernel AthenaBaseComps TrkValInterfaces StoreGateLib xAODEventInfo TrkParameters TrkValEvent AtlasHepMCLib TrkEventPrimitives TrkTrack AthContainers )

# Component(s) in the package:
atlas_add_component( TrkValTools
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${HEPPDT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} 
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${HEPPDT_LIBRARIES} ${CLHEP_LIBRARIES} TrkValToolsLib AthenaKernel xAODTracking TrkToolInterfaces CommissionEvent AtlasDetDescr EventInfo EventPrimitives GeneratorObjects TrkSurfaces TrkCompetingRIOsOnTrack TrkEventUtils TrkMaterialOnTrack TrkMeasurementBase TrkParticleBase TrkRIO_OnTrack TrkTrackSummary TrkTruthData TrkExInterfaces TrkFitterUtils xAODTruth xAODJet xAODPFlow xAODCaloEvent xAODCore AsgTools AthenaKernel AthenaMonitoringLib)

