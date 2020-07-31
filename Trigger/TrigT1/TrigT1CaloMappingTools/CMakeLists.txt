# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigT1CaloMappingTools )

# Component(s) in the package:
atlas_add_component( TrigT1CaloMappingTools
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps CaloIdentifier CaloTriggerToolLib CxxUtils GaudiKernel Identifier TrigT1CaloToolInterfaces )
