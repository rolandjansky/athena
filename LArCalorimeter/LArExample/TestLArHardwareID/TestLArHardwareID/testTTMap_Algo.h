/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TESTTTMAP_ALGO_H
#define TESTTTMAP_ALGO_H

/* author : F. Ledroit */
/* date of creation : 10/10/2006 */

#include "AthenaBaseComps/AthAlgorithm.h"
#include <string>

/** 
  * Algorithm meant to test LAr TT<->cell maping db obj
  *
  */
class CaloLVL1_ID;
class LArEM_ID;
class LArHEC_ID;
class LArFCAL_ID;
class CaloTriggerTowerService;

class testTTMap_Algo : public AthAlgorithm {

  private :

  const CaloLVL1_ID*      m_lvl1Helper;
  const LArEM_ID*         m_emHelper;
  const LArHEC_ID*        m_hecHelper;
  const LArFCAL_ID*       m_fcalHelper;
  CaloTriggerTowerService*      m_ttSvc;

  bool m_dumpMap;
  
  public :
  
  testTTMap_Algo(const std::string &name ,  ISvcLocator* pSvcLocator) ;
  virtual ~testTTMap_Algo();
  virtual  StatusCode initialize();
  virtual  StatusCode execute();
  virtual  StatusCode finalize();
  
};

#endif
