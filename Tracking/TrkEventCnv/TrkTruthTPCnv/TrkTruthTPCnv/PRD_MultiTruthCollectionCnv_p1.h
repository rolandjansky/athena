/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-

// Andrei.Gaponenko@cern.ch, 2007


#ifndef PRD_MULTITRUTHCOLLECTIONCNV_P1_H 
#define PRD_MULTITRUTHCOLLECTIONCNV_P1_H 

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "StoreGate/StoreGateSvc.h"
#include "InDetIdentifier/PixelID.h"
#include "TrkTruthTPCnv/PRD_MultiTruthCollection_p1.h"

class MsgStream;
class PRD_MultiTruthCollection;
namespace Trk { class PRD_MultiTruthCollection_p1; }


class PRD_MultiTruthCollectionCnv_p1 
  : public T_AthenaPoolTPCnvBase<PRD_MultiTruthCollection,
				 Trk::PRD_MultiTruthCollection_p1>
{ 
public: 
  
  /** Method creating the transient representation of @c Analysis::PRD_MultiTruthCollection
   *  from its persistent representation @c PRD_MultiTruthCollection_p1
   */
  PRD_MultiTruthCollectionCnv_p1(): m_pixId{},m_storeGate{},m_isInitialized(0)  {};

  virtual void persToTrans( const Trk::PRD_MultiTruthCollection_p1* persObj, 
                            PRD_MultiTruthCollection* transObj, 
                            MsgStream& msg );
  
  /** Method creating the persistent representation @c PRD_MultiTruthCollection_p1
   *  from its transient representation @c Analysis::PRD_MultiTruthCollection
   */
  virtual void transToPers( const PRD_MultiTruthCollection* transObj, 
                            Trk::PRD_MultiTruthCollection_p1* persObj, 
                            MsgStream& msg );
private:

  const PixelID *m_pixId;
  StoreGateSvc *m_storeGate;
  bool m_isInitialized;
  StatusCode initialize(MsgStream &log);


}; 

#endif/*PRD_MULTITRUTHCOLLECTIONCNV_P1_H*/
