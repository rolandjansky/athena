################################################################################
# Package: AFP_EventAthenaPool
################################################################################

# Declare the package name:
atlas_subdir( AFP_EventAthenaPool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Database/AthenaPOOL/AthenaPoolUtilities
                          PRIVATE
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          Database/AtlasSealCLHEP
                          ForwardDetectors/AFP/AFP_DigiEv
                          ForwardDetectors/AFP/AFP_EventCnv/AFP_EventTPCnv
                          ForwardDetectors/AFP/AFP_RecoEv/AFP_LocRecoEv
                          ForwardDetectors/AFP/AFP_SimEv
                          GaudiKernel )

# Component(s) in the package:
atlas_add_poolcnv_library( AFP_EventAthenaPoolPoolCnv
                           src/*.cxx
                           FILES AFP_SimEv/AFP_TDSimHitCollection.h AFP_SimEv/AFP_SIDSimHitCollection.h AFP_DigiEv/AFP_TDDigiCollection.h AFP_DigiEv/AFP_SiDigiCollection.h AFP_LocRecoEv/AFP_TDLocRecoEvCollection.h AFP_LocRecoEv/AFP_SIDLocRecoEvCollection.h
                           LINK_LIBRARIES AthenaPoolUtilities AthenaPoolCnvSvcLib AtlasSealCLHEP AFP_DigiEv AFP_EventTPCnv AFP_LocRecoEv AFP_SimEv GaudiKernel )

# Install files from the package:
atlas_install_headers( AFP_EventAthenaPool )

