/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloDetDescrUtils/CaloDetDescrBuilder.h"
#include "CaloCellVolumes.h"

#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloIdentifier/CaloIdManager.h"

#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloDetDescr/CaloDetectorElements.h"
#include "CaloDetDescr/CaloDescriptors.h"
#include "CaloDetDescr/CaloDetDescrElementContainer.h"

#include "CaloConditions/CaloCellPositionShift.h"

#include "LArReadoutGeometry/LArDetectorManager.h"

#include "LArReadoutGeometry/EMBCell.h"
#include "LArReadoutGeometry/EMBDetectorRegion.h"
#include "LArReadoutGeometry/EMBDetDescr.h"
#include "LArReadoutGeometry/EMBDetectorManager.h"

#include "LArReadoutGeometry/EMECCell.h"
#include "LArReadoutGeometry/EMECDetectorRegion.h"
#include "LArReadoutGeometry/EMECDetDescr.h"
#include "LArReadoutGeometry/EMECDetectorManager.h"

#include "LArReadoutGeometry/HECCell.h"
#include "LArReadoutGeometry/HECDetectorRegion.h"
#include "LArReadoutGeometry/HECDetDescr.h"
#include "LArReadoutGeometry/HECDetectorManager.h"

#include "LArReadoutGeometry/FCALTile.h"
#include "LArReadoutGeometry/FCALModule.h"
#include "LArReadoutGeometry/FCALDetectorManager.h"

#include "TileDetDescr/TileDetDescrManager.h"

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IMessageSvc.h"
#include "StoreGate/StoreGateSvc.h"

