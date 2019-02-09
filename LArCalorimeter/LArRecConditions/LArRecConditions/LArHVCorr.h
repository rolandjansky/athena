/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


#ifndef LARHVCORR_H
#define LARHVCORR_H

#include "Identifier/IdentifierHash.h"
#include "LArElecCalib/ILArHVScaleCorr.h"
#include "LArCabling/LArOnOffIdMapping.h"
#include "CaloIdentifier/CaloCell_ID.h"

#include <vector>

class CaloCell_ID;

class LArHVCorr : public ILArHVScaleCorr {
 
  public: 
   LArHVCorr(std::vector<float>&& vVec, const LArOnOffIdMapping* cabling, const CaloCell_ID*       caloidhelper);
   ~LArHVCorr () {};


   // retrieving HVScaleCorr using online ID  
   virtual const float& HVScaleCorr(const HWIdentifier& chid) const override final;

   // retrieving HVScaleCorr using offline ID  
   virtual const float& HVScaleCorr(const Identifier& chid) const;


  private:
  const LArOnOffIdMapping* m_larCablingSvc;
  const CaloCell_ID*       m_calo_id;

  std::vector<float>       m_hvCorr;
  const float              m_noCorr;
};

#include "AthenaKernel/CondCont.h"
CLASS_DEF( LArHVCorr, 52206080, 1)
CONDCONT_DEF( LArHVCorr, 24667986 );

#endif
