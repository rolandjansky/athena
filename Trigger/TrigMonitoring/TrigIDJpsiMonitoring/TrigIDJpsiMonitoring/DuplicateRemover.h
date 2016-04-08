// emacs: this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// File name: DuplicateRemover.h
// Description: Method to remove duplicated EF muons from overlapping RoIs
// Author: Peter Wijeratne (paw@hep.ucl.ac.uk)
// Author: Christian Gutschow (chris.g@cern.ch)

#ifndef DUPLICATEREMOVER_H
#define DUPLICATEREMOVER_H

#include "TrigInDetAnalysisUtils/TIDARoiDescriptorBuilder.h"

#include "TrigMuonEvent/TrigMuonEFInfoContainer.h"
#include "TrigMuonEvent/TrigMuonEFInfo.h"
#include "TrigMuonEvent/TrigMuonEFTrack.h"
#include "TrigMuonEvent/TrigMuonEFCbTrack.h"
#include "TrigMuonEvent/TrigMuonEFInfoTrackContainer.h"

#include "Particle/TrackParticle.h"

#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h" 

//tmp
#include "TrigDecisionTool/TrigDecisionTool.h"

class DuplicateRemover {

public:

  DuplicateRemover() { }
  virtual ~DuplicateRemover() { }

  void muonMaker( Trig::FeatureContainer::combination_const_iterator dItr, std::string chain,
                  ToolHandle<Trig::TrigDecisionTool>* tool, unsigned int &trigDefs) {


    //    std::cout << "START REMOVER" << std::endl;
    //    while(dItr != dEnd){
    ///tmp
    //double roiEta0 = 0.;
    //double roiPhi0 = 0.;
    ///tmp

    //cout << "duplicate remover -- get features..." << endl;

    std::vector< Trig::Feature<TrigRoiDescriptor> > initRois = dItr->get<TrigRoiDescriptor>("forID", trigDefs);//TrigDefs::Physics); 

    //cout << "duplicate remover -- get init rois..." << endl;
    if(initRois.empty()) initRois = dItr->get<TrigRoiDescriptor>("", TrigDefs::Physics); 
    if(initRois.empty()) initRois = dItr->get<TrigRoiDescriptor>("initialRoI", TrigDefs::Physics); 

    //    std::vector< Trig::Feature< TrigMuonEFInfoContainer > > muons = dItr->get< TrigMuonEFInfoContainer >("MuonEFInfo", TrigDefs::Physics);
   
    std::string probeTE = "";
    std::string tagTE = "";

    ///default names are for data 12
    if(chain.find("_2mu4T_")!=std::string::npos){
      probeTE = "EF_SA_mu4T_IDTrkNoCut";
      tagTE = "EF_mu4T";
    }
    if(chain.find("_2mu13_")!=std::string::npos){
      probeTE = "EF_SA_mu13_IDTrkNoCut";
      tagTE = "EF_mu13";
    }

    //cout << "duplicate remover -- get tag muons..." << endl;
    std::vector< Trig::Feature< TrigMuonEFInfoContainer > > tagmuons = dItr->get< TrigMuonEFInfoContainer >("MuonEFInfo", trigDefs, tagTE);
    //cout << "duplicate remover -- get probe muons..." << endl;
    std::vector< Trig::Feature< TrigMuonEFInfoContainer > > probemuons = dItr->get< TrigMuonEFInfoContainer >("MuonEFInfo", trigDefs, probeTE);

    //cout << "duplicate remover -- done, try again?" << endl;
    ///if they've got size 0, it's probably because we're running on data or MC 11; if just 0 probes, probably MC12
    if( tagmuons.size() == 0 ){
      if(chain.find("_2mu4T_")!=std::string::npos) tagTE = "EF_MS_mu4T";
      if(chain.find("_2mu13_")!=std::string::npos) tagTE = "EF_MS_mu13";
      tagmuons = dItr->get< TrigMuonEFInfoContainer >("MuonEFInfo", TrigDefs::Physics, tagTE);
    }      
    if( probemuons.size() == 0 ){
      if(chain.find("_2mu4T_")!=std::string::npos) probeTE = "EF_MS_mu4T_IDTrkNoCut";
      if(chain.find("_2mu13_")!=std::string::npos) probeTE = "EF_MS_mu13_IDTrkNoCut";
      probemuons = dItr->get< TrigMuonEFInfoContainer >("MuonEFInfo", TrigDefs::Physics, probeTE);
    }
    
    /*std::cout << "\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
      std::cout << "!!!!!!!!! tagmuons.size = " << tagmuons.size() << std::endl;
      std::cout << "!!!!!!!!! probemuons.size = " << probemuons.size() << std::endl;
      std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;*/
    
    
    /*  std::cout << "\n??????????????????????????????????????????????????" << std::endl;
	std::cout << "????????? muons.size = " << muons.size() << std::endl;
	std::cout << "????????????????????????????????????????????????????" << std::endl;*/
    
    double PI = 3.14159265;

    //////
    /////// ADD BREAK OUT OF LOOP IF roiIt2 == muons.size()
    /////

    int probeRoiID = 0;
    int tagRoiID = 0;

    std::vector<const TrigRoiDescriptor*> rois;

    for( unsigned roiIt1 = 0; roiIt1 < probemuons.size(); roiIt1++ ){

      Trig::Feature< TrigMuonEFInfoContainer > trackFeature1 = probemuons.at(roiIt1);
      const TrigMuonEFInfoContainer* trigMuon1 = trackFeature1.cptr();
      TrigMuonEFInfoContainer::const_iterator muonItr1  = trigMuon1->begin();
      if (muonItr1==trigMuon1->end()) continue;
      const TrigMuonEFInfo* muoninfo1 = (*muonItr1);

      ///ROI associated with this Feature
      Trig::Feature<TrigRoiDescriptor> trackroi1 = (*tool)->ancestor<TrigRoiDescriptor>(trackFeature1, "forID");
      const TrigRoiDescriptor* roid1 = trackroi1.cptr();
      TIDARoiDescriptor roiInfo1 = TIDARoiDescriptorBuilder(*roid1);
      /*      std::cout << "\nROI probe eta0 = " << roiInfo1.eta() << " phi0 = " << roiInfo1.phi() << " zed0 = " << roiInfo1.zed()
	      << "\neta halfwidth = " << roiInfo1.etaHalfWidth() << " phi halfwidth = " << roiInfo1.phiHalfWidth() 
	      << " Z halfwidth = " << roiInfo1.zedHalfWidth() << " roiId = " << roid1->roiId() << "\n"
	      << std::endl;*/
      probeRoiID = roid1->roiId();

      rois.push_back(roid1);

      if( !initRois.empty() ){

        //	const TrigRoiDescriptor* roid1 = initRois[roiIt1].cptr();
        //	TIDARoiDescriptor roiInfo1 = TIDARoiDescriptorBuilder(*roid1);

        //	std::cout << "\nROI probe eta = " << roiInfo1.eta() << " phi = " << roiInfo1.phi() 
        //		  << std::endl;

        if( muoninfo1->hasTrack() ){
	  
	  const TrigMuonEFInfoTrackContainer* tc1 = muoninfo1->TrackContainer();

	  bool selected = false;

	  for( TrigMuonEFInfoTrackContainer::const_iterator trackItr = tc1->begin(); trackItr != tc1->end(); trackItr++){

	    const TrigMuonEFInfoTrack* muonInfo1 = (*trackItr);

	    if( muonInfo1->hasExtrapolatedTrack() ){

	      TrigMuonEFTrack* eTrack1 = muonInfo1->ExtrapolatedTrack();

	      ///tmp
	      //			Trig::Feature<TrigRoiDescriptor> trackroi1 = (*tool)->ancestor<TrigRoiDescriptor>(trackFeature1);
	      //			const TrigRoiDescriptor* roid1 = trackroi1.cptr();
	      //			std::cout << "\n!!!Probe!!! probe1 RoI eta = " << roid1->eta() << " phi = " << roid1->phi() 
	      //				  << " probe pt = " << eTrack1->pt() << std::endl;
	      ///tmp


	      //	      std::cout << "RoI " << muoninfo1->RoINum() << " extrapolated track " << *eTrack1 << std::endl;

	      for( unsigned roiIt2 = 0; roiIt2 < probemuons.size(); roiIt2++ ){
		
		if( roiIt1 != roiIt2 ){

		  Trig::Feature< TrigMuonEFInfoContainer > trackFeature2 = probemuons.at(roiIt2);
		  const TrigMuonEFInfoContainer* trigMuon2 = trackFeature2.cptr();
		  TrigMuonEFInfoContainer::const_iterator muonItr2  = trigMuon2->begin();
		  const TrigMuonEFInfo* muoninfo2 = (*muonItr2);

		  Trig::Feature<TrigRoiDescriptor> trackroi2 = (*tool)->ancestor<TrigRoiDescriptor>(trackFeature2);
		  const TrigRoiDescriptor* roid2 = trackroi2.cptr();
		  TIDARoiDescriptor roiInfo2 = TIDARoiDescriptorBuilder(*roid2);

		  if( !initRois.empty() ){

		    /*		  const TrigRoiDescriptor* roid2 = initRois[roiIt2].cptr();
				  TIDARoiDescriptor roiInfo2 = TIDARoiDescriptorBuilder(*roid2);*/

		    if( muoninfo2->hasTrack() ){
		    
		      const TrigMuonEFInfoTrackContainer* tc2 = muoninfo2->TrackContainer();
	  
		      for( TrigMuonEFInfoTrackContainer::const_iterator trackItr = tc2->begin(); trackItr != tc2->end(); trackItr++){
	  
			const TrigMuonEFInfoTrack* muonInfo2 = (*trackItr);
	  
			if( muonInfo2->hasExtrapolatedTrack() ){

			  TrigMuonEFTrack* eTrack2 = muonInfo2->ExtrapolatedTrack();

			  ///tmp
			  //			Trig::Feature<TrigRoiDescriptor> trackroi2 = (*tool)->ancestor<TrigRoiDescriptor>(trackFeature2);
			  //			const TrigRoiDescriptor* roid2 = trackroi2.cptr();
			  //			std::cout << "\n!!!Probe!!! probe2 RoI eta = " << roid2->eta() << " phi = " << roid2->phi0() 
			  //				  << " probe pt = " << eTrack2->pt() << std::endl;
			  ///tmp

			  //	std::cout << "RoI " << muoninfo2->RoINum() << " extrapolated track " << *eTrack2 << std::endl;

			  double deta = (eTrack1->eta() - eTrack2->eta());
			  double dphi = (eTrack1->phi() - eTrack2->phi());
			  if ( dphi > PI ) dphi -= 2*PI;
			  if ( dphi < -PI ) dphi += 2*PI;

			  ///tmp
			  //roiEta0 = roiInfo1.eta();
			  //roiPhi0 = roiInfo1.phi();
			  ///tmp

			  if( muoninfo1->RoINum() != muoninfo2->RoINum() && ((std::fabs(deta) < 0.08) && (std::fabs(dphi) < 0.08)) ){
			    /// probably the same track

			    double dphi1 = eTrack1->phi() - roiInfo1.phi();
			    double dphi2 = eTrack2->phi() - roiInfo2.phi();
			    if ( dphi1 > PI ) dphi1 -= 2*PI;
			    if ( dphi1 < -PI ) dphi1 += 2*PI;
			    if ( dphi2 > PI ) dphi2 -= 2*PI;
			    if ( dphi2 < -PI ) dphi2 += 2*PI;
			    /// track angles with respect to their RoIs
			    double deltarT1Roi1 = std::fabs( pow(eTrack1->eta() - roiInfo1.eta(),2) + pow(dphi1,2) );
			    double deltarT2Roi2 = std::fabs( pow(eTrack2->eta() - roiInfo2.eta(),2) + pow(dphi2,2) );

			    /// pick the roi that is better aligned with this track
			    if( deltarT1Roi1 > deltarT2Roi2 ){
			      eTrack1 = eTrack2;
			      ///tmp
			      ///tmp
			      //roiEta0 = roiInfo2.eta();
			      //roiPhi0 = roiInfo2.phi();
			      ///tmp
			      /// want to keep this information to check if probe was matched to an ID
			      muonInfo1 = muonInfo2;
			      roiInfo1 = roiInfo2;
			      //			    std::cout << "Switched!" << std::endl;
			    }
			  
			  }//check duplcates - pick 1 closest to centre of its RoI
			  
			}
		      } // loop over tracks from RoI 2
		    } // muoninfo2->hasTrack()
		  } // if ! initRois.empty()

		  //cout << "muonInfo1: " << muonInfo1 << endl;
		  //cout << "roiInfo1: " << roiInfo1 << endl;

		}//roi1!=roi2
		
	      }//start 2nd RoI loop
	      	      
	      for( std::vector<const TrigMuonEFTrack*>::const_iterator setIt = m_muonExtrapolatedTracks.begin(); setIt != m_muonExtrapolatedTracks.end(); ++setIt){
		
		double deta = ((*setIt)->eta() - eTrack1->eta());
		double dphi = ((*setIt)->phi() - eTrack1->phi());
		if ( dphi > PI ) dphi -= 2*PI;
		if ( dphi < -PI ) dphi += 2*PI;
		
		if( (std::fabs(deta) < 0.08) && (std::fabs(dphi) < 0.08) ){
		  selected = true;
		}
	      }
	      
	      if( !selected ){
		m_muonExtrapolatedTracks.push_back(eTrack1);
		m_probeRoIs.push_back(probeRoiID);
		if(muonInfo1->hasCombinedTrack()){
		  //		  std::cout << "This should happen most of the time... " << (muonInfo1->CombinedTrack())->getIDTrackParticle() << std::endl;
		  m_matchedProbes.push_back((muonInfo1->CombinedTrack())->getIDTrackParticle());
		  //tmp
		}
		else m_matchedProbes.push_back(NULL);
		// std::cout << "\nExtrapolatedTrack1 inserted." << *eTrack1 << "\n" << std::endl;
	      }
	      
	    }//extrapolated track?
	  }
	}
      }
    }

    //int iroi = 0;
    for( unsigned roiIt1 = 0; roiIt1 < tagmuons.size(); roiIt1++ ){
      Trig::Feature< TrigMuonEFInfoContainer > trackFeature1 = tagmuons.at(roiIt1);
      Trig::Feature<TrigRoiDescriptor> trackroi1 = (*tool)->ancestor<TrigRoiDescriptor>(trackFeature1, "");
      const TrigRoiDescriptor* roid1 = trackroi1.cptr();
      rois.push_back(roid1);
      //cout <<iroi++ << "rois: " << rois.back() << endl;
    }
    
    for( unsigned roiIt1 = 0; roiIt1 < tagmuons.size(); roiIt1++ ){

      ///tmp
      //double roiEta = 0.;
      //double roiPhi = 0.;
      ///tmp
      
      Trig::Feature< TrigMuonEFInfoContainer > trackFeature1 = tagmuons.at(roiIt1);
      const TrigMuonEFInfoContainer* trigMuon1 = trackFeature1.cptr();
      TrigMuonEFInfoContainer::const_iterator muonItr1  = trigMuon1->begin();
      const TrigMuonEFInfo* muoninfo1 = (*muonItr1);

      ///ROI associated with this Feature
      Trig::Feature<TrigRoiDescriptor> trackroi1 = (*tool)->ancestor<TrigRoiDescriptor>(trackFeature1, "");
      const TrigRoiDescriptor* roid1 = trackroi1.cptr();
      TIDARoiDescriptor roiInfo1 = TIDARoiDescriptorBuilder(*roid1);
      /*                  std::cout << "\nROI tag eta0 = " << roiInfo1.eta() << " phi0 = " << roiInfo1.phi() << " zed0 = " << roiInfo1.zed()
			  << "\neta halfwidth = " << roiInfo1.etaHalfWidth() << " phi halfwidth = " << roiInfo1.phiHalfWidth() 
			  << " Z halfwidth = " << roiInfo1.zedHalfWidth() << " roiId = " << roid1->roiId() << "\n"
			  << std::endl;*/

      if( !initRois.empty() ){
	
	/*	const TrigRoiDescriptor* roid1 = initRois[roiIt1].cptr();
		TIDARoiDescriptor roiInfo1 = TIDARoiDescriptorBuilder(*roid1);
		roiID = roid1->roiId();
		std::cout << "\nROI tag eta = " << roiInfo1.eta() << " phi = " << roiInfo1.phi0() 
		<< std::endl;
		//	std::cout << "\\\\roid1 = " << roid1->roiId() << std::endl;
		*/
	if( muoninfo1->hasTrack() ){
	  
	  const TrigMuonEFInfoTrackContainer* tc1 = muoninfo1->TrackContainer();

	  bool selected = false;

	  for( TrigMuonEFInfoTrackContainer::const_iterator trackItr = tc1->begin(); trackItr != tc1->end(); trackItr++){

	    const TrigMuonEFInfoTrack* muonInfo1 = (*trackItr);
	  
	    if( muonInfo1->hasCombinedTrack() ){

	      tagRoiID = roid1->roiId();

	      TrigMuonEFCbTrack* eTrack1 = muonInfo1->CombinedTrack();

	      // check if it actually fits the RoI
	      if(tc1->size() > 1){
		int index = 0;
		double deltar = 0.;
		double temp = 9999.;
		for(unsigned i=0; i<rois.size(); i++){
		  const Rec::TrackParticle* idComp = eTrack1->getIDTrackParticle();
		  //double roiEtaHalfWidth = rois[i]->etaHalfWidth();
		  //double roiPhiHalfWidth = rois[i]->phiHalfWidth();
		  double roiEta = rois[i]->eta();
		  double roiPhi = rois[i]->phi();
		  double deltaPhi1 = 0.;
		  if(idComp) deltaPhi1 = idComp->phi() - roiPhi;
		  else deltaPhi1 = eTrack1->phi() - roiPhi;
		  if ( deltaPhi1 > PI ) deltaPhi1 -= 2*PI;
		  if ( deltaPhi1 < -PI ) deltaPhi1 += 2*PI;
		  if(idComp) deltar = std::sqrt( std::pow(deltaPhi1,2) + std::pow(idComp->eta() - roiEta, 2) );
		  else deltar = std::sqrt( std::pow(deltaPhi1,2) + std::pow(eTrack1->eta() - roiEta, 2) );
		  //	      std::cout << "\ndeltaPhi = " << deltaPhi1 << " deta = " << idComp->eta() - roiEta << std::endl;
		  if(deltar < temp){ 
		    index = i;
		    temp = deltar;
		  }
		  
		  /*	      if(std::fabs(idComp->eta() - roiEta) > roiEtaHalfWidth || std::fabs(deltaPhi1) > roiPhiHalfWidth){
			      std::cout << "Switching tag RoI ID from " << tagRoiID << " to " << probeRoiID << " because dEta = " << std::fabs(idComp->eta() - roiEta)
			      << " dPhi = " << std::fabs(deltaPhi1) << " and roiEtaHalfWidth = " << roiEtaHalfWidth << " roiPhiHalfWidth = "
			      << roiPhiHalfWidth
			      << "\ntag with eta = " << idComp->eta() << " phi = " << idComp->phi() 
			      << "\nancestor roiId = " << tagRoiID << " trigmuon EDM roiNum = " << muoninfo1->RoINum() 
			      << std::endl;
			      tagRoiID = probeRoiID;
			      }*/
		}
		//		std::cout << "Chose index " << index << " with deltar " << temp << " rois.size " << rois.size() << std::endl;
		tagRoiID = rois[index]->roiId();
	      }
	      ///tmp
	      //			Trig::Feature<TrigRoiDescriptor> trackroi1 = (*tool)->ancestor<TrigRoiDescriptor>(trackFeature1);
	      //			const TrigRoiDescriptor* roid1 = trackroi1.cptr();
	      //			std::cout << "\n!!!Tag!!! tag1 RoI eta = " << roid1->eta() << " phi = " << roid1->phi0() 
	      //				  << " tag pt = " << eTrack1->pt() << std::endl;
	      ///tmp
	      
	      //	      std::cout << "RoI " << muoninfo1->RoINum() << " combined track " << *eTrack1 << std::endl;

	      for( unsigned roiIt2 = roiIt1+1; roiIt2 < tagmuons.size(); roiIt2++ ){

		Trig::Feature< TrigMuonEFInfoContainer > trackFeature2 = tagmuons.at(roiIt2);
		const TrigMuonEFInfoContainer* trigMuon2 = trackFeature2.cptr();
		TrigMuonEFInfoContainer::const_iterator muonItr2  = trigMuon2->begin();
		const TrigMuonEFInfo* muoninfo2 = (*muonItr2);

		Trig::Feature<TrigRoiDescriptor> trackroi2 = (*tool)->ancestor<TrigRoiDescriptor>(trackFeature2);
		const TrigRoiDescriptor* roid2 = trackroi2.cptr();
		TIDARoiDescriptor roiInfo2 = TIDARoiDescriptorBuilder(*roid2);

		if( !initRois.empty() ){

		  /*		  const TrigRoiDescriptor* roid2 = initRois[roiIt2].cptr();
				  TIDARoiDescriptor roiInfo2 = TIDARoiDescriptorBuilder(*roid2);*/

		  if( muoninfo2->hasTrack() ){

		    const TrigMuonEFInfoTrackContainer* tc2 = muoninfo2->TrackContainer();
	  
		    for( TrigMuonEFInfoTrackContainer::const_iterator trackItr = tc2->begin(); trackItr != tc2->end(); trackItr++){
	  
		      const TrigMuonEFInfoTrack* muonInfo2 = (*trackItr);
	  
		      if( muonInfo2->hasCombinedTrack() ){

			TrigMuonEFCbTrack* eTrack2 = muonInfo2->CombinedTrack();

			///tmp
			//			Trig::Feature<TrigRoiDescriptor> trackroi2 = (*tool)->ancestor<TrigRoiDescriptor>(trackFeature2);
			//			const TrigRoiDescriptor* roid2 = trackroi2.cptr();
			//			std::cout << "!!!Tag!!! tag2 RoI eta = " << roid2->eta() << " phi = " << roid2->phi0() 
			//				  << " tag pt = " << eTrack2->pt() << std::endl;
			///tmp

			//	std::cout << "RoI " << muoninfo2->RoINum() << " combined track " << *eTrack2 << std::endl;

			double deta = (eTrack1->eta() - eTrack2->eta());
			double dphi = (eTrack1->phi() - eTrack2->phi());
			if ( dphi > PI ) dphi -= 2*PI;
			if ( dphi < -PI ) dphi += 2*PI;

			//tmp
			//roiEta = roiInfo1.eta();
			//roiPhi = roiInfo1.phi();
			//tmp
			
			if( muoninfo1->RoINum() != muoninfo2->RoINum() && ((std::fabs(deta) < 0.08) && (std::fabs(dphi) < 0.08)) ){
			  
			  double dphi1 = eTrack1->phi() - roiInfo1.phi();
			  double dphi2 = eTrack2->phi() - roiInfo2.phi();
			  if ( dphi1 > PI ) dphi1 -= 2*PI;
			  if ( dphi1 < -PI ) dphi1 += 2*PI;
			  if ( dphi2 > PI ) dphi2 -= 2*PI;
			  if ( dphi2 < -PI ) dphi2 += 2*PI;
			  double deltarT1Roi1 = std::fabs( pow(eTrack1->eta() - roiInfo1.eta(),2) + pow(dphi1,2) );
			  double deltarT2Roi2 = std::fabs( pow(eTrack2->eta() - roiInfo2.eta(),2) + pow(dphi2,2) );
			  
			  if( deltarT1Roi1 > deltarT2Roi2 ){
			    eTrack1 = eTrack2;
			    //			    tagRoiID = roid2->roiId();
			    //			    std::cout << "\\\\roid2 = " << roid2->roiId() << std::endl;
			    //			    std::cout << "Switched!" << std::endl;
			    //tmp
			    //roiEta = roiInfo2.eta();
			    //roiPhi = roiInfo2.phi();
			    //tmp
			  }
			  roiInfo1 = roiInfo2;			  
			}//check duplicates - pick 1 closest to centre of its RoI
			
		      }
		    }
		  }
		}
	      }//start 2nd RoI loop

	      for( std::vector<const TrigMuonEFCbTrack*>::const_iterator setIt = m_muonCombinedTracks.begin(); setIt != m_muonCombinedTracks.end(); ++setIt){
		
		double deta = ((*setIt)->eta() - eTrack1->eta());
		double dphi = ((*setIt)->phi() - eTrack1->phi());
		if ( dphi > PI ) dphi -= 2*PI;
		if ( dphi < -PI ) dphi += 2*PI;
		
		if( (std::fabs(deta) < 0.08) && (std::fabs(dphi) < 0.08) ){
		  selected = true;
		}
	      }
	      
	      if( !selected ){
		m_muonCombinedTracks.push_back(eTrack1); 
		m_tagRoiInfo.push_back(roiInfo1);
		//		std::cout << "DuplicateRemover finds tag with roiNum = " << tagRoiID << " eta = " << eTrack1->eta() << " phi = " << eTrack1->phi() << std::endl;
		m_tagRoIs.push_back(tagRoiID);
		//std::cout << "\nCombinedTrack1 inserted." << *eTrack1 << "\n" << std::endl;
	      }
	      
	    }//combined track?	    
	    
	  }//start track container loop
	}//if has roi
      }//if has track
    }//start 1st RoI loop 

    //    dItr++;
    //    }
    
  }
  
