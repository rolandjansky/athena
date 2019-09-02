/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARHV_HECHVDESCRIPTOR_H
#define LARHV_HECHVDESCRIPTOR_H

#include "GeoModelKernel/CellBinning.h"

class HECHVDescriptor
{
 public:
  HECHVDescriptor(const CellBinning &phiBinning);
  ~HECHVDescriptor();

  const CellBinning& getPhiBinning() const
  {
    return m_phiBinning;
  }
  
 private:
  HECHVDescriptor(const HECHVDescriptor& right);
  HECHVDescriptor& operator=(const HECHVDescriptor& right);

  const CellBinning m_phiBinning;
};

#endif /* HECHVDESCRIPTOR_H_HEADER_INCLUDED_B0D9EFF4 */
