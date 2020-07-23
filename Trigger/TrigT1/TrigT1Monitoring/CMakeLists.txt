# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigT1Monitoring )

# Component(s) in the package:
atlas_add_component( TrigT1Monitoring
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthContainers AthenaMonitoringLib CaloDetDescrLib CaloEvent CaloIdentifier CaloTriggerToolLib EventInfo GaudiKernel Identifier L1TopoRDO LArCablingLib LArElecCalib LWHists StoreGateLib TileCalibBlobObjs TileConditionsLib TileEvent TileIdentifier TrigConfInterfaces TrigConfL1Data TrigT1CaloCalibConditions TrigT1CaloCalibToolInterfaces TrigT1CaloCondSvcLib TrigT1CaloEventLib TrigT1CaloMonitoringToolsLib TrigT1CaloToolInterfaces TrigT1Interfaces TrigT1Result xAODTrigL1Calo )

# Install files from the package:
atlas_install_joboptions( share/*.py )

