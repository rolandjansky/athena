///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
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

class HepMcParticleLinkCnv_p1 : public T_AthenaPoolTPCnvConstBase<
                                         HepMcParticleLink, 
				         HepMcParticleLink_p1
                                       >  
{
 public: 
  using base_class::persToTrans;
  using base_class::transToPers;

  /** Default constructor: 
   */
  HepMcParticleLinkCnv_p1() = default;


  /** Method creating the transient representation of @c HepMcParticleLink
   *  from its persistent representation @c HepMcParticleLink_p1
   */
  virtual void persToTrans( const HepMcParticleLink_p1* persObj, 
			    HepMcParticleLink* transObj, 
			    MsgStream &msg ) const override;

  /** Method creating the persistent representation @c HepMcParticleLink_p1
   *  from its transient representation @c HepMcParticleLink
   */
  virtual void transToPers( const HepMcParticleLink* transObj, 
			    HepMcParticleLink_p1* persObj, 
			    MsgStream &msg ) const override;


}; 

#endif //> GENERATOROBJECTSTPCNV_HEPMCPARTICLELINKCNV_P1_H
