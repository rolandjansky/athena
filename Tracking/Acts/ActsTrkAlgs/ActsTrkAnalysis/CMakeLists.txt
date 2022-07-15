# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( ActsTrkAnalysis )

# Component(s) in the package:
atlas_add_component( ActsTrkAnalysis
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps AthenaKernel GaudiKernel Identifier InDetIdentifier InDetReadoutGeometry StoreGateLib ActsTrkEvent ActsUtilsLib xAODInDetMeasurement
		     AthenaMonitoringKernelLib AthenaMonitoringLib TrkTruthData ActsTrkToolInterfacesLib
		     PixelReadoutGeometryLib ReadoutGeometryBase InDetReadoutGeometry SCT_ReadoutGeometry )

atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
