/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARRAWCONDITIONS_LAROFCBINCOMPLETE
#define LARRAWCONDITIONS_LAROFCBINCOMPLETE

#include "LArRawConditions/LArConditionsContainer.h"
#include "LArRawConditions/LArOFCBinP.h"

class LArOFCBinComplete : public LArConditionsContainer<LArOFCBinP> {
 public:
  LArOFCBinComplete();
  virtual ~LArOFCBinComplete();

  //Getter
  const int& bin(const HWIdentifier& chid,const int& gain) const {
    return (this->get(chid,gain).m_bin); };

  //Setter
  void set(const HWIdentifier& chid, const int& gain, const int& bin);
};

CLASS_DEF(LArOFCBinComplete,113480660,1)

#endif
