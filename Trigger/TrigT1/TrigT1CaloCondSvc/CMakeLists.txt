# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigT1CaloCondSvc )

# Component(s) in the package:
atlas_add_library( TrigT1CaloCondSvcLib
   TrigT1CaloCondSvc/*.h TrigT1CaloCondSvc/*.icc src/*.cxx
   PUBLIC_HEADERS TrigT1CaloCondSvc
   LINK_LIBRARIES AthenaBaseComps AthenaKernel AthenaPoolUtilities RegistrationServicesLib StoreGateLib TrigT1CaloCalibConditions )

atlas_add_component( TrigT1CaloCondSvc
   src/components/*.cxx
   LINK_LIBRARIES LINK_LIBRARIES TrigT1CaloCondSvcLib )
