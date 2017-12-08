/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALODETDESCR_MBTSDETDESCRMANAGER_H
#define CALODETDESCR_MBTSDETDESCRMANAGER_H

#include "CLIDSvc/CLASS_DEF.h"
#include "Identifier/Identifier.h"
#include <map>

class CaloDetDescrElement;
class IMessageSvc;

class MbtsDetDescrManager
{
 public:
  MbtsDetDescrManager();
  ~MbtsDetDescrManager();

  CaloDetDescrElement* get_element(const Identifier& elementId) const;

  void add(CaloDetDescrElement* element);

 private:
  typedef std::map<Identifier, CaloDetDescrElement*> MbtsElements;
  MbtsElements m_elements;

  IMessageSvc*  m_msgSvc;
};

CLASS_DEF( MbtsDetDescrManager , 66448502 , 1 )

#endif
