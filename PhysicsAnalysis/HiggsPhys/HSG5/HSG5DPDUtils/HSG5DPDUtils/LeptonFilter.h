/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HSG5DPDUTILS_LEPTONFILTER_H
#define HSG5DPDUTILS_LEPTONFILTER_H

#include "AthenaBaseComps/AthFilterAlgorithm.h"

#include <string>

class Algorithm;

namespace HSG5 {

  class LeptonFilter : public AthFilterAlgorithm {

  public:
    LeptonFilter(const std::string& name, ISvcLocator* pSvcLocator);
    virtual ~LeptonFilter();

    virtual StatusCode initialize();
    virtual StatusCode execute();
    virtual StatusCode finalize();

  private:
    std::string findType(std::string l_property);
    std::string findName(std::string l_property);

    Algorithm* m_electronFilter;
    std::vector<Algorithm*> m_muonFilter;

    std::string m_electronFilterNameAndType;
    std::vector<std::string> m_muonFilterNameAndTypeVec;

  };

} // end namespace

#endif // HSG5DPDUTILS_LEPTONFILTER_H
