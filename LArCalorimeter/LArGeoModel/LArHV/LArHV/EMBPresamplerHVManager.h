/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EMBPresamplerHVMANAGER_H_HEADER_INCLUDED_EA5D07D1
#define EMBPresamplerHVMANAGER_H_HEADER_INCLUDED_EA5D07D1

#include "GeoModelKernel/RCBase.h"
#include "LArHV/EMBPresamplerHVModuleConstLink.h"
#include "LArHV/EMBPresamplerHVModule.h"

class EMBPresamplerHVDescriptor;


// This class provides direct access to information on the HV 
// electrodes within the barrels.  The information may be accessed either
// directly or iteratively.  Direct access is provided by the getHVModule()
// method.  Iterative access
// is by looping over valid side, eta, phi, and sector indices to
// retrieve a HV module.  From the high voltage modules one
// can obtain a list of electrodes (iteratively or directly).
// 

struct EMBPresamplerHVPayload;

class EMBPresamplerHVManager : public RCBase
{
  public:

    // Constructor
    EMBPresamplerHVManager();

    // Gets the descriptor.  (Not generally for users but nothing to hide
    // here).
    const EMBPresamplerHVDescriptor *getDescriptor() const;

    // Begin phi index
    unsigned int beginPhiIndex() const;

    // End phi index
    unsigned int endPhiIndex() const;

    // Begin eta index
    unsigned int beginEtaIndex() const;

    // End eta index
    unsigned int endEtaIndex() const;

    // Returns a high voltage module
    EMBPresamplerHVModuleConstLink getHVModule(unsigned int iSide, unsigned int iEta, unsigned int iPhi) const;

    // Begin side index (0=negative and 1= positive)
    unsigned int beginSideIndex() const;

    // End side index (0=negative and 1= positive)
    unsigned int endSideIndex() const;

    // Refresh from the database if needed
    void update() const;

    // Make the data stale.  Force update of data.
    void reset() const;

    // Get the database payload
    EMBPresamplerHVPayload *getPayload(const EMBPresamplerHVModule &) const;

  private:
    // Destructor
    //##ModelId=475533F7019B
    virtual ~EMBPresamplerHVManager();

    // Illegal operation
    //##ModelId=475533F70154
    EMBPresamplerHVManager(const EMBPresamplerHVManager& right);

    // Illegal operation
    //##ModelId=475533F701B8
    EMBPresamplerHVManager& operator=(const EMBPresamplerHVManager& right);

    friend class ImaginaryFriend;
    class Clockwork;
    Clockwork *m_c;

};



#endif /* EMBPresamplerHVMANAGER_H_HEADER_INCLUDED_EA5D07D1 */
