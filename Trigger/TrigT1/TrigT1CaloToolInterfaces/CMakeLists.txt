# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigT1CaloToolInterfaces )

# Component(s) in the package:
atlas_add_library( TrigT1CaloToolInterfaces
                   PUBLIC_HEADERS TrigT1CaloToolInterfaces
                   LINK_LIBRARIES AnalysisTriggerEvent AthContainers GaudiKernel Identifier TrigT1CaloCalibConditions TrigT1CaloEventLib TrigT1CaloUtilsLib xAODTrigL1Calo )
