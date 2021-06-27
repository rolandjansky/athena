################################################################################
# Package: HGTD_EventAthenaPool
################################################################################

# Declare the package name:
atlas_subdir( HGTD_EventAthenaPool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Database/AthenaPOOL/AthenaPoolUtilities
                          PRIVATE
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          Database/AtlasSealCLHEP
                          GaudiKernel
                          HighGranularityTimingDetector/HGTD_Reconstruction/HGTD_RecEvent/HGTD_PrepRawData
                          HighGranularityTimingDetector/HGTD_EventCnv/HGTD_EventTPCnv )

# Component(s) in the package:
atlas_add_poolcnv_library( HGTD_EventAthenaPoolPoolCnv
                           src/*.cxx
                           FILES HGTD_PrepRawData/HGTD_ClusterContainer.h
                           TYPES_WITH_NAMESPACE HGTD::HGTD_ClusterContainer
                           LINK_LIBRARIES AthenaPoolUtilities AthenaPoolCnvSvcLib AtlasSealCLHEP GaudiKernel HGTD_PrepRawData HGTD_EventTPCnv )

# Install files from the package:
atlas_install_headers( HGTD_EventAthenaPool )
