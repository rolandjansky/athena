/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file D3PDMaker/JetTagD3PDMaker/src/JetTagMSVVtxInfoGetterTool.h
 * @author Georges Aad
 * @date Apr, 2011
 * @brief getter for MSVVtxInfo object inside jets
 * 
 */



#ifndef JetTagD3PDMaker_JetTagMSVVtxInfoGetterTool_H
#define JetTagD3PDMaker_JetTagMSVVtxInfoGetterTool_H

#include "JetTagFinder.h"
#include "D3PDMakerUtils/CollectionGetterTool.h" 
#include "JetEvent/JetCollection.h"

#include <vector>


namespace Analysis{
  class MultiSVInfoPlus;
}

namespace D3PD { 



class JetTagMSVVtxInfoGetterTool 
  : public CollectionGetterTool<std::vector<const JetCollection*> >
{ 
 public: 
  typedef CollectionGetterTool<std::vector<const JetCollection*> > Base; 
  JetTagMSVVtxInfoGetterTool (const std::string& type, 
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
  JetTagFinder m_finder;

  std::vector<const JetCollection*>* createObject(bool allowMissing);
  void releaseContainers();
  const Analysis::MultiSVInfoPlus* getMultiSVInfoPlus(const Jet* jet);


  std::vector<const JetCollection*>::const_iterator m_colItr;
  std::vector<const JetCollection*>::const_iterator m_colEnd;

  JetCollection::const_iterator m_jetItr;
  JetCollection::const_iterator m_jetEnd; 
    
  std::vector<std::string> m_jetCollections;
  std::vector<const JetCollection*>* m_objects;

  unsigned int m_iMSVVtx;
  unsigned int m_eMSVVtx;

  bool m_created;
}; 

} 

#endif // JetTagD3PDMaker_JetTagMSVVtxInfoGetterTool_H
