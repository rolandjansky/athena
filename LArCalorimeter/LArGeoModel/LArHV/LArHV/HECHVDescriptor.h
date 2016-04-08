/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HECHVDESCRIPTOR_H_HEADER_INCLUDED_B0D9EFF4
#define HECHVDESCRIPTOR_H_HEADER_INCLUDED_B0D9EFF4
#include "GeoModelKernel/CellBinning.h"
class EMBHVDescriptor;

//##ModelId=47A07935031C
class HECHVDescriptor
{
  public:
    // Constructor
    //##ModelId=47A079350327
    HECHVDescriptor(const CellBinning &phiBinning);

    //##ModelId=47A079350321
    const CellBinning &getPhiBinning() const;

    //##ModelId=47A07935031D
    virtual ~HECHVDescriptor();

  private:
    //##ModelId=47A079350323
    HECHVDescriptor(const HECHVDescriptor& right);

    //##ModelId=47A079350325
    EMBHVDescriptor& operator=(const EMBHVDescriptor& right);

    //##ModelId=47AB7ACF0134
    const CellBinning m_phiBinning;
};



#endif /* HECHVDESCRIPTOR_H_HEADER_INCLUDED_B0D9EFF4 */
