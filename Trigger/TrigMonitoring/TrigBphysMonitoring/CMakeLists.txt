################################################################################
# Package: TrigBphysMonitoring
################################################################################

# Declare the package name:
atlas_subdir( TrigBphysMonitoring )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Event/xAOD/xAODTracking
                          Trigger/TrigAnalysis/TrigDecisionTool
                          Trigger/TrigMonitoring/TrigHLTMonitoring
                          PRIVATE
                          Control/AthenaBaseComps
                          Control/AthenaMonitoring
                          Control/CxxUtils
                          Event/xAOD/xAODEventInfo
                          Event/xAOD/xAODMuon
                          Event/xAOD/xAODTrigBphys
                          Event/xAOD/xAODTrigMuon
                          GaudiKernel
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkEvent/TrkParametersBase
                          Tracking/TrkEvent/VxVertex
                          Tracking/TrkVertexFitter/TrkVertexAnalysisUtils
                          Trigger/TrigEvent/TrigMuonEvent
                          Trigger/TrigEvent/TrigParticle )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread MathMore Minuit Minuit2 Matrix Physics HistPainter Rint Graf Graf3d Gpad Html Postscript Gui GX11TTF GX11 )

# Component(s) in the package:
atlas_add_library( TrigBphysMonitoringLib
                   src/*.cxx
                   PUBLIC_HEADERS TrigBphysMonitoring
                   PRIVATE_INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES xAODTracking TrigDecisionToolLib TrigHLTMonitoringLib AthenaMonitoringLib TrkVertexAnalysisUtilsLib
                   PRIVATE_LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AthenaBaseComps CxxUtils xAODEventInfo xAODMuon xAODTrigBphys xAODTrigMuon GaudiKernel TrkParameters TrkParametersBase VxVertex TrigMuonEvent TrigParticle )

atlas_add_component( TrigBphysMonitoring
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} xAODTracking TrigDecisionToolLib TrigHLTMonitoringLib AthenaBaseComps AthenaMonitoringLib CxxUtils xAODEventInfo xAODMuon xAODTrigBphys xAODTrigMuon GaudiKernel TrkParameters TrkParametersBase VxVertex TrkVertexAnalysisUtilsLib TrigMuonEvent TrigParticle TrigBphysMonitoringLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

