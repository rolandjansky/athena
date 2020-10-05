# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( L1TopoValDataCnv )

# Component(s) in the package:
atlas_add_component( L1TopoValDataCnv
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps AthenaKernel TrigSteeringEvent TrigT1Result xAODTrigger )
