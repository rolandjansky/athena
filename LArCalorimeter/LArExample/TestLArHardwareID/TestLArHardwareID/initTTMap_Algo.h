/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INITTTMAP_ALGO_H
#define INITTTMAP_ALGO_H

/* author : F. Ledroit */
/* date of creation : 10/10/2006 */

#include "AthenaBaseComps/AthAlgorithm.h"
#include <string>

/** 
  * Algorithm meant to initialise LAr TT<->cell maping
  *
  */
class CaloLVL1_ID;
class LArEM_ID;
class LArHEC_ID;
class LArFCAL_ID;
//class LArOnlineID;
class CaloTriggerTowerService;

class initTTMap_Algo : public AthAlgorithm {

  private :
  

  /* property to select TT mapping input for FCAL
   * ON  = take FCAL mapping from G. Oakham's file
   */
  std::string m_initFromFile ; 
  bool m_dumpMap;

  CaloTriggerTowerService* m_triggertowerSvc ; 
  const CaloLVL1_ID*      m_lvl1Helper;
  const LArEM_ID*         m_emHelper;
  const LArHEC_ID*        m_hecHelper;
  const LArFCAL_ID*       m_fcalHelper;
  //  const LArOnlineID*      m_onlHelper;
  
  virtual  StatusCode initMap();
  virtual  StatusCode testStruct();

  public :
  
  initTTMap_Algo(const std::string &name ,  ISvcLocator* pSvcLocator) ;
  virtual ~initTTMap_Algo();
  virtual  StatusCode initialize();
  virtual  StatusCode execute();
  virtual  StatusCode finalize();
  
};

#endif
