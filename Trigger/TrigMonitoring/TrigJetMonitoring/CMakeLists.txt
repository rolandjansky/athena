# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigJetMonitoring )

atlas_add_library( TrigJetMonitoringLib
                   src/*.cxx
                   PUBLIC_HEADERS TrigJetMonitoring
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} 
		   LINK_LIBRARIES AthenaBaseComps AthenaKernel GaudiKernel xAODTrigger AthenaMonitoringKernelLib AthenaMonitoringLib)

		   
atlas_add_component( TrigJetMonitoring
                     src/components/*.cxx
		     LINK_LIBRARIES AthenaKernel GaudiKernel TrigJetMonitoringLib)
# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
