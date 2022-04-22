//Dear emacs, this is -*-c++-*-
/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/** 
 * @class LArDigitsThinner
 * @author Nicolas Berger
 * @brief Algorithm to copy a selected subset of LArDigits
 * 
 */


#ifndef LARROD_LArDigitThinner_H
#define LARROD_LArDigitThinner_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "LArCabling/LArOnOffIdMapping.h"
//#include "Identifier/HWIdentifier.h"

#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"
#include "StoreGate/ReadCondHandle.h"
#include "AthContainers/ConstDataVector.h"

#include <vector>
#include <string>
#include <mutex>

class CaloCell_ID;
class LArOnlineID;
class LArOnOffIdMapping;
class LArDigitContainer;
class LArRawChannelContainer;

class LArDigitThinner : public AthReentrantAlgorithm
{
 public:
  LArDigitThinner(const std::string& name,
                  ISvcLocator* pSvcLocator);

  ~LArDigitThinner();
  
  StatusCode initialize() override;
  StatusCode execute(const EventContext& ctx) const override;
  StatusCode finalize() override;

 private:
     
  typedef ConstDataVector<LArDigitContainer> ConstDigitCont_t;
 
  enum Position { EMB0=0, EMB1,EMB2,EMB3,EMEC0,EMEC1,EMEC2,EMEC3,HEC,FCAL,DISCONNECTED};
  int getRegion(const HWIdentifier chid) const;

  void initCutValues(const EventContext& ctx) const;

  std::vector<int> m_energyCuts_barrel, m_energyCuts_endcap;
  int m_energyCut_hec, m_energyCut_fcal;
    
  //std::vector<int> m_seen, m_passed;

  const LArOnlineID* m_onlineID;
  const CaloCell_ID* m_caloCellId;    

  SG::ReadCondHandleKey<LArOnOffIdMapping> m_cablingKey{this,"CablingKey","LArOnOffIdMap","SG Key of LArOnOffIdMapping object"};

  //Event input:
  SG::ReadHandleKey<LArDigitContainer> m_inputKey{this, "InputContainerName","LArDigitContainer_MC", "SG Key of LArDigitContainer"};
  SG::ReadHandleKey<LArRawChannelContainer> m_rawChannelKey{this,"RawChannelContainerName","LArRawChannels", "SG key of the LArRawChannelContainer"};
  
  //Event output:
  SG::WriteHandleKey<ConstDigitCont_t> m_outputKey{this, "OutputContainerName","LArDigitContainer_Thinned", "SG Key of thinned LArDigitContainer"};

  mutable std::vector<int> m_energyCuts; //cut for each cell - indexed by online hash id
  mutable bool m_cacheFilled=false;
  mutable std::mutex m_cacheMtx; 

};

#endif
