/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloDetDescr/CaloAlignTool.h"

#include "GeoModelInterfaces/IGeoModelSvc.h"
#include "GeoModelInterfaces/IGeoModelTool.h"
#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/MsgStream.h"

#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloDetDescr/CaloDetectorElements.h"
#include "CaloDetDescr/CaloDescriptors.h"
 
#include "LArReadoutGeometry/EMBCell.h"
#include "LArReadoutGeometry/EMBDetectorRegion.h"
#include "LArReadoutGeometry/EMBDetectorManager.h"
 
#include "LArReadoutGeometry/EMECCell.h"
#include "LArReadoutGeometry/EMECDetectorRegion.h"
#include "LArReadoutGeometry/EMECDetectorManager.h"
 
#include "LArReadoutGeometry/HECCell.h"
#include "LArReadoutGeometry/HECDetectorRegion.h"
#include "LArReadoutGeometry/HECDetectorManager.h"
 
#include "LArReadoutGeometry/FCALTile.h"
#include "LArReadoutGeometry/FCALModule.h"
#include "LArReadoutGeometry/FCALDetectorManager.h"
 
#include "CaloIdentifier/CaloCell_ID.h"

#include "CaloConditions/CaloCellPositionShift.h"

CaloAlignTool::CaloAlignTool(const std::string& type, 
			     const std::string& name, 
			     const IInterface* parent) :
  AthAlgTool(type,name,parent),
  m_IOVDbSvc("IOVDbSvc", name)
{
  declareInterface<IGeoAlignTool>(this);
}
 
CaloAlignTool::~CaloAlignTool()  
{
}

StatusCode CaloAlignTool::initialize()
{
  // Get pointers to the GeoModelSvc and DetectorStore
  StatusCode status;
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "In initialize" << endmsg;

  status = m_IOVDbSvc.retrieve();
  if(status.isFailure()) {
    log << MSG::ERROR << "Could not retrieve IOVDbSvc" << endmsg;
    return status;
  }

  const IGeoModelSvc *geoModel=0;
  status = service("GeoModelSvc", geoModel);
  if(status.isFailure()) {
    log << MSG::ERROR << "Could not locate GeoModelSvc" << endmsg;
    return status;
  }

  const IGeoModelTool* larTool = geoModel->getTool("LArDetectorToolNV");
  if(larTool!=0) {
    // Register callback function on the folder /LAR/Align
    status = detStore()->regFcn(&IGeoModelTool::align,
				larTool,
				&IGeoAlignTool::align,dynamic_cast<IGeoAlignTool*>(this));
    if(status.isFailure())
      log << MSG::INFO << "Could not register callback on the folder /LAR/Align. Calo alignments will have no effect" << endmsg;
    else
      log << MSG::DEBUG << "Callback registered on the folder /LAR/Align" << endmsg;

    // Register callback function on the folder /LAR/LArCellPositionShift
    const DataHandle<CaloRec::CaloCellPositionShift> posShift;
    status = detStore()->regFcn(&IGeoAlignTool::align,dynamic_cast<IGeoAlignTool*>(this),
				posShift,"LArCellPositionShift");
    if(status.isFailure())
      log << MSG::INFO << "Could not register callback on the folder /LAR/LArCellPositionShift. Sagging corrections will have no effect" << endmsg;
    else 
      log << MSG::DEBUG << "Callback registered on the folder /LAR/LArCellPositionShift" << endmsg;

  } else {
    log << MSG::DEBUG << "Could not locate LAr Detector Tool. Skipping callback registration" << endmsg;
  }

  return StatusCode::SUCCESS;
}

StatusCode CaloAlignTool::finalize()
{
  return StatusCode::SUCCESS;
}
 
