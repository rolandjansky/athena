# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigHTTAlgorithms )

# External dependencies:
find_package( ROOT COMPONENTS Core Hist RIO )
find_package( lwtnn )

# Component(s) in the package:
atlas_add_component( TrigHTTAlgorithms
   src/*.cxx
   src/components/*.cxx
   INCLUDE_DIRS   ${LWTNN_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${LWTNN_LIBRARIES} ${ROOT_LIBRARIES} AthenaBaseComps AthenaKernel CxxUtils GaudiKernel TrigHTTBanksLib TrigHTTConfToolsLib TrigHTTHoughLib TrigHTTInputLib TrigHTTLRTLib TrigHTTMapsLib TrigHTTObjectsLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*jobOptions*.py )
atlas_install_joboptions( share/skeleton.*.py )
atlas_install_runtime( scripts/*_tf.py )
