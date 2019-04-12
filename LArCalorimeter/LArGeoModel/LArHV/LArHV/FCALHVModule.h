/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARHV_FCALHVMODULE_H
#define LARHV_FCALHVMODULE_H

class FCALHVManager;
class FCALHVLine;

/**
 * @class FCALHVModule
 * 
 * @brief Describes one HV Module within the FCAL
 *
 * It owns the pointers to HV Lines
 */

class FCALHVModule
{
 public:
  FCALHVModule(const FCALHVManager *manager
	       , unsigned int iSide
	       , unsigned int iSector
	       , unsigned int iSampling);
  ~FCALHVModule();

  // Side Index (0=Negative, 1=Positive)
  unsigned int getSideIndex() const;

  unsigned int getSamplingIndex() const;
  unsigned int getSectorIndex() const;

  unsigned int getNumHVLines() const;

  const FCALHVLine& getHVLine(unsigned int iLine) const;
  
  const FCALHVManager& getManager() const;

 private:
  FCALHVModule(const FCALHVModule& right);
  FCALHVModule& operator=(const FCALHVModule& right);

  class Clockwork;
  Clockwork *m_c;
};

#endif
