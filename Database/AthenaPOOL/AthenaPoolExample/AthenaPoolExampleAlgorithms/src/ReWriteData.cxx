/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @file ReWriteData.cxx
 *  @brief This file contains the implementation for the ReWriteData class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 *  $Id: ReWriteData.cxx,v 1.18 2008-12-10 21:28:11 gemmeren Exp $
 **/

#include "ReWriteData.h"

// the user data-class defintions
#include "AthenaPoolExampleData/ExampleHitContainer.h"
#include "AthenaPoolExampleData/ExampleTrackContainer.h"

#include <cmath>

using namespace AthPoolEx;

//___________________________________________________________________________
ReWriteData::ReWriteData(const std::string& name, ISvcLocator* pSvcLocator) : AthAlgorithm(name, pSvcLocator) {
}
//___________________________________________________________________________
ReWriteData::~ReWriteData() {
}
//___________________________________________________________________________
StatusCode ReWriteData::initialize() {
   ATH_MSG_INFO("in initialize()");
   return StatusCode::SUCCESS;
}
//___________________________________________________________________________
StatusCode ReWriteData::execute() {
   ATH_MSG_DEBUG("in execute()");

   if (evtStore()->contains<ExampleHitContainer>("MyHits")) {
      const DataHandle<ExampleHitContainer> hitCont;
      if (evtStore()->retrieve(hitCont, "MyHits").isFailure()) {
         ATH_MSG_ERROR("Could not find ExampleHitContainer/MyHits");
         return StatusCode::FAILURE;
      }
      double pT = 0.0, eta = 0.0, phi = 0.0;
      for (ExampleHitContainer::const_iterator hitObj = hitCont->begin(); hitObj != hitCont->end(); hitObj++) {
         ATH_MSG_INFO("Hit x = " << (*hitObj)->getX() << " y = " << (*hitObj)->getY() << " z = " << (*hitObj)->getZ() << " detector = " << (*hitObj)->getDetector());
         pT = pT + sqrt((*hitObj)->getX() * (*hitObj)->getX() + (*hitObj)->getY() * (*hitObj)->getY());
         eta = eta + (*hitObj)->getX() / (*hitObj)->getZ();
         phi = phi + (*hitObj)->getX() / (*hitObj)->getY();
      }
      ExampleTrack* trackObj = new ExampleTrack();
      trackObj->setPT(pT / hitCont->size());
      trackObj->setEta(eta);
      trackObj->setPhi(phi);
      trackObj->setDetector("Track made in: " + (*hitCont->begin())->getDetector());
      trackObj->getElementLink1()->toContainedElement(*hitCont, *hitCont->begin());
      ATH_MSG_INFO("ElementLink1 = " << trackObj->getElement1()->getX());
      trackObj->getElementLink2()->toIndexedElement(*hitCont, hitCont->size() - 1);
      ATH_MSG_INFO("ElementLink2 = " << trackObj->getElement2()->getX());

      ElementLink<ExampleHitContainer> eLink1, eLink2, eLink3;
      eLink1.toContainedElement(*hitCont, *hitCont->begin());
      trackObj->getElementLinkVector()->push_back(eLink1);
      eLink2.toIndexedElement(*hitCont, 1);
      trackObj->getElementLinkVector()->push_back(eLink2);
      eLink3.toContainedElement(*hitCont, (*hitCont)[3]);
      trackObj->getElementLinkVector()->push_back(eLink3);
      ATH_MSG_INFO("Link ElementLinkVector = " << trackObj->getElementLinkVector()->size());
      for (ElementLinkVector<ExampleHitContainer>::iterator iter = trackObj->getElementLinkVector()->begin(); iter != trackObj->getElementLinkVector()->end(); ++iter) {
         ATH_MSG_INFO("Element = " << (**iter) << " : " << (**iter)->getX());
      }

      trackObj->getNavigable()->putElement(hitCont, *hitCont->begin());
      trackObj->getNavigable()->putElement(hitCont, (*hitCont)[5]);
      ATH_MSG_INFO("Link Navigable = " << trackObj->getNavigable()->size());
      for (Navigable<ExampleHitContainer>::object_iter iter = trackObj->getNavigable()->begin(); iter != trackObj->getNavigable()->end(); ++iter) {
         ATH_MSG_INFO("Element = " << (*iter) << " : " << (*iter)->getX());
      }

      trackObj->getWeightedNavigable()->putElement(hitCont, *hitCont->begin(), 3.33);
      trackObj->getWeightedNavigable()->putElement(hitCont, (*hitCont)[5], 1.11);
      trackObj->getWeightedNavigable()->putElement(hitCont, (*hitCont)[3], 5.55);
      ATH_MSG_INFO("Link Weighted Navigable = " << trackObj->getWeightedNavigable()->size());
      for (Navigable<ExampleHitContainer, double>::object_iter iter = trackObj->getWeightedNavigable()->begin(); iter != trackObj->getWeightedNavigable()->end(); ++iter) {
         ATH_MSG_INFO("Element = " << (*iter) << " : " << (*iter)->getX());
      }

      ExampleTrackContainer* trackCont = new ExampleTrackContainer();
      trackCont->push_back(trackObj);
      if (evtStore()->record(trackCont, "MyTracks").isFailure()) {
         ATH_MSG_ERROR("Could not register ExampleTrackContainer/MyTracks");
         return StatusCode::FAILURE;
      } else {
         ATH_MSG_INFO("Track pt = " << trackObj->getPT() << " eta = " << trackObj->getEta() << " phi = " << trackObj->getPhi() << " detector = " << trackObj->getDetector());
      }
   }

   ATH_MSG_INFO("registered all data");
   return StatusCode::SUCCESS;
}
//___________________________________________________________________________
StatusCode ReWriteData::finalize() {
   ATH_MSG_INFO("in finalize()");
   return StatusCode::SUCCESS;
}
