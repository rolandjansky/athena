/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ROOTCOLLECTION_ATTRIBUTELISTLAYOUT_H
#define ROOTCOLLECTION_ATTRIBUTELISTLAYOUT_H

#include "TObject.h"
#include <string>
#include <vector>

namespace pool {
   class ICollectionDescription;
   class CollectionDescription;
}


struct AttributeListLayout :  public TObject
{  
  AttributeListLayout();
  virtual ~AttributeListLayout();

  AttributeListLayout( const pool::ICollectionDescription& desc );

  void fillDescription( pool::CollectionDescription& desc );

  void print() const;

  
  std::vector< std::pair<std::string, std::string> >    m_layout;
  std::vector< std::string >                            m_annotations;
  std::string                                           m_eventRefColumnName;
  
  ClassDef(AttributeListLayout,3);
};

#endif
