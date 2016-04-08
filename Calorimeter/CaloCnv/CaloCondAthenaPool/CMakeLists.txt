################################################################################
# Package: CaloCondAthenaPool
################################################################################

# Declare the package name:
atlas_subdir( CaloCondAthenaPool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Database/AthenaPOOL/AthenaPoolUtilities
                          PRIVATE
                          Calorimeter/CaloConditions
                          Calorimeter/CaloTriggerTool
                          Control/SGTools
                          Control/StoreGate
                          Database/AthenaPOOL/AthenaPoolCnvSvc )

# Component(s) in the package:
atlas_add_poolcnv_library( CaloCondAthenaPoolPoolCnv
                           src/*.cxx
                           FILES CaloTriggerTool/LArTTCellMap.h CaloTriggerTool/CaloTTOnOffIdMap.h CaloTriggerTool/CaloTTOnAttrIdMap.h CaloTriggerTool/CaloTTPpmRxIdMap.h CaloConditions/ToolConstants.h CaloConditions/CaloCellFactor.h CaloConditions/CaloCellPositionShift.h CaloConditions/CaloHadWeight.h CaloConditions/CaloEMFrac.h CaloConditions/CaloHadDMCoeff.h CaloConditions/CaloHadDMCoeff2.h CaloConditions/CaloLocalHadCoeff.h
                           TYPES_WITH_NAMESPACE CaloRec::ToolConstants CaloRec::CaloCellFactor CaloRec::CaloCellPositionShift
                           LINK_LIBRARIES AthenaPoolUtilities CaloConditions CaloTriggerToolLib SGTools StoreGateLib SGtests AthenaPoolCnvSvcLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )

