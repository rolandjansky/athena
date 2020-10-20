# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigHypoCommonTools )

# Component(s) in the package:
atlas_add_component( TrigHypoCommonTools
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES DecisionHandlingLib L1DecoderLib GaudiKernel TrigT1Result)

# Install files from the package:
atlas_install_headers( TrigHypoCommonTools )
