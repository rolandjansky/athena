/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-

// Andrei.Gaponenko@cern.ch, 2007
// Olivier.Arnaez@cern.ch, 2015

#ifndef PRD_MULTITRUTHCOLLECTIONCNV_P4_H 
#define PRD_MULTITRUTHCOLLECTIONCNV_P4_H 

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class MsgStream;
class PRD_MultiTruthCollection;
namespace Trk { class PRD_MultiTruthCollection_p4; }


class PRD_MultiTruthCollectionCnv_p4 
  : public T_AthenaPoolTPCnvBase<PRD_MultiTruthCollection,
				 Trk::PRD_MultiTruthCollection_p4>
{ 
public: 
  
  /** Method creating the transient representation of @c Analysis::PRD_MultiTruthCollection
   *  from its persistent representation @c PRD_MultiTruthCollection_p4
   */
  virtual void persToTrans( const Trk::PRD_MultiTruthCollection_p4* persObj, 
                            PRD_MultiTruthCollection* transObj, 
                            MsgStream& msg );
  
  /** Method creating the persistent representation @c PRD_MultiTruthCollection_p4
   *  from its transient representation @c Analysis::PRD_MultiTruthCollection
   */
  virtual void transToPers( const PRD_MultiTruthCollection* transObj, 
                            Trk::PRD_MultiTruthCollection_p4* persObj, 
                            MsgStream& msg );

}; 

#endif/*PRD_MULTITRUTHCOLLECTIONCNV_P4_H*/
