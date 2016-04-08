################################################################################
# Package: TrigT1CaloCalibAthenaPool
################################################################################

# Declare the package name:
atlas_subdir( TrigT1CaloCalibAthenaPool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Trigger/TrigT1/TrigT1CaloCalibConditions )

# Component(s) in the package:
atlas_add_poolcnv_library( TrigT1CaloCalibAthenaPoolPoolCnv
                           src/*.cxx
                           FILES TrigT1CaloCalibConditions/L1CaloPedestalContainer.h TrigT1CaloCalibConditions/L1CaloRampDataContainer.h
                           LINK_LIBRARIES AthenaPoolCnvSvcLib AthenaPoolUtilities TrigT1CaloCalibConditions )

