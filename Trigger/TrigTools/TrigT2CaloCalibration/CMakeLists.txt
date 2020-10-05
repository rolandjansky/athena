# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigT2CaloCalibration )

# Component(s) in the package:
atlas_add_library( TrigT2CaloCalibrationLib
                   TrigT2CaloCalibration/*.h
                   INTERFACE
                   PUBLIC_HEADERS TrigT2CaloCalibration
                   LINK_LIBRARIES GaudiKernel xAODTrigCalo )

atlas_add_component( TrigT2CaloCalibration
                     src/*.cxx src/components/*.cxx
                     LINK_LIBRARIES LINK_LIBRARIES AthenaBaseComps CaloClusterCorrectionLib CaloConditions CaloDetDescrLib CaloGeoHelpers CaloUtilsLib TrigT2CaloCalibrationLib )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
