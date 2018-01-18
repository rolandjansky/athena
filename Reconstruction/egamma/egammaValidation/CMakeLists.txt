################################################################################
# Package: egammaValidation
################################################################################

# Declare the package name:
atlas_subdir( egammaValidation )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
			  GaudiKernel
			  AsgExternal/Asg_MCUtils
                          Control/AthToolSupport/AsgTools
			  Event/xAOD/xAODEgamma
                          Event/xAOD/xAODEventInfo
			  Event/xAOD/xAODTracking
			  PhysicsAnalysis/ElectronPhotonID/ElectronPhotonSelectorTools
			  PRIVATE
                          Control/AthenaBaseComps
                          Control/AthenaMonitoring 
			  TestPolicy )

atlas_add_library (ArtTestLib src/*.h src/*.ihh src/*.cxx src/components/*.cxx
   PUBLIC_HEADERS ArtTest
   INCLUDE_DIRS
   LINK_LIBRARIES GaudiKernel AthenaBaseComps AthenaMonitoringLib AsgTools xAODEventInfo xAODEgamma xAODTracking ElectronPhotonSelectorToolsLib)

# add the library itself:
atlas_add_component (ArtTest
   src/*.h src/*.ihh src/*.cxx src/components/*.cxx
   PUBLIC_HEADERS ArtTest
   INCLUDE_DIRS 
   LINK_LIBRARIES ArtTestLib GaudiKernel AthenaBaseComps AthenaMonitoringLib AsgTools xAODEventInfo xAODEgamma xAODTracking ElectronPhotonSelectorToolsLib)

# Install files from the package:
atlas_install_headers( egammaValidation )
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
atlas_install_scripts( share/egammaRTTInit.py )
# ART scripts:
atlas_install_scripts( test/*.sh test/*.py )

