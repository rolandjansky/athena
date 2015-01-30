///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ParticleJetContainerCnv_p1.h 
// Header file for class ParticleJetContainerCnv_p1
// Author: P.A. delsart
/////////////////////////////////////////////////////////////////// 
#ifndef PARTICLEEVENTTPCNV_PARTICLEJETCONTAINERCNV_P1_H 
#define PARTICLEEVENTTPCNV_PARTICLEJETCONTAINERCNV_P1_H 

// STL includes

// Gaudi includes

// JetTagEvent includes
#include "JetEvent/JetCollection.h"

// ParticleEventTPCnv includes
#include "JetEventTPCnv/ParticleJetContainer_p1.h"
#include "JetEventTPCnv/ParticleJetCnv_p1.h"

// Forward declaration
class MsgStream;

typedef T_AthenaPoolTPCnvVector<
  JetCollection,
  ParticleJetContainer_p1,
  ParticleJetCnv_p1
  > ParticleJetContainerCnv_p1;

#endif //> PARTICLEEVENTTPCNV_PARTICLEJETCONTAINERCNV_P1_H
