/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-
/* date of creation : 12/06/2001 */

#ifndef LARCABLING_LARCABLINGLEGACYSERVICE_H
#define LARCABLING_LARCABLINGLEGACYSERVICE_H

#include "LArCabling/LArCablingBase.h"

/** 
   @class LArCablingService 
   @brief Tool for mapping online and offline identifiers, <br>
    online and calibration identifiers <br>
    and for initialising some (FEB<->ROD) map <br><br>

  @author Fabienne Ledroit
  @author Arnaud Lucotte
  @author Hong Ma
 
*/

static const InterfaceID IID_LArCablingLegacyService("LArCablingLegacyService", 1 , 0); 

class LArCablingLegacyService : public LArCablingBase {

public:
  
  // Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_LArCablingLegacyService; }
  
  /** constructor */
  LArCablingLegacyService( const std::string& type, const std::string& name, const IInterface* parent ) ;

  ~LArCablingLegacyService();
  
  StatusCode initialize( );

  StatusCode iovCallBack(IOVSVC_CALLBACK_ARGS_K(keys));
  /**
   * return Calibration slot and channel numbers 
   */
  const std::vector<HWIdentifier>& calibSlotLine(const HWIdentifier & id) ;
  
  
  /**
   * returns the LArReadoutModuleID corresponding to the given LArFEB_ID<p>
   */
  HWIdentifier getReadoutModuleID(const HWIdentifier& febId) ;

  HWIdentifier getReadoutModuleIDByHash(const IdentifierHash& febIdHash) ;
  
  /**
   *   "iterator" on LArReadoutModuleIDs  <p>
   */
  const std::vector<HWIdentifier>& getLArRoModIDvec() ;


  /**
   * @brief Make sure the onOff map is initialized.
   */
  StatusCode checkOnOff();

  
private:
  std::vector<std::vector<HWIdentifier> > m_onlHashToCalibLines;
  const uint32_t* m_pFebHashtoROD;
  //unsigned  m_nFebRod;
  std::vector<HWIdentifier> m_readoutModuleIDVec;
  const std::vector<HWIdentifier> m_vecEmpty; 


  bool readCalibMap();
  bool readFebRodMap();

  const DataHandle<AthenaAttributeList> m_attrCalib;
  const DataHandle<AthenaAttributeList> m_attrFebRod;

  std::string m_febRodMapKey;
  std::string m_calibIdKey;

  bool m_calibValid;
  bool m_febRodValid;

};


inline const std::vector<HWIdentifier>& LArCablingLegacyService::getLArRoModIDvec() {
  if (!m_febRodValid) readFebRodMap();
  return m_readoutModuleIDVec;
}

#endif 
