/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// ****************************************************************************************
//
// To perform cell per cell pedestal shift correction
//  taking the pedestal shift in MeV from the database /CALO/Pedestal/CellPedestal folder
//
// G.Unal    9 may 2008 ....  first version
//
// ****************************************************************************************

#include "CaloCellPedestalCorr.h"

#include "CaloEvent/CaloCellContainer.h"
#include "CaloDetDescr/CaloDetDescrElement.h"

//=== AttributeList
#include "CoralBase/Blob.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
//=== CaloCondBlod
#include "CaloCondBlobObjs/CaloCondBlobFlt.h"
#include "CaloCondBlobObjs/CaloCondUtils.h"

#include "CaloIdentifier/CaloCell_ID.h"

// ======================================================
// Constructor

CaloCellPedestalCorr::CaloCellPedestalCorr(
			     const std::string& type, 
			     const std::string& name, 
			     const IInterface* parent)
  : AthAlgTool(type, name, parent) {
  declareInterface<ICaloCellMakerTool>(this);
}

//========================================================
// Initialize

StatusCode CaloCellPedestalCorr::initialize()
{
  ATH_MSG_INFO( " in CaloCellPedestalCorr::initialize() "  );

  ATH_CHECK(detStore()->retrieve(m_cellId, "CaloCell_ID"));
  
  ATH_CHECK(m_pedShiftKey.initialize(!m_isMC));
  ATH_CHECK(m_caloBCIDAvg.initialize(SG::AllowEmpty));
  
  ATH_MSG_INFO( "CaloCellPedestalCorr initialize() end"  );
  return StatusCode::SUCCESS;
}


StatusCode CaloCellPedestalCorr::process( CaloCellContainer * theCellContainer, const EventContext& ctx) const {

  const CaloCellPedShift* pedShifts=nullptr;

  if (!m_isMC) {
    SG::ReadCondHandle<CaloCellPedShift> pedShiftHdl{m_pedShiftKey,ctx};    
    pedShifts=*pedShiftHdl;
  }

  const CaloBCIDAverage* bcidavgshift=nullptr;
  if (!(m_caloBCIDAvg.key().empty())) {
    SG::ReadHandle<CaloBCIDAverage> bcidavgshiftHdl(m_caloBCIDAvg,ctx);
    bcidavgshift=&(*bcidavgshiftHdl);
  }

  

  for (CaloCell* theCell : *theCellContainer) {
    float pedestal=0;
    if (!m_isMC) { 
      const unsigned int cellHash=theCell->caloDDE()->calo_hash();
      const unsigned int dbGain = CaloCondUtils::getDbCaloGain(theCell->gain()); 
      pedestal = pedShifts->pedShift(cellHash,dbGain);
    }

    if (bcidavgshift) {
      pedestal = pedestal + bcidavgshift->average(theCell->ID());
    }

    theCell->addEnergy(-pedestal);
  }//end loop over cell-container

  return StatusCode::SUCCESS;
}
