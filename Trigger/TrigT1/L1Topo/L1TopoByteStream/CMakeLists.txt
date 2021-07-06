# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( L1TopoByteStream )

# External dependencies:
find_package( tdaq-common COMPONENTS eformat_write )

# Component(s) in the package:
atlas_add_component( L1TopoByteStream
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS}
                     LINK_LIBRARIES ${TDAQ-COMMON_LIBRARIES} AthenaBaseComps AthenaKernel ByteStreamCnvSvcBaseLib ByteStreamData GaudiKernel L1TopoRDO StoreGateLib xAODTrigL1Calo )

# Install files from the package:
atlas_install_joboptions( share/*.py )
atlas_install_python_modules( python/*.py )

# Unit tests in the package:
atlas_add_test( L1TopoByteStreamConfigTest
                SCRIPT "python -m L1TopoByteStream.L1TopoByteStreamConfig"
                POST_EXEC_SCRIPT nopost.sh )
