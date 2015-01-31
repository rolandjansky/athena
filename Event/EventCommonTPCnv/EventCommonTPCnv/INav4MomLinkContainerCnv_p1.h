///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** INav4MomLinkContainerCnv_p1.h
 *  @author: Karsten Koeneke (DESY)
 *  @date July 2009
 *  @brief this defines the t/p conversion of a vector
 *         of ElementLinks to INavigable4MomentumCollection
 *****************************************************************/

#ifndef EVENTCOMMONTPCNV_INAV4MOMLINKCONTAINERCNV_P1_H
#define EVENTCOMMONTPCNV_INAV4MOMLINKCONTAINERCNV_P1_H

// STL includes
#include <vector>

// DataModel includes
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "DataModelAthenaPool/VectorElementLinkCnv_p1.h"
#include "DataModelAthenaPool/VectorElementLink_p1.h"
#include "EventCommonTPCnv/INav4MomLinkContainer_p1.h"
#include "NavFourMom/INav4MomLinkContainer.h"


/** Definition of the t/p converter version of INav4MomLinkContainer */
class INav4MomLinkContainerCnv_p1 
  : public T_AthenaPoolTPCnvBase< INav4MomLinkContainer, INav4MomLinkContainer_p1 >
{

public:
  /** Create typeded for ease of use */
  typedef   INav4MomLinkContainer_p1 PersLinkVect_t;

  /** Create typeded for ease of use */
  typedef   INav4MomLinkContainer LinkVect_t;


  /** The method to convert the persistent to the transient state */
  void persToTrans(const PersLinkVect_t* pers, LinkVect_t* trans, MsgStream& log) 
  { 
    VectorElementLinkCnv_p1< INav4MomLinkContainer > myVecTPConv;
    myVecTPConv.persToTrans( pers, trans, log ); 
  }

  /** The method to convert the transient to the persistent state */
  void transToPers(const LinkVect_t* trans, PersLinkVect_t* pers, MsgStream& log) 
  { 
    VectorElementLinkCnv_p1< INav4MomLinkContainer > myVecTPConv;
    myVecTPConv.transToPers( trans, pers, log ); 
  }

  
  /** The method to convert the persistent to the transient state, for references */
  void persToTrans(const PersLinkVect_t& pers, LinkVect_t& trans, MsgStream& log)
  {
    VectorElementLinkCnv_p1< INav4MomLinkContainer > myVecTPConv;
    myVecTPConv.persToTrans( pers, trans, log );
  }

  /** The method to convert the transient to the persistent state, for references */
  void transToPers(const LinkVect_t& trans, PersLinkVect_t& pers, MsgStream& log)
  {
    VectorElementLinkCnv_p1< INav4MomLinkContainer > myVecTPConv;
    myVecTPConv.transToPers( trans, pers, log );
  }

};


#endif // EVENTCOMMONTPCNV_INAV4MOMLINKCONTAINERCNV_P1_H
