/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloCellPedCorrCondAlg.h"
#include "CaloCondBlobObjs/CaloCondBlobFlt.h"
#include "CaloCondBlobObjs/CaloCondUtils.h"

StatusCode CaloCellPedCorrCondAlg::initialize() {

  ATH_CHECK(detStore()->retrieve(m_cellId, "CaloCell_ID"));
  
  ATH_CHECK(m_pedShiftFolder.initialize());
  ATH_CHECK(m_lumiFolderName.initialize(m_lumi0<0));
  ATH_CHECK(m_pedShiftKey.initialize());

  return StatusCode::SUCCESS;
}



StatusCode CaloCellPedCorrCondAlg::execute(const EventContext& ctx) const  {

  SG::WriteCondHandle<CaloCellPedShift> writeHandle{m_pedShiftKey,ctx};
  
  if (writeHandle.isValid()) {
    ATH_MSG_DEBUG("Found valid write handle");
    return StatusCode::SUCCESS;
  }  


  //Output object
  std::unique_ptr<CaloCellPedShift> pedShift=std::make_unique<CaloCellPedShift>(m_cellId);

  float lumi=m_lumi0;
  // Get Luminosity estimate
  if (lumi<0) {
    SG::ReadCondHandle<CondAttrListCollection> lumiHdl(m_lumiFolderName,ctx);
    writeHandle.addDependency(lumiHdl);
    const CondAttrListCollection* attrListColl=(*lumiHdl);
    if (attrListColl->size() == 0) {
      lumi = 0;
    }
    else {
      const coral::AttributeList& attrList=attrListColl->attributeList(0); //Get channel number 0
      if (attrList["LBAvInstLumi"].isNull()) {
	ATH_MSG_WARNING("No valid luminosity information in folder " << m_lumiFolderName.key() << ", attribute LBAvInstLumi");
	lumi=0;
      }
      else {
	lumi=attrList["LBAvInstLumi"].data<float>() *1e-3;  // luminosity (from 10**30 units in db to 10*33 units)
      }
    }
  }//end if lumi<0

   
  SG::ReadCondHandle<CondAttrListCollection> pedShiftHdl(m_pedShiftFolder,ctx);
  writeHandle.addDependency(pedShiftHdl);
  const CondAttrListCollection* pedShiftColl{*pedShiftHdl};
  
  const size_t maxCells=m_cellId->calo_cell_hash_max();
    
  std::pair<unsigned,std::unique_ptr<const CaloCondBlobFlt> >blobCache{999999,nullptr};
  //Loop over the CaloCellHash range
  for (size_t iCell=0;iCell<maxCells;++iCell) {
    unsigned int subHash;
    const unsigned int iCool = m_caloCoolIdTool->getCoolChannelId(iCell,subHash);
    if (iCool!=blobCache.first) {
      const coral::AttributeList& attrList=pedShiftColl->attributeList(iCool);
      const coral::Blob& blob = attrList["CaloCondBlob16M"].data<coral::Blob>();
      blobCache.first=iCool;
      blobCache.second.reset(CaloCondBlobFlt::getInstance(blob));
    }
    const size_t maxGain= m_cellId->is_tile(iCell) ? 4 : 3;
    for (size_t iGain=0;iGain<maxGain;++iGain) {
      const float value = blobCache.second->getCalib(subHash, iGain, lumi);
      pedShift->setPedShift(iCell,iGain,value);
    }//end loop over gains
  } //end loop over cells

  if(writeHandle.record(std::move(pedShift)).isFailure()) {
    ATH_MSG_ERROR("Could not record CaloCellPedShift object with " 
		  << writeHandle.key() 
		  << " with EventRange " << writeHandle.getRange()
		  << " into Conditions Store");
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO("recorded new " << writeHandle.key() << " with range " << writeHandle.getRange() << " into Conditions Store");

  return StatusCode::SUCCESS;
}
