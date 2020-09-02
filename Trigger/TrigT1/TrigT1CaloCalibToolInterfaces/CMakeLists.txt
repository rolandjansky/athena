# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigT1CaloCalibToolInterfaces )

# Component(s) in the package:
atlas_add_library( TrigT1CaloCalibToolInterfaces
                   PUBLIC_HEADERS TrigT1CaloCalibToolInterfaces
                   LINK_LIBRARIES AsgTools LArCablingLib LArElecCalib StoreGateLib TrigT1CaloEventLib xAODTrigL1Calo )
