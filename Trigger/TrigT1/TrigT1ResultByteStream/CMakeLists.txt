# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigT1ResultByteStream )

# External dependencies:
find_package( tdaq-common COMPONENTS CTPfragment eformat )

# Component(s) in the package:
atlas_add_library( TrigT1ResultByteStreamLib
                   TrigT1ResultByteStream/*.h
                   INTERFACE
                   PUBLIC_HEADERS TrigT1ResultByteStream
                   LINK_LIBRARIES ByteStreamData GaudiKernel )

atlas_add_component( TrigT1ResultByteStream
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS}
                     LINK_LIBRARIES ${TDAQ-COMMON_LIBRARIES} AthenaBaseComps AthenaKernel AthenaMonitoringKernelLib
                                    ByteStreamCnvSvcBaseLib ByteStreamData GaudiKernel L1TopoRDO StoreGateLib
                                    TrigConfData TrigT1Interfaces TrigT1Result TrigT1ResultByteStreamLib xAODTrigger )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
