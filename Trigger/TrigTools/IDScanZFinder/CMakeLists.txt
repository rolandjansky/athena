# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( IDScanZFinder )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_component( IDScanZFinder
                     src/*.cxx src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} AthenaBaseComps AthenaMonitoringKernelLib GaudiKernel IRegionSelector TrigCompositeUtilsLib TrigInDetEvent TrigInDetToolInterfacesLib xAODTrigger xAODTracking xAODTrackingCnvLib)
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} --extend-select=ATL900,ATL901 )   