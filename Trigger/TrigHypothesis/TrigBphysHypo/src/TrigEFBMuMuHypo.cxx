// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**************************************************************************
 **
 **   File: Trigger/TrigHypothesis/TrigBPhysHypo/TrigEFBMuMuHypo.cxx
 **
 **   Description: EF hypothesis algorithms for B0_s -> mu+ mu-
 **                Dummy algo for now
 **
 **   Author: S.Sivoklokov (on the base of Carlo Schiavi example)
 **
 **   Created:   12.09.07
 **   Modified:
 **
 **************************************************************************/

//#include "TrigMuonEvent/CombinedMuonFeature.h"

#include "TrigEFBMuMuHypo.h"
//#include "TrigBphysHypo/TrigEFBMuMuFex.h"

#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/DataHandle.h"

#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include <math.h>
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
//#include "TrigSteeringEvent/TrigPassBits.h"
#include "xAODTrigger/TrigPassBits.h"
#include "TrigNavigation/Navigation.h"
#include "InDetBeamSpotService/IBeamCondSvc.h"
//#include "Particle/TrackParticleContainer.h"

#include "CLHEP/Geometry/Point3D.h"

// additions of xAOD objects
#include "xAODEventInfo/EventInfo.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODMuon/Muon.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODTrigBphys/TrigBphys.h"
#include "xAODTrigBphys/TrigBphysContainer.h"
//#include "xAODTrigBphys/TrigBphysAuxContainer.h"

class ISvcLocator;

TrigEFBMuMuHypo::TrigEFBMuMuHypo(const std::string & name, ISvcLocator* pSvcLocator):
    HLT::HypoAlgo(name, pSvcLocator)
// counters
,m_lastEvent(-1)
,m_lastEventPassed(-1)
,m_countTotalEvents(0)
,m_countTotalRoI(0)
,m_countPassedEvents(0)
,m_countPassedRoIs(0)
,m_countPassedBsMass(0)
,m_countPassedChi2Cut(0)
,m_countPassedLxyCut(0)
{

  // Read cuts

  declareProperty("AcceptAll",    m_acceptAll=true);
  declareProperty("OppositeSign", m_oppositeCharge=true);
  declareProperty("LowerMassCut", m_lowerMassCut=4000.0);
  declareProperty("UpperMassCut", m_upperMassCut=6000.0);
  declareProperty("ApplyUpperMassCut", m_ApplyupperMassCut=true);
  declareProperty("ApplyChi2Cut", m_ApplyChi2Cut=true);
  declareProperty("Chi2VtxCut", m_Chi2VtxCut=20.0);
  
  declareProperty("ApplyLxyCut", m_ApplyLxyCut=false);
  declareProperty("LxyCut", m_LxyCut=0.0);                                                                                                
    
  declareMonitoredVariable("CutCounter",   m_mon_cutCounter);
  declareMonitoredVariable("MuMumass",     m_mon_MuMumass  );


}

TrigEFBMuMuHypo::~TrigEFBMuMuHypo()
{ }

HLT::ErrorCode TrigEFBMuMuHypo::hltInitialize()
{

  if(msgLvl() <= MSG::DEBUG) {

    msg() << MSG::DEBUG << "AcceptAll            = "
        << (m_acceptAll==true ? "True" : "False") << endmsg;
    msg() << MSG::DEBUG << "OppositeCharge       = "
        << (m_oppositeCharge==true ? "True" : "False") << endmsg;
    msg() << MSG::DEBUG << "LowerMassCut         = " << m_lowerMassCut << endmsg;
    msg() << MSG::DEBUG << "UpperMassCut         = " << m_upperMassCut << endmsg;
    msg() << MSG::DEBUG << "ApplyUpperMassCut         = " << m_ApplyupperMassCut << endmsg;

  }

  m_lastEvent = -1;
  m_lastEventPassed = -1;
  m_countTotalEvents =0;
  m_countTotalRoI =0;
  m_countPassedEvents =0;
  m_countPassedRoIs =0;
  m_countPassedBsMass =0;
  m_countPassedChi2Cut =0;
  m_countPassedLxyCut =0;

  return HLT::OK;
}

HLT::ErrorCode TrigEFBMuMuHypo::hltFinalize()
{
  msg() << MSG::INFO << "in finalize()" << endmsg;
  msg() << MSG::INFO << "|----------------------- SUMMARY FROM TrigEFBMuMuHypo -------------|" << endmsg;
  msg() << MSG::INFO << "Run on events/2xRoIs " << m_countTotalEvents << "/" << m_countTotalRoI <<  endmsg;
  msg() << MSG::INFO << "Passed events/2xRoIs " << m_countPassedEvents << "/" << m_countPassedRoIs <<  endmsg;
  msg() << MSG::INFO << "RoIs Passed BsMass: "  << m_countPassedBsMass << endmsg;
  msg() << MSG::INFO << "RoIs Passed Chi2 cut: "  << m_countPassedChi2Cut << endmsg;
  msg() << MSG::INFO << "RoIs Passed Lxy cut: "  << m_countPassedLxyCut << endmsg;

  return HLT::OK;
}

