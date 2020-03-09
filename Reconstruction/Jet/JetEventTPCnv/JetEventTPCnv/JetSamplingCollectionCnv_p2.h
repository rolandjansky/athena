///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// JetSamplingCollectionCnv_p2.h 
// Header file for class JetSamplingCollectionCnv_p2
// Author: Belen.Salvachua@cern.ch
// Date: Dec 2008
//    Based on JetSamplingCollectionCnv_p1
/////////////////////////////////////////////////////////////////// 
#ifndef JETEVENTTPCNV_JETSAMPLINGCOLLECTIONCNV_P2_H 
#define JETEVENTTPCNV_JETSAMPLINGCOLLECTIONCNV_P2_H 

// STL includes

// Gaudi includes

// JetEvent includes
#include "JetEvent/JetSamplingCollection.h"

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// JetEventTPCnv includes
//#include "JetEventTPCnv/JetSamplingCollection_p2.h"
//#include "JetEventTPCnv/JetSamplingCnv_p2.h"

// forward declares
class JetSamplingCollection_p2;

class JetSamplingCollectionCnv_p2 : public T_AthenaPoolTPCnvConstBase<
                                       JetSamplingCollection, 
                                       JetSamplingCollection_p2
                                   >  
{ 
 public: 
  using base_class::transToPers;
  using base_class::persToTrans;


  /** Default constructor: 
   */
  JetSamplingCollectionCnv_p2();


  /** Method creating the transient representation of @c JetSamplingCollection
   *  from its persistent representation @c JetSamplingCollection_p2
   */
  virtual void persToTrans( const JetSamplingCollection_p2* pers, 
                            JetSamplingCollection* trans, 
                            MsgStream& msg ) const override;

  /** Method creating the persistent representation @c JetSamplingCollection_p2
   *  from its transient representation @c JetSamplingCollection
   */
  virtual void transToPers( const JetSamplingCollection* trans, 
                            JetSamplingCollection_p2* pers, 
                            MsgStream& msg ) const override;
}; 


inline JetSamplingCollectionCnv_p2::JetSamplingCollectionCnv_p2()
{}

#endif //> JETEVENTTPCNV_JETSAMPLINGCOLLECTIONCNV_P2_H
