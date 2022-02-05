# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( egammaCaloUtils )

# External dependencies:

# Component(s in tne package:
atlas_add_library( egammaCaloUtils
   egammaUtils/*.h src/*.cxx
   PUBLIC_HEADERS egammaCaloUtils
   LINK_LIBRARIES CaloDetDescrLib CaloEvent CaloGeoHelpers 
   CaloIdentifier CaloUtilsLib xAODCaloEvent GaudiKernel
   PRIVATE_LINK_LIBRARIES egammaUtils)

