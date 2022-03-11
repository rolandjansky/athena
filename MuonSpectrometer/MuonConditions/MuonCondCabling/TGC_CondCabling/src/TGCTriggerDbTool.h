/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCONDTOOL_TGCTRIGGERDBTOOL_H
#define MUONCONDTOOL_TGCTRIGGERDBTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "MuonCondInterface/ITGCCablingDbTool.h"
#include "MuonCondInterface/ITGCTriggerDbTool.h"
#include "GaudiKernel/IChronoStatSvc.h"

class IIOVSvc;
class IChronoStatSvc;

class TGCTriggerDbTool: public AthAlgTool, public ITGCTriggerDbTool
{
 public:    
  /** Constructor */
  TGCTriggerDbTool(const std::string& type, 
		   const std::string& name, 
		   const IInterface* parent); 
  
  /** Required by the IAddressProvider interface */
  //  virtual StatusCode updateAddress(SG::TransientAddress* tad);
  virtual StatusCode updateAddress(StoreID::type storeID,
                                   SG::TransientAddress* tad,
                                   const EventContext& ctx);

  /** Initilize */
  virtual StatusCode initialize();
  /** Finalize */
  virtual StatusCode finalize();
 
  /** Get the folder name */
  virtual std::string getFolderName(int cwtype) const;

  virtual std::string getData(int cwtype, std::string file) const;
  virtual std::string getData(int cwtype, int channel) const;
  virtual std::string getFile(int cwtype, int channel) const;
  virtual std::string getVersion(int cwtype, int channel = 0) const;
  virtual std::string getType(int cwtype, int channel = 0) const;
  virtual bool isActive(int cwtype, int channel = 0) const;

  /** Load parameters using IOV keys */
  virtual StatusCode loadParameters(IOVSVC_CALLBACK_ARGS);

 private: 

  /** Data location */
  std::string m_DataLocation;
  /** Folder name */
  std::string m_bwFolder;
  std::string m_eifiFolder;
  std::string m_tileFolder;

  std::string m_folders[CW_NUM];

  std::map<std::string, std::string> m_datamap[CW_NUM];
  std::vector<std::string> m_data[CW_NUM];
  std::vector<std::string> m_file[CW_NUM];
  std::vector<std::string> m_version[CW_NUM];
  std::vector<std::string> m_type[CW_NUM];
  std::vector<bool>        m_active[CW_NUM];
};

#endif // MUONCONDTOOL_TGCTRIGGERDBTOOL_H
