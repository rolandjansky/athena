/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MDT_CONDCABLING_MDTCABLINGDBTOOL_H
#define MDT_CONDCABLING_MDTCABLINGDBTOOL_H

#include "MuonCondInterface/IMDTCablingDbTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"

#include "GaudiKernel/IChronoStatSvc.h"
#include "MuonCablingData/MuonMDT_CablingMap.h"
#include "AthenaKernel/IIOVDbSvc.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"

#include <string>

class IIOVSvc;

class MDTCablingDbTool: public AthAlgTool, public IMDTCablingDbTool {

public:    

  MDTCablingDbTool(const std::string& type, 
		const std::string& name, 
		const IInterface* parent); 

  /** required by the IAddressProvider interface */
  //virtual StatusCode updateAddress(SG::TransientAddress* tad);
  virtual StatusCode updateAddress(StoreID::type storeID,
                                   SG::TransientAddress* tad,
                                   const EventContext& ctx);
  
  virtual StatusCode initialize();

  virtual std::string mapFolderName() const {return m_mapFolder;}
  virtual std::string mezzanineFolderName() const {return m_mezzanineFolder;}

 private: 
  virtual StatusCode loadParameters(IOVSVC_CALLBACK_ARGS);
  virtual StatusCode loadMezzanine(IOVSVC_CALLBACK_ARGS);
  virtual StatusCode loadMDTMap(IOVSVC_CALLBACK_ARGS);

  IIOVSvc* m_IOVSvc;
  IIOVDbSvc* m_IOVDbSvc;
  ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};

  MuonMDT_CablingMap* m_cablingData; 
  std::string      m_DataLocation;
  
  std::string     m_mapFolder;
  std::string      m_mezzanineFolder;
	  
  IChronoStatSvc* m_chronoSvc;

  std::string m_chrono1;
  std::string m_chrono2;
  std::string m_chrono3;
  std::string m_chrono4;
};
 


#endif
