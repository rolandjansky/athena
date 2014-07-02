/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// generate the T/P converter entries
#include "AthenaKernel/TPCnvFactory.h"

#include "JetEventTPCnv/JetCollection_p1.h"
#include "JetEventTPCnv/JetCnv_p1.h"
#include "JetEventTPCnv/JetCollection_p2.h"
#include "JetEventTPCnv/JetCnv_p2.h"
#include "JetEventTPCnv/JetCollection_p3.h"
#include "JetEventTPCnv/JetCnv_p3.h"
#include "JetEventTPCnv/JetCollection_p4.h"
#include "JetEventTPCnv/JetCnv_p4.h"
#include "JetEventTPCnv/JetCollection_p5.h"
#include "JetEventTPCnv/JetCnv_p5.h"
#include "JetEventTPCnv/JetCollection_p6.h"
#include "JetEventTPCnv/JetCnv_p6.h"
#include "JetEventTPCnv/JetKeyDescriptor_p1.h"
#include "JetEventTPCnv/JetKeyDescriptorCollection_p1.h"

#include "JetEventTPCnv/JetSamplingCollection_p1.h"
#include "JetEventTPCnv/JetSamplingCollection_p2.h"

#include "JetEventTPCnv/JetCollectionCnv_p1.h"
#include "JetEventTPCnv/JetCollectionCnv_p2.h"
#include "JetEventTPCnv/JetCollectionCnv_p3.h"
#include "JetEventTPCnv/JetCollectionCnv_p4.h"
#include "JetEventTPCnv/JetCollectionCnv_p5.h"
#include "JetEventTPCnv/JetCollectionCnv_tlp2.h"
#include "JetEventTPCnv/JetCollectionCnv_tlp5.h"
#include "JetEventTPCnv/JetCollectionCnv_tlp6.h"
#include "JetEventTPCnv/JetKeyDescriptorCnv_p1.h"
#include "JetEventTPCnv/JetKeyDescriptorCollectionCnv_p1.h"
#include "JetEventTPCnv/JetKeyDescriptorCollectionCnv_tlp1.h"
#include "JetEventTPCnv/JetMomentMapCnv_p1.h"
#include "JetEventTPCnv/JetMomentMapCnv_p6.h"
#include "JetEventTPCnv/JetMomentMapCollectionCnv_p6.h"
#include "JetEventTPCnv/JetSamplingCollectionCnv_p1.h"
#include "JetEventTPCnv/JetSamplingCollectionCnv_p2.h"

#include "JetEventTPCnv/ParticleJetContainerCnv_p1.h"

DECLARE_NAMED_TPCNV_FACTORY(ParticleJetContainerCnv_p1,
                            ParticleJetContainerCnv_p1,
                            JetCollection,
                            ParticleJetContainer_p1,
                            Athena::TPCnvVers::Old)
                      
DECLARE_TPCNV_FACTORY(JetCollectionCnv_p1,
                      JetCollection,
                      JetCollection_p1,
                      Athena::TPCnvVers::Old)
                      
DECLARE_TPCNV_FACTORY(JetCollectionCnv_p2,
                      JetCollection,
                      JetCollection_p2,
                      Athena::TPCnvVers::Old)
                      
DECLARE_TPCNV_FACTORY(JetCollectionCnv_p3,
                      JetCollection,
                      JetCollection_p3,
                      Athena::TPCnvVers::Old)
                      
DECLARE_TPCNV_FACTORY(JetCollectionCnv_p4,
                      JetCollection,
                      JetCollection_p4,
                      Athena::TPCnvVers::Old)
                      
DECLARE_TPCNV_FACTORY(JetCollectionCnv_p5,
                      JetCollection,
                      JetCollection_p5,
                      Athena::TPCnvVers::Old)
                      
DECLARE_TPCNV_FACTORY(JetCollectionCnv_tlp2,
                      JetCollection,
                      JetCollection_tlp2,
                      Athena::TPCnvVers::Old)
                      
DECLARE_TPCNV_FACTORY(JetCollectionCnv_tlp5,
                      JetCollection,
                      JetCollection_tlp5,
                      Athena::TPCnvVers::Old)

DECLARE_TPCNV_FACTORY(JetCollectionCnv_tlp6,
                      JetCollection,
                      JetCollection_tlp6,
                      Athena::TPCnvVers::Current)
                      
DECLARE_TPCNV_FACTORY(JetKeyDescriptorCnv_p1,
                      JetKeyDescriptor,
                      JetKeyDescriptor_p1,
                      Athena::TPCnvVers::Current)

DECLARE_TPCNV_FACTORY(JetKeyDescriptorCollectionCnv_p1,
                      JetKeyDescriptorCollection,
                      JetKeyDescriptorCollection_p1,
                      Athena::TPCnvVers::Current)
                      
DECLARE_NAMED_TPCNV_FACTORY(JetMomentMapCnv_p1,
                            JetMomentMapCnv_p1,
                            JetMomentMap,
                            JetMomentMap_p1,
                            Athena::TPCnvVers::Old)
                      
DECLARE_TPCNV_FACTORY(JetMomentMapCnv_p6,
                      JetMomentMap,
                      JetMomentMap_p6,
                      Athena::TPCnvVers::Current)

DECLARE_TPCNV_FACTORY(JetMomentMapCollectionCnv_p6,
                      JetMomentMapCollection,
                      JetMomentMapCollection_p6,
                      Athena::TPCnvVers::Current)
                      
DECLARE_TPCNV_FACTORY(JetSamplingCollectionCnv_p1,
                      JetSamplingCollection,
                      JetSamplingCollection_p1,
                      Athena::TPCnvVers::Old)
                      
DECLARE_TPCNV_FACTORY(JetSamplingCollectionCnv_p2,
                      JetSamplingCollection,
                      JetSamplingCollection_p2,
                      Athena::TPCnvVers::Current)
                      

