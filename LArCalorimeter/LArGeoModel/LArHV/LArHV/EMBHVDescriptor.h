/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARHV_EMBHVDESCRIPTOR_H
#define LARHV_EMBHVDESCRIPTOR_H

#include "GeoModelKernel/CellBinning.h"

class EMBHVDescriptor
{
 public:
  EMBHVDescriptor(const CellBinning &etaBinning, const CellBinning &phiBinning);
  ~EMBHVDescriptor();

  const CellBinning &getEtaBinning() const
  {
    return m_etaBinning;
  }
  
  const CellBinning &getPhiBinning() const
  {
    return m_phiBinning;
  }
  
 private:
  EMBHVDescriptor(const EMBHVDescriptor& right);
  EMBHVDescriptor& operator=(const EMBHVDescriptor& right);

  const CellBinning m_etaBinning;
  const CellBinning m_phiBinning;
};

#endif
