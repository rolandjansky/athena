/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOTRIGGERTOWERSERVICE_H
#define CALOTRIGGERTOWERSERVICE_H

// Gaudi
#include "AthenaBaseComps/AthAlgTool.h"
#include "StoreGate/DataHandle.h"
#include "GaudiKernel/MsgStream.h"

#include "AthenaKernel/IOVSvcDefs.h"

#include <vector>
#include <string>

#include "CaloTriggerTool/LArTTCellMap.h"
#include "CaloTriggerTool/CaloTTOnOffIdMap.h"
#include "CaloTriggerTool/CaloTTOnAttrIdMap.h"
#include "CaloTriggerTool/CaloTTPpmRxIdMap.h"

#include "Identifier/HWIdentifier.h"
#include "CaloIdentifier/LArID_Exception.h"
#include "CaloIdentifier/CaloID_Exception.h"

#include "TrigT1CaloCalibConditions/L1CaloCoolChannelId.h"
#include "TrigT1CaloCalibConditions/L1CaloRxCoolChannelId.h"


class Identifier;
class HWIdentifier;
class LArOnlineID;
class LArEM_ID;
class TTOnlineID;
class CaloLVL1_ID;
class LArCablingService;

static const InterfaceID IID_CaloTriggerTowerService("CaloTriggerTowerService", 1 , 0);

/**
 * @brief Tool providing the mapping of the online and offline TriggerTower identifiers
 * as well as the TT-cells mapping
 *
 * @author Arnaud Lucotte, Fabienne Ledroit
 * @author maintained by Damien Prieur
 *
 */

class CaloTriggerTowerService : public AthAlgTool
{
 public:
  /** Retrieve interface ID */
  static const InterfaceID& interfaceID() { return IID_CaloTriggerTowerService; }

  /** constructor */
  CaloTriggerTowerService( const std::string& type, const std::string& name, const IInterface* parent ) ;

  StatusCode initialize( );
  StatusCode finalize( );

  bool is_initialized() const;

  virtual StatusCode iovCallBack(IOVSVC_CALLBACK_ARGS);

  //
  //======================= on-off mapping methods (Trigger Towers) ==================================
  //

  /**
   * create a TT HWIdentifier from a TT Identifier
   *
   */
  HWIdentifier createTTChannelID(const Identifier & id, bool bQuiet=false);


  /**
   * create a TT Identifier from a TT HWIdentifier
   *
   */
  Identifier cnvToIdentifier(const HWIdentifier & id, bool bQuiet=false);


  /**
   * create a CoolChannelId (used by TDAQ framework) from a HWIdentifier
   *
   */
  //unsigned int createL1CoolChannelId( const HWIdentifier & id );
  L1CaloCoolChannelId createL1CoolChannelId( const HWIdentifier& id );


  /**
   * create an HWIdentifier from an TDAQ CoolChannelId
   *
   */
  //HWIdentifier cnvCoolChannelIdToHWID( const unsigned int coolChannelId );
  HWIdentifier cnvCoolChannelIdToHWID( const L1CaloCoolChannelId& coolChannelId );




  /**
   * returns a TDAQ CoolChannelId corresponding to a L1CaloRxCoolChannelId
   *
   */
  L1CaloCoolChannelId cnvRxIdToCoolChannelId(const L1CaloRxCoolChannelId& rxCoolChannelId);


  /**
   * returns a vector of L1CaloRxCoolChannelId corresponding to a L1CaloCoolChannelId
   *
   */

   std::vector<L1CaloRxCoolChannelId> cnvCoolChannelIdToRxId(const L1CaloCoolChannelId& ppmCoolChannelId);


  /** return 0 for z<0 channels and 1 for z>0 channels */
  unsigned int pos_neg(const HWIdentifier & id);

  /** return the corresponding sampling of channelId:
      @return 0: for EM calorimeter
      @return 1: for HAD calorimeter
      @return 999: error
  */
  unsigned int em_had(const HWIdentifier & id);

