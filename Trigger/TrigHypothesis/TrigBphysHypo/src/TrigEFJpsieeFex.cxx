/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
// 
// NAME:     TrigEFJpsieeFex.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigBphysHypo
// author :  Julie Kirk
// ********************************************************************

#include <math.h> 
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/StatusCode.h"

#include "StoreGate/StoreGateSvc.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

#include "TrigBphysHypo/TrigEFJpsieeFex.h"
#include "TrigT1Interfaces/TrigT1Interfaces_ClassDEF.h"

#include "TrigSteeringEvent/TrigRoiDescriptor.h"


#include "CLHEP/GenericFunctions/CumulativeChiSquare.hh"


// additions of xAOD objects
#include "xAODEventInfo/EventInfo.h"

class ISvcLocator;

using namespace std;

/*------------------------------------------------------------------------------------*/
TrigEFJpsieeFex::TrigEFJpsieeFex(const std::string & name, ISvcLocator* pSvcLocator):
  HLT::FexAlgo(name, pSvcLocator)
/*------------------------------------------------------------------------------------*/
{

  // Read cuts 
  declareProperty( "AcceptAll", m_acceptAll  = false );

}

/*-------------------------------------------*/
TrigEFJpsieeFex::~TrigEFJpsieeFex()
/*-------------------------------------------*/
{

}

/*-------------------------------------------*/
HLT::ErrorCode TrigEFJpsieeFex::hltInitialize()
/*-------------------------------------------*/
{

  
  return HLT::OK;
}


/*-------------------------------------------*/
HLT::ErrorCode TrigEFJpsieeFex::hltFinalize()
/*-------------------------------------------*/
{

  
  return HLT::OK;
}


/*--------------------------------------------------------------*/
HLT::ErrorCode TrigEFJpsieeFex::acceptInput(const HLT::TriggerElement* inputTE, bool& pass)
/*--------------------------------------------------------------*/
{
  bool result = false;

    // Retrieve event info
    int IdRun   = 0;
    int IdEvent = 0;
    
    // JW - Try to get the xAOD event info
    const EventInfo* pEventInfo(0);
    const xAOD::EventInfo *evtInfo(0);
    if ( store()->retrieve(evtInfo).isFailure() ) {
        if ( msgLvl() <= MSG::DEBUG ) msg()  << MSG::DEBUG << "Failed to get xAOD::EventInfo " << endreq;
        // now try the old event ifo
        if ( store()->retrieve(pEventInfo).isFailure() ) {
            if ( msgLvl() <= MSG::DEBUG ) msg()  << MSG::DEBUG << "Failed to get EventInfo " << endreq;
            //mon_Errors.push_back( ERROR_No_EventInfo );
        } else {
            IdRun   = pEventInfo->event_ID()->run_number();
            IdEvent = pEventInfo->event_ID()->event_number();
            if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << " Run " << IdRun << " Event " << IdEvent <<  endreq;
        }// found old event info
    }else { // found the xAOD event info
        if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << " Run " << evtInfo->runNumber()
            << " Event " << evtInfo->eventNumber() << endreq;
        IdRun   = evtInfo->runNumber();
        IdEvent = evtInfo->eventNumber();
    } // get event ifo


  // Accept-All mode: temporary patch; should be done with force-accept 
  if (m_acceptAll) {
    if ( msgLvl() <= MSG::DEBUG )
      msg() << MSG::DEBUG << "AcceptAll property is set: taking all events" 
	  << endreq;

    pass = true;
  } 
  else {
    if ( msgLvl() <= MSG::DEBUG )
      msg() << MSG::DEBUG << "AcceptAll property not set: applying selection" 
	  << endreq;
    pass=false;
  }

// get RoI descriptor
  const TrigRoiDescriptor* roiDescriptor = 0;
  if (getFeature(inputTE, roiDescriptor,"initialRoI") != HLT::OK) roiDescriptor = 0;

  if ( !roiDescriptor ) {
    if ( msgLvl() <= MSG::WARNING) {
      msg() <<  MSG::WARNING << "No RoI for this Trigger Element! " << endreq;
    }    
    
    return HLT::NAV_ERROR;
  }

  if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << " In EFJpsieeFEX acceptInput" << endreq;

  if ( msgLvl() <= MSG::DEBUG ){
    msg() << MSG::DEBUG 
	<< "Using inputTE("<< inputTE <<")->getId(): " << inputTE->getId()
	<< "; RoI ID = "   << roiDescriptor->roiId()
	<< ": Eta = "      << roiDescriptor->eta()
	<< ", Phi = "      << roiDescriptor->phi()
	<< endreq;
  } 


  result=true;
  
  if (!m_acceptAll) pass = result; 

  return HLT::OK;
}


