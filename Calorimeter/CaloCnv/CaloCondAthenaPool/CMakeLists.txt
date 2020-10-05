# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( CaloCondAthenaPool )

# Component(s) in the package:
atlas_add_poolcnv_library( CaloCondAthenaPoolPoolCnv
                           src/*.cxx
                           FILES CaloTriggerTool/LArTTCellMap.h CaloTriggerTool/CaloTTOnOffIdMap.h CaloTriggerTool/CaloTTOnAttrIdMap.h CaloTriggerTool/CaloTTPpmRxIdMap.h CaloConditions/ToolConstants.h CaloConditions/CaloCellFactor.h CaloConditions/CaloCellPositionShift.h CaloConditions/CaloHadWeight.h CaloConditions/CaloEMFrac.h CaloConditions/CaloHadDMCoeff.h CaloConditions/CaloHadDMCoeff2.h CaloConditions/CaloLocalHadCoeff.h
                           TYPES_WITH_NAMESPACE CaloRec::ToolConstants CaloRec::CaloCellFactor CaloRec::CaloCellPositionShift
                           LINK_LIBRARIES CaloConditions CaloTriggerToolLib AthenaKernel GaudiKernel AthenaPoolCnvSvcLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )

