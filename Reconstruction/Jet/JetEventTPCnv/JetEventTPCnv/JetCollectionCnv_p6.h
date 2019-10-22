///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// JetCollectionCnv_p6.h 
// Header file for class JetCollectionCnv_p6
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef JETEVENTTPCNV_JETCOLLECTIONCNV_P6_H
#define JETEVENTTPCNV_JETCOLLECTIONCNV_P6_H

// STL includes

// Gaudi includes

// JetEvent includes
//#include "JetEvent/JetCollection.h"

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// JetEventTPCnv includes
//#include "JetEventTPCnv/JetCollection_p6.h"
//#include "JetEventTPCnv/JetCnv_p6.h"

// forward declares
class JetCollection;

class JetCollectionCnv_p6 : public T_AthenaPoolTPCnvConstBase<
                                       JetCollection, 
                                       JetCollection_p6
                                   >  
{ 
 public: 
  using base_class::transToPers;
  using base_class::persToTrans;


  /** Default constructor: 
   */
  JetCollectionCnv_p6();


  /** Method creating the transient representation of @c JetCollection
   *  from its persistent representation @c JetCollection_p4
   */
  virtual void persToTrans( const JetCollection_p6* pers, 
                            JetCollection* trans, 
                            MsgStream& msg ) const override;

  /** Method creating the persistent representation @c JetCollection_p1
   *  from its transient representation @c JetCollection
   */
  virtual void transToPers( const JetCollection* trans, 
                            JetCollection_p6* pers, 
                            MsgStream& msg ) const override;
}; 


inline JetCollectionCnv_p6::JetCollectionCnv_p6()
{}

#endif //> JETEVENTTPCNV_JETCOLLECTIONCNV_P6_H
