# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigT1Lucid )

# Component(s) in the package:
atlas_add_component( TrigT1Lucid
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel LUCID_RawEvent TrigConfInterfaces TrigConfL1Data TrigT1Interfaces )
