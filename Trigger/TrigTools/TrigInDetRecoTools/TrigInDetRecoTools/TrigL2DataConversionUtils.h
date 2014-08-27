/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __TRIG_L2_DATA_CONVERSION_UTILS_H__
#define __TRIG_L2_DATA_CONVERSION_UTILS_H__

//#include "VxVertex/Vertex.h"
#include "TrigInDetEvent/TrigVertex.h"
#include "TrkSpacePoint/SpacePoint.h"
#include "SiSpacePoint/PixelSpacePoint.h"
#include "SiSpacePoint/SCT_SpacePoint.h"

namespace Trk {
  class Vertex;
}


class TrigL2PixelSpacePointConverter {
 public:
 TrigL2PixelSpacePointConverter(SpacePointCollection* p) : m_outputCollection(p) {};
  ~TrigL2PixelSpacePointConverter() {};
  void operator() (const TrigSiSpacePoint* p) { 
    const IdentContIndex ici1 = p->clusters().first->getHashAndIndex();
    Trk::SpacePoint* sp = new InDet::PixelSpacePoint(ici1.collHash(), p->clusters().first); 
    m_outputCollection->push_back(sp);
  } 
 private:
  SpacePointCollection* m_outputCollection;
};

class TrigL2SCT_SpacePointConverter {
 public:
  TrigL2SCT_SpacePointConverter(SpacePointCollection* p) : m_outputCollection(p) {};
  ~TrigL2SCT_SpacePointConverter() {};
  void operator() (const TrigSiSpacePoint* p) {

    // 1. test if LVL2 spacepoint is on surface
    Amg::Vector3D glPos(p->original_x(), p->original_y(), p->z());
    const Amg::Vector2D *locpos = p->clusters().first->detectorElement()->surface().globalToLocal(glPos) ;
    if(locpos==NULL) return;
    delete locpos;
    // 2. SP is OK - converting into offline one 
    Amg::Vector3D* point = new Amg::Vector3D(p->original_x(), p->original_y(), p->z() );   
    const IdentContIndex ici1 = p->clusters().first->getHashAndIndex();
    const IdentContIndex ici2 = p->clusters().second->getHashAndIndex();
    const std::pair<IdentifierHash,IdentifierHash>  elementIdList(ici1.collHash() , ici2.collHash() ); 
    const std::pair<const Trk::PrepRawData*,const Trk::PrepRawData*>* clusList = 
      new std::pair<const Trk::PrepRawData*,const Trk::PrepRawData*>(p->clusters().first,p->clusters().second);
    Trk::SpacePoint* sp = new InDet::SCT_SpacePoint(elementIdList,point,clusList); 
    m_outputCollection->push_back(sp);
  }
 private:
  SpacePointCollection* m_outputCollection;
};

template<class T>
class TrigL2SpacePointCollectionConverter {
 public:
 TrigL2SpacePointCollectionConverter(SpacePointContainer* pO,  const TrigSiSpacePointContainer* pI) : m_outputContainer(pO),  m_inputContainer(pI) {};
  ~TrigL2SpacePointCollectionConverter() {};
  void operator() (const TrigSiSpacePointCollection* pColl) {
    if(pColl->empty()) return;
    IdentifierHash idHash = pColl->identifyHash();  
    if(m_outputContainer->end() != m_outputContainer->indexFind(idHash)) return;
    Identifier elementID = pColl->identify(); 
    SpacePointCollection* spacepointCollection = new SpacePointCollection(idHash);  
    spacepointCollection->setIdentifier(elementID);  
    spacepointCollection->reserve(pColl->size());
    T spConverter(spacepointCollection);
    std::for_each(pColl->begin(),pColl->end(),spConverter);
    if (spacepointCollection->size() == 0){ 
      delete spacepointCollection;
      return;
    } 
    StatusCode sc= m_outputContainer->addCollection( spacepointCollection, spacepointCollection->identifyHash() ); 
    if (sc.isSuccess()){ 
      //	athenaLog << MSG::INFO << spacepointCollection->size() <<" SCT SpacePoints successfully added to Container ! " <<(*idIt) << endreq;
    } 
    else{ 
      //athenaLog << MSG::WARNING << "Failed to add SCT SpacePoints to container" << endreq; 
      delete spacepointCollection; 
    }
  }
  void operator() (IdentifierHash id) {
    TrigSiSpacePointContainer::const_iterator it =  m_inputContainer->indexFind(id);
    if(it!=m_inputContainer->end()) {
      (*this)((*it));
    }
  }
 private:
  SpacePointContainer* m_outputContainer;
  const TrigSiSpacePointContainer* m_inputContainer;
};


class ZVertexCopyFunctor {
 public: 
  ZVertexCopyFunctor(std::vector<float>& v) : m_vec(v) {
    m_vec.clear();
  }
  void operator()(const Trk::Vertex& v) {
    m_vec.push_back(v.position().z());
  }
  void operator()(const TrigVertex& v) {
    m_vec.push_back(v.z());
  }
 private:
  std::vector<float>& m_vec;
};



#endif
