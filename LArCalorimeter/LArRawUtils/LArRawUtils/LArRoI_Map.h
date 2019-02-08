/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARRAWEVENT_LARROI_MAP_H
#define LARRAWEVENT_LARROI_MAP_H

#include "Identifier/Identifier.h" 
#include "Identifier/IdentifierHash.h" 
#include "CaloIdentifier/LArID_Exception.h" 
#include "Identifier/HWIdentifier.h" 
//#include "LArIdentifier/LArReadoutModuleID.h" 

#include "CaloTTDetDescr/CaloTTDescrManager.h"
#include "CaloIdentifier/CaloLVL1_ID.h" 
#include "StoreGate/DataHandle.h" 
#include "GaudiKernel/IIncidentListener.h"
#include "AthenaKernel/IOVSvcDefs.h"

#include "AthenaBaseComps/AthAlgTool.h"
#include <map> 

#include "LArCabling/LArCablingLegacyService.h" 
#include "CaloTriggerTool/CaloTriggerTowerService.h" 

class LArOnlineID ; 
class LArEM_ID ; 
class LArHEC_ID ; 
class LArFCAL_ID ; 

/**
 * @brief mapping between calorimetere trigger id to offline/online Identifier
 * and to LArReadoutModuleID (ROB).
 * @author Hong Ma
 * @version 00-00-01 , 26/08/2002 
 */


class LArRoI_Map : public AthAlgTool,
		      public IIncidentListener 
 { 

 public:
  /** constructor 
  */ 
  LArRoI_Map( const std::string& type, const std::string& name,
        const IInterface* parent ) ; 

  /** define a RoI identifier type, two ints. 
  */ 

  StatusCode initialize(); 

  // TT_ID is TT hashId + sam coded in unsigned int,  (sam+ hash<<2)
  typedef unsigned int TT_ID; 
  typedef HWIdentifier  COLL_ID ; 

  virtual ~LArRoI_Map() ;
  /** AlgTool InterfaceID
  */ 
  static const InterfaceID& interfaceID( ) ; 

  /** Incident 
   */
  virtual void handle(const Incident&); 

  /** IOV Call Back 
   */
  virtual StatusCode iovCallBack (IOVSVC_CALLBACK_ARGS);

  /**
   * return TrigTowerID for various queries. 
   */

  TT_ID  TrigTowerID (const Identifier& channel_id  ) const;

  TT_ID  TrigTowerID (const HWIdentifier& id  ) const;

  /** reserve lookup. given RoI, get CollectionID 
  */ 
  const std::vector<COLL_ID>& CollectionID(const TT_ID& id) const ; 

  /** check validity
  */ 
  bool validId(const TT_ID& tt_id) const ; 

 private:
  /** initialize the whole cache
   */
  StatusCode initData(); 

  /** Add channel to cache
   */
  void addEntry( const Identifier& id  ); 

  void print(); 

  StatusCode initCabling();

 /** Map for offline ID and online id
 */  
  typedef std::map<unsigned int, TT_ID> OFFMAP ; 
  typedef std::map<unsigned int, TT_ID> ONLMAP ; 

  // TT_ID to Collection ID map. 
  typedef std::vector<COLL_ID>  COLL_ID_VEC; 
  typedef std::map<TT_ID, COLL_ID_VEC  > ROIMAP ; 

  OFFMAP m_offmap; 
  ONLMAP m_onlmap; 
  ROIMAP m_roimap; 
 

  ToolHandle<LArCablingLegacyService> m_cablingSvc{ this, "CablingSvc", "LArCablingLegacyService", "LAr cabling tool"};
  ToolHandle<CaloTriggerTowerService> m_ttSvc{ this, "TriggerTowerSvc", "CaloTriggerTowerService", "Towers tool"};   

  bool m_print; 
  bool m_loadAtBegin;
  bool m_validCache;

  const CaloLVL1_ID* m_TT_ID; 
  const LArOnlineID* m_onlineID; 
  const LArEM_ID*  m_em_id   ; 
  const LArHEC_ID* m_hec_id  ; 
  const LArFCAL_ID* m_fcal_id; 
};

  

#endif


