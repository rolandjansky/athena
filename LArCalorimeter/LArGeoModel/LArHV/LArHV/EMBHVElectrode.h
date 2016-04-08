/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EMBHVELECTRODE_H_HEADER_INCLUDED_D823FBB9
#define EMBHVELECTRODE_H_HEADER_INCLUDED_D823FBB9
#include "GeoModelKernel/RCBase.h"
#include "LArHV/EMBHVModuleConstLink.h"
//  
//##ModelId=475696070335
class EMBHVElectrode : public RCBase
{
  public:
    // Constructor
    //##ModelId=475697F20323
    EMBHVElectrode(EMBHVModuleConstLink module, unsigned int iElectrode);

    // Returns the index of this electrode.
    //##ModelId=4756987F0395
    unsigned int getElectrodeIndex() const;

    // returns a pointer to the module that owns this electrode.
    //##ModelId=479BCD0B039F
    EMBHVModuleConstLink getModule() const;

    //##ModelId=47AA6C740007
    double getPhi() const;

    // HV Status
    bool hvOn(int iGap) const;
    
    // Voltage
    double voltage(int iGap) const;

    // Current
    double current(int iGap) const;

    // HVLine no
    int hvLineNo(int iGap) const;

    // Voltage and current at the same tine.:
    void voltage_current(int iGap, double& v, double& i) const;


  private:
    // Destructor
    //##ModelId=475697F20382
    virtual ~EMBHVElectrode();

    // Illegal operation
    //##ModelId=475697F20337
    EMBHVElectrode(const EMBHVElectrode& right);

    // Illegal operation
    //##ModelId=475697F203A5
    EMBHVElectrode& operator=(const EMBHVElectrode& right);

    class Clockwork;
    Clockwork *m_c;
};



#endif /* EMBHVELECTRODE_H_HEADER_INCLUDED_D823FBB9 */
