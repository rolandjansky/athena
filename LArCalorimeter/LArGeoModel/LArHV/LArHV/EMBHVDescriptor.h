/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EMBHVDESCRIPTOR_H_HEADER_INCLUDED_F36D906D
#define EMBHVDESCRIPTOR_H_HEADER_INCLUDED_F36D906D

#include "GeoModelKernel/CellBinning.h"

//##ModelId=475308BA007D
class EMBHVDescriptor
{
  public:
    // Constructor
    //##ModelId=4755BF39021B
  EMBHVDescriptor(const CellBinning &etaBinning, const CellBinning &phiBinning);
  
  //##ModelId=4755BF390264
  virtual ~EMBHVDescriptor();
  //##ModelId=47564B2A030D
  const CellBinning &getEtaBinning() const
    {
      return m_etaBinning;
    }
  
  
  //##ModelId=47564B5C0224
  const CellBinning &getPhiBinning() const
    {
      return m_phiBinning;
    }
  
 private:
  //##ModelId=4755BF39022A
  EMBHVDescriptor(const EMBHVDescriptor& right);
  
  //##ModelId=4755BF390280
  EMBHVDescriptor& operator=(const EMBHVDescriptor& right);
    //##ModelId=47A3AC8F01E0
    const CellBinning m_etaBinning;
  
    //##ModelId=47A3AC8F01F1
    const CellBinning m_phiBinning;

};



#endif /* EMBHVDESCRIPTOR_H_HEADER_INCLUDED_F36D906D */
