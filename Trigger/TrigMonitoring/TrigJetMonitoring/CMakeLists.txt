# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigJetMonitoring )

# Declare all components in the package.
atlas_add_library( TrigJetMonitoringLib
   TrigJetMonitoring/*.h
   INTERFACE
   PUBLIC_HEADERS TrigJetMonitoring
   LINK_LIBRARIES GaudiKernel )

atlas_add_component( TrigJetMonitoring
   src/*.h src/*.cxx src/components/*.cxx
   LINK_LIBRARIES AthenaBaseComps AsgDataHandlesLib xAODTrigger
   AthenaMonitoringKernelLib AthenaMonitoringLib TrigJetMonitoringLib )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
