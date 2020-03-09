///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// JetSamplingCollectionCnv_p1.h 
// Header file for class JetSamplingCollectionCnv_p1
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef JETEVENTTPCNV_JETSAMPLINGCOLLECTIONCNV_P1_H 
#define JETEVENTTPCNV_JETSAMPLINGCOLLECTIONCNV_P1_H 

// STL includes

// Gaudi includes

// JetEvent includes
#include "JetEvent/JetSamplingCollection.h"

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// JetEventTPCnv includes
//#include "JetEventTPCnv/JetSamplingCollection_p1.h"
//#include "JetEventTPCnv/JetSamplingCnv_p1.h"

// forward declares
class JetSamplingCollection_p1;

class JetSamplingCollectionCnv_p1 : public T_AthenaPoolTPCnvConstBase<
                                       JetSamplingCollection, 
                                       JetSamplingCollection_p1
                                   >  
{ 
 public: 
  using base_class::transToPers;
  using base_class::persToTrans;


  /** Default constructor: 
   */
  JetSamplingCollectionCnv_p1();


  /** Method creating the transient representation of @c JetSamplingCollection
   *  from its persistent representation @c JetSamplingCollection_p1
   */
  virtual void persToTrans( const JetSamplingCollection_p1* pers, 
                            JetSamplingCollection* trans, 
                            MsgStream& msg ) const override;

  /** Method creating the persistent representation @c JetSamplingCollection_p1
   *  from its transient representation @c JetSamplingCollection
   */
  virtual void transToPers( const JetSamplingCollection* trans, 
                            JetSamplingCollection_p1* pers, 
                            MsgStream& msg ) const override;
}; 


inline JetSamplingCollectionCnv_p1::JetSamplingCollectionCnv_p1()
{}

#endif //> JETEVENTTPCNV_JETSAMPLINGCOLLECTIONCNV_P1_H