std::unique_ptr<CaloDetDescrManager> buildCaloDetDescr(ISvcLocator* svcLocator
						       , IMessageSvc* msgSvc
						       , const GeoAlignmentStore* geoAlignStore
						       , const CaloRec::CaloCellPositionShift* cellPosShift)
{
  MsgStream log(msgSvc, "buildCaloDetDescr"); 

  // --- --- Get CaloCell_ID and CaloIdManager helpers --- ---
  StoreGateSvc* detStore{nullptr};
  if(svcLocator->service("DetectorStore", detStore)!=StatusCode::SUCCESS)
    throw std::runtime_error("buildCaloDetDescr failed to acquire a pointer to DetectorStore");

  const CaloCell_ID* cell_id{nullptr};
  if(detStore->retrieve(cell_id, "CaloCell_ID")!=StatusCode::SUCCESS)
    throw std::runtime_error("buildCaloDetDescr failed to acquire a pointer to CaloCell_ID helper");
  log << MSG::DEBUG << "Found the CaloCell_ID helper. " << endmsg;

  const CaloIdManager* caloId_mgr{nullptr};
  if(detStore->retrieve(caloId_mgr, "CaloIdManager")!=StatusCode::SUCCESS)
    throw std::runtime_error("buildCaloDetDescr failed to acquire a pointer to CaloIdManager helper");
  log << MSG::DEBUG << "Found the CaloIdManager helper. " << endmsg;
  // --- --- Get CaloCell_ID and CaloIdManager helpers --- ---

  // --- --- Create CaloDetDescrManager object --- ---
  std::unique_ptr<CaloDetDescrManager> caloMgr = std::make_unique<CaloDetDescrManager>();

  // set helpers
  caloMgr->set_helper(cell_id);
  caloMgr->set_helper(caloId_mgr);

  caloMgr->initialize();
  // --- --- Create CaloDetDescrManager object --- ---

  // Cell volumes
  CaloCellVolumes cellVol(svcLocator, cell_id);

  // geometry layout
  caloMgr->set_lar_geometry(cellVol.layout());

  const LArEM_ID* em_id = caloId_mgr->getEM_ID();
  const LArHEC_ID* hec_id = caloId_mgr->getHEC_ID();
  const LArFCAL_ID* fcal_id = caloId_mgr->getFCAL_ID();

  // Check whether we are working with Test Beam geometry
  bool isTestBeam = false;
  const LArDetectorManager* larMgr{nullptr};
  StatusCode status = detStore->retrieve(larMgr);
  if(status.isFailure()) {
    log << MSG::WARNING << "Could not get LArDetectorManager. Assuming TestBeam=false" << endmsg;
  }
  else {
    isTestBeam = larMgr->isTestBeam();
  }

  // Get minimal value for Hash ID - needed to retrieve sagging information
  IdentifierHash minHash,maxHash;
  cell_id->calo_cell_hash_range(CaloCell_ID::LAREM,minHash,maxHash);

  // Two objects needed to pass sagging info to updateAlignment() methods
  CaloElementPositionShift elementPosShift;
  CaloRec::CaloCellPositionShift::size_type posShiftInd;

  // ****************************************************************
  // **                   --- --- EMB --- ---                      **
  // ****************************************************************

  // --- Retrieve EMB Detector Manager
  const EMBDetectorManager* embManager{nullptr};
  status = detStore->retrieve(embManager);
  if(status.isFailure()) {
    log << MSG::WARNING << "Could not get the EMBDetectorManager. No Calo Elements will be built for EMB" << endmsg;
  }
  else {
    //         --- --- Iterate over EMB regions and cells --- ---
    EMBDetectorManager::DetectorRegionConstIterator embregIt;

    for (embregIt=embManager->beginDetectorRegion(); embregIt!=embManager->endDetectorRegion(); ++embregIt) {
      const EMBDetectorRegion *embRegion = *embregIt;

      // _________ Create descriptor for this region _________

      // Build region identifier.
      // Do some mapping between LArReadoutGeometry and CaloID
      int barrel_ec = 0;
      switch(embRegion->getEndcapIndex()) {
      case EMBDetectorRegion::NEG:
	barrel_ec = -1; // negative side
	break;
      case EMBDetectorRegion::POS:
	barrel_ec = 1;  // positive side
	break;
      default:
	throw std::runtime_error("buildCaloDetDescr Wrong Side Index for EMB region " + std::to_string(embRegion->getEndcapIndex()));
      }

      Identifier regId = em_id->region_id(barrel_ec
					  , embRegion->getSamplingIndex()
					  , embRegion->getRegionIndex());

      EMBDescriptor* embDescr = new EMBDescriptor(regId
						  , (AtlasDetectorID *)cell_id
						  , cell_id
						  , embRegion
						  , geoAlignStore);
      caloMgr->add(embDescr);

      double phi_min = 0.;
      double z_min = 10000.;
      double z_max = -10000.;
      double r_min = 10000.;
      double r_max = -10000.;

      double reg_min = 10000.;
      double reg_max = -10000.;

      std::vector<double> depth_in;
      std::vector<double> depth_out;
      // __________________________________________________________

      // _________ Create EMB detector elements _________
      for (unsigned int iPhi=embRegion->beginPhiIndex(); iPhi<embRegion->endPhiIndex(); ++iPhi) {
	for (unsigned int iEta=embRegion->beginEtaIndex(); iEta<embRegion->endEtaIndex(); ++iEta) {
	  EMBCellConstLink cellPtr = embRegion->getEMBCell(iEta,iPhi);

	  // build hash identifier for this cell
	  Identifier chanId = em_id->channel_id(barrel_ec
						, cellPtr->getSamplingIndex()
						, cellPtr->getRegionIndex()
						, iEta
						, iPhi);

	  // Create new element and store it
	  if(cellPosShift) {
            posShiftInd = cell_id->calo_cell_hash(chanId) - minHash;
            elementPosShift.dx = cellPosShift->deltaX(posShiftInd);
            elementPosShift.dy = cellPosShift->deltaY(posShiftInd);
            elementPosShift.dz = cellPosShift->deltaZ(posShiftInd);
          }
	  EMBDetectorElement* embElement = new EMBDetectorElement(em_id->channel_hash(chanId)
								  , 0
								  , 0
								  , embDescr
								  , cellPtr
								  , embRegion
								  , isTestBeam
								  , geoAlignStore
								  , cellPosShift ? &elementPosShift : nullptr);
	  if(cellPosShift) {
            if(iPhi==0 && (iEta==0||iEta==1)) {
              phi_min = embElement->phi() - 0.5*embElement->dphi();
            }
          }
          else {
            if(iPhi==embRegion->beginPhiIndex()) {
              phi_min = embElement->phi() - 0.5*embElement->dphi();
            }
          }

	  // cell volume
	  embElement->set_volume(cellVol.CellVolume(chanId));

	  caloMgr->add(embElement);

	  if(r_min > embElement->r()-0.5*embElement->dr())
	    r_min = embElement->r()-0.5*embElement->dr();
	  if(r_max < embElement->r()+0.5*embElement->dr())
	    r_max = embElement->r()+0.5*embElement->dr();

	  if(z_min > std::abs(embElement->z_raw())-0.5*embElement->dz())
	    z_min = std::abs(embElement->z_raw())-0.5*embElement->dz();
	  if(z_max < std::abs(embElement->z_raw())+0.5*embElement->dz())
	    z_max = std::abs(embElement->z_raw())+0.5*embElement->dz();

	  if(reg_min > embElement->eta()-0.5*embElement->deta())
	    reg_min = embElement->eta()-0.5*embElement->deta();
	  if(reg_max < embElement->eta()+0.5*embElement->deta())
	    reg_max = embElement->eta()+0.5*embElement->deta();

	  // deal with depth
	  if(iPhi==embRegion->beginPhiIndex()) {
	    depth_in.push_back(cellPtr->getRLocal(EMBCell::FRONT));
	    depth_out.push_back(cellPtr->getRLocal(EMBCell::BACK));
	  }
	} // Eta loop
      } // Phi loop
      // _____________________________________________________________

      double eta_min = (embRegion->getSamplingIndex()==1 && embRegion->getRegionIndex()==0)
	? embRegion->getDescriptor()->getEtaBinning().getStart() - embRegion->getDescriptor()->getEtaBinning().getDelta()
	: embRegion->getDescriptor()->getEtaBinning().getStart();

      double eta_max = (embRegion->getDescriptor()->getEtaBinning()).getEnd();
      double phi_max = phi_min + std::abs((embRegion->getDescriptor()->getPhiBinning()).getDelta())*embDescr->n_phi();

      // 'ideal' values
      embDescr->setCaloEtaMin(eta_min);
      embDescr->setCaloEtaMax(eta_max);
      embDescr->setCaloPhiMin(phi_min);
      embDescr->setCaloPhiMax(phi_max);
      embDescr->setCaloZMin(z_min);
      embDescr->setCaloZMax(z_max);
      embDescr->setCaloRMin(r_min);
      embDescr->setCaloRMax(r_max);

      // depths
      embDescr->set_n_calo_depth(depth_in.size());
      embDescr->set_depth_in(depth_in);
      embDescr->set_depth_out(depth_out);

      // 'alignable' values
      embDescr->setLArRegMin(reg_min);
      embDescr->setLArRegMax(reg_max);
      embDescr->setLArPhiMin(phi_min);
      if(cellPosShift) {
        if(embRegion->getEndcapIndex()==EMBDetectorRegion::NEG) {
          embDescr->setLArEtaMin(-reg_max);
        }
        else {
          embDescr->setLArEtaMin(reg_min);
        }
      }
      else {
        embDescr->setLArEtaMin(eta_min);
      }
    }// Region loop
  } // if EMB manager has been retrieved

  // ****************************************************************
  // **                   --- --- EMB --- ---                     **
  // ****************************************************************

  // ****************************************************************
  // **                   --- --- EMEC --- ---                     **
  // ****************************************************************

  // --- Retrieve Emec Detector Manager
  const EMECDetectorManager* emecManager{nullptr};
  status = detStore->retrieve(emecManager);
  if(status.isFailure()) {
    log << MSG::WARNING << "Could not get the EMECDetectorManager. No Calo Elements will be built for EMEC" << endmsg;
  }
  else {
    //         --- --- Iterate over EMEC regions and cells --- ---
    EMECDetectorManager::DetectorRegionConstIterator emecregIt;

    for (emecregIt=emecManager->beginDetectorRegion(); emecregIt!=emecManager->endDetectorRegion(); ++emecregIt) {
      const EMECDetectorRegion *emecRegion = *emecregIt;

      // _________ Create descriptor for this region _________

      // Build Region identifier.
      // Do some mapping between LArReadoutGeometry and CaloID
      EMECDetectorRegion::DetectorSide endcapInd = emecRegion->getEndcapIndex();

      unsigned int radialInd = emecRegion->getRadialIndex();
      int barrel_ec;

      switch(endcapInd) {
      case EMECDetectorRegion::NEG:
	barrel_ec = -1; // negative side
	break;
      case EMECDetectorRegion::POS:
	barrel_ec = 1;  // positive side
	break;
      default:
	throw std::runtime_error("Wrong Endcap Index for EMEC region " + std::to_string(endcapInd));
      }// switch(endcapInd)

      switch(radialInd) {
      case 0:
	barrel_ec *= 2; // outer wheel
	break;
      case 1:
	barrel_ec *= 3;  // inner wheel
	break;
      default:
	throw std::runtime_error("Wrong Radial Index for  EMEC region " + std::to_string(radialInd));
      }// switch(radialInd)

      Identifier regId = em_id->region_id(barrel_ec
					  , emecRegion->getSamplingIndex()
					  , emecRegion->getRegionIndex());

      EMECDescriptor* emecDescr = new EMECDescriptor(regId
						     , (AtlasDetectorID *)cell_id
						     , cell_id
						     , emecRegion
						     , geoAlignStore);
      caloMgr->add(emecDescr);

      double phi_min = 0.;
      double z_min = 10000.;
      double z_max = -10000.;
      double r_min = 10000.;
      double r_max = -10000.;

      double reg_min = 10000.;
      double reg_max = -10000.;

      std::vector<double> depth_in;
      std::vector<double> depth_out;
      // _____________________________________________________________

      // _________ Create EMEC detector elements _________
      for (unsigned int iPhi=emecRegion->beginPhiIndex(); iPhi<emecRegion->endPhiIndex(); ++iPhi) {
	for (unsigned int iEta=emecRegion->beginEtaIndex(); iEta<emecRegion->endEtaIndex(); ++iEta) {
	  EMECCellConstLink cellPtr = emecRegion->getEMECCell(iEta,iPhi);

	  Identifier chanId = em_id->channel_id(barrel_ec
						, cellPtr->getSamplingIndex()
						, cellPtr->getRegionIndex()
						, iEta
						, iPhi);

	  // Create new element and store it
	  if(cellPosShift) {
            posShiftInd = cell_id->calo_cell_hash(chanId) - minHash;
            elementPosShift.dx = cellPosShift->deltaX(posShiftInd);
            elementPosShift.dy = cellPosShift->deltaY(posShiftInd);
            elementPosShift.dz = cellPosShift->deltaZ(posShiftInd);
          }
	  EMECDetectorElement* emecElement = new EMECDetectorElement(em_id->channel_hash(chanId)
								     , 0
								     , 0
								     , emecDescr
								     , cellPtr
								     , emecRegion
								     , isTestBeam
								     , geoAlignStore
								     , cellPosShift ? &elementPosShift : nullptr);
	  if((cellPosShift && iPhi==0)
	     ||(!cellPosShift && iPhi==emecRegion->beginPhiIndex())) {
	    phi_min = emecElement->phi() - 0.5*emecElement->dphi();
	  }

	  // cell volume
	  emecElement->set_volume(cellVol.CellVolume(chanId));
	  caloMgr->add(emecElement);

	  if(r_min > emecElement->r()-0.5*emecElement->dr())
	    r_min = emecElement->r()-0.5*emecElement->dr();
	  if(r_max < emecElement->r()+0.5*emecElement->dr())
	    r_max = emecElement->r()+0.5*emecElement->dr();

	  if(z_min > std::abs(emecElement->z_raw())-0.5*emecElement->dz())
	    z_min = std::abs(emecElement->z_raw())-0.5*emecElement->dz();
	  if(z_max < std::abs(emecElement->z_raw())+0.5*emecElement->dz())
	    z_max = std::abs(emecElement->z_raw())+0.5*emecElement->dz();

	  if(reg_min > emecElement->eta()-0.5*emecElement->deta())
	    reg_min = emecElement->eta()-0.5*emecElement->deta();
	  if(reg_max < emecElement->eta()+0.5*emecElement->deta())
	    reg_max = emecElement->eta()+0.5*emecElement->deta();

	  // depths
	  if(iPhi==emecRegion->beginPhiIndex()) {
	    depth_in.push_back(std::abs(emecElement->z_raw())-emecElement->dz());
	    depth_out.push_back(std::abs(emecElement->z_raw())+emecElement->dz());
	  }
	} // Eta loop
      } // Phi loop
      // ____________________________________________________________________

      double eta_min = emecRegion->getDescriptor()->getEtaBinning().getStart();
      double eta_max = emecRegion->getDescriptor()->getEtaBinning().getEnd();
      double phi_max = phi_min + std::abs(emecRegion->getDescriptor()->getPhiBinning().getDelta())*emecDescr->n_phi();

      // 'ideal' values
      emecDescr->setCaloEtaMin(eta_min);
      emecDescr->setCaloEtaMax(eta_max);
      emecDescr->setCaloPhiMin(phi_min);
      emecDescr->setCaloPhiMax(phi_max);
      emecDescr->setCaloZMin(z_min);
      emecDescr->setCaloZMax(z_max);
      emecDescr->setCaloRMin(r_min);
      emecDescr->setCaloRMax(r_max);

      // depths
      emecDescr->set_n_calo_depth(depth_in.size());
      emecDescr->set_depth_in(depth_in);
      emecDescr->set_depth_out(depth_out);

      // 'alignable' values
      emecDescr->setLArRegMin(reg_min);
      emecDescr->setLArRegMax(reg_max);
      emecDescr->setLArPhiMin(phi_min);
      if(cellPosShift) {
        if(emecRegion->getEndcapIndex()==EMECDetectorRegion::NEG) {
          emecDescr->setLArEtaMin(-reg_max);
        }
        else {
          emecDescr->setLArEtaMin(reg_min);
        }
      }
      else {
        emecDescr->setLArEtaMin(eta_min);
      }
    }// Region loop
  }// if EMEC manager has been retrieved

  // ****************************************************************
  // **                   --- --- EMEC --- ---                     **
  // ****************************************************************


  // ****************************************************************
  // **                    --- --- HEC --- ---                     **
  // ****************************************************************

  // --- Retrieve Hec Detector Manager
  const HECDetectorManager* hecManager{nullptr};
  status = detStore->retrieve(hecManager);
  if(status.isFailure()) {
    log << MSG::WARNING << "Could not get the HECDetectorManager. No Calo Elements will be built for HEC" << endmsg;
  }
  else {
    //         --- --- Iterate over HEC regions and cells --- ---
    HECDetectorManager::DetectorRegionConstIterator hecregIt;

    for (hecregIt=hecManager->beginDetectorRegion(); hecregIt!=hecManager->endDetectorRegion(); ++hecregIt) {
      const HECDetectorRegion *hecregion = *hecregIt;

      // _________ Create descriptor for this region _________

      // Build Region identifier.
      // Do some mapping between LArReadoutGeometry and CaloID
      HECDetectorRegion::DetectorSide endcapInd = hecregion->getEndcapIndex();
      int pos_neg = endcapInd==HECDetectorRegion::NEG ? -2 : 2;

      Identifier regId = hec_id->region_id(pos_neg
					   , hecregion->getSamplingIndex()
					   , hecregion->getRegionIndex());

      HECDescriptor* hecDescr = new HECDescriptor(regId
						  , (AtlasDetectorID *)cell_id
						  , cell_id
						  , hecregion
						  , geoAlignStore);
      caloMgr->add(hecDescr);

      double phi_min = 0.;
      float z_min = 10000.f;
      float z_max = -10000.f;
      double r_min = 10000.;
      double r_max = -10000.;

      double reg_min = 10000.;
      double reg_max = -10000.;

      std::vector<double> depth_in;
      std::vector<double> depth_out;
      // _____________________________________________________________

      // _________ Create HEC detector elements _________
      for (unsigned int iPhi=hecregion->beginPhiIndex(); iPhi<hecregion->endPhiIndex(); ++iPhi) {
	for (unsigned int iEta=hecregion->beginEtaIndex(); iEta<hecregion->endEtaIndex(); ++iEta) {
	  HECCellConstLink cellPtr = hecregion->getHECCell(iEta,iPhi);
	  // build hash identifier for this cell
	  // Do some mapping between LArReadoutGeometry and CaloID

	  if(cellPtr) {
	    Identifier chanId = hec_id->channel_id(pos_neg
						   , cellPtr->getSamplingIndex()
						   , cellPtr->getRegionIndex()
						   , iEta
						   , iPhi);

	    // Create the element and store it
	    if(cellPosShift) {
              posShiftInd = cell_id->calo_cell_hash(chanId) - minHash;
              elementPosShift.dx = cellPosShift->deltaX(posShiftInd);
              elementPosShift.dy = cellPosShift->deltaY(posShiftInd);
              elementPosShift.dz = cellPosShift->deltaZ(posShiftInd);
            }
	    HECDetectorElement* hecElement = new HECDetectorElement(hec_id->channel_hash(chanId)
								    , 0
								    , 0
								    , hecDescr
								    , cellPtr
								    , hecregion
								    , isTestBeam
								    , geoAlignStore
								    , cellPosShift ? &elementPosShift : nullptr);

	    if((cellPosShift && iPhi==0)
               || (!cellPosShift && iPhi==hecregion->beginPhiIndex())) {
              phi_min = hecElement->phi() - 0.5*hecElement->dphi();
            }

	    // cell volume
	    hecElement->set_volume(cellVol.CellVolume(chanId));

	    caloMgr->add(hecElement);

	    if(r_min > cellPtr->getRMinLocalNominal(HECCell::FRONT))
	      r_min = cellPtr->getRMinLocalNominal(HECCell::FRONT);
	    if(r_max < cellPtr->getRMaxLocalNominal(HECCell::FRONT))
	      r_max = cellPtr->getRMaxLocalNominal(HECCell::FRONT);

	    if(z_min > std::abs(hecElement->z_raw())-hecElement->dz())
	      z_min = std::abs(hecElement->z_raw())-hecElement->dz();
	    if(z_max < std::abs(hecElement->z_raw())+hecElement->dz())
	      z_max = std::abs(hecElement->z_raw())+hecElement->dz();

	    if(reg_min > hecElement->eta()-0.5*hecElement->deta())
	      reg_min = hecElement->eta()-0.5*hecElement->deta();
	    if(reg_max < hecElement->eta()+0.5*hecElement->deta())
	      reg_max = hecElement->eta()+0.5*hecElement->deta();

	    if(iPhi==hecregion->beginPhiIndex() && iEta==hecregion->beginEtaIndex()) {
	      depth_in.push_back(std::abs(hecElement->z_raw())-hecElement->dz());
	      depth_out.push_back(std::abs(hecElement->z_raw())+hecElement->dz());
	    }

	  }
	} // Eta loop
      } // Phi loop
      //            *** *** *** Iterate over cells *** *** ***

      double eta_min = hecregion->getDescriptor()->getEtaBinning().getStart();
      double eta_max = hecregion->getDescriptor()->getEtaBinning().getEnd();
      double phi_max = phi_min + std::abs(hecregion->getDescriptor()->getPhiBinning().getDelta())*hecDescr->n_phi();

      // 'ideal' values
      hecDescr->setCaloEtaMin(eta_min);
      hecDescr->setCaloEtaMax(eta_max);
      hecDescr->setCaloPhiMin(phi_min);
      hecDescr->setCaloPhiMax(phi_max);
      hecDescr->setCaloZMin(z_min);
      hecDescr->setCaloZMax(z_max);
      hecDescr->setCaloRMin(r_min);
      hecDescr->setCaloRMax(r_max);

      // depths
      hecDescr->set_n_calo_depth(depth_in.size());
      hecDescr->set_depth_in(depth_in);
      hecDescr->set_depth_out(depth_out);

      // 'alignable' values
      hecDescr->setLArRegMin(reg_min);
      hecDescr->setLArRegMax(reg_max);
      hecDescr->setLArPhiMin(phi_min);
      if(cellPosShift) {
        if(hecregion->getEndcapIndex()==HECDetectorRegion::NEG) {
          hecDescr->setLArEtaMin(-reg_max);
        }
        else {
          hecDescr->setLArEtaMin(reg_min);
        }
      }
      else {
        hecDescr->setLArEtaMin(eta_min);
      }

    }// Region loop
  } // if HEC manager has been retrieved

  // ****************************************************************
  // **                   --- --- HEC --- ---                      **
  // ****************************************************************


  // ****************************************************************
  // **                    --- --- FCAL --- ---                    **
  // ****************************************************************

  // --- Retrieve Fcal Detector Manager
  const FCALDetectorManager* fcalManager{nullptr};
  status = detStore->retrieve(fcalManager);
  if(status.isFailure()) {
    log << MSG::WARNING << "Could not get the FCALDetectorManager. No Calo Elements will be built for FCAL" << endmsg;
  }
  else {
    //         --- --- Iterate over FCAL modules and tiles --- ---
    FCALDetectorManager::ConstIterator fcalmodIt;

    for (fcalmodIt=fcalManager->beginFCAL(); fcalmodIt!=fcalManager->endFCAL(); ++fcalmodIt) {
      const FCALModule* fcalmodule = *fcalmodIt;

      // _________ Create descriptor for this module _________

      // Build module identifier.
      // Do some mapping between LArReadoutGeometry and CaloID

      FCALModule::Endcap endcapInd = fcalmodule->getEndcapIndex();

      int pos_neg = endcapInd==FCALModule::NEG ? -2 : 2;

      Identifier regId = fcal_id->module_id(pos_neg,(int)fcalmodule->getModuleIndex());

      FCALDescriptor* fcalDescr = new FCALDescriptor(regId
						     , (AtlasDetectorID *)cell_id
						     , cell_id
						     , fcalmodule
						     , geoAlignStore);
      caloMgr->add(fcalDescr);

      double eta_min = 10000.;
      double eta_max = -10000.;
      double z_min = 10000.;
      double z_max = -10000.;
      double r_min = 10000.;
      double r_max = -10000.;

      double reg_min = 10000.;
      double reg_max = -10000.;

      std::vector<double> depth_in;
      std::vector<double> depth_out;
      // ___________________________________________________________

      // _________ Create FCAL detector elements _________
      FCALModule::ConstIterator fcaltileIt;
      for (fcaltileIt=fcalmodule->beginTiles(); fcaltileIt!=fcalmodule->endTiles(); ++fcaltileIt) {
	Identifier chanId = fcal_id->channel_id(pos_neg
						, (int)fcalmodule->getModuleIndex()
						, fcaltileIt->getIndexJ()   // eta
						, fcaltileIt->getIndexI());  // phi

	if(cellPosShift) {
          posShiftInd = cell_id->calo_cell_hash(chanId) - minHash;
          elementPosShift.dx = cellPosShift->deltaX(posShiftInd);
          elementPosShift.dy = cellPosShift->deltaY(posShiftInd);
          elementPosShift.dz = cellPosShift->deltaZ(posShiftInd);
        }
	FCALDetectorElement* fcalElement = new FCALDetectorElement(fcal_id->channel_hash(chanId)
								   , 0
								   , 0
								   , fcalDescr
								   , &(*fcaltileIt)
								   , fcalmodule
								   , isTestBeam
								   , geoAlignStore
								   , cellPosShift ? &elementPosShift : nullptr);
	// calculate cell volume
	double tubeSpacing = cellVol.getFcalTubeSpacing((int)fcalmodule->getModuleIndex());
	unsigned int numTubes = fcaltileIt->getNumTubes();
	double dz = fcalmodule->getFullDepthZ(*fcaltileIt);
	fcalElement->set_volume(.5*sqrt(3)*numTubes*tubeSpacing*tubeSpacing*dz);

	caloMgr->add(fcalElement);

	if(eta_min > std::abs(fcalElement->eta_raw())-0.5*fcalElement->deta())
	  eta_min = std::abs(fcalElement->eta_raw())-0.5*fcalElement->deta();
	if(eta_max < std::abs(fcalElement->eta_raw())+0.5*fcalElement->deta())
	  eta_max = std::abs(fcalElement->eta_raw())+0.5*fcalElement->deta();
	if(r_min > fcalElement->r() - 0.5*fcalElement->dr())
	  r_min = fcalElement->r() - 0.5*fcalElement->dr();
	if(r_max < fcalElement->r() + 0.5*fcalElement->dr())
	  r_max = fcalElement->r() + 0.5*fcalElement->dr();
	if(z_min > std::abs(fcalElement->z_raw()) - 0.5*fcalElement->dz())
	  z_min = std::abs(fcalElement->z_raw()) - 0.5*fcalElement->dz();
	if(z_max < std::abs(fcalElement->z_raw()) + 0.5*fcalElement->dz())
	  z_max = std::abs(fcalElement->z_raw()) + 0.5*fcalElement->dz();
	if(reg_min > fcalElement->eta()-0.5*fcalElement->deta())
	  reg_min = fcalElement->eta()-0.5*fcalElement->deta();
	if(reg_max < fcalElement->eta()+0.5*fcalElement->deta())
	  reg_max = fcalElement->eta()+0.5*fcalElement->deta();

	if(fcaltileIt==fcalmodule->beginTiles()) {
	  depth_in.push_back(std::abs(fcalElement->z_raw()) - fcalElement->dz());
	  depth_out.push_back(std::abs(fcalElement->z_raw()) + fcalElement->dz());
	}
      }

      // These values have no importance for FCAL - hardwire them here.
      fcalDescr->setCaloPhiMin(0.);
      fcalDescr->setCaloPhiMax(2*M_PI);
      fcalDescr->setLArPhiMin(0.);


      fcalDescr->setCaloEtaMin(eta_min);
      fcalDescr->setCaloEtaMax(eta_max);
      fcalDescr->setCaloZMin(z_min);
      fcalDescr->setCaloZMax(z_max);
      fcalDescr->setCaloRMin(r_min);
      fcalDescr->setCaloRMax(r_max);
      fcalDescr->setLArRegMin(reg_min);
      fcalDescr->setLArRegMax(reg_max);

      // depths
      fcalDescr->set_n_calo_depth(depth_in.size());
      fcalDescr->set_depth_in(depth_in);
      fcalDescr->set_depth_out(depth_out);

      if(fcalmodule->getEndcapIndex()==FCALModule::NEG) {
	fcalDescr->setLArEtaMin(-reg_max);
      }
      else {
	fcalDescr->setLArEtaMin(reg_min);
      }
    }// Module loop
  }// if FCAL manager has been retrieved

  // ****************************************************************
  // **                   --- --- FCAL --- ---                      **
  // ****************************************************************


  // ****************************************************************
  // **                   --- --- Tile --- ---                      **
  // ****************************************************************
  const TileDetDescrManager* tile_mgr{nullptr};
  status = detStore->retrieve(tile_mgr);
  if (status.isFailure()) {
    log << MSG::WARNING << "Could not get the TileDetectorManager. No Calo Elements will be built for Tile" << endmsg;
  }
  else {
    log << MSG::DEBUG << " Found the TileDetDescrManager " << endmsg;
    cell_id->calo_cell_hash_range((int)CaloCell_ID::TILE,minHash,maxHash);
    unsigned idHashMax = maxHash-minHash;
    for(unsigned int idhash=0; idhash < idHashMax; ++idhash) {
      CaloDetDescrElement* newelt = tile_mgr->get_cell_element(idhash);
      if(newelt) {
	caloMgr->add(newelt);
      }
    }

    std::vector<CaloDetDescriptor*>::const_iterator itr = tile_mgr->calo_descriptors_begin();
    std::vector<CaloDetDescriptor*>::const_iterator end = tile_mgr->calo_descriptors_end();

    for(; itr != end; ++itr) {
      caloMgr->add_tile(*itr);
    }
  }
  // ****************************************************************
  // **                   --- --- Tile --- ---                     **
  // ****************************************************************

  // ****************************************************************
  // **                 --- Additional elements  ---               **
  // ****************************************************************
  if(isTestBeam) {
    CaloDetDescrElementContainer *cDDEvec;
    if(detStore->retrieve(cDDEvec) == StatusCode::SUCCESS) {
      // We have additional elements
      for (CaloDetDescrElement* elt : *cDDEvec) {
	caloMgr->add (elt);
      }
    }
  }

  return caloMgr;
}
