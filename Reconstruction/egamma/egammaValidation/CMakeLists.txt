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
	TestPolicy 
	Tracking/TrkValidation/TrkValHistUtils) 

# Add a shared library
atlas_add_library (egammaValidationLib 
	egammaValidation/*.h  src/*.cxx 
	PUBLIC_HEADERS egammaValidation
	INCLUDE_DIRS
	PRIVATE_INCLUDE_DIRS
	LINK_LIBRARIES GaudiKernel AsgTools xAODEventInfo xAODEgamma xAODTracking ElectronPhotonSelectorToolsLib    
	PRIVATE_LINK_LIBRARIES AthenaBaseComps TrkValHistUtils)

# add the Module/component library :
atlas_add_component (egammmaValidation
	src/*.cxx src/components/*.cxx 
	INCLUDE_DIRS 
	LINK_LIBRARIES egammaValidationLib GaudiKernel AthenaBaseComps AsgTools xAODEventInfo 
	xAODEgamma xAODTracking ElectronPhotonSelectorToolsLib TrkValHistUtils)

# Install JO/python files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
# ART shell executable scripts :
atlas_install_scripts( test/*.sh)

