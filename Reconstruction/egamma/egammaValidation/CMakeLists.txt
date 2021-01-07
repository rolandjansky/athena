# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( egammaValidation )

# add the Module/component library :
atlas_add_component ( egammmaValidation
	src/*.cxx 
	src/components/*.cxx 
	LINK_LIBRARIES AsgTools AthenaBaseComps EgammaAnalysisInterfacesLib GaudiKernel IsolationSelectionLib 
	MCTruthClassifierLib PATCoreAcceptLib xAODBase xAODEgamma xAODEventInfo 
	xAODTracking xAODTruth )

atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
# Install JO files from the package:
atlas_install_joboptions( share/*.py )
# ART shell executable scripts :
atlas_install_scripts( test/*.sh scripts/*py )

atlas_add_test( ut_egammaARTJob_test
	        SCRIPT test/ut_egammaARTJob_test.sh
		PROPERTIES TIMEOUT 1200
)
