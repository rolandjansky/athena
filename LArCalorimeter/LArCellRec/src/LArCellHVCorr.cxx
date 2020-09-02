/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCellHVCorr.h" 
#include "CaloEvent/CaloCell.h"


LArCellHVCorr::LArCellHVCorr (const std::string& type, 
				  const std::string& name, 
				  const IInterface* parent) :
  CaloCellCorrection(type, name, parent)
{ 
  declareInterface<CaloCellCorrection>(this); 
  declareInterface<ILArCellHVCorrTool>(this);
}


LArCellHVCorr::~LArCellHVCorr() {}


StatusCode LArCellHVCorr::initialize() {

  ATH_CHECK( m_cablingKey.initialize() );
  ATH_CHECK( m_scaleCorrKey.initialize() );

  return StatusCode::SUCCESS;
}


float LArCellHVCorr::getCorrection(const Identifier id)
{
  //Here we ADD const-ness so const_cast is fine
  return const_cast<const LArCellHVCorr*>(this)->getCorrection (id);
}

float LArCellHVCorr::getCorrection(const Identifier id) const
{
  // this is highly ineffective, but this tool will be soon decommissioned, so could live with this for some time...
  SG::ReadCondHandle<LArOnOffIdMapping> cablingHdl{m_cablingKey};
  const LArOnOffIdMapping* cabling{*cablingHdl};
  if(!cabling){
      ATH_MSG_WARNING("Do not have mapping object " << m_cablingKey.key() << " returning 1.");
      return 1.;
  }
 SG::ReadCondHandle<ILArHVScaleCorr> scaleCorr (m_scaleCorrKey);
 float hvcorr = scaleCorr->HVScaleCorr(cabling->createSignalChannelID(id));

 if (hvcorr<0.9 ) {
   if (hvcorr<0.4) ATH_MSG_WARNING( "HV corr for cell with id " << id.get_identifier32().get_compact() << " = " << hvcorr  );
   else ATH_MSG_DEBUG( "HV corr for cell with id " << id.get_identifier32().get_compact() << " = " << hvcorr  );
 }


 //hvcorr might be zero in case of problems with the DCS database
 if (hvcorr<0.01) hvcorr=1.0;
 
 return hvcorr;
}


void LArCellHVCorr::MakeCorrection (CaloCell* theCell,
                                    const EventContext& /*ctx*/) const
{
  const Identifier id=theCell->ID();
  const float hvcorr=getCorrection(id);
  theCell->setEnergy(theCell->energy()*hvcorr);
}  

