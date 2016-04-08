################################################################################
# Package: RecEventAthenaPool
################################################################################

# Declare the package name:
atlas_subdir( RecEventAthenaPool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Database/AtlasSealCLHEP
                          PRIVATE
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          GaudiKernel
                          Reconstruction/RecEvent
                          Reconstruction/RecEventTPCnv )

# Component(s) in the package:
atlas_add_poolcnv_library( RecEventAthenaPoolPoolCnv
                           src/*.cxx
                           FILES RecEvent/RecoTimingObj.h
                           TYPES_WITH_NAMESPACE RecoTimingObj
                           LINK_LIBRARIES AthenaPoolUtilities AthenaPoolCnvSvcLib GaudiKernel RecEvent RecEventTPCnv )

# Install files from the package:
atlas_install_headers( RecEventAthenaPool )

