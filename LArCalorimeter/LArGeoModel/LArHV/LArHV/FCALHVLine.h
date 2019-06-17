/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARHV_FCALHVLINE_H
#define LARHV_FCALHVLINE_H

class FCALHVModule;

#ifndef SIMULATIONBASE
class LArHVIdMapping;
#endif

class FCALHVLine
{
 public:
  FCALHVLine(const FCALHVModule* module, unsigned int iLine);
  ~FCALHVLine();

  // returns a pointer to the module that owns this electrode.
  const FCALHVModule& getModule() const;

  unsigned int getLineIndex() const;

  bool hvOn() const;
  double voltage() const;
  double current() const;

  // Voltage and current at the same time:
  void voltage_current(double& v, double& i) const;

#ifndef SIMULATIONBASE
  int hvLineNo(const LArHVIdMapping* hvIdMapping=nullptr) const;
#else
  int hvLineNo() const;
#endif

 private: 
  FCALHVLine(const FCALHVLine& right);
  FCALHVLine& operator=(const FCALHVLine& right);

  class Clockwork;
  Clockwork *m_c;
};

#endif
