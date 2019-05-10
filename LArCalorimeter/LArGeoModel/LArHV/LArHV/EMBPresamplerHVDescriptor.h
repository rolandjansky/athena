/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARHV_EMBPRESAMPLERHVDESCRIPTOR_H
#define LARHV_EMBPRESAMPLERHVDESCRIPTOR_H

#include "GeoModelKernel/CellBinning.h"
#include "GeoModelKernel/CellPartitioning.h"

class EMBPresamplerHVDescriptor
{
 public:
  EMBPresamplerHVDescriptor(const CellPartitioning &etaPartitioning, const CellBinning &phiBinning);
  ~EMBPresamplerHVDescriptor();
  
  const CellPartitioning &getEtaPartitioning() const
  {
    return m_etaPartitioning;
  }
  
  const CellBinning &getPhiBinning() const
  {
    return m_phiBinning;
  }
  
 private:
  // Illegal operations
  EMBPresamplerHVDescriptor(const EMBPresamplerHVDescriptor& right);
  EMBPresamplerHVDescriptor& operator=(const EMBPresamplerHVDescriptor& right);
  
  const CellPartitioning m_etaPartitioning;
  const CellBinning m_phiBinning;
};

#endif
