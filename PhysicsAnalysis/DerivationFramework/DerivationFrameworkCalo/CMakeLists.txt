# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( DerivationFrameworkCalo )

# Component(s) in the package:
atlas_add_component( DerivationFrameworkCalo
                     src/*.cxx src/components/*.cxx
                     LINK_LIBRARIES LINK_LIBRARIES AthenaBaseComps CaloClusterCorrectionLib CaloEvent CaloGeoHelpers CaloIdentifier CaloUtilsLib DerivationFrameworkInterfaces ExpressionEvaluationLib FourMomUtils GaudiKernel LArCablingLib StoreGateLib xAODBase xAODCaloEvent xAODEgamma xAODJet xAODMuon xAODTau )

# Install files from the package:
atlas_install_python_modules( python/*.py )
