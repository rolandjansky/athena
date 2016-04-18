################################################################################
# Package: JetEventAthenaPool
################################################################################

# Declare the package name:
atlas_subdir( JetEventAthenaPool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Database/AtlasSealCLHEP
                          GaudiKernel
                          PRIVATE
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          PhysicsAnalysis/JetTagging/JetTagInfoAthenaPool
                          Reconstruction/Jet/JetEvent
                          Reconstruction/Jet/JetEventTPCnv )

# Component(s) in the package:
atlas_add_poolcnv_library( JetEventAthenaPoolPoolCnv
                           src/*.cxx
                           FILES JetEvent/JetMomentMap.h JetEvent/JetSamplingCollection.h JetEvent/JetCollection.h JetEvent/JetKeyDescriptor.h
                           TYPES_WITH_NAMESPACE 
                           LINK_LIBRARIES AthenaPoolUtilities GaudiKernel AthenaPoolCnvSvcLib JetEvent JetEventTPCnv )

# Install files from the package:
atlas_install_headers( JetEventAthenaPool )
atlas_install_joboptions( share/*.py )

