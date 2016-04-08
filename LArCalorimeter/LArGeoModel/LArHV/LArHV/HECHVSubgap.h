/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HECHVSUBGAP_H_HEADER_INCLUDED_E1B34569
#define HECHVSUBGAP_H_HEADER_INCLUDED_E1B34569
class HECHVModule;
#include "GeoModelKernel/RCBase.h"
#include "LArHV/HECHVModuleConstLink.h"
//  
//##ModelId=47A0797F0247
class HECHVSubgap : public RCBase
{
  public:
    // Constructor
    //##ModelId=47A0797F024A
    HECHVSubgap(HECHVModuleConstLink module, unsigned int iSubgap);

    // returns a pointer to the module that owns this electrode.
    //##ModelId=47A0797F024D
    HECHVModuleConstLink getModule() const;

    // Returns the index of this electrode.
    //##ModelId=47A0797F024F
    unsigned int getSubgapIndex() const;

    // HV Status
    bool hvOn() const;
    
    // Voltage
    double voltage() const;

    // Current
    double current() const;

    // HVLine no
    int hvLineNo() const;

    // Voltage and current at the same time:
    void voltage_current(double& v, double& i) const;

  private:
    // Destructor
    //##ModelId=47A0797F0251
    virtual ~HECHVSubgap();

    // Illegal operation
    //##ModelId=47A0797F0253
    HECHVSubgap(const HECHVSubgap& right);

    // Illegal operation
    //##ModelId=47A0797F0255
    HECHVSubgap& operator=(const HECHVSubgap& right);

    class Clockwork;
    Clockwork *m_c;

};



#endif /* HECHVSUBGAP_H_HEADER_INCLUDED_E1B34569 */
