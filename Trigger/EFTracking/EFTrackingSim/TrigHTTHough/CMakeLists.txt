# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigHTTHough )

# External dependencies:
find_package( Boost )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( TrigHTTHoughLib
   src/*.cxx
   PUBLIC_HEADERS TrigHTTHough
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps GaudiKernel TrigHTTMapsLib TrigHTTObjectsLib
   PRIVATE_INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
   PRIVATE_LINK_LIBRARIES ${Boost_LIBRARIES} TrigHTTBanksLib TrigHTTConfToolsLib )

atlas_add_component( TrigHTTHough
   src/components/*.cxx
   LINK_LIBRARIES TrigHTTBanksLib TrigHTTHoughLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_scripts( scripts/Hough_plots.py )


