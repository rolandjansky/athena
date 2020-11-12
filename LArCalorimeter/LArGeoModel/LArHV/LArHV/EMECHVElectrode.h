/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARHV_EMECHVELECTRODE_H
#define LARHV_EMECHVELECTRODE_H

class EMECHVModule;

#if !(defined(SIMULATIONBASE) || defined(GENERATIONBASE))
class LArHVIdMapping;
#endif

class EMECHVElectrode
{
 public:
  // The ownership of the EMBHVModule pointer NOT transferred to EMECHVElectrode
  EMECHVElectrode(const EMECHVModule* module, unsigned int iElectrode);
  ~EMECHVElectrode();

  unsigned int getElectrodeIndex() const;

  // returns a pointer to the module that owns this electrode.
  const EMECHVModule& getModule() const;

  double getPhi() const;

  // HVLine no
#if !(defined(SIMULATIONBASE) || defined(GENERATIONBASE))
  int hvLineNo(int iGap, const LArHVIdMapping* hvIdMapping) const;
#endif

 private:
  EMECHVElectrode(const EMECHVElectrode& right);
  EMECHVElectrode& operator=(const EMECHVElectrode& right);

  class Clockwork;
  Clockwork *m_c;
};

#endif
