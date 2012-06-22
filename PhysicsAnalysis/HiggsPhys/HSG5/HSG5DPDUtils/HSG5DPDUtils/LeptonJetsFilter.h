/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HSG5DPDUTILS_LEPTONJETSFILTER_H
#define HSG5DPDUTILS_LEPTONJETSFILTER_H

#include "AthenaBaseComps/AthFilterAlgorithm.h"

#include <string>


namespace HSG5 {

  class LeptonJetsFilter : public AthFilterAlgorithm {

  public:

    LeptonJetsFilter(const std::string& name, ISvcLocator* pSvcLocator);
    virtual ~LeptonJetsFilter();

    virtual StatusCode initialize();
    virtual StatusCode execute();
    virtual StatusCode finalize();

  private:
    
    std::string m_selectedJetContainerName;
    std::string m_selectedMuonContainerName;

    int m_minSumJetsAndMuon;
    int m_maxSumJetsAndMuon;
    
    int m_nProcessed;
    int m_nEventPassTrigger;
  };

} // end namespace

#endif // HSG5DPDUTILS_LEPTONJETSFILTER_H
