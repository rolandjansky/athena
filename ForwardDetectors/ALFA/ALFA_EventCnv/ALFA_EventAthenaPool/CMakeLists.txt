################################################################################
# Package: ALFA_EventAthenaPool
################################################################################

# Declare the package name:
atlas_subdir( ALFA_EventAthenaPool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Database/AthenaPOOL/AthenaPoolUtilities
                          PRIVATE
                          Control/StoreGate
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          Database/AtlasSealCLHEP
                          ForwardDetectors/ALFA/ALFA_CLinkEv
                          ForwardDetectors/ALFA/ALFA_EventCnv/ALFA_EventTPCnv
                          ForwardDetectors/ALFA/ALFA_RawEv
                          ForwardDetectors/ALFA/ALFA_RecEv/ALFA_GloRecEv
                          ForwardDetectors/ALFA/ALFA_RecEv/ALFA_LocRecCorrEv
                          ForwardDetectors/ALFA/ALFA_RecEv/ALFA_LocRecEv
                          ForwardDetectors/ALFA/ALFA_SimEv
                          GaudiKernel )

# Component(s) in the package:
atlas_add_poolcnv_library( ALFA_EventAthenaPoolPoolCnv
                           src/*.cxx
                           FILES ALFA_SimEv/ALFA_HitCollection.h ALFA_SimEv/ALFA_ODHitCollection.h ALFA_RawEv/ALFA_DigitCollection.h ALFA_RawEv/ALFA_ODDigitCollection.h ALFA_RawEv/ALFA_RawDataContainer.h ALFA_LocRecEv/ALFA_LocRecEvCollection.h ALFA_LocRecEv/ALFA_LocRecODEvCollection.h ALFA_LocRecCorrEv/ALFA_LocRecCorrEvCollection.h ALFA_LocRecCorrEv/ALFA_LocRecCorrODEvCollection.h ALFA_GloRecEv/ALFA_GloRecEvCollection.h ALFA_CLinkEv/ALFA_CLinkEvent.h
                           LINK_LIBRARIES AthenaPoolUtilities StoreGateLib SGtests AthenaPoolCnvSvcLib AtlasSealCLHEP ALFA_CLinkEv ALFA_EventTPCnv ALFA_RawEv ALFA_GloRecEv ALFA_LocRecCorrEv ALFA_LocRecEv ALFA_SimEv GaudiKernel )

# Install files from the package:
atlas_install_headers( ALFA_EventAthenaPool )

