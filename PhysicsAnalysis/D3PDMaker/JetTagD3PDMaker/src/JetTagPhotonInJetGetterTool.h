/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JetTagD3PDMaker_JetTagPhotonInJetGetterTool_H
#define JetTagD3PDMaker_JetTagPhotonInJetGetterTool_H

#include "D3PDMakerUtils/CollectionGetterTool.h" 
#include "JetEvent/JetCollection.h"
#include "egammaEvent/PhotonAssociation.h"

#include <vector>
#include <set>

namespace D3PD { 



class JetTagPhotonInJetGetterTool 
  : public CollectionGetterTool<std::vector<const JetCollection*> >
{ 
 public: 
  typedef CollectionGetterTool<std::vector<const JetCollection*> > Base; 
  JetTagPhotonInJetGetterTool (const std::string& type, 
				    const std::string& name, 
				    const IInterface* parent); 

  virtual StatusCode initialize();
  virtual const std::vector<const JetCollection*>* get (bool allowMissing=false);
  virtual size_t sizeHint(bool=false); 
  virtual StatusCode reset(bool=false); 
  virtual const void* nextUntyped(); 
  virtual const std::type_info& typeinfo() const; 
  virtual const std::type_info& elementTypeinfo() const; 
  
 private: 

  std::vector<const JetCollection*>* createObject(bool allowMissing);
  void releaseContainers();
  bool selectPhoton(const Analysis::Photon* elec);


  std::vector<const JetCollection*>::const_iterator m_colItr;
  std::vector<const JetCollection*>::const_iterator m_colEnd;

  JetCollection::const_iterator m_jetItr;
  JetCollection::const_iterator m_jetEnd; 
  
  Navigable<PhotonContainer,double>::object_iter m_phItr;
  Navigable<PhotonContainer,double>::object_iter m_phEnd;
  
  std::set<const Analysis::Photon*> m_inserted;

  std::vector<std::string> m_jetCollections;
  std::vector<const JetCollection*>* m_objects;

  bool m_created;

}; 

} 

#endif // JetTagD3PDMaker_JetTagPhotonInJetGetterTool_H
