# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( AthenaPoolTestAthenaPool )

# Component(s) in the package:
atlas_add_poolcnv_library( AthenaPoolTestAthenaPoolPoolCnv
                           src/*.cxx
                           FILES AthenaPoolTestData/IAthenaPoolTestCollection.h AthenaPoolTestData/AthenaPoolTestMatrix.h AthenaPoolTestData/AthenaPoolTestMap.h AthenaPoolTestData/FauxTriggerMap.h AthenaPoolTestData/dummy_A.h AthenaPoolTestData/dummy_E.h AthenaPoolTestData/TrigPath.h AthenaPoolTestData/TestGaudiProperty.h AthenaPoolTestData/PixelRDOElemLinkVec.h
                           LINK_LIBRARIES AthenaPoolCnvSvcLib AthenaPoolTestData DataModelAthenaPoolLib )

atlas_add_dictionary( AthenaPoolTestAthenaPoolCnvDict
                      AthenaPoolTestAthenaPool/AthenaPoolTestAthenaPoolDict.h
                      AthenaPoolTestAthenaPool/selection.xml
                      LINK_LIBRARIES DataModelAthenaPoolLib AthenaPoolTestData AthenaPoolCnvSvcLib AthenaPoolUtilities )
