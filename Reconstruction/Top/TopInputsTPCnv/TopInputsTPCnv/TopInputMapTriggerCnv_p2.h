/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -*-c++-*-

///////////////////////////////////////////////////////////////////
// TopInputMapTriggerCnv_p2.h 
// Header file for class TopInputMapTriggerCnv_p2, handles transient<->persistent TopInputMapTrigger
// Template class, must be inherited by the class handling instantiations of TopInputMapTrigger
/////////////////////////////////////////////////////////////////// 

#ifndef TOPINPUTSTPCNV_TOPINPUTMAPTRIGGERCNV_P2_H
#define TOPINPUTSTPCNV_TOPINPUTMAPTRIGGERCNV_P2_H
 
// AthenaPoolCnvSvc includes
#include "DataModelAthenaPool/ElementLinkVectorCnv_p1.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "GaudiKernel/MsgStream.h"
#include "TopInputs/TopInputMapTrigger.h"
#include "TopInputs/TopOverlapTriggerVector.h"
#include "TopInputsTPCnv/TopOverlapTriggerVectorCnv_p1.h"
#include "TopInputsTPCnv/TopInputMapTrigger_p1.h"
#include <string>

template<class Container> 
class TopInputMapTriggerCnv_p2 : public T_AthenaPoolTPCnvBase<
  TopRec::TopInputMapTrigger<Container>, 
  TopInputMapTrigger_p1
  >  
{ 
  
  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
public: 
  
  TopInputMapTriggerCnv_p2(){;}
  virtual ~TopInputMapTriggerCnv_p2(){;}
  
  /////////////////////////////////////////////////////////////////// 
  // Conversion methods: 
  ///////////////////////////////////////////////////////////////////
  
  virtual 
  void persToTrans( const TopInputMapTrigger_p1* persObj, 
		    TopRec::TopInputMapTrigger<Container>* transObj, 
		    MsgStream &msg );
  
  /* Method creating the persistent representation @c Decay_p1
   *  from its transient representation @c Decay
   */
  virtual 
  void transToPers( const TopRec::TopInputMapTrigger<Container>* transObj, 
		    TopInputMapTrigger_p1* persObj, 
		    MsgStream &msg );

private:
  ElementLinkVectorCnv_p1< ElementLinkVector<Container> > m_elv_cnv;
  TopOverlapTriggerVectorCnv_p1 m_tov_cnv;
  
}; 

template<class Container>
void TopInputMapTriggerCnv_p2<Container>::transToPers(const TopRec::TopInputMapTrigger<Container>* transObj, 
					       	      TopInputMapTrigger_p1* persObj, 
						      MsgStream &msg ){
  msg<<MSG::DEBUG<<typeid(*transObj).name()<<" called"<<endreq;

  ElementLinkVector<Container> elv;
  ElementLinkIntVector_p1      eliv_p1;
  elv.reserve(transObj->size());

  TopRec::TopOverlapTriggerVector tov;
  TopOverlapTriggerVector_p1      tov_p1;
  tov.reserve(transObj->size());

  typedef TopRec::TopInputMapTrigger<Container> Map;
  typename Map::const_iterator ilItr = transObj->begin();
  typename Map::const_iterator ilEnd = transObj->end();
  for(; ilItr!=ilEnd; ++ilItr) {
    elv.push_back(ilItr->first);
    TopRec::TopOverlapTrigger to = ilItr->second;
    to.setIndex(ilItr->first.index());
    tov.push_back(to);
  }

  m_elv_cnv.transToPers(&elv,&eliv_p1,msg);
  m_tov_cnv.transToPers(&tov,&tov_p1,msg);
  
  persObj->m_eliv_p1=eliv_p1;
  persObj->m_tov_p1=tov_p1;

  msg << MSG::DEBUG << "Created TopInputMapTrigger_p1, persistent state of "<<typeid(*transObj).name()<<endreq;
  
}

template<class Container>
void TopInputMapTriggerCnv_p2<Container>::persToTrans(const TopInputMapTrigger_p1* persObj, 
					       	      TopRec::TopInputMapTrigger<Container>* transObj, 
					       	      MsgStream &msg ){
  msg<<MSG::DEBUG<<"TopInputMapTriggerCnv_p2::persToTrans() called"<<endreq;

  ElementLinkVector<Container> elv;
  ElementLinkIntVector_p1      eliv_p1=persObj->m_eliv_p1;
  m_elv_cnv.persToTrans(&eliv_p1,&elv,msg);

  TopRec::TopOverlapTriggerVector tov;
  TopOverlapTriggerVector_p1      tov_p1=persObj->m_tov_p1;
  m_tov_cnv.persToTrans(&tov_p1,&tov,msg);

  typedef ElementLinkVector<Container> 		ElementLinkVec;
  typedef TopRec::TopOverlapTriggerVector	TopOverlapVec;

  typename ElementLinkVec::const_iterator ilItr = elv.begin();
  typename ElementLinkVec::const_iterator ilEnd = elv.end();
  for(; ilItr!=ilEnd; ++ilItr) {
    ElementLink<Container> el = *ilItr;
    TopRec::TopOverlapTrigger to;
    unsigned ipart=el.index();
    TopOverlapVec::const_iterator toItr = tov.begin();
    TopOverlapVec::const_iterator toEnd = tov.end();
    for(; toItr!=toEnd; ++toItr) {
      if(toItr->index()==ipart) {
	to=(*toItr);
      }
    }
    transObj->insert(std::make_pair(el,to));
  }

  msg<<MSG::DEBUG<<"Loading "<<typeid(*transObj).name()<<" from TopInputMapTrigger_p1"<<endreq;
}


#endif // TOPINPUTSTPCNV_TOPINPUTMAPTRIGGERCNV_P2_H
