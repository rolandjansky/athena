/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ARATOOLBASE_H
#define ARATOOLBASE_H

#include "GaudiKernel/Property.h"
#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/IAlgTool.h"
#include <string>


class AraToolBase
{
 protected:
  PropertyMgr *m_propertyMgr;
  bool RunningARA;

 public:


  AraToolBase(PropertyMgr * pmgr=0);

  virtual ~AraToolBase();


  template<class TYPE>
    Property* declareProperty
    ( const std::string&       name  ,
      TYPE&                    value,
      const std::string&       doc = "none" ) ;

  PropertyMgr* getPropertyMgr() {return m_propertyMgr;}

 public:

};

template<class TYPE>
inline Property *
AraToolBase::declareProperty
( const std::string& name  ,
  TYPE&              value,
  const std::string& doc )
{

  if(RunningARA) {
  } else {
    return m_propertyMgr->declareProperty(name, value, doc);
  }

  return 0;
}

#endif

