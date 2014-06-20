/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMBD3PDMAKER_LUCIDDIGITFILLERTOOL_H
#define TRIGMBD3PDMAKER_LUCIDDIGITFILLERTOOL_H

#include "D3PDMakerUtils/BlockFillerTool.h"
#include "LUCID_RawEvent/LUCID_DigitContainer.h"

namespace TrigMbD3PD {

  typedef LUCID_DigitContainer LucidDigitFillerObjectType;
  typedef D3PD::BlockFillerTool<LucidDigitFillerObjectType> LucidDigitFillerBase;
  
  class LucidDigitFillerTool: public LucidDigitFillerBase {
    
  public:
    
    LucidDigitFillerTool (const std::string& type,
			  const std::string& name,
			  const IInterface* parent);
    
    virtual StatusCode initialize();
    virtual StatusCode book();
    virtual StatusCode fill(const LucidDigitFillerObjectType& p);
    
  private:
    
    void clearData();
    
    /* NTuple variables **/
    std::vector<unsigned short>* m_tubeID;
    std::vector<unsigned short>* m_npePmt;
    std::vector<double>*         m_npe;
    std::vector<bool>*           m_isHit;
    std::vector<bool>*           m_isSideA;
    std::vector<bool>*           m_isSideC;
  };
}

#endif