StatusCode CaloAlignTool::align(IOVSVC_CALLBACK_ARGS)
{
  StatusCode status;
  MsgStream log(msgSvc(), name());

  // Get CaloDetDescrManager
  const CaloDetDescrManager* caloMgr_const;

  status = detStore()->retrieve(caloMgr_const);
  if (status.isFailure()) {
    log << MSG::WARNING << "Unable to retrieve CaloDetDescrManager from DetectorStore. No alignments&sagging for Calo." << endmsg;
    return StatusCode::SUCCESS;
  } else 
    log << MSG::DEBUG << "Successfully retrieved CaloDetDescrManager from the DetectorStore" << endmsg;

  // FIXME: This tool changes the content of the (const) CaloDetDescrManager
  // recorded in the detector store.  Need to get rid of this for MT.
  // This should go away with the new scheme for dealing with alignments.
  CaloDetDescrManager* caloMgr = const_cast<CaloDetDescrManager*>(caloMgr_const);

  const CaloRec::CaloCellPositionShift* posShift = 0;
  status = detStore()->retrieve(posShift,"LArCellPositionShift");
  if(status.isFailure() || posShift==0)
    log << MSG::WARNING << "Unable to retrieve CaloCellPositionShift. No sagging corrections for Calo" << endmsg;
  else 
    log << MSG::DEBUG << "Successfully retrieved CaloCellPositionShift from the DetectorStore" << endmsg;

  const CaloCell_ID* cell_id = caloMgr->getCaloCell_ID();
  const LArEM_ID* em_id = cell_id->em_idHelper();
  const LArHEC_ID* hec_id = cell_id->hec_idHelper();
  const LArFCAL_ID* fcal_id = cell_id->fcal_idHelper();

  // Get minimal value for Hash ID - needed to retrieve sagging information
  IdentifierHash emmin,emmax;
  cell_id->calo_cell_hash_range(CaloCell_ID::LAREM,emmin,emmax);

  // Two objects needed to pass sagging info to updateAlignment() methods
  CaloElementPositionShift elementPosShift;
  CaloRec::CaloCellPositionShift::size_type posShiftInd;

  // Go through subsystems and apply alignments

  // ****************************************************************
  // **                   --- --- EMB --- ---                      **
  // ****************************************************************

  // --- Retrieve Emec Detector Manager
  const EMBDetectorManager* embManager = 0;
  status = detStore()->retrieve(embManager);
  if(status.isFailure())
  {
    log << MSG::WARNING << "Could not get the EMBDetectorManager. No lignment infromation for EMB" << endmsg;
  }
  else
  {     
    //         --- --- Iterate over EMB regions and cells --- --- 
    EMBDetectorManager::DetectorRegionConstIterator embregIt;

    for (embregIt=embManager->beginDetectorRegion(); embregIt!=embManager->endDetectorRegion(); embregIt++) 
    {
      const EMBDetectorRegion *embRegion = *embregIt;
      
      // *** *** Retrieve region descriptor *** ***
      // Region identifier. 
      // Do some mapping between LArReadoutGeometry and CaloID
      
      int barrel_ec = 0;
      switch(embRegion->getEndcapIndex())
      {
      case EMBDetectorRegion::NEG:
	{
	  barrel_ec = -1; // negative side
	  break;
	}
      case EMBDetectorRegion::POS:
	{
	  barrel_ec = 1;  // positive side
	  break;
	}
      default:
	{
	  log << MSG::WARNING << "Wrong Side Index for EMB region " << embRegion->getEndcapIndex() 
	      << ". No alignments for this region" << endmsg;
	  continue;
	}
      }

      Identifier regId = em_id->region_id(barrel_ec,
					  embRegion->getSamplingIndex(),
					  embRegion->getRegionIndex());

      EMBDescriptor* embDescr = dynamic_cast<EMBDescriptor*>(caloMgr->get_descriptor_nonconst(regId));

      if(embDescr==0)
      {
	log << MSG::WARNING << "0 pointer to EMBDescriptor " << regId.getString() 
	    << ". No alignments for this region" << endmsg;
	continue;
      }

      double phi_min = 0.;
      double reg_min = 10000.;
      double reg_max = -10000.;
      
      // *** *** Retrieve region descriptor *** ***

      //
      //            *** *** *** Iterate over cells *** *** ***
      //
      for (unsigned int iPhi=embRegion->beginPhiIndex();iPhi<embRegion->endPhiIndex();iPhi++) 
      {
	for (unsigned int iEta=embRegion->beginEtaIndex();iEta<embRegion->endEtaIndex();iEta++) 
	{
	  EMBCellConstLink cellPtr = embRegion->getEMBCell(iEta,iPhi);
	  
	  // build hash identifier for this cell
	  Identifier chanId = em_id->channel_id(barrel_ec,
						cellPtr->getSamplingIndex(),
						cellPtr->getRegionIndex(),
						iEta,iPhi);
	  
	  
	  // Retrieve DD element
	  EMBDetectorElement* embElement = dynamic_cast<EMBDetectorElement*>(caloMgr->get_element_nonconst(chanId));

	  if(embElement==0)
	  {
	    log << MSG::DEBUG << "0 pointer to EMBDetectorElement " << chanId.getString()
		<< ". No alignments for this element" << endmsg;
	    continue;
	  }

	  // get hash id for the element and apply alignment & sagging
	  if(posShift) {
	    posShiftInd = cell_id->calo_cell_hash(chanId) - emmin;
	    elementPosShift.dx = posShift->deltaX(posShiftInd);
	    elementPosShift.dy = posShift->deltaY(posShiftInd);
	    elementPosShift.dz = posShift->deltaZ(posShiftInd);

	    embElement->updateAlignment(cellPtr,embRegion,&elementPosShift);
	  } else {
	    embElement->updateAlignment(cellPtr,embRegion);
	  }
	   

	  if(iPhi==0 && (iEta==0||iEta==1))
	    phi_min = embElement->phi() - 0.5*embElement->dphi();

	  if(reg_min > embElement->eta()-0.5*embElement->deta())
	    reg_min = embElement->eta()-0.5*embElement->deta();
	  if(reg_max < embElement->eta()+0.5*embElement->deta())
	    reg_max = embElement->eta()+0.5*embElement->deta();
	} // Eta loop
      } // Phi loop
      //            *** *** *** Iterate over cells *** *** ***

      embDescr->setLArRegMin(reg_min);
      embDescr->setLArRegMax(reg_max);
      embDescr->setLArPhiMin(phi_min);

      if(embRegion->getEndcapIndex()==EMBDetectorRegion::NEG)
	embDescr->setLArEtaMin(-reg_max);
      else
	embDescr->setLArEtaMin(reg_min);
      
    }// Region loop
  } // if EMB manager has been retrieved

  // ****************************************************************
  // **                   --- --- EMB --- ---                      **
  // ****************************************************************

  // ****************************************************************
  // **                   --- --- EMEC --- ---                     **
  // ****************************************************************

  // --- Retrieve Emec Detector Manager
  const EMECDetectorManager* emecManager = 0;
  status = detStore()->retrieve(emecManager);
  if(status.isFailure())
  {
    log << MSG::WARNING << "Could not get the EMECDetectorManager. No alignment infromation for EMEC" << endmsg;
  }
  else
  {
    //         --- --- Iterate over EMEC regions and cells --- --- 
    EMECDetectorManager::DetectorRegionConstIterator emecregIt;

    for (emecregIt=emecManager->beginDetectorRegion(); emecregIt!=emecManager->endDetectorRegion(); emecregIt++) 
    {
      const EMECDetectorRegion *emecRegion = *emecregIt;
      
      // *** ***  Retrieve region descriptor *** ***
      // Region identifier. 
      // Do some mapping between LArReadoutGeometry and CaloID
      EMECDetectorRegion::DetectorSide endcapInd = emecRegion->getEndcapIndex();
      
      unsigned int radialInd = emecRegion->getRadialIndex();
      int barrel_ec;
      
      switch(endcapInd)
      {
      case EMECDetectorRegion::NEG:
	{
	  barrel_ec = -1; // negative side
	  break;
	}
      case EMECDetectorRegion::POS:
	{
	  barrel_ec = 1;  // positive side
	  break;
	}
      default:
	{
	  log << MSG::ERROR << "Wrong Endcap Index for EMEC region " << endcapInd 
	      << ". No alignments for this region" << endmsg;
	  continue;
	}
      }// switch(endcapInd)
      
      switch(radialInd)
      {
      case 0:
	{
	  barrel_ec *= 2; // outer wheel
	  break;
	}
      case 1:
	{
	  barrel_ec *= 3;  // inner wheel 
	  break;
	}
      default:
	{
	  log << MSG::ERROR << "Wrong Radial Index for  EMEC region " << radialInd
	      << ". No alignments for this region" << endmsg;
	  continue;
	}
      }// switch(radialInd)
      
      Identifier regId = em_id->region_id(barrel_ec,
					  emecRegion->getSamplingIndex(),
					  emecRegion->getRegionIndex());

      EMECDescriptor* emecDescr = dynamic_cast<EMECDescriptor*>(caloMgr->get_descriptor_nonconst(regId));

      if(emecDescr==0)
      {
	log << MSG::WARNING << "0 pointer to EMECDescriptor " << regId.getString() 
	    << ". No alignments for this region" << endmsg;
	continue;
      }
      
      double phi_min = 0.;
      double reg_min = 10000.;
      double reg_max = -10000.;
      
      //
      //            *** *** *** Iterate over cells *** *** ***
      //
      for (unsigned int iPhi=emecRegion->beginPhiIndex();iPhi<emecRegion->endPhiIndex();iPhi++) 
      {
	for (unsigned int iEta=emecRegion->beginEtaIndex();iEta<emecRegion->endEtaIndex();iEta++) 
	{
	  EMECCellConstLink cellPtr = emecRegion->getEMECCell(iEta,iPhi);
	  
	  Identifier chanId = em_id->channel_id(barrel_ec,
						cellPtr->getSamplingIndex(),
						cellPtr->getRegionIndex(),
						iEta,iPhi);
	  
	  // Retrieve DD element
	  EMECDetectorElement* emecElement = dynamic_cast<EMECDetectorElement*>(caloMgr->get_element_nonconst(chanId));

	  if(emecElement==0)
	  {
	    log << MSG::DEBUG << "0 pointer to EMECDetectorElement " << chanId.getString()
		<< ". No alignments for this element" << endmsg;
	    continue;
	  }

	  // get hash id for the element and apply alignment & sagging
	  if(posShift) {
	    posShiftInd = cell_id->calo_cell_hash(chanId) - emmin;
	    elementPosShift.dx = posShift->deltaX(posShiftInd);
	    elementPosShift.dy = posShift->deltaY(posShiftInd);
	    elementPosShift.dz = posShift->deltaZ(posShiftInd);

	    emecElement->updateAlignment(cellPtr,emecRegion,&elementPosShift);
	  } else {
	    emecElement->updateAlignment(cellPtr,emecRegion);
	  }

	  if(iPhi==0)
	    phi_min = emecElement->phi() - 0.5*emecElement->dphi();

	  if(reg_min > emecElement->eta()-0.5*emecElement->deta())
	    reg_min = emecElement->eta()-0.5*emecElement->deta();
	  if(reg_max < emecElement->eta()+0.5*emecElement->deta())
	    reg_max = emecElement->eta()+0.5*emecElement->deta();

	} // Eta loop
      } // Phi loop
      //            *** *** *** Iterate over cells *** *** ***

      emecDescr->setLArRegMin(reg_min);
      emecDescr->setLArRegMax(reg_max);
      emecDescr->setLArPhiMin(phi_min);

      if(emecRegion->getEndcapIndex()==EMECDetectorRegion::NEG)
	emecDescr->setLArEtaMin(-reg_max);
      else
	emecDescr->setLArEtaMin(reg_min);

    }// Region loop
  }// if EMEC manager has been retrieved

  // ****************************************************************
  // **                   --- --- EMEC --- ---                     **
  // ****************************************************************
  

  // ****************************************************************
  // **                    --- --- HEC --- ---                     **
  // ****************************************************************

  // --- Retrieve Hec Detector Manager
  const HECDetectorManager* hecManager = 0;
  status = detStore()->retrieve(hecManager);
  if(status.isFailure())
  {
    log << MSG::WARNING << "Could not get the HECDetectorManager. No alignment infromation for HEC" << endmsg;
  }
  else
  {
    //         --- --- Iterate over HEC regions and cells --- --- 
    HECDetectorManager::DetectorRegionConstIterator hecregIt;
    
    for (hecregIt=hecManager->beginDetectorRegion(); hecregIt!=hecManager->endDetectorRegion(); hecregIt++) 
    {
      const HECDetectorRegion *hecRegion = *hecregIt;
      
      // *** ***  Retrieve region descriptor *** ***
      // Region identifier. 
      // Do some mapping between LArReadoutGeometry and CaloID
      HECDetectorRegion::DetectorSide endcapInd = hecRegion->getEndcapIndex();
      int pos_neg = endcapInd==HECDetectorRegion::NEG ? -2 : 2;
      
      Identifier regId = hec_id->region_id(pos_neg,
					   hecRegion->getSamplingIndex(),
					   hecRegion->getRegionIndex());
      
      HECDescriptor* hecDescr = dynamic_cast<HECDescriptor*>(caloMgr->get_descriptor_nonconst(regId));

      if(hecDescr==0)
      {
	log << MSG::WARNING << "0 pointer to HECDescriptor " << regId.getString() 
	    << ". No alignments for this region" << endmsg;
	continue;
      }
      
      double phi_min = 0.;
      double reg_min = 10000.;
      double reg_max = -10000.;

      //
      //            *** *** *** Iterate over cells *** *** ***
      //
      for (unsigned int iPhi=hecRegion->beginPhiIndex();iPhi<hecRegion->endPhiIndex();iPhi++) 
      {
	for (unsigned int iEta=hecRegion->beginEtaIndex();iEta<hecRegion->endEtaIndex();iEta++) 
	{
	  HECCellConstLink cellPtr = hecRegion->getHECCell(iEta,iPhi);
	  // build hash identifier for this cell
	  // Do some mapping between LArReadoutGeometry and CaloID
	  
	  if(cellPtr)
	  {
	    Identifier chanId = hec_id->channel_id(pos_neg,
						   cellPtr->getSamplingIndex(),
						   cellPtr->getRegionIndex(),
						   iEta,iPhi);

	    // Retrieve DD element
	    HECDetectorElement* hecElement = dynamic_cast<HECDetectorElement*>(caloMgr->get_element_nonconst(chanId));
	    
	    if(hecElement==0)
	    {
	      log << MSG::DEBUG << "0 pointer to HECDetectorElement " << chanId.getString()
		  << ". No alignments for this element" << endmsg;
	      continue;
	    }

	    // get hash id for the element and apply alignment & sagging
	    if(posShift) {
	      posShiftInd = cell_id->calo_cell_hash(chanId) - emmin;
	      elementPosShift.dx = posShift->deltaX(posShiftInd);
	      elementPosShift.dy = posShift->deltaY(posShiftInd);
	      elementPosShift.dz = posShift->deltaZ(posShiftInd);

	      hecElement->updateAlignment(cellPtr,hecRegion,&elementPosShift);
	    } else {
	      hecElement->updateAlignment(cellPtr,hecRegion);
	    }

	    if(iPhi==0)
	      phi_min = hecElement->phi() - 0.5*hecElement->dphi();

	    if(reg_min > hecElement->eta()-0.5*hecElement->deta())
	      reg_min = hecElement->eta()-0.5*hecElement->deta();
	    if(reg_max < hecElement->eta()+0.5*hecElement->deta())
	      reg_max = hecElement->eta()+0.5*hecElement->deta();
	  }
	} // Eta loop
      } // Phi loop
      //            *** *** *** Iterate over cells *** *** ***
      
      hecDescr->setLArRegMin(reg_min);
      hecDescr->setLArRegMax(reg_max);
      hecDescr->setLArPhiMin(phi_min);

      if(hecRegion->getEndcapIndex()==HECDetectorRegion::NEG)
	hecDescr->setLArEtaMin(-reg_max);
      else
	hecDescr->setLArEtaMin(reg_min);
      
    }// Region loop
  } // if HEC manager has been retrieved

  // ****************************************************************
  // **                   --- --- HEC --- ---                      **
  // ****************************************************************


  // ****************************************************************
  // **                    --- --- FCAL --- ---                    **
  // ****************************************************************

  // --- Retrieve Fcal Detector Manager
  const FCALDetectorManager* fcalManager = 0;
  status = detStore()->retrieve(fcalManager);
  if(status.isFailure())
  {
    log << MSG::WARNING << "Could not get the FCALDetectorManager. No alignment infromation for FCAL" << endmsg;
  }
  else
  {
    //         --- --- Iterate over FCAL modules and tiles --- --- 
    FCALDetectorManager::ConstIterator fcalmodIt;
    
    for (fcalmodIt=fcalManager->beginFCAL(); fcalmodIt!=fcalManager->endFCAL(); fcalmodIt++) 
    {
      const FCALModule* fcalmodule = *fcalmodIt;
      
      // *** ***  Retrieve module descriptor *** ***
      // Module identifier. 
      // Do some mapping between LArReadoutGeometry and CaloID
      
      FCALModule::Endcap endcapInd = fcalmodule->getEndcapIndex();
      
      int pos_neg = endcapInd==FCALModule::NEG ? -2 : 2;
      
      Identifier regId = fcal_id->module_id(pos_neg,
					    (int)fcalmodule->getModuleIndex());
      

      FCALDescriptor* fcalDescr = dynamic_cast<FCALDescriptor*>(caloMgr->get_descriptor_nonconst(regId));

      if(fcalDescr==0)
      {
	log << MSG::WARNING << "0 pointer to FCALDescriptor " << regId.getString() 
	    << ". No alignments for this region" << endmsg;
	continue;
      }

      double reg_min = 10000.;
      double reg_max = -10000.;

      // *** ***  Retrieve module descriptor  *** ***
      
      //
      //            *** *** *** Iterate over cells *** *** ***
      //
      FCALModule::ConstIterator fcaltileIt;
      for (fcaltileIt=fcalmodule->beginTiles();fcaltileIt!=fcalmodule->endTiles();fcaltileIt++)
      {
	Identifier chanId = fcal_id->channel_id(pos_neg,
						(int)fcalmodule->getModuleIndex(),
						fcaltileIt->getIndexJ(),   // eta
						fcaltileIt->getIndexI());  // phi
	
	// Retrieve DD element
	FCALDetectorElement* fcalElement = dynamic_cast<FCALDetectorElement*>(caloMgr->get_element_nonconst(chanId));
	
	if(fcalElement==0)
	{
	  log << MSG::DEBUG << "0 pointer to FCALDetectorElement " << chanId.getString()
	      << ". No alignments for this element" << endmsg;
	  continue;
	}

	// get hash id for the element and apply alignment & sagging
	if(posShift) {
	  posShiftInd = cell_id->calo_cell_hash(chanId) - emmin;
	  elementPosShift.dx = posShift->deltaX(posShiftInd);
	  elementPosShift.dy = posShift->deltaY(posShiftInd);
	  elementPosShift.dz = posShift->deltaZ(posShiftInd);
	  
	  fcalElement->updateAlignment(&(*fcaltileIt),fcalmodule,&elementPosShift);
	} else {
	  fcalElement->updateAlignment(&(*fcaltileIt),fcalmodule);
	}

	if(reg_min > fcalElement->eta()-0.5*fcalElement->deta())
	  reg_min = fcalElement->eta()-0.5*fcalElement->deta();
	if(reg_max < fcalElement->eta()+0.5*fcalElement->deta())
	  reg_max = fcalElement->eta()+0.5*fcalElement->deta();
      }
      
      fcalDescr->setLArRegMin(reg_min);
      fcalDescr->setLArRegMax(reg_max);

      if(fcalmodule->getEndcapIndex()==FCALModule::NEG)
	fcalDescr->setLArEtaMin(-reg_max);
      else
	fcalDescr->setLArEtaMin(reg_min);

    }// Module loop
  }// if FCAL manager has been retrieved

  // ****************************************************************
  // **                   --- --- FCAL --- ---                      **
  // ****************************************************************


  // ****************************************************************
  // **                   --- --- Tile --- ---                     **
  // ****************************************************************
  //
  // Tile Alignments are not stored in the Cond DB. Nothing to be
  // done here for the moment.
  //
  // ****************************************************************

  // Clean up StoreGate from conditions objects
  if(m_IOVDbSvc->dropObject("LArCellPositionShift",true)) 
    log << MSG::DEBUG << "LArCellPositionShift released" << endmsg;

  return StatusCode::SUCCESS;
}
