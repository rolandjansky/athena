/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FCALHVLINE_H_HEADER_INCLUDED_B7B4E2E0
#define FCALHVLINE_H_HEADER_INCLUDED_B7B4E2E0
#include "GeoModelKernel/RCBase.h"
#include "LArHV/FCALHVModuleConstLink.h"

class FCALHVModule;


//  
//##ModelId=47ABB4D9030F
class FCALHVLine : public RCBase
{
  public:
    // Constructor
    //##ModelId=47ABB4D90312
    FCALHVLine(FCALHVModuleConstLink module, unsigned int iLine);

    // returns a pointer to the module that owns this electrode.
    //##ModelId=47ABB4D90315
    FCALHVModuleConstLink getModule() const;

    // Returns the index of this electrode.
    //##ModelId=47ABB4D90317
    unsigned int getLineIndex() const;

    // HV Status
    bool hvOn() const;
    
    // Voltage
    double voltage() const;

    // Current
    double current() const;

    // Voltage and current at the same time:
    void voltage_current(double& v, double& i) const;

    // HVLine no
    int hvLineNo() const;

  private:
    // Destructor
    //##ModelId=47ABB4D9031B
    virtual ~FCALHVLine();

    // Illegal operation
    //##ModelId=47ABB4D9031D
    FCALHVLine(const FCALHVLine& right);

    // Illegal operation
    //##ModelId=47ABB4D9031F
    FCALHVLine& operator=(const FCALHVLine& right);

    class Clockwork;
    Clockwork *m_c;

};



#endif /* FCALHVLINE_H_HEADER_INCLUDED_B7B4E2E0 */
