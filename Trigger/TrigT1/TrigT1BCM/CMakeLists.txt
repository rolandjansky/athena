# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigT1BCM )

# Component(s) in the package:
atlas_add_component( TrigT1BCM
                     src/*.cxx src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel InDetBCM_RawData TrigConfInterfaces TrigConfL1Data TrigT1Interfaces )
