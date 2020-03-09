///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/** IParticleLinkContainerCnv_p1.h
 *  @author: Karsten Koeneke (DESY)
 *  @date July 2009
 *  @brief this defines the t/p conversion of a vector
 *         of ElementLinks to IParticleContainer
 *****************************************************************/

#ifndef EVENTCOMMONTPCNV_IPARTICLELINKCONTAINERCNV_P1_H
#define EVENTCOMMONTPCNV_IPARTICLELINKCONTAINERCNV_P1_H

// STL includes
#include <vector>

// DataModel includes
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "DataModelAthenaPool/VectorElementLinkCnv_p1.h"
#include "DataModelAthenaPool/VectorElementLink_p1.h"
#include "EventCommonTPCnv/IParticleLinkContainer_p1.h"
#include "NavFourMom/IParticleLinkContainer.h"


/** Definition of the t/p converter version of IParticleLinkContainer */
class IParticleLinkContainerCnv_p1 
  : public T_AthenaPoolTPCnvConstBase< IParticleLinkContainer, IParticleLinkContainer_p1 >
{

public:
  using base_class::transToPers; 
  using base_class::persToTrans;


  /** Create typeded for ease of use */
  typedef   IParticleLinkContainer_p1 PersLinkVect_t;

  /** Create typeded for ease of use */
  typedef   IParticleLinkContainer LinkVect_t;


  /** The method to convert the persistent to the transient state */
  virtual
  void persToTrans(const PersLinkVect_t* pers, LinkVect_t* trans, MsgStream& log) const override
  {
    VectorElementLinkCnv_p1< IParticleLinkContainer > myVecTPConv;
    myVecTPConv.persToTrans( pers, trans, log ); 
  }

  /** The method to convert the transient to the persistent state */
  virtual
  void transToPers(const LinkVect_t* trans, PersLinkVect_t* pers, MsgStream& log) const override
  { 
    VectorElementLinkCnv_p1< IParticleLinkContainer > myVecTPConv;
    myVecTPConv.transToPers( trans, pers, log ); 
  }

  
  /** The method to convert the persistent to the transient state, for references */
  void persToTrans(const PersLinkVect_t& pers, LinkVect_t& trans, MsgStream& log) const
  {
    VectorElementLinkCnv_p1< IParticleLinkContainer > myVecTPConv;
    myVecTPConv.persToTrans( pers, trans, log );
  }

  /** The method to convert the transient to the persistent state, for references */
  void transToPers(const LinkVect_t& trans, PersLinkVect_t& pers, MsgStream& log) const
  {
    VectorElementLinkCnv_p1< IParticleLinkContainer > myVecTPConv;
    myVecTPConv.transToPers( trans, pers, log );
  }

  /** The method to convert the transient to the persistent state, for references */
  void transToPers(const LinkVect_t& trans,
                   PersLinkVect_t& pers,
                   const SG::ThinningCache* cache,
                   MsgStream& log) const
  {
    VectorElementLinkCnv_p1< IParticleLinkContainer > myVecTPConv;
    myVecTPConv.transToPers( trans, pers, cache, log );
  }

};


#endif // EVENTCOMMONTPCNV_IPARTICLELINKCONTAINERCNV_P1_H
