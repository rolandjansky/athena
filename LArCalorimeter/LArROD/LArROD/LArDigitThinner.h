/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-

/** 
 * @class LArDigitsThinner
 * @author Nicolas Berger
 * @brief Algorithm to copy a selected subset of LArDigits
 * 
 */


#ifndef LARROD_LArDigitThinner_H
#define LARROD_LArDigitThinner_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "LArCabling/LArCablingService.h"
#include "Identifier/HWIdentifier.h"

#include <vector>
#include <string>

class CaloCell_ID;
class LArOnlineID;

class LArDigitThinner : public AthAlgorithm
{
 public:
  LArDigitThinner(const std::string& name,
                  ISvcLocator* pSvcLocator);

  ~LArDigitThinner();
  
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

 private:
     
  enum Position { EMB0=0, EMB1,EMB2,EMB3,EMEC0,EMEC1,EMEC2,EMEC3,HEC,FCAL,DISCONNECTED};
  int getRegion(const HWIdentifier chid) const;

  StatusCode iovCallBack(IOVSVC_CALLBACK_ARGS);
  void initCutValues();

  std::vector<int> m_energyCuts_barrel, m_energyCuts_endcap;
  int m_energyCut_hec, m_energyCut_fcal;
    
  std::vector<int> m_seen, m_passed;
    
  ToolHandle<LArCablingService> m_larCablingSvc; // yes, LArCablingService is actually a tool, not a service...
  const LArOnlineID* m_onlineID;
  const CaloCell_ID* m_caloCellId;
  
  std::string m_inputContainerName;
  std::string m_outputContainerName; 
  std::string m_rawChannelContainerName;

  std::vector<int> m_energyCuts; //cut for each cell - indexed by online hash id
  std::vector<unsigned> m_digitsPerRegion; //for reporting in finalize

};

#endif
