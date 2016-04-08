/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EMECPresamplerHVMANAGER_H_HEADER_INCLUDED_994115CA
#define EMECPresamplerHVMANAGER_H_HEADER_INCLUDED_994115CA
class EMECPresamplerHVDescriptor;
#include "GeoModelKernel/RCBase.h"
#include "LArHV/EMECPresamplerHVModuleConstLink.h"
#include "LArHV/EMECPresamplerHVModule.h"
//##ModelId=47530759003A

struct EMECPresamplerHVPayload;
class CellBinning;

class EMECPresamplerHVManager : public RCBase
{
  public:

  // Constructor
    EMECPresamplerHVManager();

    
    const CellBinning *getPhiBinning() const;


    unsigned int beginPhiIndex() const;


    unsigned int endPhiIndex() const;


    // Begin side index (0=negative and 1= positive)
    unsigned int beginSideIndex() const;
    
    // End side index (0=negative and 1= positive)
    unsigned int endSideIndex() const;

    // Get a link to the HV module:
    EMECPresamplerHVModuleConstLink getHVModule(unsigned int iSide, unsigned int iPhi) const;

    // Refresh from the database if needed
    void update() const;

    // Make the data stale.  Force update of data.
    void reset() const;

    // Get the database payload
    EMECPresamplerHVPayload *getPayload(const EMECPresamplerHVModule &) const;


  private:

    
    virtual ~EMECPresamplerHVManager();


    EMECPresamplerHVManager& operator=(const EMECPresamplerHVManager& right);

    EMECPresamplerHVManager(const EMECPresamplerHVManager& right);


    class Clockwork;
    Clockwork *m_c;


};


#endif /* EMECPresamplerHVMANAGER_H_HEADER_INCLUDED_994115CA */
