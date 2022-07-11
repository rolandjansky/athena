# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TauDQA )

# External dependencies:
find_package( ROOT COMPONENTS Core )

# Component(s) in the package:
atlas_add_component( TauDQA
   src/*.h src/*.cxx src/components/*.cxx
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} GaudiKernel AthenaBaseComps
   AthenaMonitoringLib TauAnalysisToolsLib TrkValHistUtils xAODJet xAODEgamma
   xAODTau xAODTruth xAODEventInfo )
atlas_install_python_modules( python/*.py )
atlas_install_runtime( scripts/*.py )
