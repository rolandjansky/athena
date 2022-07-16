# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigBtagEmulationTool )

find_package( ROOT COMPONENTS Core )

atlas_add_library(TrigBtagEmulationToolLib
		  INTERFACE
		  PUBLIC_HEADERS TrigBtagEmulationTool
		  LINK_LIBRARIES GaudiKernel
		  )

atlas_add_component(TrigBtagEmulationTool
		      src/*.cxx 
		      src/*.h 
		      src/components/*.cxx
		    INCLUDE_DIRS 
		      ${ROOT_INCLUDE_DIRS}
		    LINK_LIBRARIES 
		      ${ROOT_LIBRARIES}
		      TrigBtagEmulationToolLib
		      AthenaBaseComps
		      StoreGateLib
		      xAODJet
		      xAODBTagging
		      xAODTrigger
		      TrigDecisionToolLib
		      FlavorTagDiscriminants
		    )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_scripts( test/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
