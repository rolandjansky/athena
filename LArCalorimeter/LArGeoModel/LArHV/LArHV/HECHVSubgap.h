/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARHV_HECHVSUBGAP_H
#define LARHV_HECHVSUBGAP_H

class HECHVModule;

#ifndef SIMULATIONBASE
class LArHVIdMapping;
#endif

class HECHVSubgap
{
 public:
  HECHVSubgap(const HECHVModule* module, unsigned int iSubgap);
  ~HECHVSubgap();

  // returns a pointer to the module that owns this electrode.
  const HECHVModule& getModule() const;

  // Returns the index of this electrode.
  unsigned int getSubgapIndex() const;

  bool hvOn() const;
  double voltage() const;
  double current() const;

#ifndef SIMULATIONBASE
  int hvLineNo(const LArHVIdMapping* hvIdMapping=nullptr) const;
#else
  int hvLineNo() const;
#endif

  // Voltage and current at the same time:
  void voltage_current(double& v, double& i) const;

 private:
  HECHVSubgap(const HECHVSubgap& right);
  HECHVSubgap& operator=(const HECHVSubgap& right);

  class Clockwork;
  Clockwork *m_c;
};

#endif
