/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** 
 * @class LArDigitsThinner
 * @author Nicolas Berger
 * @brief Algorithm to copy a selected subset of LArDigits
 * 
 */


#ifndef LARMUID_LArDigitThinnerFromIIC_H
#define LARMUID_LArDigitThinnerFromIIC_H

#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/Algorithm.h"

#include "StoreGate/StoreGateSvc.h"
#include "LArTools/LArCablingService.h"
#include "LArIdentifier/LArOnlineID.h"
#include "CaloIdentifier/LArEM_ID.h"
#include "CaloIdentifier/LArHEC_ID.h"
#include "CaloIdentifier/LArFCAL_ID.h"

#include <vector>
#include <string>


class CaloDetDescrManager; 
class CaloDetDescrElement; 

class HWIdentifier;
class Identifier;

class LArDigitThinnerFromIIC : public Algorithm
{
 public:
  
   static const unsigned int nChannels = 195072;

  struct CaloPosition {
    CaloPosition(): cached(false),element(0)    {} 
    bool cached; 
    const CaloDetDescrElement* element ; 
  };
   
  typedef std::vector< CaloPosition > CablingCache;
  
  LArDigitThinnerFromIIC(const std::string& name,
                  ISvcLocator* pSvcLocator);

  ~LArDigitThinnerFromIIC();
  
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
 
  const CaloDetDescrElement* getDDE(const HWIdentifier& id) const;

  private:
    
    StatusCode iovCallBack(IOVSVC_CALLBACK_ARGS_P(/*I*/,/*keys*/));

    StatusCode getEtaPhi();
    bool channelSelection(const HWIdentifier& id)  ;

    MsgStream* m_log;
    MsgStream& log() const { return *m_log; }
    
    ServiceHandle<StoreGateSvc> m_storeGate, m_detStore;
    ToolHandle<LArCablingService> m_larCablingSvc; 
    const LArOnlineID* m_onlineID;
    const CaloDetDescrManager* m_ddm; 

    std::string m_inputContainerName, m_outputContainerName;
    std::string m_impactInCaloName;

    mutable CablingCache m_cablingCache;

    typedef std::pair<float,float> EtaPhi;
    std::vector<std::vector<EtaPhi> > m_seeds ;

    float m_dr_min;
};

#endif
