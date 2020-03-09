///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// JetCollectionCnv_p4.h 
// Header file for class JetCollectionCnv_p1
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef JETEVENTTPCNV_JETCOLLECTIONCNV_P4_H 
#define JETEVENTTPCNV_JETCOLLECTIONCNV_P4_H 

// STL includes

// Gaudi includes

// JetEvent includes
//#include "JetEvent/JetCollection.h"

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// JetEventTPCnv includes
//#include "JetEventTPCnv/JetCollection_p4.h"
//#include "JetEventTPCnv/JetCnv_p4.h"

// forward declares
class JetCollection;

class JetCollectionCnv_p4 : public T_AthenaPoolTPCnvConstBase<
                                       JetCollection, 
                                       JetCollection_p4
                                   >  
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 
  using base_class::transToPers;
  using base_class::persToTrans;


  /** Default constructor: 
   */
  JetCollectionCnv_p4();


  /** Method creating the transient representation of @c JetCollection
   *  from its persistent representation @c JetCollection_p4
   */
  virtual void persToTrans( const JetCollection_p4* pers, 
                            JetCollection* trans, 
                            MsgStream& msg ) const override;

  /** Method creating the persistent representation @c JetCollection_p1
   *  from its transient representation @c JetCollection
   */
  virtual void transToPers( const JetCollection* trans, 
                            JetCollection_p4* pers, 
                            MsgStream& msg ) const override;
}; 


inline JetCollectionCnv_p4::JetCollectionCnv_p4()
{}

#endif //> JETEVENTTPCNV_JETCOLLECTIONCNV_P4_H
