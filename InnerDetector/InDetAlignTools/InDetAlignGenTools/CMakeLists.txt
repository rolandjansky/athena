################################################################################
# Package: InDetAlignGenTools
################################################################################

# Declare the package name:
atlas_subdir( InDetAlignGenTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/CxxUtils
                          Database/AthenaPOOL/AthenaPoolUtilities
                          DetectorDescription/GeoPrimitives
                          DetectorDescription/Identifier
                          Event/EventPrimitives
                          InnerDetector/InDetAlignEvent/InDetAlignTrkInfo
                          InnerDetector/InDetAlignTools/InDetAlignToolInterfaces
                          InnerDetector/InDetRecEvent/InDetPrepRawData
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkEventUtils
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkExtrapolation/TrkExInterfaces
                          Tracking/TrkTools/TrkToolInterfaces
                          Event/EventContainers
                          PRIVATE
                          Control/AthContainers
                          Database/RegistrationServices
                          DetectorDescription/AtlasDetDescr
                          DetectorDescription/DetDescrCond/DetDescrConditions
                          GaudiKernel
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry
			  InnerDetector/InDetDetDescr/PixelReadoutGeometry
			  InnerDetector/InDetDetDescr/SCT_ReadoutGeometry
			  InnerDetector/InDetDetDescr/TRT_ReadoutGeometry
                          Simulation/G4Sim/TrackRecord
                          Tracking/TrkEvent/TrkMeasurementBase
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkEvent/TrkPrepRawData
                          Tracking/TrkEvent/TrkRIO_OnTrack
                          Tracking/TrkEvent/TrkTrackSummary
                          Tracking/TrkEvent/TrkTruthData
                          Tracking/TrkFitter/TrkFitterInterfaces 
                          Generators/AtlasHepMC
                          )

# External dependencies:
find_package( CLHEP )
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )
find_package( Eigen )
find_package( HepPDT )

atlas_add_library( InDetAlignGenToolsLib
                   InDetAlignGenTools/*.h
                   INTERFACE
                   PUBLIC_HEADERS InDetAlignGenTools
                   LINK_LIBRARIES GaudiKernel CxxUtils Identifier AthenaKernel AthenaBaseComps InDetAlignTrkInfo EventPrimitives GeoPrimitives AthContainers TrkTrack AthenaPoolUtilities InDetPrepRawData InDetReadoutGeometry StoreGateLib TrkToolInterfaces InDetAlignToolInterfaces TrkEventUtils TrkEventPrimitives )


# Component(s) in the package:
atlas_add_component( InDetAlignGenTools
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${CORAL_INCLUDE_DIRS} ${HEPPDT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}  ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CORAL_LIBRARIES} ${HEPPDT_LIBRARIES} ${CLHEP_LIBRARIES} InDetAlignGenToolsLib AtlasHepMCLib ${EIGEN_LIBRARIES} TrkExInterfaces AtlasDetDescr DetDescrConditions InDetIdentifier PixelReadoutGeometry SCT_ReadoutGeometry TRT_ReadoutGeometry TrkMeasurementBase TrkParameters TrkPrepRawData TrkRIO_OnTrack TrkTrackSummary TrkTruthData TrkFitterInterfaces EventContainers TrackRecordLib RegistrationServicesLib )

