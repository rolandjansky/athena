/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/** 
 * @class ReadLArDigits
 * @author Walter Lampl
 * @brief CTB: code to read digits
 * 
 */


//Dear emacs, this is -*- c++ -*-

#ifndef READLARDIGITS_H
#define READLARDIGITS_H
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/MsgStream.h"
#include  "StoreGate/StoreGateSvc.h"
#include "LArRawEvent/LArDigitContainer.h"
#include "CaloIdentifier/LArEM_ID.h"
#include "GaudiKernel/INTupleSvc.h"
#include "GaudiKernel/NTuple.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "LArIdentifier/LArOnlineID.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "LArCabling/LArOnOffIdMapping.h"

#include <fstream>

class ReadLArDigits : public AthAlgorithm
{
 public:
  ReadLArDigits(const std::string & name, ISvcLocator * pSvcLocator);

  ~ReadLArDigits();

  //standart algorithm methods
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

 private:
  SG::ReadCondHandleKey<LArOnOffIdMapping> m_cablingKey{this, "OnOffMap", "LArOnOffIdMap", "SG key for mapping object"};
  int m_count;
  const LArEM_ID* m_emId;
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
  
/** 
 * @class SortDigits
 * @author Walter Lampl
 * @brief CTB: code to read digits
 * 
 */


  class SortDigits
    {
     public:
      SortDigits(const LArOnlineID* onlineHelper);
      inline bool operator()(LArDigit* a, LArDigit* b);
     private:
      const LArOnlineID* m_onlineHelper;
    };
};


inline bool  ReadLArDigits::SortDigits::operator()(LArDigit* a, LArDigit* b)
{const HWIdentifier chid_a=a->channelID();
 const HWIdentifier chid_b=b->channelID();
 if (m_onlineHelper->feb_Id(chid_a).get_compact() == m_onlineHelper->feb_Id(chid_b).get_compact())
   return (m_onlineHelper->channel(chid_a) <  m_onlineHelper->channel(chid_b));
 else
   return (m_onlineHelper->feb_Id(chid_a).get_compact() < m_onlineHelper->feb_Id(chid_b).get_compact());
}


#endif
