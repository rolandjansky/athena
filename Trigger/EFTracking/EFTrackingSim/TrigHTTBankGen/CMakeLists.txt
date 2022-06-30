# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigHTTBankGen )

# External dependencies:
find_package( Boost )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO Matrix )

# Declare Gaudi component(s) in the package
atlas_add_component( TrigHTTBankGen
   src/*.cxx
   src/components/*.cxx
   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} AthenaBaseComps GaudiKernel TrigHTTBanksLib TrigHTTConfToolsLib TrigHTTHoughLib TrigHTTInputLib TrigHTTMapsLib TrigHTTObjectsLib TrigHTTSGInputLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_runtime( scripts/*_tf.py )


