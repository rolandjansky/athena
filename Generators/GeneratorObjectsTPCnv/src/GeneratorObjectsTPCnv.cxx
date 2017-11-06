/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// generate the T/P converter entries
#include "AthenaKernel/TPCnvFactory.h"

#include "GeneratorObjectsTPCnv/HepMcParticleLink_p1.h"
#include "GeneratorObjectsTPCnv/HepMcParticleLink_p2.h"

#include "GeneratorObjectsTPCnv/GenParticle_p1.h"
#include "GeneratorObjectsTPCnv/GenVertex_p1.h"
#include "GeneratorObjectsTPCnv/GenEvent_p1.h"
#include "GeneratorObjectsTPCnv/McEventCollection_p1.h"
#include "GeneratorObjectsTPCnv/McEventCollectionCnv_p1.h"

#include "GeneratorObjectsTPCnv/GenParticle_p2.h"
#include "GeneratorObjectsTPCnv/GenVertex_p2.h"
#include "GeneratorObjectsTPCnv/GenEvent_p2.h"
#include "GeneratorObjectsTPCnv/McEventCollection_p2.h"
#include "GeneratorObjectsTPCnv/McEventCollectionCnv_p2.h"

#include "GeneratorObjectsTPCnv/GenParticle_p3.h"
#include "GeneratorObjectsTPCnv/GenVertex_p3.h"
#include "GeneratorObjectsTPCnv/GenEvent_p3.h"
#include "GeneratorObjectsTPCnv/McEventCollection_p3.h"
#include "GeneratorObjectsTPCnv/McEventCollectionCnv_p3.h"

#include "GeneratorObjectsTPCnv/GenParticle_p4.h"
#include "GeneratorObjectsTPCnv/GenVertex_p4.h"
#include "GeneratorObjectsTPCnv/GenEvent_p4.h"
#include "GeneratorObjectsTPCnv/McEventCollection_p4.h"
#include "GeneratorObjectsTPCnv/McEventCollectionCnv_p4.h"

#include "GeneratorObjectsTPCnv/GenParticle_p5.h"
#include "GeneratorObjectsTPCnv/GenVertex_p5.h"
#include "GeneratorObjectsTPCnv/GenEvent_p5.h"
#include "GeneratorObjectsTPCnv/McEventCollection_p5.h"
#include "GeneratorObjectsTPCnv/McEventCollectionCnv_p5.h"

DECLARE_TPCNV_FACTORY(McEventCollectionCnv_p1,
                      McEventCollection,
                      McEventCollection_p1,
                      Athena::TPCnvVers::Old)
                      
DECLARE_TPCNV_FACTORY(McEventCollectionCnv_p2,
                      McEventCollection,
                      McEventCollection_p2,
                      Athena::TPCnvVers::Old)
                      
DECLARE_TPCNV_FACTORY(McEventCollectionCnv_p3,
                      McEventCollection,
                      McEventCollection_p3,
                      Athena::TPCnvVers::Old)
                      
DECLARE_TPCNV_FACTORY(McEventCollectionCnv_p4,
                      McEventCollection,
                      McEventCollection_p4,
                      Athena::TPCnvVers::Old)

DECLARE_TPCNV_FACTORY(McEventCollectionCnv_p5,
                      McEventCollection,
                      McEventCollection_p5,
                      Athena::TPCnvVers::Current)
