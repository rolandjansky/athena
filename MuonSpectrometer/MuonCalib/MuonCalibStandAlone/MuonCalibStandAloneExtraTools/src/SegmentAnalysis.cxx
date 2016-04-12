/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonCalib_SegmentAnalysisCXX
#define MuonCalib_SegmentAnalysisCXX

#define PI 3.14159265


#include "MuonCalibStandAloneExtraTools/SegmentAnalysis.h"
#include "TH1.h"

//
// MuonCalib Fitter
#include "MdtCalibFitters/DCSLFitter.h"
#include "MuonCalibStandAloneExtraTools/PhiEtaUtils.h"
#include "MuonCalibStandAloneExtraTools/StringUtil.h"
#include "MuonCalibStandAloneExtraTools/MDTName.h"

using namespace std;

namespace MuonCalib{

SegmentAnalysis::SegmentAnalysis(RegionSelectionSvc *punt, HistogramManager *histoman, bool verbose, bool dohitresids) {
  m_histoManager=histoman;
  p_reg_sel_svc=punt;
  m_verbose=verbose;
  m_doHitResids=dohitresids;

  PhiEtaNameConverter phiEtaConverter;
  string testName="BIL1A01" ;
  MDTName NameConverter(testName) ;
  m_SectorMin = 20 ;
  m_SectorMax = 0 ;
  const std::vector<MuonCalib::NtupleStationId> stationsInRegion = p_reg_sel_svc->GetStationsInRegions() ;
  std::vector<MuonCalib::NtupleStationId>::const_iterator itstation ;
  for (itstation = stationsInRegion.begin() ; itstation!=stationsInRegion.end(); itstation++) {
    int stationNameId = itstation->GetStation() ;
    int phi = itstation->GetPhi() ;
    int sector = phiEtaConverter.phi_8to16(stationNameId,phi);
    if(sector>m_SectorMax) m_SectorMax = sector ;
    if(sector<m_SectorMin) m_SectorMin = sector ;
  }
}  //end SegmentAnalysis::SegmentAnalysis

void SegmentAnalysis::handleEvent(const MuonCalibEvent &event, int /*evnt_nr*/, const std::vector<MuonCalibSegment *> &segments, unsigned int position) {

  //float ADCCUT=70. ;
  float CHI2CUT=50. ;

  //Event number
  int evtNumber = event.eventInfo().eventNumber() ;

  ToString ts;
  PhiEtaNameConverter phiEtaConverter;

  if( !p_reg_sel_svc){
    //    std::cout << "Region Selection Service NOT INITIALIZED " << std::endl;
    return;
  }

  string histoType="seg";
  TH1F *h1seg = (TH1F*) m_histoManager->GetHisto("GLOBAL",histoType);
  float NumOfSeg = (float) segments.size() - position ;
  if (h1seg) h1seg->Fill( NumOfSeg );

  //===========================================================================

  // ==== SEGMENT LOOP =========================================================
  //Looping over all segments in the event
  //
  //  if(m_verbose) std::cout<<"MySegmentLoop Start"<<std::endl;
  TH1F * hseg;
  TH2F * h2;

  for (unsigned int k=position; k<segments.size(); k++) {    // LOOP OVER SEGMENTS 
    MuonCalibSegment *segment(segments[k]);

/*
  unsigned int segmentIndex = 0;
  unsigned int patternIndex = 0;
    
  MuonCalibEvent::MCPVecCit pat_it ;
  MuonCalibEvent::MCPVecCit pat_it_start = event.patternBegin();
  MuonCalibEvent::MCPVecCit pat_it_end = event.patternEnd(); 
*/

//  for(pat_it = pat_it_start ; pat_it != pat_it_end; ++pat_it){
//    patternIndex++;
//    segmentIndex = 0;
//    MuonCalibPattern::MuonSegIt seg_it ;
//    MuonCalibPattern::MuonSegIt seg_it_start = (*pat_it)->muonSegBegin();
//    MuonCalibPattern::MuonSegIt seg_it_end   = (*pat_it)->muonSegEnd();
//
//    for (seg_it = seg_it_start ; seg_it != seg_it_end; ++seg_it){
//      if ((*seg_it)->author()!=SEGMENTAUTHOR) continue ;
//      segmentIndex++;

      // int MinNumOfHitsPerSegment = 0 ;
    unsigned int MinNumOfHitsPerSegment = 4 ;

    if ( segment->mdtHitsOnTrack()==0) {
      //      if(m_verbose) std::cout<<" Hit on seg: 0 "<<std::endl;
      continue;
    }

    if ( segment->mdtHitsOnTrack()<MinNumOfHitsPerSegment) {
      //      if(m_verbose) std::cout<<" Hits on segment : "<< segment->mdtHitsOnTrack()
      //			     <<" less than minimum: " << MinNumOfHitsPerSegment<< std::endl;
      continue;
    }

    //    if (m_verbose) std::cout<<" MySegments "<<std::endl;
    std::vector<MdtCalibHitBase*>::const_iterator hit_it ;
    std::vector<MdtCalibHitBase*>::const_iterator hit_it_start = segment->mdtHOTBegin();
    std::vector<MdtCalibHitBase*>::const_iterator hit_it_end = segment->mdtHOTEnd();   

    bool segmentIsInRegion = false ;
    for (hit_it=hit_it_start; hit_it != hit_it_end; ++hit_it) {
      if (p_reg_sel_svc->isInRegion((*hit_it)->identify())) segmentIsInRegion = true ;
    }
    if (!segmentIsInRegion) {
      //      if(m_verbose) std::cout<<" The Segment has no hits in the selected region "<<std::endl;
      continue;
    }

    //std::vector<RpcCalibHitBase*>::const_iterator rpc_hit_it = (*seg_it)->rpcHOTBegin();
    //std::vector<RpcCalibHitBase*>::const_iterator rpc_hit_it_end = (*seg_it)->rpcHOTEnd();  
    //std::cout<<" rpc hit "<<(*seg_it)->rpcHOTBegin()<<" "<<(*seg_it)->rpcHOTEnd()<<std::endl;


    //Example for a refit with variable t0
    // 	T0Fitter t0fit;
    // 	double Chi2_before = (*seg_it)->chi2() ;
    // 	TimedSegment* updatedSeg = t0fit.fit( (*seg_it) );
    // 	std::cout << "Fitting with variable t0 improves chi2 from " << Chi2_before << " to " 
    // 		  << updatedSeg->segment()->chi2() << std::endl;
    
    //Filling histograms.
    //plotATLAS( (*seg_it) );

    //=========
    int crossing = multiChambSeg(segment);
    if (crossing) {
      //      if(m_verbose) std::cout<<"Multi Chamber seg"<<std::endl;
      continue;
    }

    int segStation = segmentStation(segment);
    int segEta =  segmentEta(segment);
    int segPhi =  segmentPhi(segment);
    //    if(m_verbose) std::cout<<"-- Station Eta Phi: "<<segStation<<" "<<segEta<<" "<<segPhi<<std::endl;

    // int segPhi2=phi_8to16(segStation,segPhi);
    int segPhi2=phiEtaConverter.phi_8to16(segStation,segPhi);
       
    bool doSector=false;
    if(segPhi2>=m_SectorMin && segPhi2<=m_SectorMax) doSector=true;
    if(!doSector) continue;        

    // if(segStation > 6) continue;  // Commented on SEPTEMBER 11 2008

    //== build chamber name

    MuonFixedId  id;
    string segstn = id.stationNumberToFixedStationString(segStation);
    MDTName chambseg(segstn,segPhi,segEta);    

    string chamberName = chambseg.getOnlineName();  
    string side=chambseg.getSide();
    string region=chambseg.getRegion();

    // Fill HitsOnSegments and Prob(chi2,ndeg) 
    //
    float hitsOnSeg = (float) segment->mdtHitsOnTrack() ;
    float SegChi2DoF = segment->chi2() ; // this returns the reduced chi2. 
                                           // Hopefully the correct nDoF is considered when t0-refit is active!
    float nSegParams = 2. ;
    if (segment->hasFittedT0()) nSegParams = 3. ;
    float DoF = hitsOnSeg - nSegParams ;
    float SegChi2 = SegChi2DoF*DoF ;

    histoType="HitsOnSegment"+segstn;
    //    if(m_verbose) std::cout<<" histoType "<<histoType<<" "<<side<<" "<<segPhi2<<std::endl;
    hseg = (TH1F*) m_histoManager->GetMdtHisto(histoType,region,side,segPhi2);
    //  if(m_verbose) std::cout<<"hitsOnSeg "<<hitsOnSeg<<" "<<hseg<<std::endl;
    if(hseg) hseg->Fill(hitsOnSeg);

    histoType="D_ChamberHitsOnSegment";
    //  if(m_verbose) std::cout<<" histoType "<<histoType<<std::endl;
    hseg = (TH1F*) m_histoManager->GetMdtHisto(histoType,chambseg);
    if(hseg) hseg->Fill(hitsOnSeg);

    histoType="c_Segment_ProbChi2";
    hseg = (TH1F*) m_histoManager->GetMdtHisto(histoType,chambseg);
    if (hseg) hseg->Fill(TMath::Prob(SegChi2,(int)DoF));
    //cout << " DEBUG DEBUG hitsOnSeg nSegParams SegChi2DoF DoF SegChi2"
    //     << hitsOnSeg<<" "<< nSegParams<<" "<< SegChi2DoF<<" "<< DoF<<" "<< SegChi2 << endl ;
    // 
    // HIT and FIT Residuals (HIT: hit included in segment ; FIT: hit excluded )
    //
    // NIR. REPLACED BY HITRESID vs Radius
    ///////////////////// SNIP BEGIN
    
    float hitsOnSeg2 = 0;

    histoType="b_SegResidVsRadius";
    h2 = (TH2F*) m_histoManager->GetMdtHisto(histoType,chambseg);
    histoType="b_SegmentResidual";
    hseg = (TH1F*) m_histoManager->GetMdtHisto(histoType,chambseg);

    MuonCalib::DCSLFitter dcslFitter;
    IMdtSegmentFitter::HitSelection ref_hitsel(segment->mdtHitsOnTrack(), 0); // Reference hit selection, containing 0 for all hits in segment
    for (std::vector<MdtCalibHitBase*>::iterator hit_it =segment->mdtHOTBegin() ;
	 hit_it!=segment->mdtHOTEnd(); ++hit_it) { // Loop on all hits on tracks

      hitsOnSeg2++;
      //MuonFixedId id =  (*hit_it)->identify();

      float resid         = 0.0; // residual
      float radius        = 0.0; // hit drift radius
      //        float dToTrack      = 0.0; // distance to track
      //        float orig_resid    = 0.0; // original residual
      //        float orig_dToTrack = 0.0; // original distance to track
      //        float orig_radius   = 0.0; // original drift radius        
      //        float chi2DoF       = 0.0; // segment chi2/dof
      if (m_doHitResids) { // Do hit residuals vs radius          
	unsigned int                    iHit   = hit_it-segment->mdtHOTBegin();
	IMdtSegmentFitter::HitSelection hitsel = ref_hitsel;
	// Ignore hit iHit;
	hitsel[iHit]                           = 1;
	// Clone segment:
	MuonCalibSegment segmentClone(*segment);
	dcslFitter.fit(segmentClone, hitsel);
	
	//Get the residual of the ignored hit:
	resid         = segmentClone.mdtHOT()[iHit]->radialResidual();
	radius        = segmentClone.mdtHOT()[iHit]->driftRadius();
	//          dToTrack      = segmentClone.mdtHOT()[iHit]->signedDistanceToTrack();
	//          chi2DoF       = segmentClone.chi2();
	//          orig_resid    = segment->mdtHOT()[iHit]->radialResidual();
	//          orig_dToTrack = segment->mdtHOT()[iHit]->signedDistanceToTrack();
	//          orig_radius   = segment->mdtHOT()[iHit]->driftRadius();
	
	// printf("orig_dToTrack = %6.2f dToTrack = %6.2f orig_driftR = %6.2f driftR = %6.2f orig_resid = %6.2f resid = %6.  2f   orig_chi2 = %6.2f new_chi2 = %6.2f   n = %d\n",
	// orig_dToTrack, dToTrack, orig_driftR, driftR, orig_resid, resid, segment.chi2(), segmentClone.chi2(), (int) segment.mdtHitsOnTrack());
	
      } else { // Do fit residuals vs radius
	resid  = (*hit_it)->radialResidual();
	radius = (*hit_it)->driftRadius();
      }
      if (radius<0) radius = -radius ;  // First take the absolute value of radius
      if ((*hit_it)->signedDistanceToTrack()<0.) radius=-radius ; // Then assign the sign of signedDistanceToTrack
      
      if (h2) h2->Fill(radius,resid);
      if (hseg) hseg->Fill(resid);
      
      // if (m_verbose){
      // 	std::cout << " - " << (*hit_it)->identify() << " " << (*hit_it)->tdcCount()
      // 		  << " " << (*hit_it)->adcCount() << " " << (*hit_it)->driftTime()<< std::endl;
      // }
    } // End loop over hits       
      ///////////////////// SNIP END
    
      /* 
	 histoType="HitsOnSegment"+segstn;
      if(m_verbose) std::cout<<" histoType "<<histoType<<" "<<side<<" "<<segPhi2<<std::endl;
      hseg = (TH1F*) m_histoManager->GetMdtHisto(histoType,region,side,segPhi2);
      if(m_verbose) std::cout<<"hitsOnSeg2 "<<hitsOnSeg2<<" "<<hseg<<std::endl;
      if(hseg) hseg->Fill(hitsOnSeg2);

      histoType="D_ChamberHitsOnSegment";
      if(m_verbose) std::cout<<" histoType "<<histoType<<std::endl;
      hseg = (TH1F*) m_histoManager->GetMdtHisto(histoType,chambseg);
      if(hseg) hseg->Fill(hitsOnSeg2);
      */ 


      //====================================================================================
      //====================================================================================
      //    GLOBAL segment_Phi Vs segment_Eta  plot 
      // 
    float gdirX = segment->globalDirection().x();
    float gdirY = segment->globalDirection().y();
    float gdirZ = segment->globalDirection().z();
    float globalPhi = atan2(gdirY,gdirX)* 180./PI; ;
    float globalTheta = acos(gdirZ) ;
    float globalEta = -log(tan(globalTheta/2.)) ;
    histoType="phi_vs_eta";
    //    if(m_verbose) std::cout<<" histoType "<<histoType<<std::endl;

    h2 = (TH2F*) m_histoManager->GetHisto("GLOBAL",histoType);
    if(h2) h2->Fill( globalEta, globalPhi );	

      //====================================================================================
      //====================================================================================
      //    slope corr   - Still here but no Histograms Filled anymore !!!
      // 
      //      float aSeg_BM=-999.;
      //      float aSeg_BI=-999.;
      //      float aSeg_BO=-999.;

      // ?? I am not sure this works ...
      //      if(gdirZ !=0. ) {
      //	float aseg=gdirY/gdirZ;
	//	if(segstn.substr(0,2)=="BM") aSeg_BM=aseg;
	//	if(segstn.substr(0,2)=="BI") aSeg_BI=aseg;
	//	if(segstn.substr(0,2)=="BO") aSeg_BO=aseg;
      //      }

      //====================================================================================
      //====================================================================================
      //    MDT-RPC Correlations FOR THE BARREL
      //
    if ( region=="Barrel") { 
      //double c = 299.792458; //approximate speed of the muons
      TH1F *hrpc;
      TH2F *h2rpc;
      std::vector<RpcCalibHitBase*>::const_iterator rpc_hit_it = segment->rpcHOTBegin();
      std::vector<RpcCalibHitBase*>::const_iterator rpc_hit_it_end = segment->rpcHOTEnd();  
      //      if(m_verbose) std::cout<<" rpc hit "<<std::endl; 
      for(; rpc_hit_it != rpc_hit_it_end; ++rpc_hit_it){
	//local
	//double x = (*rpc_hit_it)->localPosition().x();
	double y = (*rpc_hit_it)->localPosition().y();
	double z = (*rpc_hit_it)->localPosition().z();
	MuonFixedId ID = (*rpc_hit_it)->identify();
	//int station=ID.stationName();
	string cname = ID.stationNumberToFixedStationString( ID.stationName() ) ;
	// if(m_verbose) {
	//   std::cout<< " RPC CHAMBER name : "<< cname << std::endl;
	//   std::cout<< " MDT CHAMBER name : "<< chambseg.getOnlineName() << std::endl;
	// }
	//if(cname.substr(0,2)!="BM") continue;
	if(segment->chi2() > CHI2CUT ) continue;
	int eta=ID.eta();
	//int phi=ID.phi();
	// int phi2=phi_8to16(station,phi);	  
	int phi2=phiEtaConverter.phi_8to16(segStation,segPhi);
	string side="A";
	if(eta<0) side="C";
	string phisec=ts(phi2);
	if(phi2<10) phisec="0"+phisec;
	
	bool doSector=false;
	if(phi2>=m_SectorMin && phi2<=m_SectorMax) doSector=true;
	if(!doSector) continue;        
	//local
	double dirY=segment->direction().y();
	double dirZ=segment->direction().z();
	double posY=segment->position().y();
	double posZ=segment->position().z();
	double aseg=0.;
	double bseg=0.;
	if(dirZ !=0. ) {
	  aseg=dirY/dirZ; 
	  bseg= posY - aseg*posZ;
	}
	double dist=(-1*aseg*z+y-bseg)/sqrt(aseg*aseg+1);
        
	//	if(m_verbose) std::cout<<" CHAMBER name : "<< cname <<" "<<side<<" "<<phi2<< std::endl;
	
	histoType="MDT_RPC";
	hrpc = (TH1F*) m_histoManager->GetTDaqHisto(histoType,region,side,phi2);
	if(hrpc)hrpc->Fill(dist);
      
	histoType="z_MDT_RPC";
	hrpc = (TH1F*) m_histoManager->GetMdtHisto(histoType,region,side,phi2);
	if(hrpc)hrpc->Fill(dist);
	
	histoType="MDT_RPC_Vs_Event";
	h2rpc = (TH2F*) m_histoManager->GetTDaqHisto(histoType,region,side,phi2);
	//if(h2rpc) h2rpc->Fill(evtCounter,dist);
	if(h2rpc) h2rpc->Fill((float)evtNumber,dist);
           
	//	if(m_verbose){
	//	  std::cout<<"RPC ========== "<<std::endl;
	//	  std::cout<<z<<" "<<y<<" "<<aseg<<" "<<bseg<<" "<<dist<<std::endl;
	//	  std::cout<<histoType<<" "<<cname
	//		   <<" "<<side<<" "<<phi2<<" "<<hrpc<<" "<<h2rpc<<std::endl; 
	//	}

      } // end RPC hit loop
    } // end if region == Barrel
    //    if(m_verbose) std::cout<<"RPC loop end "<<std::endl;

    //====================================================================================
    //====================================================================================
    //    MDT-TGC Correlations FOR THE ENDCAP
    // added by D. Levin,march 18, 2009
    if ( region=="Endcap") { 

      TH1F *htgc;
      TH2F *h2tgc;

      std::vector<TgcCalibHitBase*>::const_iterator tgc_hit_it = segment->tgcHOTBegin();
      std::vector<TgcCalibHitBase*>::const_iterator tgc_hit_it_end = segment->tgcHOTEnd();  
      //      if(m_verbose) std::cout<<" tgc hit "<<std::endl;
 
      for(; tgc_hit_it != tgc_hit_it_end; ++tgc_hit_it){
          
	//global
	//double x = (*tgc_hit_it)->globalPosition().x();
	//double y = (*tgc_hit_it)->globalPosition().y();
	//double z = (*tgc_hit_it)->globalPosition().z();
	
	//local
	//double x = (*tgc_hit_it)->localPosition().x();
	double y = (*tgc_hit_it)->localPosition().y();
	double z = (*tgc_hit_it)->localPosition().z();
	
	//double R = sqrt(x*x + y*y + z*z);
	//double perp = sqrt(x*x + y*y);
	//double t = (*tgc_hit_it)->time();
	//double TOF = R/c;
	MuonFixedId ID = (*tgc_hit_it)->identify();
	string cname = ID.stationNumberToFixedStationString( ID.stationName() ) ;
	
	// if(m_verbose){
	//   std::cout<< " TGC CHAMBER name : "<< cname << std::endl;
	//   std::cout<< " MDT CHAMBER name : "<< chambseg.getOnlineName() << std::endl;
	// }
	//if(cname.substr(0,2)!="EM") continue;

	if(segment->chi2() > CHI2CUT ) continue;

	int eta=ID.eta();
	int phi2=phiEtaConverter.phi_8to16(segStation,segPhi);
	string side="A";
	if(eta<0) side="C";
	string phisec=ts(phi2);
	if(phi2<10) phisec="0"+phisec;

	bool doSector=false;
	if(phi2>=m_SectorMin && phi2<=m_SectorMax) doSector=true;
	if(!doSector) continue;        
        
	//global
	//double dirY=(*seg_it)->globalDirection().y();
	//double dirZ=(*seg_it)->globalDirection().z();
	//double posY=(*seg_it)->globalPosition().y();
	//double posZ=(*seg_it)->globalPosition().z();
	
	//local
	double dirY=segment->direction().y();
	double dirZ=segment->direction().z();
	double posY=segment->position().y();
	double posZ=segment->position().z();

	double aseg=0.;
	double bseg=0.;
	if(dirZ !=0. ) {
	  aseg=dirY/dirZ; 
	  bseg= posY - aseg*posZ;
	}
	double dist=(-1*aseg*z+y-bseg)/sqrt(aseg*aseg+1);
          
	//	if(m_verbose) std::cout<<" CHAMBER name : "<< cname <<" "<<side<<" "<<phi2<< std::endl;

	histoType="MDT_TGC";
	htgc = (TH1F*) m_histoManager->GetTDaqHisto(histoType,region,side,phi2);
	if(htgc)htgc->Fill(dist);
	  
	histoType="z_MDT_TGC";
	htgc = (TH1F*) m_histoManager->GetMdtHisto(histoType,region,side,phi2);
	if(htgc)htgc->Fill(dist);

	histoType="MDT_TGC_Vs_Event";
	h2tgc = (TH2F*) m_histoManager->GetTDaqHisto(histoType,region,side,phi2);
	//if(h2tgc) h2tgc->Fill(evtCounter,dist);
	if(h2tgc) h2tgc->Fill((float)evtNumber,dist);
           
	//	if(m_verbose) {
	//	  std::cout<<"TGC ========== "<<std::endl;
	//	  std::cout<<z<<" "<<y<<" "<<aseg<<" "<<bseg<<" "<<dist<<std::endl;
	//	  std::cout<<histoType<<" "<<cname
	//		   <<" "<<side<<" "<<phi2<<" "<<htgc<<" "<<h2tgc<<std::endl; 
	//	}

      } // end TGC hit loop
    } // end if region == Endcpa
    //    if(m_verbose) std::cout<<"TGC loop end "<<std::endl;
    //    if (m_verbose) std::cout<<"MySegmentLoop End"<<std::endl;

  }//End LOOP over Segments

  return;			      
}  //end SegmentAnalysis::handleEvent

int SegmentAnalysis::multiChambSeg( MuonCalibSegment *segment  ){
  
  int flag = false;
  
  if(segment->mdtHitsOnTrack()<=0) return flag;
  
  std::vector<MdtCalibHitBase*>::const_iterator hit_it = segment->mdtHOTBegin();
  std::vector<MdtCalibHitBase*>::const_iterator hit_it_end = segment->mdtHOTEnd();
  
  MuonFixedId ID1;
  ID1 =  (*hit_it)->identify() ;
  
  MuonFixedId ID;
  
  //Loop over all hits in the segment
  for(; hit_it != hit_it_end; ++hit_it) {
   
    ID = (*hit_it)->identify();
    
    if(ID.eta()!=ID1.eta()) flag= true;
    if(ID.stationName()!=ID1.stationName()) flag= true;
    if(ID.phi()!=ID1.phi()) flag= true;
    
    if(flag) {
      //std::cout<<" Crossing Segment "<<std::endl;
      //std::cout<<"  "<< ID <<std::endl;
      //std::cout<<"  "<< ID1<<std::endl;
      return flag;
    }
      
  }//loop

  return flag;
}  //end SegmentAnalysis::multiChambSeg

int SegmentAnalysis::segmentStation( MuonCalibSegment *segment  ){
  //std::cout<<" segmentStation ... "<<std::endl;
  std::vector<MdtCalibHitBase*>::const_iterator hit_it = segment->mdtHOTBegin();
  //std::cout<<" hit_it "<<(*hit_it)<<std::endl;
  MuonFixedId ID;
  ID =  (*hit_it)->identify() ;
  //std::cout<<" ID "<<ID<<std::endl;
  return ID.stationName();
}

int SegmentAnalysis::segmentEta( MuonCalibSegment *segment  ){
  std::vector<MdtCalibHitBase*>::const_iterator hit_it = segment->mdtHOTBegin();
  MuonFixedId ID;
  ID =  (*hit_it)->identify() ;
  return ID.eta();
}

int SegmentAnalysis::segmentPhi( MuonCalibSegment *segment  ){
  std::vector<MdtCalibHitBase*>::const_iterator hit_it = segment->mdtHOTBegin();
  MuonFixedId ID;
  ID =  (*hit_it)->identify() ;
  return ID.phi();
}

}//namespace MuonCalib
#endif
