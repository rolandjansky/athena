/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TESTLARHWID_ALGO_H
#define TESTLARHWID_ALGO_H

/* author : F. Ledroit */
/* modif  : A. Lucotte */
/* date of creation : 01/07/2001 */

#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "LArCabling/LArOnOffIdMapping.h"
#include "LArRecConditions/LArHVIdMapping.h"

#include <string>

/** 
  * Algorithm example testing online<-> offline mapping
  *
  */
class CaloLVL1_ID;
class LArEM_ID;
class LArHEC_ID;
class LArFCAL_ID;
class LArOnlineID;
class LArHVCellID;
class LArHVLineID;
class LArElectrodeID;
class LArHVCablingTool;

class TestLArHWID_Algo : public AthAlgorithm {

  private :
  
  /** property to select sub-system 
    * ALL  = all detectors
    * EM   = Electromagnetic 
    * EMB  = EM Barrel
    * EMEC = EM EndCap
    * HEC  = hadronic endcap
    * FCAL = Forward calorimeter
    * anything else = default = EMB 
    */
  std::string m_Detector ; 

  /* property to choose wherefrom to take the helpers:
   *  via Detector Description manager or directly Detector Store
   * MGR = default = via DetDescrMgr
   * DIRECT = directly
   */
  std::string m_Manager ; 


  /* property to select HighVoltage Option
   * ON  = provide HV cells
   * OFF = does not (default)
   */
  std::string m_Calibration ; 


  /* property to select Calibration Option
   * ON  = provide calibration
   * OFF = does not (default)
   */
  std::string m_HighVoltage ; 

  /* property to select HVline info
   * ON  = provide HVline info
   * OFF = does not (default)
   */
  std::string m_HVlineToOffline; 

  /* property to select HVelectrode info
   * ON  = provide HVline info
   * OFF = does not (default)
   */
  std::string m_HVelectrodeToOffline; 

  /* property to select Connected Option
   * ON  = Connected channels only
   * OFF = Disconnected channels only
   * ALL = All channels
   */
  std::string m_Connected ; 

  /* property to select Online identifiers Test
   * ON  = provide tests
   * OFF = does not (default)
   */
  std::string m_FeedthroughTest ; 

  /* property to list feedthrough list per detector
   * ON  
   * OFF (default)
   */
  std::string m_OnlineTest ; 


  /* property to select Offline identifiers Test 
   * ON  = provide Online tests
   * OFF = does not (default)
   */
  std::string m_OfflineTest ; 


  /* property to select SubSystem
   * EM1, EM2, EM3, EMBPS, etc...
   */
  std::string m_SubDetector ; 


  /* property to select TT mapping Test Option
   * ON  = provide TT mapping tests
   * OFF = does not (default)
   */
  //std::string m_Connected ; 


  const CaloLVL1_ID*      m_lvl1Helper;
  const LArEM_ID*         m_emHelper;
  const LArHEC_ID*        m_hecHelper;
  const LArFCAL_ID*       m_fcalHelper;
  const LArOnlineID*      m_onlineHelper;
  const LArHVLineID*      m_hvHelper;
  const LArElectrodeID*   m_electrodeHelper;
  //LArHVCablingTool*       m_hvcablingTool;
  SG::ReadCondHandleKey<LArOnOffIdMapping> m_cablingReadKey{this,"CablingKey","LArOnOffIdMap","Key for OnOffId map"};
  SG::ReadCondHandleKey<LArHVIdMapping> m_HVReadKey{this,"HVMapKey","LArHVIdMap", "LArHV mapping object key"};
  

  public :
  
  TestLArHWID_Algo(const std::string &name ,  ISvcLocator* pSvcLocator) ;
  virtual ~TestLArHWID_Algo();
  virtual  StatusCode initialize();
  virtual  StatusCode execute();
  virtual  StatusCode finalize();
  
};

#endif