  /** To know if a channel belongs to the barrel, end cap or fcal parts
      @return 0: barrel
      @return 1: end cap
      @return 2: fcal
      @return 999: error
  */
  unsigned int barrel_endcap_fcal(const HWIdentifier & id);

  /** To get the type of the module connected to the channel:
      @return 1: PPM
      @return 2: CPM
      @return 3: JEM
      @return 999: error
  */
  unsigned int module_type(const HWIdentifier & id);


  //
  //======================= Trigger Tower - cell/channel mapping methods ==================================
  //

  /**
     Return a vector of LArOnlineID for a TTOnlineID   (online-online TT-cell mapping)

     @param extTt used to decide wether the 'layer' info should be used or not <br>
     extTt = 0 : the full TT is returned <br>
     else      : the 'extended'TT (only 1 layer) is returned.

     @warning the actual mapping is offline-offline, so this method is CPU expensive.
     @warning this method is valid only for LAr Id so far
  */
  std::vector<HWIdentifier> createChannelIDvec(const HWIdentifier & id, int extTt) ;

  /**
     return the TTOnlineID of the TT to which a LArOnlineID belongs   (online-online TT-cell mapping)

     @warning the actual mapping is offline-offline, so this method is CPU expensive.
     @warning this method is valid only for LAr Id so far
  */
  HWIdentifier whichTTChannelID(const HWIdentifier & id);

  /**
     Return a vector of offline Identifiers (corresponding helpers =
     LArEM_ID, LArHEC_ID, LArFCAL_ID) for a TT offline id (helper=CaloLVL1_ID)
     -> all layers together (Rem: equivalent to above extTt=0) <br>
     The call is formarded to the DB object LArTTCellMap
  */
  std::vector<Identifier> createCellIDvecTT(const Identifier& id) ;

  /**
     Return a vector of offline Identifiers (corresponding helpers =
     LArEM_ID, LArHEC_ID, LArFCAL_ID) for a TT-layer offline id (helper=CaloLVL1_ID)
     -> one single layer (Rem: equivalent to above extTt!=0) <br>
     The call is formarded to the DB object LArTTCellMap
  */
  std::vector<Identifier> createCellIDvecLayer(const Identifier& id) ;

  /**
     return the offline id (CaloLVL1_ID) of the TT to which a cell id (CaloCell_ID) belongs <br>
     The call is formarded to the DB object LArTTCellMap
  */
  Identifier whichTTID(const Identifier & id);

  /**
     return true if the channel is in the lvl1 sums; <br>
     some channels mapped to TT are only in HLT (last HEC compartment and last cells of barrel PS)
  */
  bool is_in_lvl1(const HWIdentifier & id) ;
  /**
     return true if the cell is in the lvl1 sums; <br>
     some channels mapped to TT are only in HLT (last HEC compartment and last cells of barrel PS)
  */
  bool is_in_lvl1(const Identifier & id) ;


 private:

    /** pointer to the LArCablingSvc tool */
    LArCablingService   * m_larcablingSvc;
    /** pointer to the LAr Online Id helper */
    const LArOnlineID   * m_onlineHelper ;
    /** pointer to the LArEM  offline Id helper */
    const LArEM_ID      * m_emHelper ;
    /** pointer to the Calo TT offline Id helper */
    const CaloLVL1_ID   * m_lvl1Helper ;
    /** pointer to the Calo TT online Id helper */
    const TTOnlineID    * m_ttonlineHelper;

    const DataHandle<LArTTCellMap>      m_TTCellMap;
    const DataHandle<CaloTTOnOffIdMap>  m_caloTTOnOffIdMap;
    const DataHandle<CaloTTOnAttrIdMap> m_caloTTOnAttrIdMap;
    const DataHandle<CaloTTPpmRxIdMap>  m_caloTTPpmRxIdMap;

    std::string       m_TTCellMapKey;
    std::string       m_caloTTOnOffIdMapKey;
    std::string       m_caloTTOnAttrIdMapKey;
    std::string       m_caloTTPpmRxIdMapKey;
};

#endif //CALOTRIGGERTOWERSERVICE_H
