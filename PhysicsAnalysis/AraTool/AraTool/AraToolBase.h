/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ARATOOLBASE_H
#define ARATOOLBASE_H

#include "Gaudi/Property.h"
#include "GaudiKernel/PropertyHolder.h"
#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/IDataHandleHolder.h"
#include "GaudiKernel/CommonMessaging.h"
#include <string>


class AraToolBase
{
public:
  typedef PropertyHolder<CommonMessaging<implements<IAlgTool, IDataHandleHolder, IProperty, IStateful> > > PropertyMgr;

protected:
  bool runningARA() const { return m_runningARA; }

private:
  PropertyMgr *m_propertyMgr;
  bool m_runningARA;


public:


  AraToolBase(PropertyMgr * pmgr=0);

  virtual ~AraToolBase();


  template<class TYPE>
  Gaudi::Details::PropertyBase* declareProperty
    ( const std::string&       name  ,
      TYPE&                    value,
      const std::string&       doc = "none" ) ;

  PropertyMgr* getPropertyMgr() {return m_propertyMgr;}

 public:

};

template<class TYPE>
inline Gaudi::Details::PropertyBase *
AraToolBase::declareProperty
( const std::string& name  ,
  TYPE&              value,
  const std::string& doc )
{

  if(m_runningARA) {
  } else {
    return m_propertyMgr->declareProperty(name, value, doc);
  }

  return 0;
}

#endif

