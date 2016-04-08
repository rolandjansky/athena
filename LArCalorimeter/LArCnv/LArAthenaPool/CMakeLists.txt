################################################################################
# Package: LArAthenaPool
################################################################################

# Declare the package name:
atlas_subdir( LArAthenaPool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Database/AtlasSealCLHEP
                          PRIVATE
                          AtlasTest/TestTools
                          Calorimeter/CaloDetDescr
                          Control/SGTools
                          Control/StoreGate
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          GaudiKernel
                          LArCalorimeter/LArCnv/LArTPCnv
                          LArCalorimeter/LArRawEvent
                          LArCalorimeter/LArRecEvent )

# Component(s) in the package:
atlas_add_poolcnv_library( LArAthenaPoolPoolCnv
                           src/*.cxx
                           FILES LArRawEvent/LArRawChannelContainer.h LArRawEvent/LArDigitContainer.h LArRawEvent/LArTTL1Container.h LArRawEvent/LArFebErrorSummary.h LArRecEvent/LArNoisyROSummary.h
                           LINK_LIBRARIES AthenaPoolUtilities CaloDetDescrLib SGTools StoreGateLib SGtests AthenaPoolCnvSvcLib GaudiKernel LArTPCnv LArRawEvent LArRecEvent )

# Install files from the package:
atlas_install_headers( LArAthenaPool )
atlas_install_joboptions( share/*.py )

