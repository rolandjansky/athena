################################################################################
# Package: InDetPerformanceRTT
################################################################################

# Declare the package name:
atlas_subdir( InDetPerformanceRTT )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaMonitoring
                          Control/StoreGate
                          DetectorDescription/AtlasDetDescr
                          Event/xAOD/xAODEventInfo
                          GaudiKernel
                          Generators/GeneratorObjects
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry
                          TestPolicy
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkTools/TrkToolInterfaces
                          PRIVATE
                          DetectorDescription/GeoPrimitives
                          DetectorDescription/IdDictDetDescr
                          Event/EventPrimitives
                          InnerDetector/InDetRecEvent/InDetPrepRawData
                          InnerDetector/InDetRecEvent/InDetRIO_OnTrack
                          Reconstruction/Jet/JetEvent
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkTrackSummary
                          Tracking/TrkEvent/TrkTruthData
                          Tracking/TrkTools/TrkTrackSummaryTool )

# External dependencies:
find_package( Eigen )
find_package( HepMC )
find_package( HepPDT )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread MathMore Minuit Minuit2 Matrix Physics HistPainter Rint )

# Component(s) in the package:
atlas_add_component( InDetPerformanceRTT
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${HEPPDT_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${HEPPDT_LIBRARIES} ${EIGEN_LIBRARIES} ${HEPMC_LIBRARIES} AthenaMonitoringLib StoreGateLib AtlasDetDescr xAODEventInfo GaudiKernel GeneratorObjects InDetIdentifier InDetReadoutGeometry TrkTrack TrkToolInterfaces SGtests GeoPrimitives IdDictDetDescr EventPrimitives InDetPrepRawData InDetRIO_OnTrack JetEvent TrkEventPrimitives TrkTrackSummary TrkTruthData )

# Install files from the package:
atlas_install_headers( InDetPerformanceRTT )
atlas_install_joboptions( share/*.py )
atlas_install_runtime( test/InDetPerformanceRTT_TestConfiguration.xml scripts/make*.C scripts/IDPerf*.py ExtraFiles/ALL*.html )

