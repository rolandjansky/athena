/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARHV_EMBHVELECTRODE_H
#define LARHV_EMBHVELECTRODE_H

class EMBHVModule;

class EMBHVElectrode
{
  public:
    // Constructor
    EMBHVElectrode(const EMBHVModule* module, unsigned int iElectrode);

    // Destructor
    virtual ~EMBHVElectrode();

    // Returns the index of this electrode.
    unsigned int getElectrodeIndex() const;

    // returns a pointer to the module that owns this electrode.
    const EMBHVModule& getModule() const;

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
    // Illegal operation
    EMBHVElectrode(const EMBHVElectrode& right);

    // Illegal operation
    EMBHVElectrode& operator=(const EMBHVElectrode& right);

    class Clockwork;
    Clockwork *m_c;
};

#endif 
