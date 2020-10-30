# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TestLArHardwareID )

# Component(s) in the package:
atlas_add_component( TestLArHardwareID
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel CaloIdentifier CaloTriggerToolLib AthenaKernel StoreGateLib LArCablingLib LArIdentifier LArRecConditions PathResolver RegistrationServicesLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )
