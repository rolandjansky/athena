# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigT1CaloUtils )

# Component(s) in the package:
atlas_add_library( TrigT1CaloUtilsLib
                   src/*.cxx
                   PUBLIC_HEADERS TrigT1CaloUtils
                   LINK_LIBRARIES AthContainers AthenaKernel CaloEvent GaudiKernel TrigConfInterfaces TrigT1CaloEventLib TrigT1Interfaces xAODTrigL1Calo
                   PRIVATE_LINK_LIBRARIES TrigConfData TrigConfL1Data TrigT1Result )
