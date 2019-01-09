################################################################################
# Package: TrigFTK_RawDataAlgs
################################################################################

# Declare the package name:
atlas_subdir( TrigFTK_RawDataAlgs )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/PileUpTools
                          GaudiKernel
                          InnerDetector/InDetConditions/InDetCondServices
                          Tools/PyJobTransforms
                          Tracking/TrkEvent/TrkEventPrimitives
                          Trigger/TrigFTK/FTK_DataProviderInterfaces
                          Trigger/TrigFTK/TrigFTKPool
                          Trigger/TrigFTK/TrigFTKSim
                          Trigger/TrigFTK/TrigFTK_RawData
                          PRIVATE
                          DetectorDescription/AtlasDetDescr
                          Event/EventInfo
                          Event/xAOD/xAODTracking
                          Event/xAOD/xAODTruth
                          InnerDetector/InDetRecEvent/InDetRIO_OnTrack
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkEvent/TrkTrackSummary 
                          Trigger/TrigFTK/FTK_RecToolInterfaces)

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( TrigFTK_RawDataAlgsLib
                   src/*.cxx
                   PUBLIC_HEADERS TrigFTK_RawDataAlgs
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps GaudiKernel TrkEventPrimitives FTK_DataProviderInterfaces TrigFTKPool TrigFTK_RawData PileUpToolsLib TrigFTKSimLib
                   PRIVATE_LINK_LIBRARIES AtlasDetDescr EventInfo xAODTracking xAODTruth InDetIdentifier InDetReadoutGeometry TrkTrack TrkTrackSummary VxVertex )

atlas_add_component( TrigFTK_RawDataAlgs
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps PileUpToolsLib GaudiKernel TrkEventPrimitives  FTK_DataProviderInterfaces TrigFTKPool TrigFTKSimLib TrigFTK_RawData AtlasDetDescr EventInfo xAODTracking  xAODTruth InDetIdentifier InDetReadoutGeometry TrkTrack TrkTrackSummary VxVertex TrigFTK_RawDataAlgsLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