//-------------------------------------------------------------------------------------
HLT::ErrorCode TrigEFBMuMuHypo::hltExecute(const HLT::TriggerElement* outputTE, bool& pass)
{

  bool PassedBsMass=false;
  bool PassedChi2Cut=false;
  bool PassedLxyCut=false;
  bool result = false;
  m_mon_cutCounter = -1;
    // Retrieve event info
    int IdRun   = 0;
    int IdEvent = 0;
    
    // JW - Try to get the xAOD event info
    const EventInfo* pEventInfo(0);
    const xAOD::EventInfo *evtInfo(0);
    if ( store()->retrieve(evtInfo).isFailure() ) {
        if ( msgLvl() <= MSG::DEBUG ) msg()  << MSG::DEBUG << "Failed to get xAOD::EventInfo " << endmsg;
        // now try the old event ifo
        if ( store()->retrieve(pEventInfo).isFailure() ) {
            if ( msgLvl() <= MSG::DEBUG ) msg()  << MSG::DEBUG << "Failed to get EventInfo " << endmsg;
            //m_mon_Errors.push_back( ERROR_No_EventInfo );
        } else {
            IdRun   = pEventInfo->event_ID()->run_number();
            IdEvent = pEventInfo->event_ID()->event_number();
            if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << " Run " << IdRun << " Event " << IdEvent <<  endmsg;
        }// found old event info
    }else { // found the xAOD event info
        if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << " Run " << evtInfo->runNumber()
            << " Event " << evtInfo->eventNumber() << endmsg;
        IdRun   = evtInfo->runNumber();
        IdEvent = evtInfo->eventNumber();
    } // get event ifo

  if (IdEvent != m_lastEvent) {
    m_countTotalEvents++;
    m_lastEvent=IdEvent;
  }
  m_countTotalRoI++;

  // Accept-All mode: temporary patch; should be done with force-accept
  if (m_acceptAll) {
    pass = true;
    //return HLT::OK;
  }
  if(msgLvl() <= MSG::DEBUG) {
    if (m_acceptAll) {
      msg() << MSG::DEBUG << "AcceptAll property is set: taking all events"
          << endmsg;
    } else {
      msg() << MSG::DEBUG << "AcceptAll property not set: applying selection"
          << endmsg;
    }
  }

  //  create vector for TrigEFBphys particles
    // const TrigEFBphysContainer* trigBphysColl = 0;
    const xAOD::TrigBphysContainer* trigBphysColl(0);
    
//  const TrigEFBContainer* trigBphysColl = 0;
//  const VxContainer* VertexColl;

  HLT::ErrorCode status = getFeature(outputTE, trigBphysColl, "EFBMuMuFex");

  if ( status != HLT::OK ) {
    if ( msgLvl() <= MSG::WARNING) {
      msg() << MSG::WARNING << "Failed to get xAOD::TrigBphysics collection" << endmsg;
    }

    return HLT::OK;
  }

  if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " Retrieved Bphys collection  trigBphysColl = " << trigBphysColl << endmsg;
  if ( trigBphysColl == 0 ) {
    if ( msgLvl() <= MSG::DEBUG )
      msg() << MSG::DEBUG << "No Bphys particles to analyse, leaving!" << endmsg;

    return HLT::OK;
  }

  if ( msgLvl() <= MSG::DEBUG ) {
    msg() << MSG::DEBUG << "Got TrigBphys collection with " << trigBphysColl->size()
        << " TrigBphys particles " << endmsg;
  }

  // if no Bphys particles were found, just leave TrigBphysColl. empty and leave
  if ( trigBphysColl->size() == 0 ) {
    if ( msgLvl() <= MSG::DEBUG )
      msg() << MSG::DEBUG << "No Bphys particles to analyse, leaving!" << endmsg;

    return HLT::OK;
  }

// Beam spot                                                                                                                            
   IBeamCondSvc* iBeamCondSvc;   
    //JW EDM   HepGeom::Point3D<double> m_beamSpot_CLHEP;
    Amg::Vector3D beamSpot(0.,0.,0.);
    if ( service("BeamCondSvc", iBeamCondSvc).isFailure() || iBeamCondSvc == 0)                                                       
       { msg() << MSG::DEBUG<< "Could not retrieve Beam Conditions Service. " << endmsg;                                                     
       }else {                                                                                                                              
          beamSpot = iBeamCondSvc->beamPos();
          int beamSpotBitMap = iBeamCondSvc->beamStatus();                                                                          
//* Check if beam spot is from online algorithms *//                                                                                  
          int beamSpotStatus = ((beamSpotBitMap & 0x4) == 0x4);                                                                        
          if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "  beamSpotBitMap= "<< beamSpotBitMap<<" beamSpotStatus= "<<beamSpotStatus<<endmsg;               
//std::cout<<" BBBBBBB:  beamSpot.x()= "<< beamSpot.x()<<" beamSpot.y()= "<<beamSpot.y()<<std::endl;               
       }                                                                                                                                       
                                                

  m_mon_cutCounter = 0;
  //TrigPassBits *bits = HLT::makeTrigPassBits(trigBphysColl);
  std::unique_ptr<xAOD::TrigPassBits> xBits = xAOD::makeTrigPassBits<xAOD::TrigBphysContainer>(trigBphysColl);

  // now loop over Bphys particles to see if one passes cuts
    for (xAOD::TrigBphysContainer::const_iterator bphysIter = trigBphysColl->begin(); bphysIter !=  trigBphysColl->end(); ++bphysIter) {
        
        if ((*bphysIter)->particleType() == xAOD::TrigBphys::BMUMU ) {
            //    if ((*bphysIter)->particleType() == TrigEFB::BMUMU ) {
            
            double BsMass = (*bphysIter)->mass();
            
            double Dx = (*bphysIter)->fitx() - beamSpot.x();
            double Dy = (*bphysIter)->fity() - beamSpot.y();
            double BsLxy = -99999.;
            
            
            //const ElementLinkVector<Rec::TrackParticleContainer> trackVector = (*bphysIter)->trackVector();
            //const ElementLinkVector<xAOD::TrackParticleContainer> trackVector = (*bphysIter)->trackParticleLinks();
            const std::vector<ElementLink<xAOD::TrackParticleContainer> > trackVector = (*bphysIter)->trackParticleLinks();
            
            if (trackVector.size() != 0) {
                if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " got track vector size: " << trackVector.size() << endmsg;
                std::vector<ElementLink<xAOD::TrackParticleContainer> >::const_iterator trkIt=trackVector.begin();
                double pBx = 0, pBy=0;
                for (int itrk=0 ; trkIt!= trackVector.end(); ++itrk, ++trkIt) {
                    //                    // JW EDM const Trk::MeasuredPerigee* trackPerigee=(*(*trkIt))->measuredPerigee();
                    //                    const Trk::Perigee* trackPerigee=(*(*trkIt))->measuredPerigee();
                    //                    if(msgLvl() <= MSG::VERBOSE) msg() << MSG::VERBOSE << "track, iterator, pointer " << itrk << " " << *trkIt << " " << *(*trkIt) << endmsg;
                    //                    //double phi = trackPerigee->parameters()[Trk::phi];
                    //                    //double theta = trackPerigee->parameters()[Trk::theta];
                    //                    pBx += trackPerigee->momentum()[Trk::px];
                    //                    pBy += trackPerigee->momentum()[Trk::py];
                    //                    if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "track " << itrk << " px=" <<trackPerigee->momentum()[Trk::px]<<
                    //                        " py= "<< trackPerigee->momentum()[Trk::py] << endmsg;
                    if(msgLvl() <= MSG::VERBOSE) msg() << MSG::VERBOSE << "track, iterator, pointer " << itrk << " " << *trkIt << " " << *(*trkIt) << endmsg;
                    pBx += (*(*trkIt))->p4().Px(); // FIXME - is there a more optimal way
                    pBy += (*(*trkIt))->p4().Py();
                    
                    if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "track " << itrk
                        << " px=" << (*(*trkIt))->p4().Px()
                        << " py= "<< (*(*trkIt))->p4().Py() << endmsg;

                } // for loop
                double pBt = sqrt(pBx*pBx + pBy*pBy);
                
                BsLxy = (pBx*Dx+pBy*Dy)/pBt;
            } else {
                if(msgLvl() <= MSG::DEBUG)  msg() << MSG::DEBUG << " no track vector!!! "  << endmsg;
            }
            
            if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "EFBphys with mass: " << BsMass <<" GeV" << "   chi2 " << (*bphysIter)->fitchi2() <<
                " Lxy  "<<BsLxy<<"  lxy= "<<sqrt(Dx*Dx+Dy*Dy)<<endmsg;
            bool thisPassedBsMass = (m_lowerMassCut < BsMass && ((BsMass < m_upperMassCut) || (!m_ApplyupperMassCut) ));
            // PassedBsMass |= thisPassedBsMass;
            bool thisPassedChi2Cut = ((!m_ApplyChi2Cut) || ((*bphysIter)->fitchi2() < m_Chi2VtxCut && (*bphysIter)->fitchi2() >= -1e-10) );
            // PassedChi2Cut |= thisPassedChi2Cut;
            bool thisPassedLxyCut = ((!m_ApplyLxyCut) || ( BsLxy > m_LxyCut) );
            // PassedLxyCut |= thisPassedLxyCut;
            
            if(thisPassedBsMass)
                if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Passed mass cut " << BsMass <<" GeV" << endmsg;
            m_mon_MuMumass = ((BsMass*0.001));
            if(thisPassedChi2Cut)
                if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Apply chi2 cut : " << m_ApplyChi2Cut << " Passed Chi2 cut < "<< m_Chi2VtxCut << endmsg;
            if(thisPassedLxyCut)
                if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Apply Lxy cut : " << m_ApplyLxyCut << " Passed Lxy cut  "<< m_LxyCut << endmsg;
            if(!thisPassedBsMass && !thisPassedChi2Cut)
                if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Did not pass mass & chi2 cuts < "<< endmsg;
            if(!thisPassedLxyCut)
                if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Did not pass Lxy cuts  "<<  BsLxy << " <  Lxy cut "<< m_LxyCut<< endmsg;
            
            if (thisPassedBsMass)  { m_countPassedBsMass++; m_mon_cutCounter++;
                if (thisPassedChi2Cut) { m_countPassedChi2Cut++; m_mon_cutCounter++;
                    if (thisPassedLxyCut)  { m_countPassedLxyCut++; m_mon_cutCounter++; }
                }
            }
              
            if( thisPassedBsMass && thisPassedChi2Cut && thisPassedLxyCut )
            {
                PassedBsMass = true;
                PassedChi2Cut = true;
                PassedLxyCut = true;
                //HLT::markPassing(bits, *bphysIter, trigBphysColl);
                xBits->markPassing((*bphysIter),trigBphysColl,true);
            }
            
        }


    // JK check tracks, for debugging only
    /*
    const ElementLinkVector<Rec::TrackParticleContainer> trackVector = (*bphysIter)->trackVector();
    if (trackVector.size() != 0) {
    if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " got track vector size: " << trackVector.size() << endmsg;
  } else {
    if(msgLvl() <= MSG::DEBUG)  msg() << MSG::DEBUG << " no track vector!!! "  << endmsg;
  }
    ElementLinkVector<Rec::TrackParticleContainer>::const_iterator trkIt=trackVector.begin();
    for (int itrk=0 ; trkIt!= trackVector.end(); ++itrk, ++trkIt) {
    const Trk::MeasuredPerigee* trackPerigee=(*(*trkIt))->measuredPerigee();
    if(msgLvl() <= MSG::VERBOSE) msg() << MSG::VERBOSE << "track, iterator, pointer " << itrk << " " << *trkIt << " " << *(*trkIt) << endmsg;
    double phi = trackPerigee->parameters()[Trk::phi];
    double theta = trackPerigee->parameters()[Trk::theta];
    double px = trackPerigee->momentum()[Trk::px];
    double py = trackPerigee->momentum()[Trk::py];
    double pt = sqrt(px*px + py*py);
    double eta = -std::log(tan(theta/2));

    if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "track " << itrk << " pt phi eta " << pt << " " <<
    phi << " " << eta << endmsg;
  }
    */
    // end JK debug printout
  }



    /*
    if (PassedBsMass)  { m_countPassedBsMass++; m_mon_cutCounter++;
        if (PassedChi2Cut) { m_countPassedChi2Cut++; m_mon_cutCounter++;
            if (PassedLxyCut)  { m_countPassedLxyCut++; m_mon_cutCounter++; }
        }
    }
    */
    
    if ( PassedBsMass && PassedChi2Cut && PassedLxyCut ) {
        result = true;
    }
    
    if (result) {
        m_countPassedRoIs++;
        if (IdEvent!= m_lastEventPassed) {
            m_countPassedEvents++;
            m_lastEventPassed=IdEvent;
        }
        pass=true;
    }
    if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " Run " << IdRun << " Event " << IdEvent << " TrigEFBMuMuHypo result : " << pass << endmsg;
    // store result
    //if ( attachBits(outputTE, bits) != HLT::OK ) {
    //    msg() << MSG::ERROR << "Problem attaching TrigPassBits! " << endmsg;
    //}
    if(attachFeature(outputTE, xBits.release(),"passbits") != HLT::OK)
      ATH_MSG_ERROR("Could not store TrigPassBits! ");


  return HLT::OK;
}
