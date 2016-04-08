/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EMECHVMANAGER_H_HEADER_INCLUDED_994115CA
#define EMECHVMANAGER_H_HEADER_INCLUDED_994115CA
class EMECHVDescriptor;
#include "GeoModelKernel/RCBase.h"
#include "LArHV/EMECHVModuleConstLink.h"
#include "LArHV/EMECHVModule.h"
//##ModelId=47530759003A

struct EMECHVPayload;


class EMECHVManager : public RCBase
{
  public:
    //##ModelId=47A2892503A6
    typedef EMECHVModule::IOType IOType;

    //##ModelId=43FBFC0A034F
    EMECHVManager(IOType wheel);

    //##ModelId=478D1079010F
    const EMECHVDescriptor *getDescriptor() const;

    //##ModelId=478D10790120
    unsigned int beginPhiIndex() const;

    //##ModelId=478D10790129
    unsigned int endPhiIndex() const;

    //##ModelId=478D10790133
    unsigned int beginEtaIndex() const;

    //##ModelId=478D1079013E
    unsigned int endEtaIndex() const;

    //##ModelId=478D10790149
    EMECHVModuleConstLink getHVModule(unsigned int iSide, unsigned int iEta, unsigned int iPhi, unsigned int iSector) const;


    // Begin side index (0=negative and 1= positive)
    //##ModelId=47A07A81015E
    unsigned int beginSideIndex() const;


    // End side index (0=negative and 1= positive)
    //##ModelId=47A07A81016F
    unsigned int endSideIndex() const;

  
    // Begin sector index
    //##ModelId=47A07AA60006
    unsigned int beginSectorIndex() const;

  
    // End sector index
    //##ModelId=47A07AA6006B
    unsigned int endSectorIndex() const;
    // Gets the Wheel, 0 for the Outer Wheel HV Manager and 1 for the inner
    // Wheel HV Manager.

    //##ModelId=47A26D390123
    EMECHVManager::IOType getWheelIndex() const;

    // Refresh from the database if needed
    void update() const;

    // Make the data stale.  Force update of data.
    void reset() const;

    // Get the database payload
    EMECHVPayload *getPayload(const EMECHVElectrode &) const;


  private:

    
    //##ModelId=478D10790154
    virtual ~EMECHVManager();


    //##ModelId=478D10790179
    EMECHVManager& operator=(const EMECHVManager& right);

    //##ModelId=478D10790160
    EMECHVManager(const EMECHVManager& right);


    //##ModelId=47A29AAF00F8
    class Clockwork;
    Clockwork *m_c;


};


#endif /* EMECHVMANAGER_H_HEADER_INCLUDED_994115CA */
