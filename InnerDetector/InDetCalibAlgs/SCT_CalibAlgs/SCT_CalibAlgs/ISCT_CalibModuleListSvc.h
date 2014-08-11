/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
* @file SCT_CalibModuleListSvc.h
* Header file for the SCT_CalibModuleListSvc class
* @author Shaun Roe
**/

#ifndef ISCT_CalibModuleListSvc_h
#define ISCT_CalibModuleListSvc_h

// STL and boost headers
#include <string>
#include <vector>
#include <set>
#include <map>
#include <utility>

#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/ServiceHandle.h"
#include "Identifier/Identifier.h"

class ISCT_CalibModuleListSvc:virtual public IInterface{
public:
  //@name Service methods, reimplemented
   //@{
  ISCT_CalibModuleListSvc(){/**nop**/}; 
  virtual ~ISCT_CalibModuleListSvc(){/**nop**/};
  virtual StatusCode initialize()=0;
  virtual StatusCode finalize()=0;
  static const InterfaceID & interfaceID();
   //@}
  virtual StatusCode readModuleList( std::map< Identifier, std::set<Identifier> >& moduleList )=0;
   
protected:
  template<class S>
  std::pair<std::string, bool> retrievedService(S & service){
     if (service.retrieve().isFailure() ) return std::make_pair(std::string("Unable to retrieve ")+service.name(), false);
     return std::make_pair("",true);
   }
 };
 
 inline const InterfaceID & ISCT_CalibModuleListSvc::interfaceID(){
   static const InterfaceID IID("ISCT_CalibModuleListSvc",1,0);
   return IID;
 }
#endif
