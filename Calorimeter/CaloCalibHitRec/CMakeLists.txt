# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( CaloCalibHitRec )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_library( CaloCalibHitRecLib
                   src/*.cxx
		           PUBLIC_HEADERS CaloCalibHitRec
                   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   LINK_LIBRARIES AthenaBaseComps AthenaKernel CaloDmDetDescr CaloGeoHelpers CaloIdentifier CaloRecLib CaloSimEvent CxxUtils GaudiKernel Identifier StoreGateLib xAODCaloEvent xAODTruth
                   PRIVATE_LINK_LIBRARIES ${CLHEP_LIBRARIES} AtlasDetDescr CaloDetDescrLib CaloEvent CaloUtilsLib LArRecEvent McParticleEvent PathResolver TileEvent )

# Component(s) in the package:
atlas_add_component( CaloCalibHitRec
                     src/components/*.cxx
                     LINK_LIBRARIES CaloCalibHitRecLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