  virtual void clear() {
    m_muonExtrapolatedTracks.clear(); m_muonCombinedTracks.clear(); 
    m_matchedProbes.clear();          m_tagRoIs.clear(); 
    m_probeRoIs.clear();              m_tagRoiInfo.clear();
  }
  
  std::vector< const TrigMuonEFTrack* > getExTrks() const { return m_muonExtrapolatedTracks; }
  std::vector< const TrigMuonEFCbTrack* > getCbTrks() const { return m_muonCombinedTracks; }

  std::vector< const Rec::TrackParticle* > getMatches() { return m_matchedProbes; };

  ///temp, store tag muon RoI number
  std::vector<int> getTagRoIs() { return m_tagRoIs; }
  std::vector<int> getProbeRoIs() { return m_probeRoIs; }
  std::vector<TIDARoiDescriptor> getRoiInfo() { return m_tagRoiInfo; }
  
protected:
  
  std::vector< const TrigMuonEFTrack* > m_muonExtrapolatedTracks;
  std::vector< const TrigMuonEFCbTrack* > m_muonCombinedTracks;

  std::vector< const Rec::TrackParticle* > m_matchedProbes;
  
  ///temp
  std::vector<int> m_tagRoIs;
  std::vector<int> m_probeRoIs;
  std::vector<TIDARoiDescriptor> m_tagRoiInfo;
  
};

#endif
