/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DUMPLARDIGITS_H
#define DUMPLARDIGITS_H
#include "AthenaBaseComps/AthAlgorithm.h"
#include "LArRawEvent/LArDigitContainer.h"
#include "CaloIdentifier/LArEM_ID.h"
#include "LArIdentifier/LArOnlineID.h"
#include "LArCabling/LArOnOffIdMapping.h"
#include "StoreGate/ReadCondHandleKey.h"

//#include "LArDetDescr/LArDetDescrManager.h"
#include <fstream>

class DumpLArDigits : public AthAlgorithm
{
 public:
  DumpLArDigits(const std::string & name, ISvcLocator * pSvcLocator);

  ~DumpLArDigits();

  //standart algorithm methods
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
 private:
  SG::ReadCondHandleKey<LArOnOffIdMapping> m_cablingKey{this,"CablingKey","LArOnOffIdMap","SG Key of LArOnOffIdMapping object"};
  int m_count, m_chan;
  //LArDigitContainer* m_larDigitCont;
  const LArOnlineID*       m_onlineHelper; 
  const LArEM_ID* m_emId;
  std::ofstream m_outfile;
  std::string m_key, m_FileName;
 private:
  class mySort {
  public:
    bool operator()(const LArDigit* a, const LArDigit* b);
  };
};

#endif
