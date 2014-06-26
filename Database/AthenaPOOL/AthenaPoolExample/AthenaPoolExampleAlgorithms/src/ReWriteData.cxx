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

#include "GaudiKernel/MsgStream.h"

#include "StoreGate/StoreGateSvc.h"

#include <cmath>

using namespace AthPoolEx;

//___________________________________________________________________________
ReWriteData::ReWriteData(const std::string& name, ISvcLocator* pSvcLocator) : Algorithm(name, pSvcLocator), p_SGevent("StoreGateSvc", name) {
}
//___________________________________________________________________________
ReWriteData::~ReWriteData() {
}
//___________________________________________________________________________
StatusCode ReWriteData::initialize() {
   MsgStream log(msgSvc(), name());
   log << MSG::INFO << "in initialize()" << endreq;

   // locate the StoreGateSvc and initialize our local ptr
   StatusCode sc = p_SGevent.retrieve();
   if (!sc.isSuccess() || 0 == p_SGevent) {
      log << MSG::ERROR << "Could not find StoreGateSvc" << endreq;
   }
   return(sc);
}
//___________________________________________________________________________
StatusCode ReWriteData::execute() {
   MsgStream log(msgSvc(), name());
   log << MSG::DEBUG << "in execute()" << endreq;

   if (p_SGevent->contains<ExampleHitContainer>("MyHits")) {
      const DataHandle<ExampleHitContainer> hitCont;
      StatusCode sc = p_SGevent->retrieve(hitCont, "MyHits");
      if (!sc.isSuccess()) {
         log << MSG::ERROR << "Could not find ExampleHitContainer/MyHits" << endreq;
         return(StatusCode::FAILURE);
      }
      double pT = 0.0, eta = 0.0, phi = 0.0;
      for (ExampleHitContainer::const_iterator hitObj = hitCont->begin(); hitObj != hitCont->end(); hitObj++) {
         log << MSG::INFO << "Hit x = " << (*hitObj)->getX() << " y = " << (*hitObj)->getY() << " z = " << (*hitObj)->getZ() << " detector = " << (*hitObj)->getDetector() << endreq;
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
      log << MSG::INFO << "ElementLink1 = " << trackObj->getElement1()->getX() << endreq;
      trackObj->getElementLink2()->toIndexedElement(*hitCont, hitCont->size() - 1);
      log << MSG::INFO << "ElementLink2 = " << trackObj->getElement2()->getX() << endreq;

      ElementLink<ExampleHitContainer> eLink1, eLink2, eLink3;
      eLink1.toContainedElement(*hitCont, *hitCont->begin());
      trackObj->getElementLinkVector()->push_back(eLink1);
      eLink2.toIndexedElement(*hitCont, 1);
      trackObj->getElementLinkVector()->push_back(eLink2);
      eLink3.toContainedElement(*hitCont, (*hitCont)[3]);
      trackObj->getElementLinkVector()->push_back(eLink3);
      log << MSG::INFO << "Link ElementLinkVector = " << trackObj->getElementLinkVector()->size() << endreq;
      for (ElementLinkVector<ExampleHitContainer>::iterator iter = trackObj->getElementLinkVector()->begin(); iter != trackObj->getElementLinkVector()->end(); ++iter) {
         log << MSG::INFO << "Element = " << (**iter) << " : " << (**iter)->getX() << endreq;
      }

      trackObj->getNavigable()->putElement(hitCont, *hitCont->begin());
      trackObj->getNavigable()->putElement(hitCont, (*hitCont)[5]);
      log << MSG::INFO << "Link Navigable = " << trackObj->getNavigable()->size() << endreq;
      for (Navigable<ExampleHitContainer>::object_iter iter = trackObj->getNavigable()->begin(); iter != trackObj->getNavigable()->end(); ++iter) {
         log << MSG::INFO << "Element = " << (*iter) << " : " << (*iter)->getX() << endreq;
      }

      trackObj->getWeightedNavigable()->putElement(hitCont, *hitCont->begin(), 3.33);
      trackObj->getWeightedNavigable()->putElement(hitCont, (*hitCont)[5], 1.11);
      trackObj->getWeightedNavigable()->putElement(hitCont, (*hitCont)[3], 5.55);
      log << MSG::INFO << "Link Weighted Navigable = " << trackObj->getWeightedNavigable()->size() << endreq;
      for (Navigable<ExampleHitContainer, double>::object_iter iter = trackObj->getWeightedNavigable()->begin(); iter != trackObj->getWeightedNavigable()->end(); ++iter) {
         log << MSG::INFO << "Element = " << (*iter) << " : " << (*iter)->getX() << endreq;
      }

      ExampleTrackContainer* trackCont = new ExampleTrackContainer();
      trackCont->push_back(trackObj);
      sc = p_SGevent->record(trackCont, "MyTracks");
      if (sc.isFailure()) {
         log << MSG::ERROR << "Could not register ExampleTrackContainer/MyTracks" << endreq;
         return(StatusCode::FAILURE);
      } else {
         log << MSG::INFO << "Track pt = " << trackObj->getPT() << " eta = " << trackObj->getEta() << " phi = " << trackObj->getPhi() << " detector = " << trackObj->getDetector() << endreq;
      }
   }

   log << MSG::INFO << "registered all data" << endreq;
   return(StatusCode::SUCCESS);
}
//___________________________________________________________________________
StatusCode ReWriteData::finalize() {
   MsgStream log(msgSvc(), name());
   log << MSG::INFO << "in finalize()" << endreq;
   return(StatusCode::SUCCESS);
}
