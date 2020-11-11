/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARHV_HECHVSUBGAP_H
#define LARHV_HECHVSUBGAP_H

class HECHVModule;

#if !(defined(SIMULATIONBASE) || defined(GENERATIONBASE))
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

#if !(defined(SIMULATIONBASE) || defined(GENERATIONBASE))
  int hvLineNo(const LArHVIdMapping* hvIdMapping) const;
#endif

 private:
  HECHVSubgap(const HECHVSubgap& right);
  HECHVSubgap& operator=(const HECHVSubgap& right);

  class Clockwork;
  Clockwork *m_c;
};

#endif
