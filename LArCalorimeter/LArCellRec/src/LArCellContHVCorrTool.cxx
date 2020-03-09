/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCellContHVCorrTool.h" 
#include "CaloEvent/CaloCell.h"
#include "CaloEvent/CaloCellContainer.h"


LArCellContHVCorrTool::LArCellContHVCorrTool (const std::string& type, 
				  const std::string& name, 
				  const IInterface* parent) :
  base_class (type, name, parent) {}

LArCellContHVCorrTool::~LArCellContHVCorrTool() {}


StatusCode LArCellContHVCorrTool::initialize() {

  ATH_CHECK(m_offlineHVScaleCorrKey.initialize());
  ATH_CHECK( m_cablingKey.initialize() );

  return StatusCode::SUCCESS;
}

StatusCode LArCellContHVCorrTool::process(CaloCellContainer* cellCollection, const EventContext& ctx) const {

   // get offline HVScaleCorr
   SG::ReadCondHandle<ILArHVScaleCorr> oflHVCorrHdl(m_offlineHVScaleCorrKey, ctx);
   const ILArHVScaleCorr *oflHVCorr = *oflHVCorrHdl;
   if(!oflHVCorr) {
       ATH_MSG_ERROR("Do not have ofline HV corr. conditions object !!!!");
       return StatusCode::FAILURE;
   }

   SG::ReadCondHandle<LArOnOffIdMapping> cablingHdl{m_cablingKey, ctx};
   const LArOnOffIdMapping* cabling{*cablingHdl};
   if(!cabling){
     ATH_MSG_ERROR("Do not have mapping object " << m_cablingKey.key() );
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
     float hvcorr = oflHVCorr->HVScaleCorr(cabling->createSignalChannelID((theCell->ID())));

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
