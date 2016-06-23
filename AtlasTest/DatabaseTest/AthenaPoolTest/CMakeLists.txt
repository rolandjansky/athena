################################################################################
# Package: AthenaPoolTest
################################################################################

# Declare the package name:
atlas_subdir( AthenaPoolTest )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          AtlasTest/DatabaseTest/AthenaPoolTestData
                          Calorimeter/CaloDetDescr
                          Calorimeter/CaloEvent
                          Calorimeter/CaloIdentifier
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/StoreGate
                          Database/AthenaPOOL/AthenaPoolUtilities
                          DetectorDescription/GeoModel/GeoModelInterfaces
                          DetectorDescription/GeoPrimitives
                          DetectorDescription/Identifier
                          Event/EventInfo
                          Event/EventInfoMgt
                          GaudiKernel
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry
                          InnerDetector/InDetRawEvent/InDetRawData
                          InnerDetector/InDetRecEvent/InDetCompetingRIOsOnTrack
                          InnerDetector/InDetRecEvent/InDetPrepRawData
                          InnerDetector/InDetRecEvent/InDetRIO_OnTrack
                          LArCalorimeter/LArRecEvent
                          TestPolicy
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkCompetingRIOsOnTrack
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkMaterialOnTrack
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkEvent/TrkVertexOnTrack )

# External dependencies:
find_package( Eigen )

# Component(s) in the package:
atlas_add_component( AthenaPoolTest
                     src/AthenaPoolTestDataReader.cxx
                     src/AthenaPoolTestDataWriter.cxx
                     src/AthenaPoolTestAttrWriter.cxx
                     src/AthenaPoolTestAttrReader.cxx
                     src/InDetRawDataFakeReader.cxx
                     src/InDetRawDataFakeWriter.cxx
                     src/LArCellContFakeWriter.cxx
                     src/LArCellContFakeReader.cxx
                     src/EventInfoWriter.cxx
                     src/EventInfoReader.cxx
                     src/PileUpEventInfoWriter.cxx
                     src/PileUpEventInfoReader.cxx
                     src/TrkTrackFakeReader.cxx
                     src/TrkTrackFakeWriter.cxx
                     src/FakeTrackBuilder.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${EIGEN_LIBRARIES} AthenaPoolTestData CaloDetDescrLib CaloEvent CaloIdentifier AthenaBaseComps AthenaKernel StoreGateLib SGtests AthenaPoolUtilities GeoPrimitives Identifier EventInfo GaudiKernel InDetIdentifier InDetReadoutGeometry InDetRawData InDetCompetingRIOsOnTrack InDetPrepRawData InDetRIO_OnTrack LArRecEvent TrkSurfaces TrkCompetingRIOsOnTrack TrkEventPrimitives TrkMaterialOnTrack TrkParameters TrkTrack TrkVertexOnTrack )

# Install files from the package:
atlas_install_headers( AthenaPoolTest )
atlas_install_joboptions( share/*.py )

