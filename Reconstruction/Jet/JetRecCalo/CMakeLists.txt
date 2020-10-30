# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( JetRecCalo )

# Component(s) in the package:
atlas_add_library( JetRecCaloLib
   JetRecCalo/*.h
   INTERFACE
   PUBLIC_HEADERS JetRecCalo
   LINK_LIBRARIES GaudiKernel CaloIdentifier LArRecConditions TileConditionsLib JetUtils
   StoreGateLib AsgTools JetInterface )

atlas_add_component( JetRecCalo
   src/*.cxx src/components/*.cxx
   LINK_LIBRARIES AsgTools AthenaKernel CaloDetDescrLib CaloEvent CaloIdentifier GaudiKernel JetInterface JetRecCaloLib JetUtils LArRecConditions StoreGateLib TileConditionsLib )