/*--------------------------------------------------------------*/
HLT::ErrorCode TrigEFJpsieeFex::hltExecute(const HLT::TriggerElement* /* inputTE */, HLT::TriggerElement* outputTE)
/*--------------------------------------------------------------*/
{
   if ( msgLvl() <= MSG::DEBUG )  msg() << MSG::DEBUG << " In EFJpsieeFEX hltExecute" << endreq;

// get RoI descriptor
  const TrigRoiDescriptor* roiDescriptor = 0;
  if (getFeature(outputTE, roiDescriptor,"initialRoI") != HLT::OK) roiDescriptor = 0;

  if ( !roiDescriptor ) {
    if ( msgLvl() <= MSG::WARNING) {
      msg() <<  MSG::WARNING << "No RoI for this Trigger Element! " << endreq;
    }    
    
    return HLT::NAV_ERROR;
  }

  if ( msgLvl() <= MSG::DEBUG ){
    msg() << MSG::DEBUG 
	<< "Using outputTE("<< outputTE <<")->getId(): " << outputTE->getId()
	<< "; RoI ID = "   << roiDescriptor->roiId()
	<< ": Eta = "      << roiDescriptor->eta()
	<< ", Phi = "      << roiDescriptor->phi()
	<< endreq;
  } 


     return HLT::OK;
}

/*-------------------------------------------*/
bool TrigEFJpsieeFex::PassTrackPair(const Trk::Perigee* trckPerigee1, const Trk::Perigee* trckPerigee2)
/*-------------------------------------------*/
{
  bool passed=false;

  double phi1 = trckPerigee1->parameters()[Trk::phi];
  double theta1 = trckPerigee1->parameters()[Trk::theta];
  double z0_1 = trckPerigee1->parameters()[Trk::z0];
  double px1 = trckPerigee1->momentum()[Trk::px];
  double py1 = trckPerigee1->momentum()[Trk::py];
  double pt1 = sqrt(px1*px1 + py1*py1);
  double eta1 = -std::log(tan(theta1/2));

  double phi2 = trckPerigee2->parameters()[Trk::phi];
  double theta2 = trckPerigee2->parameters()[Trk::theta];
  double z0_2 = trckPerigee2->parameters()[Trk::z0];
  double px2 = trckPerigee2->momentum()[Trk::px];
  double py2 = trckPerigee2->momentum()[Trk::py];
  double pt2 = sqrt(px2*px2 + py2*py2);
  double eta2 = -std::log(tan(theta2/2));


  if ( msgLvl() <= MSG::VERBOSE) msg() << MSG::VERBOSE << "PassTrackPair: track 1 : pt= " << pt1 
    <<  " phi0 = " << phi1 << "  eta = " << eta1 << "   z0 = " << z0_1 << endreq;
  if ( msgLvl() <= MSG::VERBOSE) msg() << MSG::VERBOSE << "PassTrackPair: track 2 : pt= " << pt2 
    <<  " phi0 = " << phi2 << "  eta = " << eta2 << "   z0 = " << z0_2 << endreq;

  double deta = eta1-eta2;
  double dphi = phi1-phi2;
  double absdphi = fabs(dphi);
  if (2.* M_PI -absdphi < absdphi) {
     if (dphi > 0) { 
        dphi = dphi - 2.* M_PI;
     } else { 
        dphi = 2* M_PI - absdphi;
     }
  }	
  dphi=fabs(dphi);

  double dz0  = z0_1 - z0_2;
  double ptsum= pt1+pt2;


  if ( msgLvl() <= MSG::VERBOSE ) msg() << MSG::VERBOSE
      << " deta = " << deta << "    dphi = " << dphi << "    dz0 = " << dz0 << "    ptsum = " << ptsum 
      << endreq;

  //  m_dEta.push_back(deta);
  //m_dPhi.push_back(dphi);
  //m_dz0.push_back(dz0);
  //m_Ptsum.push_back(ptsum/1000.);


  if (fabs(deta)< m_dEta_cut && fabs(dphi)<m_dPhi_cut && fabs(dz0)<m_dz0_cut && ptsum>m_pTsum_cut) 
     passed=true;



  if ( msgLvl() <= MSG::VERBOSE ) msg() << MSG::VERBOSE << "Track pairs passed: " << passed << endreq;
  
  return passed;
}

