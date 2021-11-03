# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( OverlayByteStreamUtils )

# External dependencies:
find_package( ROOT COMPONENTS MathCore )

# Component(s) in the package:
atlas_add_component( OverlayByteStreamUtils
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps AthenaKernel ByteStreamCnvSvcLib GaudiKernel TrigConfData TrigConfInterfaces TrigDecisionToolLib )

# Utility scripts
atlas_install_runtime( scripts/*.py )
atlas_install_runtime( scripts/*.sh )

atlas_add_executable (LB_analyze util/LB_analyze.cxx
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} )
