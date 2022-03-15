# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( RpcClusterization )

# Component(s) in the package:
atlas_add_component( RpcClusterization
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps StoreGateLib Identifier EventPrimitives GaudiKernel MuonReadoutGeometry MuonDigitContainer MuonPrepRawData MuonIdHelpersLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )

