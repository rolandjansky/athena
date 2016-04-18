################################################################################
# Package: RecAthenaPool
################################################################################

# Declare the package name:
atlas_subdir( RecAthenaPool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Database/AtlasSealCLHEP
                          PRIVATE
                          AtlasTest/TestTools
                          Control/CLIDSvc
                          Control/SGTools
                          Control/StoreGate
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          GaudiKernel
                          Reconstruction/MissingETEvent
                          Reconstruction/MuonIdentification/muonEvent
                          Reconstruction/RecTPCnv )

# Component(s) in the package:
atlas_add_poolcnv_library( RecAthenaPoolPoolCnv
                           src/*.cxx
                           FILES MissingETEvent/MissingEtCalo.h MissingETEvent/MissingEtTruth.h MissingETEvent/MissingET.h MissingETEvent/MissingETComposition.h muonEvent/MuonSpShowerContainer.h muonEvent/MuonCaloEnergyContainer.h muonEvent/MuonContainer.h src/MuonCaloEnergies.h
                           TYPES_WITH_NAMESPACE MuonCaloEnergyContainer Rec::MuonSpShowerContainer Analysis::MuonContainer TPCnv::MuonCaloEnergies
                           LINK_LIBRARIES AthenaPoolUtilities SGTools StoreGateLib SGtests AthenaPoolCnvSvcLib GaudiKernel MissingETEvent muonEvent RecTPCnv )

# Install files from the package:
atlas_install_headers( RecAthenaPool )
atlas_install_joboptions( share/*.py )

