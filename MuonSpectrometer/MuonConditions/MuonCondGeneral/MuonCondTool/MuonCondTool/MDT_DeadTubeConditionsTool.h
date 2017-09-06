/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCONDTOOL_MDT_DEADTUBECONDITIONSTOOL_H
#define MUONCONDTOOL_MDT_DEADTUBECONDITIONSTOOL_H

#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "MuonCondInterface/IMDT_DeadTubeConditionsTool.h"
#include "GaudiKernel/IChronoStatSvc.h"
//#include "MuonCondData/MdtDeadTubeStatusContainer.h"
//#include "MuonCondData/MdtDeadTubeStatus.h"
//#include "MuonCondData/MdtDeadChamberStatus.h"
#include "MuonCondInterface/IMDT_MapConversion.h"
#include "GaudiKernel/MsgStream.h"
//#include "StoreGate/StoreGateSvc.h"

class Identifier;
class StoreGateSvc;

class MdtHelper;
class IIOVSvc;
class StatusCode;
class IMDT_MapConversion;

class MDT_DeadTubeConditionsTool: public AthAlgTool, virtual public IMDT_DeadTubeConditionsTool
{

public:   

  MDT_DeadTubeConditionsTool(const std::string& type,
                const std::string& name,
                const IInterface* parent);

  /** required by the IAddressProvider interface */
  // virtual StatusCode updateAddress(SG::TransientAddress* tad);
  virtual StatusCode updateAddress(StoreID::type storeID,
                                   SG::TransientAddress* tad,
                                   const EventContext& ctx) override;
  
  virtual StatusCode initialize() override;

  virtual std::string DeadTubeFolderName() const override {return m_deadtubeFolder;}
  

 private:


 
  virtual StatusCode loadParameters(IOVSVC_CALLBACK_ARGS) override;

  virtual StatusCode loadDeadTube(IOVSVC_CALLBACK_ARGS) override;

  virtual const std::vector<std::string>& deadTubes() const override
  { return m_cachedDeadTubes;}      
  virtual const std::vector<Identifier>& deadTubesId() const override
  { return m_cachedDeadTubesId;}      
  virtual const std::map<Identifier,std::string>& Tube_MapId() const
  { return m_Tube_Map;}      
  virtual const std::vector<Identifier>& List_Chambers_with_deadTube() const override
  { return m_Chamber_with_deadTube;}
  StoreGateSvc* m_detStore;
  IIOVSvc* m_IOVSvc;

  //  mutable MdtDeadTubeStatusContainer * m_tubeStatusData;
  //mutable MdtDeadChamberStatus * m_chamberStatusData;

  const MdtIdHelper* m_mdtIdHelper;
  

  std::map<std::string, Identifier> m_Chamber_Map;

  std::map<Identifier,std::string> m_Tube_Map;
 
  std::string     m_tubeStatusDataLocation; 
  std::string     m_deadtubeFolder;
  bool            m_dcsAnalysis;
  
  std::vector<std::string> m_cachedDeadTubes;  
  std::vector<Identifier>  m_cachedDeadTubesId; 
  std::vector<Identifier>  m_Chamber_with_deadTube;
  IChronoStatSvc* m_chronoSvc;

  std::string m_chrono1;
  std::string m_chrono2;
  std::string m_chrono3;
  std::string m_chrono4;


  ToolHandle<IMDT_MapConversion> m_condMapTool; 
  
  MsgStream m_log;
  bool      m_debug;
  bool      m_verbose;

};
 


#endif
