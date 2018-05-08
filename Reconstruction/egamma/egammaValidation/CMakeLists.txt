################################################################################
# Package: egammaValidation
################################################################################

# Declare the package name:
atlas_subdir( egammaValidation )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
	GaudiKernel
	AsgExternal/Asg_MCUtils
	Control/AthenaBaseComps
	Control/AthToolSupport/AsgTools
	Event/xAOD/xAODEgamma
	Event/xAOD/xAODEventInfo
	Event/xAOD/xAODTracking
        PhysicsAnalysis/Interfaces/EgammaAnalysisInterfaces
	TestPolicy 
	Tracking/TrkValidation/TrkValHistUtils) 

# add the Module/component library :
atlas_add_component (egammmaValidation
	src/*.cxx 
	src/components/*.cxx 
	INCLUDE_DIRS 
	LINK_LIBRARIES  GaudiKernel AthenaBaseComps AsgTools xAODEventInfo 
	xAODEgamma xAODTracking TrkValHistUtils AsgAnalysisInterfaces )

# Install JO/python files from the package:
atlas_install_joboptions( share/*.py )
# ART shell executable scripts :
atlas_install_scripts( scripts/*py )

