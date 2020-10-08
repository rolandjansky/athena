# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( CaloInterface )

# Component(s) in the package:
atlas_add_library( CaloInterfaceLib
                   CaloInterface/*.h
                   INTERFACE
                   PUBLIC_HEADERS CaloInterface
                   LINK_LIBRARIES AthenaKernel CaloConditions CaloEvent CaloIdentifier CxxUtils GaudiKernel xAODCaloEvent )

atlas_add_dictionary( CaloInterfaceDict
                      CaloInterface/CaloInterfaceDict.h
                      CaloInterface/selection.xml
                      LINK_LIBRARIES CaloInterfaceLib )
