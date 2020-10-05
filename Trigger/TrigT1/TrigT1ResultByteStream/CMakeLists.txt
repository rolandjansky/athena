# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigT1ResultByteStream )

# External dependencies:
find_package( tdaq-common COMPONENTS CTPfragment eformat )

# Component(s) in the package:
atlas_add_library( TrigT1ResultByteStreamLib
                   src/*.cxx
                   PUBLIC_HEADERS TrigT1ResultByteStream
                   INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS}
                   LINK_LIBRARIES AthenaBaseComps AthenaKernel ByteStreamCnvSvcBaseLib ByteStreamCnvSvcLib ByteStreamData GaudiKernel L1TopoRDO TrigConfInterfaces TrigConfL1Data TrigT1Interfaces TrigT1Result xAODTrigger
                   PRIVATE_LINK_LIBRARIES ${TDAQ-COMMON_LIBRARIES} )

atlas_add_component( TrigT1ResultByteStream
                     src/components/*.cxx
                     LINK_LIBRARIES TrigT1ResultByteStreamLib )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py )
