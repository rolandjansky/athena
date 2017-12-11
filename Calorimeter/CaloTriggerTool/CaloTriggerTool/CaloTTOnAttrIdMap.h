/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOTRIGGERTOOL_CALOTTONATTRIDMAP_H
#define CALOTRIGGERTOOL_CALOTTONATTRIDMAP_H

#include "CaloTriggerTool/CaloTTOnAttrId.h"
#include "CaloTriggerTool/CaloTTOnAttrId_P.h"
#include "CaloIdentifier/TTOnlineID.h"
#include "Identifier/Identifier.h"

#include <map>

/**
 * @brief This is a StoreGate object holding the TT online-attribute map
 *
 * Its persistifiable version is the CaloTTOnAttrId_P class
 *
 * @author Damien Prieur
 * @author maintained by Damien Prieur
 *
 */

struct AttrStruct {
    unsigned int em_had;
    unsigned int pos_neg;
    unsigned int barrel_endcap_fcal;
    unsigned int module_type;
};

class CaloTTOnAttrIdMap
{
public:

  typedef CaloTTOnAttrId PDATA;

  /* Constructor
  **/
  CaloTTOnAttrIdMap( )  ;

  /* Destructor
  **/
  ~CaloTTOnAttrIdMap()  ;

  /**  initialize from Nova */
  void set(  const PDATA& t ) ;

  /** return the corresponding sampling of channelId:
      @return 0: for EM calorimeter
      @return 1: for HAD calorimeter
      @return 999: error
  */
  unsigned int em_had            ( HWIdentifier channelId ) const;

  /** return 0 for z<0 channels and 1 for z>0 channels */
  unsigned int pos_neg           ( HWIdentifier channelId ) const;

  /** To know if a channel belongs to the barrel, end cap or fcal parts
      @return 0: barrel
      @return 1: end cap
      @return 2: fcal
      @return 999: error
  */
  unsigned int barrel_endcap_fcal( HWIdentifier channelId ) const;

  /** To get the type of the module connected to the channel:
      @return 1: PPM
      @return 2: CPM
      @return 3: JEM
      @return 999: error
  */
  unsigned int module_type       ( HWIdentifier channelId ) const;

  /** return the persistified map */
  CaloTTOnAttrId_P* getP() ;

  /**  initialize from POOL */
  void set(  const CaloTTOnAttrId_P& t ) ; // initialize from POOL

private:

  void convert_to_P(const CaloTTOnAttrId &t) ; // from NOVA to POOL
  void convert_to_D(const CaloTTOnAttrId_P &t, CaloTTOnAttrId& d) ; // POOL to NOVA

  /* Online Identifier to Attribute mapping */
  std::map<HWIdentifier,AttrStruct>  m_on2attrIdMap;

  CaloTTOnAttrId_P m_persData;

};

#include "AthenaKernel/CLASS_DEF.h"
CLASS_DEF( CaloTTOnAttrIdMap,167960877,1)

#endif
