/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
  : AthAlgTool(type, name, parent),
    m_caloCoolIdTool("CaloCoolIdTool",this),
    m_cellId(nullptr),
    //m_caloLumiBCIDTool(""),
    m_isMC(false){
  declareInterface<ICaloCellMakerTool>(this);
  declareProperty("Luminosity",m_lumi0=0,"Luminosity in 10**33 units");
  declareProperty("CaloCoolIdTool",m_caloCoolIdTool,"Tool for Calo cool Id");
  declareProperty("isMC",m_isMC,"Data/MC flag");
}

//========================================================
// Initialize

StatusCode CaloCellPedestalCorr::initialize()
{
  ATH_MSG_INFO( " in CaloCellPedestalCorr::initialize() "  );

  ATH_CHECK(detStore()->retrieve(m_cellId, "CaloCell_ID"));

  if (m_lumi0<0 && !m_isMC) {
    ATH_CHECK(m_lumiFolderName.initialize());
  }
  if (!m_isMC) {
    ATH_CHECK(m_pedShiftFolder.initialize());
    ATH_CHECK(m_caloCoolIdTool.retrieve());
  }

  if (!m_caloBCIDAvg.key().empty()) {
    ATH_CHECK(m_caloBCIDAvg.initialize());
  }
  
  ATH_MSG_INFO( "CaloCellPedestalCorr initialize() end"  );
  return StatusCode::SUCCESS;
}


StatusCode CaloCellPedestalCorr::process( CaloCellContainer * theCellContainer, const EventContext& ctx) const {

  const CondAttrListCollection* pedShiftColl=nullptr;
  float lumi=m_lumi0;
  if (!m_isMC) {
    // Get Luminosity estimate
    if (lumi<0) {
      SG::ReadCondHandle<CondAttrListCollection> lumiHdl(m_lumiFolderName,ctx);
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
    }

    //Get Pedestal shifts
    SG::ReadCondHandle<CondAttrListCollection> pedShiftHdl(m_pedShiftFolder,ctx);
    pedShiftColl=(*pedShiftHdl);   
  }

  const CaloBCIDAverage* bcidavgshift=nullptr;
  if (!(m_caloBCIDAvg.key().empty())) {
    SG::ReadHandle<CaloBCIDAverage> bcidavgshiftHdl(m_caloBCIDAvg,ctx);
    bcidavgshift=&(*bcidavgshiftHdl);
  }

  std::pair<unsigned,const CaloCondBlobFlt*> blobCache{999999,nullptr};

  for (CaloCell* theCell : *theCellContainer) {
    float pedestal=0;
    if (!m_isMC) { 
      const unsigned int cellHash=theCell->caloDDE()->calo_hash();
      unsigned int subHash;
      const unsigned int iCool = m_caloCoolIdTool->getCoolChannelId(cellHash,subHash);
      if (iCool!=blobCache.first) {
	const coral::AttributeList& attrList=pedShiftColl->attributeList(iCool);
	const coral::Blob& blob = attrList["CaloCondBlob16M"].data<coral::Blob>();
	//std::unique_ptr<const CaloCondBlobFlt> flt(CaloCondBlobFlt::getInstance(blob));
	blobCache.first=iCool;
	delete blobCache.second;
	blobCache.second=CaloCondBlobFlt::getInstance(blob);
      }

      const unsigned int dbGain = CaloCondUtils::getDbCaloGain(theCell->gain());
      pedestal = blobCache.second->getCalib(subHash, dbGain, lumi);
    }

    if (bcidavgshift) {
      pedestal = pedestal + bcidavgshift->average(theCell->ID());
    }

    theCell->addEnergy(-pedestal);
  }//end loop over cell-container

  return StatusCode::SUCCESS;
}
