################################################################################
# Package: CaloAthenaPool
################################################################################

# Declare the package name:
atlas_subdir( CaloAthenaPool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Database/AthenaPOOL/AthenaPoolUtilities
                          PRIVATE
                          AtlasTest/TestTools
                          Calorimeter/CaloCnv/CaloTPCnv
                          Calorimeter/CaloDetDescr
                          Calorimeter/CaloEvent
                          Calorimeter/CaloInterface
                          Calorimeter/CaloUtils
                          Control/SGTools
                          Control/StoreGate
                          Database/AthenaPOOL/AthenaPoolCnvSvc )

# Component(s) in the package:
atlas_add_poolcnv_library( CaloAthenaPoolPoolCnv
                           src/*.cxx
                           FILES CaloEvent/CaloCellContainer.h CaloEvent/CaloClusterContainer.h CaloEvent/CaloClusterCellLinkContainer.h CaloEvent/CaloTopoTowerContainer.h CaloEvent/CaloTowerContainer.h CaloEvent/CaloCellLinkContainer.h CaloEvent/CaloShowerContainer.h
                           LINK_LIBRARIES AthenaPoolUtilities CaloTPCnv CaloDetDescrLib CaloEvent CaloUtilsLib SGTools StoreGateLib SGtests AthenaPoolCnvSvcLib )

# Install files from the package:
atlas_install_headers( CaloAthenaPool )
atlas_install_joboptions( share/*.py )

