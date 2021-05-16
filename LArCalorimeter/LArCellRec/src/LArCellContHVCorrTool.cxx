/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCellContHVCorrTool.h" 
#include "CaloEvent/CaloCell.h"
#include "CaloEvent/CaloCellContainer.h"




StatusCode LArCellContHVCorrTool::initialize() {

  ATH_CHECK(m_offlineHVScaleCorrKey.initialize());

  return StatusCode::SUCCESS;
}

StatusCode LArCellContHVCorrTool::process(CaloCellContainer* cellCollection, const EventContext& ctx) const {

   // get offline HVScaleCorr
   SG::ReadCondHandle<LArHVCorr> oflHVCorrHdl(m_offlineHVScaleCorrKey, ctx);
   const LArHVCorr *oflHVCorr = *oflHVCorrHdl;
   if(!oflHVCorr) {
       ATH_MSG_ERROR("Do not have ofline HV corr. conditions object !!!!");
       return StatusCode::FAILURE;
   }

   if (!cellCollection) {
     ATH_MSG_ERROR( "Cell Correction tool receives invalid cell Collection"  );
     return StatusCode::FAILURE;
   }
    
   
   for (CaloCell* theCell : *cellCollection) {
     if (theCell->caloDDE()->is_tile()) {
       continue; 
       //Could possibly also break the loop, assuming all Tile cells come after LAr in an 
       //odered CaloCellContainer
     }
     float hvcorr = oflHVCorr->HVScaleCorr(theCell->ID());

     //Report large correction values
     if (hvcorr<0.9 ) {
       if (hvcorr<0.4) {
         ATH_MSG_WARNING( "HV corr for cell with id " << theCell->ID().get_identifier32().get_compact() 
			  << " = " << hvcorr  );
       } else 
	 ATH_MSG_DEBUG( "HV corr for cell with id " << theCell->ID().get_identifier32().get_compact() 
			<< " = " << hvcorr  );
     }
   
     //hvcorr might be zero in case of problems with the DCS database
     //we are not interested in corrections less the 1%
     if (hvcorr<0.01) hvcorr=1.0;

     theCell->setEnergy(theCell->energy()*hvcorr);
   }// End loop over cell-container
   return StatusCode::SUCCESS;
}
