/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMBD3PDMAKER_LUCIDRAWDATAFILLERTOOL_H
#define TRIGMBD3PDMAKER_LUCIDRAWDATAFILLERTOOL_H

#include "D3PDMakerUtils/BlockFillerTool.h"
#include "LUCID_RawEvent/LUCID_RawDataContainer.h"

namespace TrigMbD3PD {

  typedef LUCID_RawDataContainer LucidRawDataFillerObjectType;
  typedef D3PD::BlockFillerTool<LucidRawDataFillerObjectType> LucidRawDataFillerBase;

  class LucidRawDataFillerTool: public LucidRawDataFillerBase {

  public:
    
    LucidRawDataFillerTool (const std::string& type,
			    const std::string& name,
			    const IInterface* parent);
    
    virtual StatusCode initialize();
    virtual StatusCode book();
    virtual StatusCode fill(const LucidRawDataFillerObjectType& p);

  private:
    
    void clearData();
    
    /* NTuple variables **/
    int* m_word0;
    int* m_word1;
    int* m_word2;
    int* m_word3;
    int* m_word0p;
    int* m_word1p;
    int* m_word2p;
    int* m_word3p;
    int* m_word0n;
    int* m_word1n;
    int* m_word2n;
    int* m_word3n;
    int* m_status;
    int* m_totalHits;
    int* m_aSideHits;
    int* m_cSideHits;
  };
}

#endif
