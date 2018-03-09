################################################################################
# Package: RecEventAthenaPool
################################################################################

# Declare the package name:
atlas_subdir( RecEventAthenaPool )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PRIVATE
   Database/AthenaPOOL/AthenaPoolCnvSvc
   GaudiKernel
   Reconstruction/RecEvent
   Reconstruction/RecEventTPCnv )

# Component(s) in the package:
atlas_add_poolcnv_library( RecEventAthenaPoolPoolCnv
   src/*.h src/*.cxx
   FILES RecEvent/RecoTimingObj.h
   LINK_LIBRARIES AthenaPoolCnvSvcLib GaudiKernel RecEvent RecEventTPCnv )