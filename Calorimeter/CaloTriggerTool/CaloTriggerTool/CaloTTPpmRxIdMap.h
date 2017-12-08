/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOTRIGGERTOOL_CALOTTPPMRXIDMAP_H
#define CALOTRIGGERTOOL_CALOTTPPMRXIDMAP_H

#include "CaloTriggerTool/CaloTTPpmRxId.h"
#include "CaloTriggerTool/CaloTTPpmRxId_P.h"

#include "TrigT1CaloCalibConditions/L1CaloCoolChannelId.h"
#include "TrigT1CaloCalibConditions/L1CaloRxCoolChannelId.h"

#include <map>

/**
 * @brief This is a StoreGate object holding the TT PPM to RX channels map
 *
 * Its persistifiable version is the CaloTTPpmRxId_P class
 *
 * @author Damien Prieur
 * @author maintained by Damien Prieur
 *
 */

class CaloTTPpmRxIdMap
{
public:

  typedef CaloTTPpmRxId PDATA;

  /* Constructor
  **/
  CaloTTPpmRxIdMap()  ;

  /* Destructor
  **/
  ~CaloTTPpmRxIdMap()  ;

  /**  initialize from Nova */
  void set(  const PDATA& t ) ;

  /**  return the corresponding Rx channels to the Ppm input one */
  std::vector<L1CaloRxCoolChannelId> ppmToRxId(const L1CaloCoolChannelId& ppmChannelId) const;

  /**  return the corresponfing ppm channel to the Rx input one */
  L1CaloCoolChannelId rxToPpmId(const L1CaloRxCoolChannelId& rxChannelId) const;

  /** return the persistified map */
  CaloTTPpmRxId_P* getP() ;

  /**  initialize from POOL */
  void set(  const CaloTTPpmRxId_P& t ) ; // initialize from POOL

private:

  void convert_to_P(const CaloTTPpmRxId &t) ; // from NOVA to POOL
  void convert_to_D(const CaloTTPpmRxId_P &t, CaloTTPpmRxId& d) ; // POOL to NOVA

  /* Ppm to Rx channel mapping */
  std::map<L1CaloCoolChannelId, std::vector<L1CaloRxCoolChannelId> > m_mPpmIdToRx;
  std::map<L1CaloRxCoolChannelId, L1CaloCoolChannelId > m_mRxIdToPpm;

  CaloTTPpmRxId_P m_persData;
};

#include "CLIDSvc/CLASS_DEF.h"
CLASS_DEF( CaloTTPpmRxIdMap,196539089,1)

#endif
