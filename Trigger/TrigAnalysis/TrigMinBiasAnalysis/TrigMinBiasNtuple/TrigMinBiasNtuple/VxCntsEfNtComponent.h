/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMINBIASNTUPLE_VXCNTSEFNTCOMPONENT_H
#define TRIGMINBIASNTUPLE_VXCNTSEFNTCOMPONENT_H

#include "TrigNtupleBase/NtupleComponent.h"
#include <vector>

namespace NtComponent {
  class VxCntsEfNtComponent: public NtupleComponent {
  public:
    VxCntsEfNtComponent(NtupleAlgorithm *mainAlg,
			NtComponentParameters parameters);
    virtual ~VxCntsEfNtComponent();
    
    StatusCode book(void);
    StatusCode fill(void);

  private:
    std::string m_vertexCountsEfContainerName;

    // NTUPLE variables
    unsigned int m_nElements;
    std::vector<unsigned int>* m_vtxNtrks;
    std::vector<float>* m_vtxTrkPtSqSum;
  };
}

#endif
