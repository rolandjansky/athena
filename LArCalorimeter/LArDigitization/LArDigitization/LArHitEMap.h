/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARDIGITIZATION_LARHITEMAP_H
#define LARDIGITIZATION_LARHITEMAP_H
//
#include <utility>
#include <vector>

#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/DataHandle.h"
#include "LArDigitization/LArHitList.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/LArEM_ID.h"
#include "CaloIdentifier/LArHEC_ID.h"
#include "CaloIdentifier/LArFCAL_ID.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "LArRawEvent/LArDigit.h"
#include "CLIDSvc/CLASS_DEF.h"

class ISvcLocator;
class LArCablingService;

class LArHitEMap
{

public: 
 typedef std::vector<LArHitList*>  EMAP;
 typedef std::vector<const LArDigit*> DIGMAP;

 enum { EMBARREL_INDEX  = 0
       ,EMENDCAP_INDEX  = 1
       ,HADENDCAP_INDEX = 2
       ,FORWARD_INDEX   = 3
       ,NUMDET=4
     };

private:
 EMAP m_emap; 
 DIGMAP m_digmap;
 //StoreGateSvc* m_storeGateSvc;
 int m_ncellem,m_ncellhec,m_ncellfcal;
 const DataHandle<CaloIdManager> m_caloIdMgr;
 const LArEM_ID*   m_larem_id;
 const LArHEC_ID*  m_larhec_id;
 const LArFCAL_ID* m_larfcal_id;
 const DataHandle<CaloDetDescrManager> m_calodetdescrmgr;
 ToolHandle<LArCablingService> m_cablingService;

 bool m_initialized; 
  
public:
  LArHitEMap(void);
  ~LArHitEMap(void);
  bool Initialize(std::vector<bool>& flags, bool windows=false, bool digit=false ) ;
  void EnergyReset(void);
  bool AddEnergy(unsigned int index,float energy, float time);
  bool AddEnergy(const Identifier & cellid, float energy, float time);
  bool BuildWindows(float deta, float dphi, float ptmin);
  int GetNbCells(void);
  inline int get_ncellem(){return m_ncellem;} ;
  inline int get_ncellhec(){return m_ncellhec;} ;
  inline int get_ncellfcal(){return m_ncellfcal;} ;
  inline LArHitList* GetCell(unsigned int index){return m_emap[index];} ;
  std::vector<std::pair<float,float> > * GetTimeE(unsigned int index);
  int find(Identifier cellid);
  bool  initialized(); 
  void DigitReset();
  bool AddDigit(const LArDigit* digit);
  inline const LArDigit* GetDigit(unsigned int index) {
    if (index<m_digmap.size()) return m_digmap[index];
    else return 0; 
  }

};

CLASS_DEF( LArHitEMap , 103846231 , 1 )
#endif
