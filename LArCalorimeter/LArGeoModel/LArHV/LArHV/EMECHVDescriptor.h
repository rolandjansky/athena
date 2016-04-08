/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EMECHVDESCRIPTOR_H_HEADER_INCLUDED_A53E30B4
#define EMECHVDESCRIPTOR_H_HEADER_INCLUDED_A53E30B4
#include "GeoModelKernel/CellBinning.h"
#include "GeoModelKernel/CellPartitioning.h"
//##ModelId=478D135F006C
class EMECHVDescriptor
{
  public:
    //##ModelId=478D138C01BE
    virtual ~EMECHVDescriptor();

    //##ModelId=478D138C01CA
    const CellPartitioning &getEtaBinning() const
    {
      return m_etaBinning;
    }

    //##ModelId=478D138C01D5
    const CellBinning &getPhiBinning() const
    {
      return m_phiBinning;
    }

    // Constructor
    //##ModelId=478D138C0214
    EMECHVDescriptor(const CellPartitioning &etaBinning, const CellBinning &phiBinning, const CellBinning &sectorBinning);

    //##ModelId=43FC51F00276
    const CellBinning &getSectorBinning() const
    {
      return m_sectorBinning;
    }


  private:

    //##ModelId=478D138C01DF
    EMECHVDescriptor(const EMECHVDescriptor& right);

    //##ModelId=478D138C01F4
    EMECHVDescriptor& operator=(const EMECHVDescriptor& right);
    //##ModelId=478D5F730160
    const CellPartitioning m_etaBinning;
    //##ModelId=478D5FC5018F
    const CellBinning m_phiBinning;
    //##ModelId=43FBFE39000C
    const CellBinning m_sectorBinning;



};



#endif /* EMECHVDESCRIPTOR_H_HEADER_INCLUDED_A53E30B4 */
