/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_IJETGROUPER_H
#define TRIGHLTJETHYPO_IJETGROUPER_H

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/HypoJetDefs.h"
#include <string>

class IJetGrouper{
  public:
  virtual ~IJetGrouper(){}
  virtual HypoJetGroupVector group(HypoJetIter&,
                                   HypoJetIter&) const = 0;
  virtual std::string toString() const = 0; 
  virtual std::string getName() const = 0; 
};
#endif
