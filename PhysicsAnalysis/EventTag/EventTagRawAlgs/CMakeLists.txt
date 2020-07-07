################################################################################
# Package: EventTagRawAlgs
################################################################################

# Declare the package name:
atlas_subdir( EventTagRawAlgs )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          GaudiKernel
                          Tracking/TrkEvent/TrkSegment
                          PRIVATE
                          Calorimeter/CaloEvent
                          Commission/CommissionEvent
                          Control/StoreGate
                          Event/xAOD/xAODCaloEvent
                          InnerDetector/InDetRawEvent/InDetBCM_RawData
                          InnerDetector/InDetRecEvent/InDetPrepRawData
                          LArCalorimeter/LArRecEvent
                          PhysicsAnalysis/EventTag/TagEvent
                          TileCalorimeter/TileEvent
                          TileCalorimeter/TileIdentifier
                          Tracking/TrkEvent/TrkSpacePoint
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkEvent/TrkTrackSummary
                          MagneticField/MagFieldElements
                          MagneticField/MagFieldConditions
			  )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_component( EventTagRawAlgs
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} AthenaBaseComps AthenaKernel GaudiKernel TrkSegment CaloEvent CommissionEvent StoreGateLib SGtests xAODCaloEvent InDetBCM_RawData InDetPrepRawData LArRecEvent TagEvent TileEvent TileIdentifier TrkSpacePoint TrkTrack TrkTrackSummary MagFieldElements MagFieldConditions )

# Install files from the package:
atlas_install_joboptions( share/*.py )

