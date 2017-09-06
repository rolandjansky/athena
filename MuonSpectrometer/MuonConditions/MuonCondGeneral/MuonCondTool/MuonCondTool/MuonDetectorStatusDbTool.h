/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCONDTOOL_MUONDETECTORDBTOOL_H
#define MUONCONDTOOL_MUONDETECTORDBTOOL_H

#include "GaudiKernel/AlgTool.h"
#include "MuonCondInterface/IMuonDetectorStatusDbTool.h" 
#include "MuonCondData/MdtDeadTubeStatusContainer.h"
#include "MuonCondData/MdtDeadTubeStatus.h"
#include "MuonCondData/MdtDeadChamberStatus.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"
class Identifier; 
class StoreGateSvc; 
class MdtIdHelper;
class IIOVSvc;
class StatusCode;


class MuonDetectorStatusDbTool: public AthAlgTool,
	             virtual public IMuonDetectorStatusDbTool
{

public:    

  MuonDetectorStatusDbTool(const std::string& type, 
		const std::string& name, 
		const IInterface* parent); 

  /** required by the IAddressProvider interface */
  //  virtual StatusCode updateAddress(SG::TransientAddress* tad);
  virtual StatusCode updateAddress(StoreID::type storeID,
                                   SG::TransientAddress* tad,
                                   const EventContext& ctx);

private: 

  StatusCode initialize();
  
  virtual StatusCode loadParameterStatus(IOVSVC_CALLBACK_ARGS);
  virtual StatusCode loadTubeStatus(IOVSVC_CALLBACK_ARGS);

  ServiceHandle<IIOVSvc> m_IOVSvc;
  const MdtIdHelper* m_mdtIdHelper;

  
  //mutable MdtDeadTubeStatusContainer * m_tubeStatusData;
  mutable MdtDeadChamberStatus * m_chamberStatusData;
  
  std::string      m_tubeFolder;
  std::string      m_tubeStatusDataLocation;
  

};



#endif
