/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//vitrual interface for all the Muon MDT technologies 
//
#ifndef IMDTConditionsSvc_h
#define IMDTConditionsSvc_h
//Gaudi Includes
#include "GaudiKernel/IInterface.h"
//#include "MuonCondSvc/MuonHierarchy.h"
//STL includes
#include <list>
#include <string>
#include "AthenaKernel/IOVSvcDefs.h"
//forward declarations
class Identifier;
class IdentifierHash;
class StatusCode;


class IMDTConditionsSvc: virtual public IInterface{
public:
  virtual ~IMDTConditionsSvc(){}
  static const InterfaceID & interfaceID(); //!< reimplemented from IInterface
 
  ///Return whether this service can report on the hierarchy level 
  //virtual bool canReportAbout(MuonConditions::Hierarchy h) = 0;

  //@todo introduce hash identifier method
  virtual StatusCode initInfo(IOVSVC_CALLBACK_ARGS) = 0;
  //virtual StatusCode update_MDT(IOVSVC_CALLBACK_ARGS) = 0;


  virtual bool isGoodChamber(const Identifier & Id) const = 0;
  virtual bool isGoodMultiLayer(const Identifier & Id)const  = 0;
  virtual bool isGoodChannel(const Identifier & Id) const = 0;


  
  virtual const std::vector<std::string>& deadMultiLayers()const =0 ;
  virtual const std::vector<std::string>& deadTubes()const =0 ;
  virtual const std::vector<std::string>& deadStations()const =0;
  
  virtual const std::vector<Identifier>& deadLayersId()const =0 ;
  virtual const std::vector<Identifier>& deadMultiLayersId()const =0 ;
  virtual const std::vector<Identifier>& deadTubesId()const =0 ;
  virtual const std::vector<Identifier>& deadStationsId()const =0;
  virtual const std::vector<Identifier>& List_Chambers_with_deadTube() const =0;
 
};

inline const InterfaceID & IMDTConditionsSvc::interfaceID(){
  static const InterfaceID IID_IMDTConditionsSvc("MDTConditionsSvc",1,0);
  return IID_IMDTConditionsSvc;
}

#endif
