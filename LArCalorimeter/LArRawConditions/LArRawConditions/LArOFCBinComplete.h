/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARRAWCONDITIONS_LAROFCBINCOMPLETE
#define LARRAWCONDITIONS_LAROFCBINCOMPLETE

#include "LArRawConditions/LArConditionsContainer.h"
#include "LArElecCalib/ILArOFCBin.h"
#include "LArRawConditions/LArOFCBinP.h"

class LArOFCBinComplete : public ILArOFCBin, 
                         public LArConditionsContainer<LArOFCBinP> {
 public:
  LArOFCBinComplete();
  virtual ~LArOFCBinComplete();

  //Getter
  virtual const int& bin(const HWIdentifier& chid,const int& gain) const {
    return (this->get(chid,gain).m_bin); };

  //Setter
  void set(const HWIdentifier& chid, const int& gain, const int& bin);
};

#include "AthenaKernel/CondCont.h"
CLASS_DEF(LArOFCBinComplete,113480660,1)
CONDCONT_DEF(LArOFCBinComplete,18486430,ILArOFCBin);

#endif
