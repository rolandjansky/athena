/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOTRIGGERTOOL_CALOTTONOFFIDMAP_H
#define CALOTRIGGERTOOL_CALOTTONOFFIDMAP_H

#include "CaloTriggerTool/CaloTTOnOffId.h"
#include "CaloTriggerTool/CaloTTOnOffId_P.h"
#include "CaloIdentifier/TTOnlineID.h"
#include "Identifier/Identifier.h"

#include <map>

/**
 * @brief This is a StoreGate object holding the TT offline-online map
 *
 * Its persistifiable version is the CaloTTOnOffId_P class
 *
 * @author Damien Prieur
 * @author maintained by Damien Prieur
 *
 */

class CaloTTOnOffIdMap
{
public:

  typedef CaloTTOnOffId PDATA;

  /* Constructor
  **/
  CaloTTOnOffIdMap()  ;

  /* Destructor
  **/
  ~CaloTTOnOffIdMap()  ;

  /**  initialize from Nova */
  void set(  const PDATA& t ) ;

  /**  return the corresponfing online Id of an offline one */
  HWIdentifier createSignalChannelID(const Identifier& id, bool bQuiet=false) const ;

  /**  return the corresponfing offline Id of an online one */
  Identifier cnvToIdentifier(const HWIdentifier & sid, bool bQuiet=false) const;

  /** return the persistified map */
  CaloTTOnOffId_P* getP() ;

  /**  initialize from POOL */
  void set(  const CaloTTOnOffId_P& t ) ; // initialize from POOL

private:

  void convert_to_P(const CaloTTOnOffId &t) ; // from NOVA to POOL
  void convert_to_D(const CaloTTOnOffId_P &t, CaloTTOnOffId& d) ; // POOL to NOVA

  /* Offline Identifier to HWIdentifier mapping */
  std::map<Identifier,HWIdentifier>  m_off2onIdMap;
  std::map<HWIdentifier,Identifier>  m_on2offIdMap;

  CaloTTOnOffId_P m_persData;
};

#include "AthenaKernel/CLASS_DEF.h"
CLASS_DEF( CaloTTOnOffIdMap,267570070,1)

#endif
