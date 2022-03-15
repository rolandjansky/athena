# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( MuonByteStream )

# External dependencies:
find_package( tdaq-common )

# Component(s) in the package:
atlas_add_component( MuonByteStream
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS}
                     LINK_LIBRARIES ${TDAQ-COMMON_LIBRARIES} AthenaBaseComps AthenaKernel ByteStreamCnvSvcBaseLib ByteStreamData GaudiKernel AthenaPoolUtilities CSCcablingLib IRegionSelector MuonIdHelpersLib MuonRDO TrigSteeringEvent MuonMDT_CnvToolsLib MuonCSC_CnvToolsLib MuonRPC_CnvToolsLib MuonCnvToolInterfacesLib ViewAlgsLib MuonAlignmentData )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py )

