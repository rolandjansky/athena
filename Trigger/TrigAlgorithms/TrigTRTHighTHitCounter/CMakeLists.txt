# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigTRTHighTHitCounter )

# Component(s) in the package:
atlas_add_component( TrigTRTHighTHitCounter
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES CxxUtils GaudiKernel GeoPrimitives Identifier InDetIdentifier InDetPrepRawData TrigCaloEvent TrigInterfacesLib TrigSteeringEvent xAODTrigRinger )
