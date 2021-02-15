# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( egammaUtils )

# External dependencies:
find_package( ROOT COMPONENTS Core Hist RIO )

# Component(s in tne package:
atlas_add_library( egammaUtils
   egammaUtils/*.h Root/*.cxx
   PUBLIC_HEADERS egammaUtils
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} xAODCaloEvent xAODEgamma GeoPrimitives
   PRIVATE_LINK_LIBRARIES PathResolver )

atlas_add_dictionary( egammaUtilsDict
   egammaUtils/egammaUtilsDict.h
   egammaUtils/selection.xml
   LINK_LIBRARIES egammaUtils )
