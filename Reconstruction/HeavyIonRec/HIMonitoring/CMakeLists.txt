################################################################################
# Package: HIMonitoring
################################################################################

# Declare the package name:
atlas_subdir( HIMonitoring )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaMonitoring
                          Event/xAOD/xAODEgamma
                          Event/xAOD/xAODForward
                          Event/xAOD/xAODHIEvent
                          Event/xAOD/xAODMuon
                          Event/xAOD/xAODTracking
                          Event/xAOD/xAODTrigMinBias
                          ForwardDetectors/ZDC/ZdcAnalysis
			  GaudiKernel
                          PhysicsAnalysis/ElectronPhotonID/ElectronPhotonSelectorTools
                          PRIVATE
                          Tools/LWHists )

# External dependencies:
find_package( ROOT COMPONENTS MathCore Core Tree Hist RIO pthread Graf Graf3d Gpad Html Postscript Gui GX11TTF GX11 )

# Component(s) in the package:
atlas_add_library( HIMonitoringLib
                   src/*.cxx
                   PUBLIC_HEADERS HIMonitoring
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
		   LINK_LIBRARIES ${ROOT_LIBRARIES} xAODEgamma xAODForward xAODHIEvent xAODMuon xAODTracking xAODTrigMinBias GaudiKernel AthenaMonitoringLib ZdcAnalysisLib ElectronPhotonSelectorToolsLib
                   PRIVATE_LINK_LIBRARIES LWHists )

atlas_add_component( HIMonitoring
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
		     LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaMonitoringLib xAODEgamma xAODForward xAODHIEvent xAODMuon xAODTracking xAODTrigMinBias GaudiKernel ElectronPhotonSelectorToolsLib LWHists ZdcAnalysisLib HIMonitoringLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )

