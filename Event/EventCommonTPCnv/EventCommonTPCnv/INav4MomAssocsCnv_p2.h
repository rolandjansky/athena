///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// INav4MomAssocsCnv_p2.h 
// Header file for class INav4MomAssocsCnv_p2
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef EVENTCOMMONTPCNV_INAV4MOMASSOCSCNV_P2_H 
#define EVENTCOMMONTPCNV_INAV4MOMASSOCSCNV_P2_H 

// STL includes


// Gaudi includes

// DataModel includes
#include "AthLinks/ElementLink.h"

// DataModelAthenaPool includes
#include "DataModelAthenaPool/ElementLinkCnv_p2.h"
#include "DataModelAthenaPool/DataLinkCnv_p1.h"

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "AthenaPoolCnvSvc/ITPConverter.h"

// NavFourMom includes
#include "NavFourMom/INavigable4MomentumCollection.h"
#include "NavFourMom/INav4MomAssocs.h"

// EventCommonTPCnv includes
#include "EventCommonTPCnv/INav4MomAssocs_p2.h"

// Forward declaration
class MsgStream;

class INav4MomAssocsCnv_p2 : public T_AthenaPoolTPCnvConstBase<
                                              INav4MomAssocs, 
                                              INav4MomAssocs_p2
                                            >  
{ 
 public: 
  using base_class::transToPers; 
  using base_class::persToTrans;

  /// converter for @c ElementLink to @c INavigable4MomentumCollection
  typedef ElementLinkCnv_p2< ElementLink<INavigable4MomentumCollection> >
          INav4MomCnv_t;
  typedef DataLinkCnv_p1<DataLink<INav4MomAssocs> > IAssocStoresCnv_t;

 public: 

  /** Default constructor: 
   */
  INav4MomAssocsCnv_p2();

  /** Destructor: 
   */
  virtual ~INav4MomAssocsCnv_p2();


  /** Method creating the transient representation of @c INav4MomAssocs
   *  from its persistent representation @c INav4MomAssocs_p2
   */
  virtual void persToTrans( const INav4MomAssocs_p2* pers, 
                            INav4MomAssocs* trans, 
                            MsgStream& msg ) const override;

  /** Method creating the persistent representation @c INav4MomAssocs_p2
   *  from its transient representation @c INav4MomAssocs
   */
  virtual void transToPers( const INav4MomAssocs* trans, 
                            INav4MomAssocs_p2* pers, 
                            MsgStream& msg ) const override;

protected: 
  /// converter for @c ElementLink to @c INavigable4MomentumCollection
  INav4MomCnv_t m_inav4MomLinkCnv;

  /// converter for @c DataLink to @c INav4MomAssocs
  IAssocStoresCnv_t m_assocStoresCnv;
}; 


inline INav4MomAssocsCnv_p2::INav4MomAssocsCnv_p2()
{}

inline INav4MomAssocsCnv_p2::~INav4MomAssocsCnv_p2()
{}

#endif //> EVENTCOMMONTPCNV_INAV4MOMASSOCSCNV_P2_H
