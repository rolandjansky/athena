# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( HGTD_ConditionsAlgorithms )

# Component(s) in the package:
atlas_add_component( HGTD_ConditionsAlgorithms
   src/*.h src/*.cxx src/components/*.cxx
   LINK_LIBRARIES AthenaKernel AthenaBaseComps GaudiKernel GeoPrimitives
   HGTD_ReadoutGeometry StoreGateLib TrkGeometry TrkSurfaces )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
