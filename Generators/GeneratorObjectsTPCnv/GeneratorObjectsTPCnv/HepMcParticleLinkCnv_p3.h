///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GENERATOROBJECTSTPCNV_HEPMCPARTICLELINKCNV_P3_H
#define GENERATOROBJECTSTPCNV_HEPMCPARTICLELINKCNV_P3_H

// STL includes
#include <map>

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// GeneratorObjects includes
#include "GeneratorObjects/HepMcParticleLink.h"

// GeneratorObjectsAthenaPool includes
#include "GeneratorObjectsTPCnv/HepMcParticleLink_p3.h"

// Forward declaration
class MsgStream;

class HepMcParticleLinkCnv_p3 : public T_AthenaPoolTPCnvBase<
                                         HepMcParticleLink,
                                         HepMcParticleLink_p3
                                       >
{
  ///////////////////////////////////////////////////////////////////
  // Public methods:
  ///////////////////////////////////////////////////////////////////
public:

  /** Default constructor:
   */
  HepMcParticleLinkCnv_p3();

  /** Destructor:
   */
  //virtual ~HepMcParticleLinkCnv_p3();

  ///////////////////////////////////////////////////////////////////
  // Const methods:
  ///////////////////////////////////////////////////////////////////

  /** Method creating the transient representation of @c HepMcParticleLink
   *  from its persistent representation @c HepMcParticleLink_p3
   */
  virtual void persToTrans( const HepMcParticleLink_p3* persObj,
                            HepMcParticleLink* transObj,
                            MsgStream &msg );

  /** Method creating the persistent representation @c HepMcParticleLink_p3
   *  from its transient representation @c HepMcParticleLink
   */
  virtual void transToPers( const HepMcParticleLink* transObj,
                            HepMcParticleLink_p3* persObj,
                            MsgStream &msg );

  ///////////////////////////////////////////////////////////////////
  // Protected methods:
  ///////////////////////////////////////////////////////////////////
protected:

  ///////////////////////////////////////////////////////////////////
  // Protected data:
  ///////////////////////////////////////////////////////////////////
protected:

};

///////////////////////////////////////////////////////////////////
/// Inline methods:
///////////////////////////////////////////////////////////////////

inline HepMcParticleLinkCnv_p3::HepMcParticleLinkCnv_p3()
{}

#endif //> GENERATOROBJECTSTPCNV_HEPMCPARTICLELINKCNV_P3_H
