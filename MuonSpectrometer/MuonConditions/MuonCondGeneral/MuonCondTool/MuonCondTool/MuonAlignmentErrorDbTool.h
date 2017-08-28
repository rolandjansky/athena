/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCONDTOOL_MUONALIGNMENTERRORDBTOOL_H
#define MUONCONDTOOL_MUONALIGNMENTERRORDBTOOL_H

#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "MuonCondInterface/IMuonAlignmentErrorDbTool.h"
#include "GaudiKernel/IChronoStatSvc.h"
//#include "MuonCondInterface/IMDT_MapConversion.h"
#include "GaudiKernel/MsgStream.h"
//#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"

class Identifier;
class StoreGateSvc;

class MdtHelper;
class IIOVSvc;
class StatusCode;
//class IMDT_MapConversion;

class MuonAlignmentErrorDbTool: public AthAlgTool, virtual public IMuonAlignmentErrorDbTool
{

public:   

  MuonAlignmentErrorDbTool(const std::string& type,
                const std::string& name,
                const IInterface* parent);

  /** required by the IAddressProvider interface */
  //virtual StatusCode updateAddress(SG::TransientAddress* tad);
  virtual StatusCode updateAddress(StoreID::type storeID,
                                   SG::TransientAddress* tad,
                                   const EventContext& ctx) override;
  
  virtual StatusCode initialize() override;

  virtual std::string ErrorFolderName() const override {return m_errorFolder;}
  virtual const std::string& errorValue() const override {return m_errorlistValue;};

 private:


 
  virtual StatusCode loadParameters(IOVSVC_CALLBACK_ARGS) override;
  virtual StatusCode loadAlignmentError(IOVSVC_CALLBACK_ARGS) override;
  

       
  StoreGateSvc* m_detStore;
  IIOVSvc* m_IOVSvc;


  const MdtIdHelper* m_mdtIdHelper;
 

  std::string     m_errorFolder;
  std::string     m_errorlistValue;
 
  IChronoStatSvc* m_chronoSvc;
  std::vector<std::string>      m_parlineFolder;
  std::string m_chrono1;



  MsgStream m_log;
  bool      m_debug;
  bool      m_verbose;
};
 


#endif
