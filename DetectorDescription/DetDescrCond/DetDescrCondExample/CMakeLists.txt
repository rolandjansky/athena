# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( DetDescrCondExample )

# External dependencies:
find_package( ROOT COMPONENTS Hist Core RIO )

# Component(s) in the package:
atlas_add_component( DetDescrCondExample
                     src/ReadAlignTrans.cxx
                     src/ReadWriteCoolStrFile.cxx
                     src/CoolHistExample.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps AthenaKernel AthenaPoolUtilities CxxUtils DetDescrCondToolsLib DetDescrConditions GaudiKernel GeoPrimitives Identifier InDetReadoutGeometry )

# Install files from the package:
atlas_install_joboptions( share/*.py )
