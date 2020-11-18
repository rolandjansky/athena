/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOTRIGGERTOWERSERVICE_H
#define CALOTRIGGERTOWERSERVICE_H

// Gaudi
#include "AthenaBaseComps/AthAlgTool.h"
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
#include "CxxUtils/CachedPointer.h"


class Identifier;
class HWIdentifier;
class LArOnlineID;
class LArEM_ID;
class TTOnlineID;
class CaloLVL1_ID;

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
  HWIdentifier createTTChannelID(const Identifier & id, bool bQuiet=false) const;


  /**
   * create a TT Identifier from a TT HWIdentifier
   *
   */
  Identifier cnvToIdentifier(const HWIdentifier & id, bool bQuiet=false) const;


  /**
   * create a CoolChannelId (used by TDAQ framework) from a HWIdentifier
   *
   */
  //unsigned int createL1CoolChannelId( const HWIdentifier & id );
  L1CaloCoolChannelId createL1CoolChannelId( const HWIdentifier& id ) const;


  /**
   * create an HWIdentifier from an TDAQ CoolChannelId
   *
   */
  //HWIdentifier cnvCoolChannelIdToHWID( const unsigned int coolChannelId );
  HWIdentifier cnvCoolChannelIdToHWID( const L1CaloCoolChannelId& coolChannelId ) const;




  /**
   * returns a TDAQ CoolChannelId corresponding to a L1CaloRxCoolChannelId
   *
   */
  L1CaloCoolChannelId cnvRxIdToCoolChannelId(const L1CaloRxCoolChannelId& rxCoolChannelId) const;


  /**
   * returns a vector of L1CaloRxCoolChannelId corresponding to a L1CaloCoolChannelId
   *
   */

   std::vector<L1CaloRxCoolChannelId> cnvCoolChannelIdToRxId(const L1CaloCoolChannelId& ppmCoolChannelId) const;


  /** return 0 for z<0 channels and 1 for z>0 channels */
  unsigned int pos_neg(const HWIdentifier & id) const;

  /** return the corresponding sampling of channelId:
      @return 0: for EM calorimeter
      @return 1: for HAD calorimeter
      @return 999: error
  */
  unsigned int em_had(const HWIdentifier & id) const;

  /** To know if a channel belongs to the barrel, end cap or fcal parts
      @return 0: barrel
      @return 1: end cap
      @return 2: fcal
      @return 999: error
  */
  unsigned int barrel_endcap_fcal(const HWIdentifier & id) const;

  /** To get the type of the module connected to the channel:
      @return 1: PPM
      @return 2: CPM
      @return 3: JEM
      @return 999: error
  */
  unsigned int module_type(const HWIdentifier & id) const;


  //
  //======================= Trigger Tower - cell/channel mapping methods ==================================
  //

  /**
     Return a vector of offline Identifiers (corresponding helpers =
     LArEM_ID, LArHEC_ID, LArFCAL_ID) for a TT offline id (helper=CaloLVL1_ID)
     -> all layers together (Rem: equivalent to above extTt=0) <br>
     The call is formarded to the DB object LArTTCellMap
  */
  std::vector<Identifier> createCellIDvecTT(const Identifier& id) const;

  /**
     Return a vector of offline Identifiers (corresponding helpers =
     LArEM_ID, LArHEC_ID, LArFCAL_ID) for a TT-layer offline id (helper=CaloLVL1_ID)
     -> one single layer (Rem: equivalent to above extTt!=0) <br>
     The call is formarded to the DB object LArTTCellMap
  */
  std::vector<Identifier> createCellIDvecLayer(const Identifier& id) const;

  /**
     return the offline id (CaloLVL1_ID) of the TT to which a cell id (CaloCell_ID) belongs <br>
     The call is formarded to the DB object LArTTCellMap
  */
  Identifier whichTTID(const Identifier & id) const;

  /**
     return true if the cell is in the lvl1 sums; <br>
     some channels mapped to TT are only in HLT (last HEC compartment and last cells of barrel PS)
  */
  bool is_in_lvl1(const Identifier & id) const;


 private:
    const LArTTCellMap*      getTTCellMap() const;
    const CaloTTOnOffIdMap*  getCaloTTOnOffIdMap() const;
    const CaloTTOnAttrIdMap* getCaloTTOnAttrIdMap() const;
    const CaloTTPpmRxIdMap*  getCaloTTPpmRxIdMap() const;

    /** pointer to the LAr Online Id helper */
    const LArOnlineID   * m_onlineHelper ;
    /** pointer to the LArEM  offline Id helper */
    const LArEM_ID      * m_emHelper ;
    /** pointer to the Calo TT offline Id helper */
    const CaloLVL1_ID   * m_lvl1Helper ;
    /** pointer to the Calo TT online Id helper */
    const TTOnlineID    * m_ttonlineHelper;

    CxxUtils::CachedPointer<const LArTTCellMap>  m_TTCellMap;
    CxxUtils::CachedPointer<const CaloTTOnOffIdMap>  m_caloTTOnOffIdMap;
    CxxUtils::CachedPointer<const CaloTTOnAttrIdMap> m_caloTTOnAttrIdMap;
    CxxUtils::CachedPointer<const CaloTTPpmRxIdMap>  m_caloTTPpmRxIdMap;

    std::string       m_TTCellMapKey;
    std::string       m_caloTTOnOffIdMapKey;
    std::string       m_caloTTOnAttrIdMapKey;
    std::string       m_caloTTPpmRxIdMapKey;
};

#endif //CALOTRIGGERTOWERSERVICE_H
