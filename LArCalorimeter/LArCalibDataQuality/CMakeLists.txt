# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( LArCalibDataQuality )

# Component(s) in the package:
atlas_add_component( LArCalibDataQuality
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES CaloIdentifier AthenaBaseComps StoreGateLib GaudiKernel LArElecCalib LArRawConditions LArRecConditions LArCablingLib xAODEventInfo LArIdentifier LArRawEvent )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py )

