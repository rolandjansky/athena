/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/


#ifndef LARHVCORR_H
#define LARHVCORR_H

#include "Identifier/IdentifierHash.h"
#include "LArElecCalib/ILArHVScaleCorr.h"
#include "LArCabling/LArOnOffIdMapping.h"
#include "CaloIdentifier/CaloCell_ID.h"

#include <vector>

class CaloCell_ID;
class MsgStream;

class LArHVCorr : public ILArHVScaleCorr {
 
  friend class LArHVCondAlg; //The conditions alg filling this object 

  public: 
   LArHVCorr(const std::vector<float>& vVec, const LArOnOffIdMapping* cabling, const CaloCell_ID*       caloidhelper);
   ~LArHVCorr () { if(m_log) delete m_log; } 


   // retrieving HVScaleCorr using online ID  
   virtual const float& HVScaleCorr(const HWIdentifier& chid) const override final;

   // retrieving HVScaleCorr using offline ID  
   virtual const float& HVScaleCorr(const Identifier& id) const override final;


  private:
  const LArOnOffIdMapping* m_larCablingSvc;
  const CaloCell_ID*       m_calo_id;
  MsgStream*               m_log;

  std::vector<float>       m_hvCorr;
  const float              m_badCorr;
};

#include "AthenaKernel/CLASS_DEF.h"
CLASS_DEF( LArHVCorr, 52206080, 1)
#include "AthenaKernel/CondCont.h"
CLASS_DEF( CondCont<LArHVCorr>, 24667986, 1) 

#endif
