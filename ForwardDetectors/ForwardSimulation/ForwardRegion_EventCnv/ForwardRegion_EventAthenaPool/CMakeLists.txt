################################################################################
# Package: ForwardRegion_EventAthenaPool
################################################################################

# Declare the package name:
atlas_subdir( ForwardRegion_EventAthenaPool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Database/AthenaPOOL/AthenaPoolUtilities
                          PRIVATE
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          Database/AtlasSealCLHEP
                          ForwardDetectors/ForwardSimulation/ForwardRegion_EventCnv/ForwardRegion_EventTPCnv
                          ForwardDetectors/ForwardSimulation/ForwardRegion_SimEv
                          GaudiKernel )

# Component(s) in the package:
atlas_add_poolcnv_library( ForwardRegion_EventAthenaPoolPoolCnv
                           src/*.cxx
                           FILES ForwardRegion_SimEv/SimulationHitCollection.h
                           LINK_LIBRARIES AthenaPoolUtilities AthenaPoolCnvSvcLib AtlasSealCLHEP ForwardRegion_EventTPCnv ForwardRegion_SimEv GaudiKernel )

# Install files from the package:
atlas_install_headers( ForwardRegion_EventAthenaPool )

