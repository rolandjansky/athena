/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EMBHVMANAGER_H_HEADER_INCLUDED_EA5D07D1
#define EMBHVMANAGER_H_HEADER_INCLUDED_EA5D07D1

#include "GeoModelKernel/RCBase.h"
#include "LArHV/EMBHVModuleConstLink.h"
#include "LArHV/EMBHVModule.h"

class EMBHVDescriptor;


// This class provides direct access to information on the HV 
// electrodes within the barrels.  The information may be accessed either
// directly or iteratively.  Direct access is provided by the getHVModule()
// method.  Iterative access
// is by looping over valid side, eta, phi, and sector indices to
// retrieve a HV module.  From the high voltage modules one
// can obtain a list of electrodes (iteratively or directly).
// 
//##ModelId=475306FC0255

struct EMBHVPayload;

class EMBHVManager : public RCBase
{
  public:

    // Constructor
    //##ModelId=475533F70142
    EMBHVManager();

    // Gets the descriptor.  (Not generally for users but nothing to hide
    // here).
    //##ModelId=4755C0750375
    const EMBHVDescriptor *getDescriptor() const;

    // Begin phi index
    //##ModelId=4755C07E0225
    unsigned int beginPhiIndex() const;

    // End phi index
    //##ModelId=4755C08F02F7
    unsigned int endPhiIndex() const;

    // Begin eta index
    //##ModelId=4755C094026E
    unsigned int beginEtaIndex() const;

    // End eta index
    //##ModelId=4755C09A015F
    unsigned int endEtaIndex() const;

    // Begin sector index
    //##ModelId=479B66510283
    unsigned int beginSectorIndex() const;

    // End sector index
    //##ModelId=479B66510295
    unsigned int endSectorIndex() const;

    // Returns a high voltage module
    //##ModelId=4755C0A200A6
    EMBHVModuleConstLink getHVModule(unsigned int iSide, unsigned int iEta, unsigned int iPhi, unsigned int iSector) const;

    // Begin side index (0=negative and 1= positive)
    //##ModelId=479BB7670398
    unsigned int beginSideIndex() const;

    // End side index (0=negative and 1= positive)
    //##ModelId=479BB76703AB
    unsigned int endSideIndex() const;

    // Refresh from the database if needed
    void update() const;

    // Make the data stale.  Force update of data.
    void reset() const;

    // Get the database payload
    EMBHVPayload *getPayload(const EMBHVElectrode &) const;

  private:
    // Destructor
    //##ModelId=475533F7019B
    virtual ~EMBHVManager();

    // Illegal operation
    //##ModelId=475533F70154
    EMBHVManager(const EMBHVManager& right);

    // Illegal operation
    //##ModelId=475533F701B8
    EMBHVManager& operator=(const EMBHVManager& right);

    friend class ImaginaryFriend;
    class Clockwork;
    Clockwork *m_c;

};



#endif /* EMBHVMANAGER_H_HEADER_INCLUDED_EA5D07D1 */
