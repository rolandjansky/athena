################################################################################
# Package: egammaPerformance
################################################################################

# Declare the package name:
atlas_subdir( egammaPerformance )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaMonitoring
                          Control/StoreGate
                          Database/CoraCool
                          Event/xAOD/xAODEgamma
                          Event/xAOD/xAODEventInfo
                          GaudiKernel
                          Trigger/TrigAnalysis/TrigDecisionTool
                          Trigger/TrigEvent/TrigParticle
                          Trigger/TrigEvent/TrigSteeringEvent )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread MathMore Minuit Minuit2 Matrix Physics HistPainter Rint Graf Graf3d Gpad Html Postscript Gui GX11TTF GX11 )

atlas_add_component( egammaPerformance
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaMonitoringLib StoreGateLib SGtests xAODEgamma xAODEventInfo GaudiKernel TrigDecisionToolLib TrigParticle TrigSteeringEvent )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
atlas_install_runtime( share/egammaPerformance_hist.C share/egammaPerformance_ID_hist.C share/egammaPerformanceHistsToPostScript.C share/egammaPerformanceRunMacros.C )

