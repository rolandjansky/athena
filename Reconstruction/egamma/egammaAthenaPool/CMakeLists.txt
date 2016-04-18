################################################################################
# Package: egammaAthenaPool
################################################################################

# Declare the package name:
atlas_subdir( egammaAthenaPool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Database/AtlasSealCLHEP
                          PRIVATE
                          Control/AthenaBaseComps
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          GaudiKernel
                          Reconstruction/egamma/egammaEvent
                          Reconstruction/egamma/egammaEventTPCnv )

# Component(s) in the package:
atlas_add_poolcnv_library( egammaAthenaPoolPoolCnv
                           src/*.cxx
                           FILES egammaEvent/egammaContainer.h egammaEvent/ElectronContainer.h egammaEvent/PhotonContainer.h egammaEvent/egDetailContainer.h egammaEvent/EMShowerMinimalContainer.h egammaEvent/CaloRingsContainer.h
                           TYPES_WITH_NAMESPACE 
                           LINK_LIBRARIES AthenaPoolUtilities AthenaBaseComps AthenaPoolCnvSvcLib GaudiKernel egammaEvent egammaEventTPCnv )

# Install files from the package:
atlas_install_headers( egammaAthenaPool )
atlas_install_joboptions( share/*.py )

