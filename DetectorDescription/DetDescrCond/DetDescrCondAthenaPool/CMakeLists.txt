################################################################################
# Package: DetDescrCondAthenaPool
################################################################################

# Declare the package name:
atlas_subdir( DetDescrCondAthenaPool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Database/AthenaPOOL/AthenaPoolUtilities
                          PRIVATE
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          DetectorDescription/DetDescrCond/DetDescrCondTPCnv
                          DetectorDescription/DetDescrCond/DetDescrConditions )

# Component(s) in the package:
atlas_add_poolcnv_library( DetDescrCondAthenaPoolPoolCnv
                           src/*.cxx
                           FILES DetDescrConditions/AlignableTransform.h DetDescrConditions/AlignableTransformContainer.h DetDescrConditions/DetCondCFloat.h DetDescrConditions/DetCondKeyTrans.h DetDescrConditions/DetStatusMap.h
                           MULT_CHAN_TYPES AlignableTransformContainer
                           LINK_LIBRARIES AthenaPoolUtilities AthenaPoolCnvSvcLib DetDescrCondTPCnv DetDescrConditions )

