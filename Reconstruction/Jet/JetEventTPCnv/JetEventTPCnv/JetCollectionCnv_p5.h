///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// JetCollectionCnv_p5.h 
// Header file for class JetCollectionCnv_p5
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef JETEVENTTPCNV_JETCOLLECTIONCNV_P5_H
#define JETEVENTTPCNV_JETCOLLECTIONCNV_P5_H

// STL includes

// Gaudi includes

// JetEvent includes
//#include "JetEvent/JetCollection.h"

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// JetEventTPCnv includes
//#include "JetEventTPCnv/JetCollection_p5.h"
//#include "JetEventTPCnv/JetCnv_p5.h"

// forward declares
class JetCollection;

class JetCollectionCnv_p5 : public T_AthenaPoolTPCnvConstBase<
                                       JetCollection, 
                                       JetCollection_p5
                                   >  
{ 
 public: 
  using base_class::transToPers;
  using base_class::persToTrans;


  /** Default constructor: 
   */
  JetCollectionCnv_p5();


  /** Method creating the transient representation of @c JetCollection
   *  from its persistent representation @c JetCollection_p4
   */
  virtual void persToTrans( const JetCollection_p5* pers, 
                            JetCollection* trans, 
                            MsgStream& msg ) const override;

  /** Method creating the persistent representation @c JetCollection_p1
   *  from its transient representation @c JetCollection
   */
  virtual void transToPers( const JetCollection* trans, 
                            JetCollection_p5* pers, 
                            MsgStream& msg ) const override;
}; 


inline JetCollectionCnv_p5::JetCollectionCnv_p5()
{}

#endif //> JETEVENTTPCNV_JETCOLLECTIONCNV_P5_H
