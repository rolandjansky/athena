/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EMBPresamplerHVDESCRIPTOR_H_HEADER_INCLUDED_F36D906D
#define EMBPresamplerHVDESCRIPTOR_H_HEADER_INCLUDED_F36D906D

#include "GeoModelKernel/CellBinning.h"
#include "GeoModelKernel/CellPartitioning.h"

class EMBPresamplerHVDescriptor
{
 public:
  // Constructor
  EMBPresamplerHVDescriptor(const CellPartitioning &etaPartitioning, const CellBinning &phiBinning);
  
  virtual ~EMBPresamplerHVDescriptor();
  
  const CellPartitioning &getEtaPartitioning() const
    {
      return etaPartitioning;
    }
  
  
  const CellBinning &getPhiBinning() const
    {
      return phiBinning;
    }
  
 private:
  
  
  EMBPresamplerHVDescriptor(const EMBPresamplerHVDescriptor& right);
  
  EMBPresamplerHVDescriptor& operator=(const EMBPresamplerHVDescriptor& right);
  
  
  const CellPartitioning etaPartitioning;
  
  const CellBinning phiBinning;
  
};



#endif /* EMBPresamplerHVDESCRIPTOR_H_HEADER_INCLUDED_F36D906D */
