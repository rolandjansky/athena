# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigT2CaloEgamma )

# Component(s) in the package:
atlas_add_component( TrigT2CaloEgamma
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthLinks AthenaBaseComps AthenaMonitoringKernelLib CaloDetDescrLib CaloEvent CaloGeoHelpers CaloIdentifier CxxUtils GaudiKernel IRegionSelector StoreGateLib TrigSteeringEvent TrigT2CaloCalibrationLib TrigT2CaloCommonLib TrigTimeAlgsLib xAODTrigCalo xAODTrigRinger )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
