/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCellHVCorrAlg.h" 
#include "CaloEvent/CaloCell.h"
#include "CaloEvent/CaloCellContainer.h"


LArCellHVCorrAlg::LArCellHVCorrAlg (const std::string& type, 
				  const std::string& name, 
				  const IInterface* parent) :
  CaloCellCorrection(type, name, parent)
{ 
  declareInterface<CaloCellCorrection>(this); 
}


LArCellHVCorrAlg::~LArCellHVCorrAlg() {}


StatusCode LArCellHVCorrAlg::initialize() {

  ATH_CHECK(m_offlineHVScaleCorrKey.initialize());
  ATH_CHECK( m_cablingKey.initialize() );

  return StatusCode::SUCCESS;
}

StatusCode LArCellHVCorrAlg::execute(CaloCellContainer* cellCollection, const EventContext& ctx) const {

   // get offline HVScaleCorr
   SG::ReadCondHandle<ILArHVScaleCorr> oflHVCorrHdl(m_offlineHVScaleCorrKey);
   const ILArHVScaleCorr *oflHVCorr = *oflHVCorrHdl;
   if(!oflHVCorr) {
       ATH_MSG_ERROR("Do not have ofline HV corr. conditions object !!!!");
       return StatusCode::FAILURE;
   }

    ATH_MSG_DEBUG("Executing CaloCellCorrection");

    if (!cellCollection) {
         ATH_MSG_ERROR( "Cell Correction tool receives invalid cell Collection"  );
         return StatusCode::FAILURE;
    }
    
    // Loop over all the CaloCell Objects and call Make Correction.
    
    for (CaloCell* cell : *cellCollection) {
         MakeCorrection ( cell, ctx, oflHVCorr );
       }
    
    // Done, Return success
    
    return StatusCode::SUCCESS;

}


void LArCellHVCorrAlg::MakeCorrection (CaloCell* theCell,
                                    const EventContext& /*ctx*/) const // this method will be removed soon, but kept to make CI happy
{
   // get offline HVScaleCorr
   SG::ReadCondHandle<ILArHVScaleCorr> oflHVCorrHdl(m_offlineHVScaleCorrKey);
   const ILArHVScaleCorr *oflHVCorr = *oflHVCorrHdl;
   if(!oflHVCorr) {
       ATH_MSG_ERROR("Do not have ofline HV corr. conditions object !!!!");
       return;
   }
   SG::ReadCondHandle<LArOnOffIdMapping> cablingHdl{m_cablingKey};
   const LArOnOffIdMapping* cabling{*cablingHdl};
   if(!cabling){
         ATH_MSG_ERROR("Do not have mapping object " << m_cablingKey.key() );
         return;
   }
   float hvcorr = oflHVCorr->HVScaleCorr(cabling->createSignalChannelID((theCell->ID())));

   if (hvcorr<0.9 ) {
     if (hvcorr<0.4) {
         ATH_MSG_WARNING( "HV corr for cell with id " << theCell->ID().get_identifier32().get_compact() 
                         << " = " << hvcorr  );
     } else ATH_MSG_DEBUG( "HV corr for cell with id " << theCell->ID().get_identifier32().get_compact() 
                         << " = " << hvcorr  );
   }


 //hvcorr might be zero in case of problems with the DCS database
 //we are not interested in corrections less the 1%
 if (hvcorr<0.01) hvcorr=1.0;
 
 theCell->setEnergy(theCell->energy()*hvcorr);
}

void LArCellHVCorrAlg::MakeCorrection (CaloCell* theCell,
                                    const EventContext& /*ctx*/, const ILArHVScaleCorr* oflHVCorr, const LArOnOffIdMapping* cabling ) const
{
 float hvcorr = oflHVCorr->HVScaleCorr(cabling->createSignalChannelID(theCell->ID()));

 if (hvcorr<0.9 ) {
   if (hvcorr<0.4) {
         ATH_MSG_WARNING( "HV corr for cell with id " << theCell->ID().get_identifier32().get_compact() 
                         << " = " << hvcorr  );
   } else ATH_MSG_DEBUG( "HV corr for cell with id " << theCell->ID().get_identifier32().get_compact() 
                         << " = " << hvcorr  );
 }


 //hvcorr might be zero in case of problems with the DCS database
 if (hvcorr<0.01) hvcorr=1.0;
 
 theCell->setEnergy(theCell->energy()*hvcorr);
}
