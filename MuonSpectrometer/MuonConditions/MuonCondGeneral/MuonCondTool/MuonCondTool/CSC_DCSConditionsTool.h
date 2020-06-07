/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCONDTOOL_CSC_DCSCONDITIONSTOOL_H
#define MUONCONDTOOL_CSC_DCSCONDITIONSTOOL_H

#include "MuonCondInterface/ICSC_DCSConditionsTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"

#include "GaudiKernel/IChronoStatSvc.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"

class IIOVSvc;

class CSC_DCSConditionsTool: public AthAlgTool, virtual public ICSC_DCSConditionsTool
{

public:   

  CSC_DCSConditionsTool(const std::string& type,
                const std::string& name,
                const IInterface* parent);

  /** required by the IAddressProvider interface */
  //virtual StatusCode updateAddress(SG::TransientAddress* tad);
  virtual StatusCode updateAddress(StoreID::type storeID,
                                   SG::TransientAddress* tad,
                                   const EventContext& ctx) override;
  
  virtual StatusCode initialize() override;

  virtual std::string DropChamberFolderName() const
  {return m_dropchamberFolder;}
  virtual std::string HVFolderName() const override
  {return m_hvFolder;}
  virtual std::string ChamberFolder() const override
  {return m_chamberFolder;}
  virtual const std::map<Identifier,int>& CSC_ChamberId() const
  { return m_CSC_LayerMap;}   

  virtual const std::vector<std::string>& deadStationsStr() const
  { return m_cachedDeadStationsStr;}
  virtual const std::vector<Identifier>& deadStationsId() const override
  { return m_cachedDeadStationsId;}
  virtual const std::vector<Identifier>& deadWireLayersId() const override
  { return m_cachedDeadWireLayersId;} 

  virtual const std::vector<std::string>& deadStations() const override
  { return m_cachedDeadStationsStr;}
  virtual const std::vector<std::string>& deadWireLayers() const override
  { return m_cachedDeadWireLayers;} 
  
  
 private:


 
   virtual StatusCode loadParameters(IOVSVC_CALLBACK_ARGS) override;
   virtual StatusCode loadHV(IOVSVC_CALLBACK_ARGS) override;
   virtual StatusCode loadchamber(IOVSVC_CALLBACK_ARGS) override;
   
  IIOVSvc* m_IOVSvc;

  std::map<Identifier, int> m_CSC_LayerMap;
  std::map<Identifier, int> m_CSC_LayerMap_test;
  ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};
 
  std::vector<int> m_wireLayer;
  std::vector<Identifier> m_cachedDeadStationsId;
  std::vector<Identifier> m_cachedDeadStations;
  std::vector<Identifier> m_cachedDeadStationsId_chamber;
  std::vector<std::string> m_cachedDeadStationsStr;
  std::vector<Identifier> m_cachedDeadWireLayersId;
  std::vector<std::string> m_cachedDeadWireLayers;
  //private comparison function for Identifier sorting
  static bool compareId(Identifier x,Identifier y) { return (x > y); } 
  
  std::string m_DataLocation;
  std::string m_dropchamberFolder;
  std::string m_hvFolder;
  std::string m_chamberFolder;
        
  std::stringstream m_CSCChamDrop;
  IChronoStatSvc* m_chronoSvc;

  std::string m_chrono1;
  std::string m_chrono2;
  std::string m_chrono3;
  std::string m_chrono4;

};
 


#endif
