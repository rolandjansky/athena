#include "LArHVFraction.h"
/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/


LArHVFraction::LArHVFraction(const std::string& type,
			     const std::string& name,
			     const IInterface* parent) :
  AthAlgTool(type,name,parent) {
  declareInterface<ILArHVFraction>(this); 
}


StatusCode LArHVFraction::initialize() {
  ATH_CHECK(m_cablingKey.initialize());
  ATH_CHECK(m_hvScaleKey.initialize());
  return StatusCode::SUCCESS;
}

std::pair<double,unsigned> LArHVFraction::getLArHVFrac(const CaloClusterCellLink* cclc,const EventContext& ctx) const {
  
  SG::ReadCondHandle<LArOnOffIdMapping> cablingHdl(m_cablingKey,ctx);
  const LArOnOffIdMapping* cabling=*cablingHdl;

  SG::ReadCondHandle<ILArHVScaleCorr> hvScaleHdl(m_hvScaleKey,ctx);
  const ILArHVScaleCorr* hvcorr=*hvScaleHdl;


  double energyHVaff=0;
  unsigned numCellsHVaff=0;

for(const CaloCell* cell : *cclc) {
    if(cell->caloDDE()->is_tile()) continue;
    HWIdentifier hwid=cabling->createSignalChannelID(cell->ID());
    float corr=hvcorr->HVScaleCorr(hwid);
    //Correction should be between (0 and 2)
    if (!(corr>0. && corr<100.)) continue;
    
    if (fabs(corr-1.)>m_threshold) {
      energyHVaff+=fabs(cell->e());
      numCellsHVaff++;
    }//end of non nominal HV area
  }//end loop over cells

 return {energyHVaff,numCellsHVaff};
}
