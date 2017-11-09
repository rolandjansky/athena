/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGCALOREC_TRIGCALOQUALITY_H
#define TRIGCALOREC_TRIGCALOQUALITY_H

#include "AthenaKernel/CLASS_DEF.h"
#include <inttypes.h>

class TrigCaloQuality {
public:
  TrigCaloQuality();
  TrigCaloQuality(TrigCaloQuality&);
  virtual ~TrigCaloQuality();
  virtual void setError(uint32_t error);
  virtual uint32_t getError();
  virtual void setPrescale(unsigned int);
  virtual unsigned int getPrescale();
  virtual void setPersistencyFlag(bool pers);
  virtual bool getPersistencyFlag();
private:
  uint32_t m_error;
  unsigned int m_prescale;
  bool m_persistencyFlag;
};

CLASS_DEF(TrigCaloQuality, 81651823, 1)

#endif
