################################################################################
# Package: LUCID_EventAthenaPool
################################################################################

# Declare the package name:
atlas_subdir( LUCID_EventAthenaPool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          Database/AthenaPOOL/AthenaPoolUtilities
                          ForwardDetectors/LUCID/LUCID_Cnv/LUCID_EventTPCnv
                          ForwardDetectors/LUCID/LUCID_RawEvent
                          ForwardDetectors/LUCID/LUCID_SimUtils/LUCID_SimEvent
                          GaudiKernel )

# Component(s) in the package:
atlas_add_poolcnv_library( LUCID_EventAthenaPoolPoolCnv
                           src/*.cxx
                           FILES LUCID_SimEvent/LUCID_SimHitCollection.h LUCID_RawEvent/LUCID_DigitContainer.h LUCID_RawEvent/LUCID_RawDataContainer.h
                           LINK_LIBRARIES AthenaPoolCnvSvcLib AthenaPoolUtilities LUCID_EventTPCnv LUCID_RawEvent LUCID_SimEvent GaudiKernel )

# Install files from the package:
atlas_install_headers( LUCID_EventAthenaPool )

