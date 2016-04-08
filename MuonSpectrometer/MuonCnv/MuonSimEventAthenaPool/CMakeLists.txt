################################################################################
# Package: MuonSimEventAthenaPool
################################################################################

# Declare the package name:
atlas_subdir( MuonSimEventAthenaPool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Database/AtlasSealCLHEP
                          PRIVATE
                          AtlasTest/TestTools
                          Control/StoreGate
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          GaudiKernel
                          MuonSpectrometer/MuonCnv/MuonSimEventTPCnv
                          MuonSpectrometer/MuonSimEvent
                          Simulation/HitManagement )

# Component(s) in the package:
atlas_add_poolcnv_library( MuonSimEventAthenaPoolPoolCnv
                           src/*.cxx
                           FILES MuonSimEvent/CSCSimHitCollection.h MuonSimEvent/MDTSimHitCollection.h MuonSimEvent/RPCSimHitCollection.h MuonSimEvent/TGCSimHitCollection.h MuonSimEvent/GenericMuonSimHitCollection.h
                           LINK_LIBRARIES AthenaPoolUtilities AtlasSealCLHEP StoreGateLib SGtests AthenaPoolCnvSvcLib GaudiKernel MuonSimEventTPCnv MuonSimEvent HitManagement )

# Install files from the package:
atlas_install_headers( MuonSimEventAthenaPool )
atlas_install_joboptions( share/*.py )

