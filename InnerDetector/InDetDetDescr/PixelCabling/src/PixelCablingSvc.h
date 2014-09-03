/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PixelCablingSvc.h
//   Header file for class PixelCablingSvc
///////////////////////////////////////////////////////////////////
// (c) ATLAS Pixel Detector software
///////////////////////////////////////////////////////////////////
// Class used to get offlineIDs from their onlineIDs
// - getOfflineList() - get list of offlineIds from the RobId
// - getOfflineId() - get the offlineId from the onlineId
// - getRobId() - get RobId from offlineId
// - getRobId() - get RobId from RodId
// - getOnlineId() - get onlineId from offlineId
// - getPixelId() - get the pixelId from the offlineId, FE, row and column
// - getFE() - get the FE from the pixelId and offlineId
// - getColumn() - get the column number from the pixelId and offlineId
// - getRow() - get the row number from the pixelId and offlineId
///////////////////////////////////////////////////////////////////
//  Florian Hirsch
///////////////////////////////////////////////////////////////////

#ifndef PIXELCABLINGSVC_H
#define PIXELCABLINGSVC_H

#include "PixelCabling/IPixelCablingSvc.h"
#include "AthenaBaseComps/AthService.h"
#include "GaudiKernel/ServiceHandle.h"
#include "Identifier/IdContext.h"
#include "GaudiKernel/ToolHandle.h"
#include "PixelCablingData.h"
#include <map>
#include <deque>
#include <stdint.h>
#include <string>
#include <vector>

class StatusCode;
class ISvcLocator;
class PixelID;
class PixelCablingData;
class Identifier;
class IdentifierHash;
class StoreGateSvc;
class PixelFillCablingData_Final;
class IBLParameterSvc;

class PixelCablingSvc: virtual public IPixelCablingSvc, public AthService{

public:
  
  // constructor
  PixelCablingSvc(const std::string& name, ISvcLocator* svc ) ;
  
  // destructor	
  ~PixelCablingSvc();
  
  StatusCode initialize();
  StatusCode finalize();
  StatusCode IOVCallBack(IOVSVC_CALLBACK_ARGS);

  StatusCode queryInterface(const InterfaceID & riid, void** ppvInterface );
  
  // Get a list of offlineIds from a RobId.
  void getOfflineList(std::vector<IdentifierHash>& offlineIdHashList, int robid);
  
  // Get offlineId from a certain onlineId
  Identifier getOfflineId(uint64_t onlineId);
  IdentifierHash getOfflineIdHash(uint64_t onlineId);
  IdentifierHash getOfflineIdHashFromOfflineId(Identifier offlineId);
  Identifier getOfflineIdFromDCS(std::string DCSname);
  
  // Get RobId from a certain offlineId
  uint32_t getRobId(Identifier offlineId); 
  
  // For backward compatibility
  uint32_t getRobID(Identifier offlineId); 
  
  // Get RobId from a RodId
  int getRobId(const int rodId);

  // Get RODId from a ROBId
  int getRodId(const int robId);
  
  // get onlineId from a offlineId
  uint64_t getOnlineId(Identifier offlineId);

  // get onlineId from rodId + link or module number
  uint64_t getOnlineIdFromRobId(const uint32_t robId, const uint32_t link_module);
  
  Identifier getPixelIdfromHash(IdentifierHash offlineIdHash, uint32_t FE, uint32_t row, uint32_t column);
  Identifier getPixelId(Identifier offlineId, uint32_t FE, uint32_t row, uint32_t column);
  uint32_t getFE(Identifier * pixelId, Identifier offlineId);
  uint32_t getColumn(Identifier * pixelId, Identifier offlineId);
  uint32_t getRow(Identifier * pixelId, Identifier offlineId);
  uint32_t getFEwrtSlink(Identifier *pixelId);
  unsigned int getLocalFEI4(const uint32_t fe, const uint64_t onlineId);
  
  bool isIBL(const uint32_t robId);
  bool isIBL(const Identifier& id);
  bool isDBM(const uint32_t robId);
  bool isDBM(const Identifier& id);

  std::vector<uint32_t>& getAllRods();
  std::vector<uint32_t>& getAllRobs();

//  enum moduletype {DBM, IBL, PIX_BARREL, PIX_ENDCAP, NONE}; // moved to IPixelCablingSvc.h
  moduletype getModuleType(const Identifier& id);



   // Wrappers to PixelCablingData -- get maps
   std::map< uint64_t, Identifier > get_idMap_onoff() {return m_cabling->get_idMap_onoff();}
   std::map< Identifier, uint64_t> get_idMap_offon() {return m_cabling->get_idMap_offon();}
   std::map< Identifier, uint32_t> get_idMap_offrob() {return m_cabling->get_idMap_offrob();}
   std::map< int,int> get_idMap_rodrob() {return m_cabling->get_idMap_rodrob();}
   std::map< int,int> get_idMap_robrod() {return m_cabling->get_idMap_robrod();}
   std::map< std::string, Identifier> get_idMapDCSoff() {return m_cabling->get_idMapDCSoff();}

  
private:
  ToolHandle<PixelFillCablingData_Final> m_cablingTool;
  ServiceHandle< StoreGateSvc > m_detStore;
  ServiceHandle<IBLParameterSvc> m_IBLParameterSvc; 
  std::string m_dataString ;
  std::string m_key;
  unsigned int m_callback_calls;

  const PixelID* m_idHelper;
  PixelCablingData* m_cabling;
  
  IdContext m_context;
  
  std::string m_mappingType;
  std::string m_final_mapping_file;
  int m_bandwidth;
  std::string m_coraldbconnstring;
  std::string m_dictTag;
  std::string m_connTag;

  bool m_useIBLParameterSvc;

  bool m_IBLpresent;
  bool m_isHybrid;
  bool m_DBMpresent;

  std::vector<int> m_layer_columnsPerFE;
  std::vector<int> m_layer_rowsPerFE;
  std::vector< std::vector<int> > m_layer_FEsPerHalfModule;

  std::vector<int> m_disk_columnsPerFE;
  std::vector<int> m_disk_rowsPerFE;
  std::vector<int> m_disk_FEsPerHalfModule;
  
  int m_dbm_columnsPerFE;
  int m_dbm_rowsPerFE;
  int m_dbm_FEsPerHalfModule;

  unsigned int m_eta_module_offset;

//  std::vector<int> m_hybridModuleType;  // remove this

//  bool m_layerPropertiesFromOptions;
//  bool m_diskPropertiesFromOptions;
//  bool m_dbmPropertiesFromOptions;
 

  
};

#endif   // PIXELCABLINGSVC_H
