//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


#ifndef READLARDIGITS_H
#define READLARDIGITS_H
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/MsgStream.h"
#include  "StoreGate/StoreGateSvc.h"
#include "TBEvent/TBLArDigitContainer.h"
#include "LArCabling/LArCablingLegacyService.h"
#include "CaloIdentifier/LArEM_ID.h"
#include "CaloIdentifier/LArFCAL_ID.h"
#include "CaloIdentifier/LArHEC_ID.h"
#include "GaudiKernel/INTupleSvc.h"
#include "GaudiKernel/NTuple.h"
#include "GaudiKernel/SmartDataPtr.h"
//#include "LArDetDescr/LArDetDescrManager.h"
#include <fstream>
#include "LArIdentifier/LArOnlineID.h"

class ReadTBLArDigits : public AthAlgorithm
{
 public:
  ReadTBLArDigits(const std::string & name, ISvcLocator * pSvcLocator);

  ~ReadTBLArDigits();

  //standart algorithm methods
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

 private:
  int m_count;
  LArCablingLegacyService *m_larCablingSvc;
  const LArEM_ID*   m_emId;
  const LArFCAL_ID* m_fcalId;
  const LArHEC_ID*  m_hecId;
  const LArOnlineID* m_onlineHelper;
  std::ofstream m_outfile;
  std::string m_containerKey;
  std::string m_dumpFile;
  bool m_printCellLoc;
  bool m_printFebChan;
  //std::string m_NTupleLoc;
  
  NTuple::Tuple* m_ntuplePtr;
  //NTuple::Item<long> m_cellIndex;
  NTuple::Item<long> m_cellIndex;
  NTuple::Array<long> m_layer, m_eta, m_phi, m_gain;
  NTuple::Array<long> m_barrel_ec, m_pos_neg, m_FT, m_slot, m_channel;
  NTuple::Matrix<long> m_samples; 
  NTuple::Item<long>  m_Nsamples;
  //NTuple::Array<long>* m_samples;
  //int m_NSamples;
  
  class SortDigits
    {
     public:
      SortDigits(const LArOnlineID* onlineHelper);
      inline bool operator()(LArDigit* a, LArDigit* b);
     private:
      const LArOnlineID* m_onlineHelper;
    };
};


inline bool  ReadTBLArDigits::SortDigits::operator()(LArDigit* a, LArDigit* b)
{const HWIdentifier chid_a=a->channelID();
 const HWIdentifier chid_b=b->channelID();
 if (m_onlineHelper->feb_Id(chid_a).get_compact() == m_onlineHelper->feb_Id(chid_b).get_compact())
   return (m_onlineHelper->channel(chid_a) <  m_onlineHelper->channel(chid_b));
 else
   return (m_onlineHelper->feb_Id(chid_a).get_compact() < m_onlineHelper->feb_Id(chid_b).get_compact());
}


#endif
