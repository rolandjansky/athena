################################################################################
# Package: TrigIDtrkMonitoring
################################################################################

# Declare the package name:
atlas_subdir( TrigIDtrkMonitoring )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Calorimeter/CaloEvent
                          Calorimeter/CaloGeoHelpers
                          Control/AthenaMonitoring
                          Control/StoreGate
                          GaudiKernel
                          Reconstruction/Particle
                          Reconstruction/RecoTools/RecoToolInterfaces
                          Reconstruction/egamma/egammaEvent
                          Reconstruction/egamma/egammaInterfaces
                          Trigger/TrigAnalysis/TrigDecisionTool
                          Trigger/TrigEvent/TrigCaloEvent
                          Trigger/TrigEvent/TrigInDetEvent
                          Trigger/TrigMonitoring/TrigHLTMonitoring
                          Trigger/TrigTools/TrigInDetToolInterfaces
                          PRIVATE
                          Event/EventInfo
                          Trigger/TrigEvent/TrigParticle
                          Trigger/TrigEvent/TrigSteeringEvent )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread Table MathMore Minuit Minuit2 Matrix Physics HistPainter Rint Graf Graf3d Gpad Html Postscript Gui GX11TTF GX11 )

# tag ROOTBasicLibs was not recognized in automatic conversion in cmt2cmake

# tag ROOTSTLDictLibs was not recognized in automatic conversion in cmt2cmake

# Component(s) in the package:
atlas_add_library( TrigIDtrkMonitoringLib
                   src/*.cxx
                   PUBLIC_HEADERS TrigIDtrkMonitoring
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} CaloEvent CaloGeoHelpers GaudiKernel Particle RecoToolInterfaces egammaEvent TrigCaloEvent TrigInDetEvent AthenaMonitoringLib StoreGateLib SGtests TrigDecisionToolLib TrigHLTMonitoringLib
                   PRIVATE_LINK_LIBRARIES EventInfo TrigParticle TrigSteeringEvent )

atlas_add_component( TrigIDtrkMonitoring
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} CaloEvent CaloGeoHelpers AthenaMonitoringLib StoreGateLib SGtests GaudiKernel Particle RecoToolInterfaces egammaEvent TrigDecisionToolLib TrigCaloEvent TrigInDetEvent TrigHLTMonitoringLib EventInfo TrigParticle TrigSteeringEvent TrigIDtrkMonitoringLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

