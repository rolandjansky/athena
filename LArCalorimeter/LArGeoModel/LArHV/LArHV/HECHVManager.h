/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HECHVMANAGER_H_HEADER_INCLUDED_B88C423D
#define HECHVMANAGER_H_HEADER_INCLUDED_B88C423D
#include "GeoModelKernel/RCBase.h"
class HEECHVManager;
class HECHVDescriptor;
#include "HECHVModule.h"
typedef ConstLink<HECHVModule> HECHVModuleConstLink;

struct HECHVPayload;

//##ModelId=4753077F00A0
class HECHVManager : public RCBase
{
  public:
    //##ModelId=47A07A0C016B
    HECHVManager();

    //##ModelId=47A07A0C0176
    const HECHVDescriptor *getDescriptor() const;

    // Begin side index (0=negative and 1= positive)
    //##ModelId=47A07AC901D3
    unsigned int beginSideIndex() const;

    // End side index (0=negative and 1= positive)
    //##ModelId=47A07AC901E4
    unsigned int endSideIndex() const;

    //##ModelId=47A07A0C01A1
    unsigned int beginPhiIndex() const;

    //##ModelId=47A07A0C01AD
    unsigned int endPhiIndex() const;

    //##ModelId=47A07A0C01BA
    unsigned int beginSamplingIndex() const;

    //##ModelId=47A07A0C01C9
    unsigned int endSamplingIndex() const;

    //##ModelId=47A07A0C01D7
    HECHVModuleConstLink getHVModule(unsigned int iSide, unsigned int iPhi, unsigned int iSampling) const;

    // Refresh from the database if needed
    void update() const;

    // Make the data stale.  Force update of data.
    void reset() const;

    // Get the database payload
    HECHVPayload *getPayload(const HECHVSubgap &) const;

  private:


    //##ModelId=47A07A0C01E5
    virtual ~HECHVManager();

    //##ModelId=47A07A0C01F3
    HECHVManager(const HECHVManager& right);

    //##ModelId=47A07A0C0210
    HECHVManager& operator=(const HECHVManager& right);

    class Clockwork;
    Clockwork *m_c;

};



#endif /* HECHVMANAGER_H_HEADER_INCLUDED_B88C423D */
