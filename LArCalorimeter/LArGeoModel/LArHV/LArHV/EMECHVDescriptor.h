/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARHV_EMECHVDESCRIPTOR_H
#define LARHV_EMECHVDESCRIPTOR_H
#include "GeoModelKernel/CellBinning.h"
#include "GeoModelKernel/CellPartitioning.h"

class EMECHVDescriptor
{
 public:
  EMECHVDescriptor(const CellPartitioning &etaBinning, const CellBinning &phiBinning, const CellBinning &sectorBinning);
  ~EMECHVDescriptor();

  const CellPartitioning &getEtaBinning() const
  {
    return m_etaBinning;
  }
  
  const CellBinning &getPhiBinning() const
  {
    return m_phiBinning;
  }

  const CellBinning &getSectorBinning() const
  {
    return m_sectorBinning;
  }

 private:
  EMECHVDescriptor(const EMECHVDescriptor& right);
  EMECHVDescriptor& operator=(const EMECHVDescriptor& right);

  const CellPartitioning m_etaBinning;
  const CellBinning m_phiBinning;
  const CellBinning m_sectorBinning;
};

#endif
