/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -*-c++-*-

///////////////////////////////////////////////////////////////////
// TopInputMapCnv_p1.h 
// Header file for class TopInputMapCnv_p1, handles transient<->persistent TopInputMap
// Template class, must be inherited by the class handling instantiations of TopInputMap
/////////////////////////////////////////////////////////////////// 

#ifndef TOPINPUTSTPCNV_TOPINPUTMAPCNV_P1_H
#define TOPINPUTSTPCNV_TOPINPUTMAPCNV_P1_H
 
// AthenaPoolCnvSvc includes
#include "DataModelAthenaPool/ElementLinkVectorCnv_p1.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "GaudiKernel/MsgStream.h"
#include "TopInputs/TopInputMap.h"
#include "TopInputs/TopOverlapVector.h"
#include "TopInputsTPCnv/TopOverlapVectorCnv_p1.h"
#include "TopInputsTPCnv/TopInputMap_p1.h"
#include <string>

template<class Container> 
class TopInputMapCnv_p1 : public T_AthenaPoolTPCnvBase<
  TopRec::TopInputMap<Container>, 
  TopInputMap_p1
  >  
{ 
  
  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
public: 
  
  TopInputMapCnv_p1(){;}
  virtual ~TopInputMapCnv_p1(){;}
  
  /////////////////////////////////////////////////////////////////// 
  // Conversion methods: 
  ///////////////////////////////////////////////////////////////////
  
  virtual 
  void persToTrans( const TopInputMap_p1* persObj, 
		    TopRec::TopInputMap<Container>* transObj, 
		    MsgStream &msg );
  
  /* Method creating the persistent representation @c Decay_p1
   *  from its transient representation @c Decay
   */
  virtual 
  void transToPers( const TopRec::TopInputMap<Container>* transObj, 
		    TopInputMap_p1* persObj, 
		    MsgStream &msg );

private:
  ElementLinkVectorCnv_p1< ElementLinkVector<Container> > m_elv_cnv;
  TopOverlapVectorCnv_p1 m_tov_cnv;
  
}; 

template<class Container>
void TopInputMapCnv_p1<Container>::transToPers(const TopRec::TopInputMap<Container>* transObj, 
					       TopInputMap_p1* persObj, 
					       MsgStream &msg ){
  msg<<MSG::DEBUG<<typeid(*transObj).name()<<" called"<<endreq;

  ElementLinkVector<Container> elv;
  ElementLinkIntVector_p1      eliv_p1;
  elv.reserve(transObj->size());

  TopRec::TopOverlapVector tov;
  TopOverlapVector_p1      tov_p1;
  tov.reserve(transObj->size());

  typedef TopRec::TopInputMap<Container> Map;
  typename Map::const_iterator ilItr = transObj->begin();
  typename Map::const_iterator ilEnd = transObj->end();
  for(; ilItr!=ilEnd; ++ilItr) {
    elv.push_back(ilItr->first);
    TopRec::TopOverlap to = ilItr->second;
    to.setIndex(ilItr->first.index());
    tov.push_back(to);
  }

  m_elv_cnv.transToPers(&elv,&eliv_p1,msg);
  m_tov_cnv.transToPers(&tov,&tov_p1,msg);
  
  persObj->m_eliv_p1=eliv_p1;
  persObj->m_tov_p1=tov_p1;

  msg << MSG::DEBUG << "Created TopInputMap_p1, persistent state of "<<typeid(*transObj).name()<<endreq;
  
}

template<class Container>
void TopInputMapCnv_p1<Container>::persToTrans(const TopInputMap_p1* persObj, 
					       TopRec::TopInputMap<Container>* transObj, 
					       MsgStream &msg ){
  msg<<MSG::DEBUG<<"TopInputMapCnv_p1::persToTrans() called"<<endreq;

  ElementLinkVector<Container> elv;
  ElementLinkIntVector_p1      eliv_p1=persObj->m_eliv_p1;
  m_elv_cnv.persToTrans(&eliv_p1,&elv,msg);

  TopRec::TopOverlapVector tov;
  TopOverlapVector_p1      tov_p1=persObj->m_tov_p1;
  m_tov_cnv.persToTrans(&tov_p1,&tov,msg);

  typedef ElementLinkVector<Container> ElementLinkVec;
  typedef TopRec::TopOverlapVector     TopOverlapVec;

  typename ElementLinkVec::const_iterator ilItr = elv.begin();
  typename ElementLinkVec::const_iterator ilEnd = elv.end();
  for(; ilItr!=ilEnd; ++ilItr) {
    ElementLink<Container> el = *ilItr;
    TopRec::TopOverlap to;
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

  msg<<MSG::DEBUG<<"Loading "<<typeid(*transObj).name()<<" from TopInputMap_p1"<<endreq;
}


#endif // not TOPINPUTSTPCNV_TOPINPUTMAPCNV_P1_H
