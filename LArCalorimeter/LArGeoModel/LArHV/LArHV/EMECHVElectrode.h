/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EMECHVELECTRODE_H_HEADER_INCLUDED_B41803D9
#define EMECHVELECTRODE_H_HEADER_INCLUDED_B41803D9
#include "GeoModelKernel/RCBase.h"
#include "LArHV/EMECHVModuleConstLink.h"
// This class represents a single EMEC electrode.

//##ModelId=478D253700B9
class EMECHVElectrode : public RCBase
{
  public:

    //##ModelId=478D253700BB
    EMECHVElectrode(EMECHVModuleConstLink module, unsigned int iElectrode);

    //##ModelId=478D253700BD
    unsigned int getElectrodeIndex() const;

    // returns a pointer to the module that owns this electrode.
    //##ModelId=47AA6B8B03A4
    EMECHVModuleConstLink getModule() const;

    //##ModelId=47AA6C980288
    double getPhi() const;

    // HV Status
    bool hvOn(int iGap) const;
    
    // Voltage
    double voltage(int iGap) const;

    // Current
    double current(int iGap) const;

    // Voltage and current at the same time
    void voltage_current(int iGap, double& v, double& i) const;

    // HVLine no
    int hvLineNo(int iGap) const;


  private:

    //##ModelId=478D253700BF
    virtual ~EMECHVElectrode();

    //##ModelId=478D253700C1
    EMECHVElectrode(const EMECHVElectrode& right);

    //##ModelId=478D253700C3
    EMECHVElectrode& operator=(const EMECHVElectrode& right);

    class Clockwork;
    Clockwork *m_c;

};



#endif /* EMECHVELECTRODE_H_HEADER_INCLUDED_B41803D9 */
