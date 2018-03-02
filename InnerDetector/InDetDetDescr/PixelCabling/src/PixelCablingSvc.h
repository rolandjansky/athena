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
#include <memory>

class StatusCode;
class ISvcLocator;
class PixelID;
class PixelCablingData;
class Identifier;
class IdentifierHash;
class StoreGateSvc;
class PixelFillCablingData;
class IBLParameterSvc;

/**
 * The PixelCablingSvc class keeps track of the cabling map for pixels,
 * IBL and DBM, and does the conversion between online and offline identifiers
 * In the context of the pixel bytestream converter, an online identifier is
 * a 64-bit unsigned integer, formatted as 0xAABBDDMMMM, where MMMM = robId,
 * DD = subdetector id, AABB = link number. For pixels, there is only one link
 * number per onlineId, so AA = 00. See PixelCablingData::getOnlineIdFromRobId.
 * The offline id is the standard Identifier class.
 *
 * Technically, the cabling map is contained in the PixelCablingData class,
 * and its contents is filled by the PixelFillCablingData class. However
 * everything should be interface through PixelCablingSvc and not accessed
 * directly.
 *
 * The setProperty configurables of the class are:
 *  - MappingType:  Two choices available:
 *      - "COOL": Read cabling map from COOL (recommended)
 *      - "Final": Read map from text file in /share (for MC and Run-1)
 *  - MappingFile: Name of the file in /share that will be used if the
 *    mapping type is "Final". If reading from COOL, the database is not
 *    available at time of initialization, therefore a text file is always
 *    read. However the mapping is overwritten once COOL is connected.
 *  - Key: Name of readoutspeed folder
 *  - KeyFEI4: Name of the HitDiscCnfg folder
 *  - KeyCabling: Name of the cabling map folder
 *
 * The following are not used anymore (setting them does nothing):
 * Bandwidth, Coral_Connectionstring, DictionaryTag, ConnectivityTag
 *
 * Developers in the period 2014-2016 are
 *   Mapo Giordani
 *   Steffen Maeland
 *   Laura Franconi
 *
 */
class PixelCablingSvc: virtual public IPixelCablingSvc, public AthService {

public:
  
  /** Constructor */
  PixelCablingSvc(const std::string& name, ISvcLocator* svc ) ;

  /** Copy constructor */
  //PixelCablingSvc(const PixelCablingSvc &other, const std::string& name, ISvcLocator*svc);

  /** Assignment operator */
  //PixelCablingSvc& operator= (const PixelCablingSvc &other);
  
  /** Destructor */
  ~PixelCablingSvc();
  
  /** Initialize the tool, fill cabling map */
  StatusCode initialize();

  /** Finialize */
  StatusCode finalize();

  /** Callback to update maps */
  StatusCode IOVCallBack(IOVSVC_CALLBACK_ARGS);

  StatusCode queryInterface(const InterfaceID & riid, void** ppvInterface );
  
  /** Get a list of offlineIds from a RobId. */
  void getOfflineList(std::vector<IdentifierHash>& offlineIdHashList, int robid);
  
  /** @name Functions to get the offline identifier */
  ///@{
  Identifier getOfflineId(uint64_t onlineId);
  IdentifierHash getOfflineIdHash(uint64_t onlineId);
  IdentifierHash getOfflineIdHashFromOfflineId(Identifier offlineId);
  Identifier getOfflineIdFromDCS(std::string DCSname);
  ///@}
  
  /** Get RobId from a certain offlineId */
  uint32_t getRobId(Identifier offlineId); 
  
  /** For backward compatibility, don't use */
  uint32_t getRobID(Identifier offlineId); 
  
  /** Get ROB Id from a ROD Id */
  int getRobId(const int rodId);

  /** Get ROD Id from a ROB Id */
  int getRodId(const int robId);
  
  /** Get onlineId from a offlineId */
  uint64_t getOnlineId(Identifier offlineId);

  /** Get onlineId from rodId + link or module number */
  uint64_t getOnlineIdFromRobId(const uint32_t robId, const uint32_t link_module);
  
  Identifier getPixelIdfromHash(IdentifierHash offlineIdHash, uint32_t FE, uint32_t row, uint32_t column);
  Identifier getPixelId(Identifier offlineId, uint32_t FE, uint32_t row, uint32_t column);

  /** @name Functions to get various properties for given pixelId
   * (the offlineId is redundant info here, so these functions
   * could technically be simplified.)
   */
  ///@{
  uint32_t getFE(Identifier* pixelId, Identifier offlineId);
  uint32_t getColumn(Identifier* pixelId, Identifier offlineId);
  uint32_t getRow(Identifier* pixelId, Identifier offlineId);
  uint32_t getFEwrtSlink(Identifier*pixelId);
  unsigned int getLocalFEI4(const uint32_t fe, const uint64_t onlineId);
  ///@}

  /** @name Functions to retrieve the FE-I4 HitDiscCnfg setting */
  ///@{
  int getHitDiscCnfg(const uint32_t robId, const int link);
  int getHitDiscCnfg(Identifier* pixelId);
  ///@}
  ///
  /** @name Functions to check the type of subdetector for a
   * given ROB id or for a given Identifier
   */
  ///@{
  bool isIBL(const uint32_t robId);
  bool isIBL(const Identifier& id);
  bool isDBM(const uint32_t robId);
  bool isDBM(const Identifier& id);

  /** Get type of module
   * @param id The offline identifier of the module
   * @return Enumerator moduletype, see IPixelCablingSvc.h
   */
  moduletype getModuleType(const Identifier& id);

  ///@}

  /** Return a vector containing all RODs in the cabling map */
  std::vector<uint32_t>& getAllRods();
  /** Return a vector containing all ROBs in the cabling map */
  std::vector<uint32_t>& getAllRobs();

  /** @name Wrappers to PixelCablingData. Returns the std::map of interest. */
  ///@{
   std::map< uint64_t, Identifier > get_idMap_onoff() {return m_cabling->get_idMap_onoff();}
   std::map< Identifier, uint64_t> get_idMap_offon() {return m_cabling->get_idMap_offon();}
   std::map< Identifier, uint32_t> get_idMap_offrob() {return m_cabling->get_idMap_offrob();}
   std::map< int,int> get_idMap_rodrob() {return m_cabling->get_idMap_rodrob();}
   std::map< int,int> get_idMap_robrod() {return m_cabling->get_idMap_robrod();}
   std::map< std::string, Identifier> get_idMapDCSoff() {return m_cabling->get_idMapDCSoff();}
  ///@}
  
private:
  ToolHandle<PixelFillCablingData> m_cablingTool;
  ServiceHandle< StoreGateSvc > m_detStore;
  ServiceHandle<IBLParameterSvc> m_IBLParameterSvc; 
  const PixelID* m_idHelper;
  std::unique_ptr<PixelCablingData> m_cabling;

  unsigned int m_callback_calls;
  IdContext m_context;

  /** @name The setProperty configurables */
  ///@{
  std::string m_dataString ;
  std::string m_key;
  std::string m_keyFEI4;
  std::string m_keyCabling;
  std::string m_mappingType;
  std::string m_final_mapping_file;
  int m_bandwidth;
  std::string m_coraldbconnstring;
  std::string m_dictTag;
  std::string m_connTag;
  bool m_dump_map_to_file;
  ///@}

  /** @name Geometry-dependent properties */
  ///@{
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
  ///@}
  
};

#endif   // PIXELCABLINGSVC_H
