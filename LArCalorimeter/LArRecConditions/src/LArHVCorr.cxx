/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "LArRecConditions/LArHVCorr.h"
#include "AthenaKernel/getMessageSvc.h"

LArHVCorr::LArHVCorr(const std::vector<float>& vVec, const LArOnOffIdMapping* cabling, const CaloCell_ID*  caloidhelper): 
   m_larCablingSvc(cabling),
   m_calo_id(caloidhelper),
   m_log(nullptr),
   m_badCorr(0.) {

  m_log=new MsgStream(Athena::getMessageSvc(), "LArHVCorr")
     ;
  m_hvCorr = vVec;
}

// retrieving HVScaleCorr using offline ID  
const float& LArHVCorr::HVScaleCorr(const Identifier& id) const  {
   if(m_calo_id->sub_calo(id) < CaloCell_Base_ID::TILE) 
      return m_hvCorr[m_calo_id->calo_cell_hash(id)]; 
   else return m_badCorr;
}

const float& LArHVCorr::HVScaleCorr(const HWIdentifier& id) const  {
   return HVScaleCorr(m_larCablingSvc->cnvToIdentifier(id));
}
