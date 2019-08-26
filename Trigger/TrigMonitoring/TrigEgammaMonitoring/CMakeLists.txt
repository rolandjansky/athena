################################################################################
# Package: TrigEgammaMonitoring
################################################################################

# Declare the package name:
atlas_subdir( TrigEgammaMonitoring )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Trigger/TrigEvent/TrigCaloEvent
                          Trigger/TrigEvent/TrigInDetEvent
                          Trigger/TrigEvent/TrigParticle
                          Trigger/TrigEvent/TrigSteeringEvent
			  Control/AthenaMonitoring )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread MathMore Minuit Minuit2 Matrix Physics HistPainter Rint Graf Graf3d Gpad Html Postscript Gui GX11TTF GX11 )

atlas_add_component( TrigEgammaMonitoring
                     src/*.cxx src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} GaudiKernel AthenaMonitoringLib )

# Install files from the package:
atlas_install_headers( TrigEgammaMonitoring )
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

