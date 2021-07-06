///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// HepMcParticleLinkCnv_p1.h 
// Header file for class HepMcParticleLinkCnv_p1
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef GENERATOROBJECTSTPCNV_HEPMCPARTICLELINKCNV_P1_H 
#define GENERATOROBJECTSTPCNV_HEPMCPARTICLELINKCNV_P1_H 

// STL includes
#include <map>

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// GeneratorObjects includes
#include "GeneratorObjects/HepMcParticleLink.h"

// GeneratorObjectsAthenaPool includes
#include "GeneratorObjectsTPCnv/HepMcParticleLink_p1.h"

// Forward declaration
class MsgStream;

class HepMcParticleLinkCnv_p1 : public T_AthenaPoolTPCnvBase<
                                         HepMcParticleLink, 
				         HepMcParticleLink_p1
                                       >  
{
  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /** Default constructor: 
   */
  HepMcParticleLinkCnv_p1();


  /** Method creating the transient representation of @c HepMcParticleLink
   *  from its persistent representation @c HepMcParticleLink_p1
   */
  virtual void persToTrans( const HepMcParticleLink_p1* persObj, 
			    HepMcParticleLink* transObj, 
			    MsgStream &msg );

  /** Method creating the persistent representation @c HepMcParticleLink_p1
   *  from its transient representation @c HepMcParticleLink
   */
  virtual void transToPers( const HepMcParticleLink* transObj, 
			    HepMcParticleLink_p1* persObj, 
			    MsgStream &msg );


}; 

/////////////////////////////////////////////////////////////////// 
/// Inline methods: 
/////////////////////////////////////////////////////////////////// 

inline HepMcParticleLinkCnv_p1::HepMcParticleLinkCnv_p1()
{}

#endif //> GENERATOROBJECTSTPCNV_HEPMCPARTICLELINKCNV_P1_H
