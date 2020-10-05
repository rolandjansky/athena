# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( CaloTTDetDescrCnv )

# Component(s) in the package:
atlas_add_component( CaloTTDetDescrCnv
                     src/*.cxx
                     LINK_LIBRARIES GaudiKernel CaloDetDescrLib CaloIdentifier CaloTTDetDescr CaloTriggerToolLib StoreGateLib DetDescrCnvSvcLib )
