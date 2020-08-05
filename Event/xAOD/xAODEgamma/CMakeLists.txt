# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( xAODEgamma )

# External dependencies.
find_package( ROOT COMPONENTS Core GenVector )
find_package( xAODUtilities )

# Component(s) in the package:
atlas_add_library( xAODEgamma
   xAODEgamma/*.h xAODEgamma/versions/*.h Root/*.cxx
   PUBLIC_HEADERS xAODEgamma
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} AthContainers AthLinks GeoPrimitives
   EventPrimitives xAODBase xAODCaloEvent xAODCore xAODPrimitives xAODTracking
   xAODTruth )

atlas_add_xaod_smart_pointer_dicts(
   INPUT xAODEgamma/selection.xml
   OUTPUT _selectionFile
   CONTAINERS "xAOD::EgammaContainer_v1" "xAOD::ElectronContainer_v1"
              "xAOD::PhotonContainer_v1" )

atlas_add_dictionary( xAODEgammaDict
   xAODEgamma/xAODEgammaDict.h
   ${_selectionFile}
   LINK_LIBRARIES xAODCore xAODEgamma
   EXTRA_FILES Root/dict/*.cxx )

atlas_add_dictionary( xAODEgammaParametersDict
   xAODEgamma/xAODEgammaParametersDict.h
   xAODEgamma/selectionParameters.xml )

# Install files from the package.
atlas_install_python_modules( python/*.py )
