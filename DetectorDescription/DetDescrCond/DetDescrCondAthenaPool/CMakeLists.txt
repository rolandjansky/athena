# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( DetDescrCondAthenaPool )

# Component(s) in the package:
atlas_add_poolcnv_library( DetDescrCondAthenaPoolPoolCnv
                           src/*.cxx
                           FILES DetDescrConditions/AlignableTransform.h DetDescrConditions/AlignableTransformContainer.h DetDescrConditions/DetCondCFloat.h DetDescrConditions/DetCondKeyTrans.h DetDescrConditions/DetStatusMap.h
                           MULT_CHAN_TYPES AlignableTransformContainer
                           LINK_LIBRARIES AthenaPoolCnvSvcLib DetDescrCondTPCnv DetDescrConditions )
