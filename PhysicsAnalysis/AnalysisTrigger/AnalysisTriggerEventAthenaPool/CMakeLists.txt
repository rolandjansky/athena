################################################################################
# Package: AnalysisTriggerEventAthenaPool
################################################################################

# Declare the package name:
atlas_subdir( AnalysisTriggerEventAthenaPool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Database/AthenaPOOL/AthenaPoolUtilities
                          GaudiKernel
                          PRIVATE
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          PhysicsAnalysis/AnalysisTrigger/AnalysisTriggerEvent
                          PhysicsAnalysis/AnalysisTrigger/AnalysisTriggerEventTPCnv )

# Component(s) in the package:
atlas_add_poolcnv_library( AnalysisTriggerEventAthenaPoolPoolCnv
                           src/*.cxx
                           FILES AnalysisTriggerEvent/CTP_Decision.h AnalysisTriggerEvent/LVL1_ROI.h
                           LINK_LIBRARIES AthenaPoolUtilities GaudiKernel AthenaPoolCnvSvcLib AnalysisTriggerEvent AnalysisTriggerEventTPCnv )

# Install files from the package:
atlas_install_headers( AnalysisTriggerEventAthenaPool )

