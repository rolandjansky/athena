/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARHV_FCALHVLINE_H
#define LARHV_FCALHVLINE_H

class FCALHVModule;

#if !(defined(SIMULATIONBASE) || defined(GENERATIONBASE))
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

#if !(defined(SIMULATIONBASE) || defined(GENERATIONBASE))
  int hvLineNo(const LArHVIdMapping* hvIdMapping) const;
#endif

 private: 
  FCALHVLine(const FCALHVLine& right);
  FCALHVLine& operator=(const FCALHVLine& right);

  class Clockwork;
  Clockwork *m_c;
};

#endif
