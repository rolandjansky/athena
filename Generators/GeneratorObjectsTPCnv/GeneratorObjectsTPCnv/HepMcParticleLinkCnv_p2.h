///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GENERATOROBJECTSTPCNV_HEPMCPARTICLELINKCNV_P2_H
#define GENERATOROBJECTSTPCNV_HEPMCPARTICLELINKCNV_P2_H

// STL includes
#include <map>

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// GeneratorObjects includes
#include "GeneratorObjects/HepMcParticleLink.h"

// GeneratorObjectsAthenaPool includes
#include "GeneratorObjectsTPCnv/HepMcParticleLink_p2.h"

// Forward declaration
class MsgStream;

class HepMcParticleLinkCnv_p2 : public T_AthenaPoolTPCnvConstBase<
                                         HepMcParticleLink,
                                         HepMcParticleLink_p2
                                       >
{
public:
  using base_class::persToTrans;
  using base_class::transToPers;

  /** Default constructor:
   */
  HepMcParticleLinkCnv_p2() = default;


  /** Method creating the transient representation of @c HepMcParticleLink
   *  from its persistent representation @c HepMcParticleLink_p2
   */
  virtual void persToTrans( const HepMcParticleLink_p2* persObj,
                            HepMcParticleLink* transObj,
                            MsgStream &msg ) const override;

  /** Method creating the persistent representation @c HepMcParticleLink_p2
   *  from its transient representation @c HepMcParticleLink
   */
  virtual void transToPers( const HepMcParticleLink* transObj,
                            HepMcParticleLink_p2* persObj,
                            MsgStream &msg ) const override;
};


#endif //> GENERATOROBJECTSTPCNV_HEPMCPARTICLELINKCNV_P2_H
