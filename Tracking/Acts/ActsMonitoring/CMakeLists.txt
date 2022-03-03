# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( ActsMonitoring )

# External dependencies:
find_package( Acts COMPONENTS Core )

atlas_add_component( ActsMonitoring
		     src/*.h
		     src/*.cxx
		     src/components/*.cxx
		     LINK_LIBRARIES ActsEDM
		     AthenaMonitoringKernelLib AthenaMonitoringLib )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )



