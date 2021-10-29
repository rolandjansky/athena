/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "LArGeoCode/LArAlignHelper.h"

#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/GeoAlignableTransform.h"
#include "GeoModelUtilities/GeoAlignmentStore.h"
#include "GeoModelUtilities/StoredAlignX.h"
#include "GeoModelUtilities/StoredPhysVol.h"
#include "DetDescrConditions/DetCondKeyTrans.h"
#include "GeoPrimitives/CLHEPtoEigenConverter.h"

LArAlignHelper::LArAlignHelper()
  : AthMessaging(Athena::getMessageSvc(), "LArAlignHelper")
{
}

StatusCode LArAlignHelper::applyAlignments(const ServiceHandle<StoreGateSvc>& detStore
					   , const DetCondKeyTrans* detCondKeyTrans
					   , GeoAlignmentStore* alignmentStore) const
{
  ATH_MSG_DEBUG("Applying LAr Alignments to GeoModel");

  // Special treatment for the HEC:
  StoredAlignX *hec1AlxPos{nullptr};
  StoredAlignX *hec2AlxPos{nullptr};
  StoredAlignX *hec1AlxNeg{nullptr};
  StoredAlignX *hec2AlxNeg{nullptr};
  if (detStore->contains<StoredAlignX> ("HEC1_POS")) {
    if (detStore->retrieve(hec1AlxPos,"HEC1_POS")!=StatusCode::SUCCESS) {
      ATH_MSG_WARNING(" Unable to retrieve StoredAlignX for the key HEC1_POS");
    }
  }
  if (detStore->contains<StoredAlignX> ("HEC1_NEG")) {
    if (detStore->retrieve(hec1AlxNeg,"HEC1_NEG")!=StatusCode::SUCCESS) {
      ATH_MSG_WARNING(" Unable to retrieve StoredAlignX for the key HEC1_NEG");
    }
  }
  if (detStore->contains<StoredAlignX> ("HEC2_POS")) {
    if (detStore->retrieve(hec2AlxPos,"HEC2_POS")!=StatusCode::SUCCESS) {
      ATH_MSG_WARNING(" Unable to retrieve StoredAlignX for the key HEC2_POS");
    }
  }
  if (detStore->contains<StoredAlignX> ("HEC2_NEG")) {
    if (detStore->retrieve(hec2AlxNeg,"HEC2_NEG")!=StatusCode::SUCCESS) {
      ATH_MSG_WARNING(" Unable to retrieve StoredAlignX for the key HEC2_NEG");
    }
  }
  GeoAlignableTransform *hec1GatPos = hec1AlxPos ? hec1AlxPos->getAlignX(): nullptr;
  GeoAlignableTransform *hec1GatNeg = hec1AlxPos ? hec1AlxNeg->getAlignX(): nullptr;
  GeoAlignableTransform *hec2GatPos = hec2AlxPos ? hec2AlxPos->getAlignX(): nullptr;
  GeoAlignableTransform *hec2GatNeg = hec2AlxPos ? hec2AlxNeg->getAlignX(): nullptr;
  
  // Loop over align names and if a transform is present, then alter its delta
  for(const std::string& alignName : m_alignNames) {
    ATH_MSG_DEBUG("Working on align name " << alignName);
    HepGeom::Transform3D newDelta;
    // First try to retrieve StoredAlignX
    if(detStore->contains<StoredAlignX>(alignName)) {
      StoredAlignX* alignX{nullptr};
      if(detStore->retrieve(alignX,alignName).isFailure()) {
	ATH_MSG_ERROR(" Unable to retrieve StoredAlignX for the key " << alignName);
	return StatusCode::FAILURE;
      }

      if(!alignX) {
	ATH_MSG_ERROR("Null pointer to StoredAlignX for the key " << alignName);
	return StatusCode::FAILURE;
      }

      GeoAlignableTransform* gat = alignX->getAlignX();
      if(!gat) {
	ATH_MSG_ERROR("Null pointer to GeoAlignableTransform for the key " << alignName);
	return StatusCode::FAILURE;
      }

      // Check the existence of new delta in DetCondKeyTrans
      if(detCondKeyTrans->getTransform(alignName,newDelta)) {
	gat->setDelta(Amg::CLHEPTransformToEigen(newDelta),alignmentStore);
      }
      else {
	gat->clearDelta(alignmentStore);
      }
    }
    else if (alignName=="HEC_POS") {
      if (hec1GatPos) {
	if(detCondKeyTrans->getTransform(alignName,newDelta)) {
	  hec1GatPos->setDelta(Amg::CLHEPTransformToEigen(newDelta),alignmentStore);
	}
	else {
	  hec1GatPos->clearDelta(alignmentStore);
	}
      }
      if (hec2GatPos) {
	if(detCondKeyTrans->getTransform(alignName,newDelta)) {
	  hec2GatPos->setDelta(Amg::CLHEPTransformToEigen(newDelta),alignmentStore);
	}
	else {
	  hec2GatPos->clearDelta(alignmentStore);
	}
      }
    }
    else if (alignName=="HEC_NEG") {
      if (hec1GatNeg) {
	if(detCondKeyTrans->getTransform(alignName,newDelta)) {
	  hec1GatNeg->setDelta(Amg::CLHEPTransformToEigen(newDelta),alignmentStore);
	}
	else {
	  hec1GatNeg->clearDelta(alignmentStore);
	}
      }
      if (hec2GatNeg) {
	if(detCondKeyTrans->getTransform(alignName,newDelta)) {
	  hec2GatNeg->setDelta(Amg::CLHEPTransformToEigen(newDelta),alignmentStore);
	}
	else {
	  hec2GatNeg->clearDelta(alignmentStore);
	}
      }
    }
    else {
      ATH_MSG_DEBUG(" No StoredAlignX for the key " << alignName);
    }
  } // Loop over Align Names

  // Fill the caches of Full Physical Volumes
  //
  // !!! NB! The code assumes that StoredPhysVol-s are used only by LAr
  //         This has been true ever since the StorePhysVol-s were invented.
  //
  for(const std::string& key : detStore->keys<StoredPhysVol>()) {
    StoredPhysVol* storedPV = detStore->tryRetrieve<StoredPhysVol>(key);
    ATH_MSG_DEBUG("Building position caches for StoredPhysVol :" << key);
    if(storedPV) {
      storedPV->getPhysVol()->getAbsoluteTransform(alignmentStore);
      storedPV->getPhysVol()->getDefAbsoluteTransform(alignmentStore);
    }
  }

  // Debug printout of global positions
  if (msgLvl (MSG::DEBUG)) {
    for(const std::string& alignName : m_alignNames) {
      if(detStore->contains<StoredPhysVol>(alignName)) {
        StoredPhysVol* storedPV{nullptr};
        if(detStore->retrieve(storedPV,alignName).isSuccess()) {
          const GeoFullPhysVol* fullPV = storedPV->getPhysVol();
          const GeoTrf::Transform3D& xf =  fullPV->getAbsoluteTransform(alignmentStore);
	  GeoTrf::Vector3D trans=xf.translation();
	  GeoTrf::RotationMatrix3D rot=xf.rotation();
          ATH_MSG_DEBUG("Dump Absolute Transform: Key " << alignName 
			<< " translation [" << trans.x() << "," << trans.y() << "," << trans.z() 
			<< "] rotation (" << rot(0,0) << "," << rot(0,1) << "," << rot(0,2)
			<< "," << rot(1,0) << "," << rot(1,1) << "," << rot(1,2)
			<< "," << rot(2,0) << "," << rot(2,1) << "," << rot(2,2) << ")");
	}
      }
    }
  }

  return StatusCode::SUCCESS;
}
