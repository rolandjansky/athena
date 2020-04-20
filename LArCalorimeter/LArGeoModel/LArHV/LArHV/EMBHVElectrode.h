/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARHV_EMBHVELECTRODE_H
#define LARHV_EMBHVELECTRODE_H

class EMBHVModule;

#if !(defined(SIMULATIONBASE) || defined(GENERATIONBASE))
class LArHVIdMapping;
#endif

class EMBHVElectrode
{
 public:
  // The ownership of the EMBHVModule pointer NOT transferred to EMBHVElectrode
  EMBHVElectrode(const EMBHVModule* module, unsigned int iElectrode);
  ~EMBHVElectrode();

  // Returns the index of this electrode.
  unsigned int getElectrodeIndex() const;
  
  // returns a reference to the module that owns this electrode.
  const EMBHVModule& getModule() const;

  double getPhi() const;

  // HV Status
  bool hvOn(int iGap) const;
    
  double voltage(int iGap) const;
  double current(int iGap) const;

#if !(defined(SIMULATIONBASE) || defined(GENERATIONBASE))
  int hvLineNo(int iGap, const LArHVIdMapping* hvIdMapping=nullptr) const;
#else
  int hvLineNo(int iGap) const;
#endif

  // Voltage and current at the same tine.:
  void voltage_current(int iGap, double& v, double& i) const;

 private:
  EMBHVElectrode(const EMBHVElectrode& right);
  EMBHVElectrode& operator=(const EMBHVElectrode& right);

  class Clockwork;
  Clockwork *m_c;
};

#endif 
