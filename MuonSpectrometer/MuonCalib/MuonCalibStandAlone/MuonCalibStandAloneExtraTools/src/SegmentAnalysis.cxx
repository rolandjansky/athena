/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonCalib_SegmentAnalysisCXX
#define MuonCalib_SegmentAnalysisCXX

#include "MuonCalibStandAloneExtraTools/SegmentAnalysis.h"
#include "TH1.h"
#include "TMath.h" // for TMath::Prob()
#include <cmath>

#include "MdtCalibFitters/DCSLFitter.h"
#include "MuonCalibStandAloneExtraTools/PhiEtaUtils.h"
#include "MuonCalibStandAloneExtraTools/StringUtil.h"
#include "MuonCalibStandAloneExtraTools/MDTName.h"

namespace MuonCalib{

SegmentAnalysis::SegmentAnalysis(RegionSelectionSvc *punt, HistogramManager *histoman, bool verbose, bool dohitresids) {
  m_histoManager=histoman;
  p_reg_sel_svc=punt;
  m_verbose=verbose;
  m_doHitResids=dohitresids;

  PhiEtaNameConverter phiEtaConverter;
  std::string testName="BIL1A01" ;
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

  float CHI2CUT=50. ;

  //Event number
  int evtNumber = event.eventInfo().eventNumber() ;

  ToString ts;
  PhiEtaNameConverter phiEtaConverter;

  if( !p_reg_sel_svc){
    return;
  }

  std::string histoType="seg";
  TH1F *h1seg = (TH1F*) m_histoManager->GetHisto("GLOBAL",histoType);
  float NumOfSeg = (float) segments.size() - position ;
  if (h1seg) h1seg->Fill( NumOfSeg );

  //===========================================================================

  // ==== SEGMENT LOOP =========================================================
  //Looping over all segments in the event
  //
  TH1F * hseg;
  TH2F * h2;

  for (unsigned int k=position; k<segments.size(); k++) {    // LOOP OVER SEGMENTS 
    MuonCalibSegment *segment(segments[k]);

    unsigned int MinNumOfHitsPerSegment = 4 ;

    if ( segment->mdtHitsOnTrack()==0) {
      continue;
    }

    if ( segment->mdtHitsOnTrack()<MinNumOfHitsPerSegment) {
      continue;
    }

    std::vector<MdtCalibHitBase*>::const_iterator hit_it ;
    std::vector<MdtCalibHitBase*>::const_iterator hit_it_start = segment->mdtHOTBegin();
    std::vector<MdtCalibHitBase*>::const_iterator hit_it_end = segment->mdtHOTEnd();   

    bool segmentIsInRegion = false ;
    for (hit_it=hit_it_start; hit_it != hit_it_end; ++hit_it) {
      if (p_reg_sel_svc->isInRegion((*hit_it)->identify())) segmentIsInRegion = true ;
    }
    if (!segmentIsInRegion) {
      continue;
    }

    //=========
    int crossing = multiChambSeg(segment);
    if (crossing) {
      continue;
    }

    int segStation = segmentStation(segment);
    int segEta =  segmentEta(segment);
    int segPhi =  segmentPhi(segment);

    int segPhi2=phiEtaConverter.phi_8to16(segStation,segPhi);
       
    bool doSector=false;
    if(segPhi2>=m_SectorMin && segPhi2<=m_SectorMax) doSector=true;
    if(!doSector) continue;        

    //== build chamber name

    MuonFixedId  id;
    std::string segstn = id.stationNumberToFixedStationString(segStation);
    MDTName chambseg(segstn,segPhi,segEta);    

    std::string chamberName = chambseg.getOnlineName();  
    std::string side=chambseg.getSide();
    std::string region=chambseg.getRegion();

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
    hseg = (TH1F*) m_histoManager->GetMdtHisto(histoType,region,side,segPhi2);
    if(hseg) hseg->Fill(hitsOnSeg);

    histoType="D_ChamberHitsOnSegment";
    hseg = (TH1F*) m_histoManager->GetMdtHisto(histoType,chambseg);
    if(hseg) hseg->Fill(hitsOnSeg);

    histoType="c_Segment_ProbChi2";
    hseg = (TH1F*) m_histoManager->GetMdtHisto(histoType,chambseg);
    if (hseg) hseg->Fill(TMath::Prob(SegChi2,(int)DoF));
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

      float resid         = 0.0; // residual
      float radius        = 0.0; // hit drift radius
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
		
      } else { // Do fit residuals vs radius
	resid  = (*hit_it)->radialResidual();
	radius = (*hit_it)->driftRadius();
      }
      if (radius<0) radius = -radius ;  // First take the absolute value of radius
      if ((*hit_it)->signedDistanceToTrack()<0.) radius=-radius ; // Then assign the sign of signedDistanceToTrack
      
      if (h2) h2->Fill(radius,resid);
      if (hseg) hseg->Fill(resid);
      
    } // End loop over hits       
      ///////////////////// SNIP END


      //====================================================================================
      //====================================================================================
      //    GLOBAL segment_Phi Vs segment_Eta  plot 
      // 
    float gdirX = segment->globalDirection().x();
    float gdirY = segment->globalDirection().y();
    float gdirZ = segment->globalDirection().z();
    float globalPhi = std::atan2(gdirY,gdirX)* 180./M_PI; ;
    float globalTheta = std::acos(gdirZ) ;
    float globalEta = -std::log(std::tan(globalTheta/2.)) ;
    histoType="phi_vs_eta";

    h2 = (TH2F*) m_histoManager->GetHisto("GLOBAL",histoType);
    if(h2) h2->Fill( globalEta, globalPhi );	

      //====================================================================================
      //====================================================================================
      //    slope corr   - Still here but no Histograms Filled anymore !!!
      // 

      //====================================================================================
      //====================================================================================
      //    MDT-RPC Correlations FOR THE BARREL
      //
    if ( region=="Barrel") { 
      TH1F *hrpc;
      TH2F *h2rpc;
      std::vector<RpcCalibHitBase*>::const_iterator rpc_hit_it = segment->rpcHOTBegin();
      std::vector<RpcCalibHitBase*>::const_iterator rpc_hit_it_end = segment->rpcHOTEnd();  
      for(; rpc_hit_it != rpc_hit_it_end; ++rpc_hit_it){
	//local
	double y = (*rpc_hit_it)->localPosition().y();
	double z = (*rpc_hit_it)->localPosition().z();
	MuonFixedId ID = (*rpc_hit_it)->identify();
	std::string cname = ID.stationNumberToFixedStationString( ID.stationName() ) ;
	if(segment->chi2() > CHI2CUT ) continue;
	int eta=ID.eta();
	int phi2=phiEtaConverter.phi_8to16(segStation,segPhi);
	std::string side="A";
	if(eta<0) side="C";
	std::string phisec=ts(phi2);
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
	double dist=(-1*aseg*z+y-bseg)/std::sqrt(aseg*aseg+1);
        
	
	histoType="MDT_RPC";
	hrpc = (TH1F*) m_histoManager->GetTDaqHisto(histoType,region,side,phi2);
	if(hrpc)hrpc->Fill(dist);
      
	histoType="z_MDT_RPC";
	hrpc = (TH1F*) m_histoManager->GetMdtHisto(histoType,region,side,phi2);
	if(hrpc)hrpc->Fill(dist);
	
	histoType="MDT_RPC_Vs_Event";
	h2rpc = (TH2F*) m_histoManager->GetTDaqHisto(histoType,region,side,phi2);
	if(h2rpc) h2rpc->Fill((float)evtNumber,dist);

      } // end RPC hit loop
    } // end if region == Barrel

    //====================================================================================
    //====================================================================================
    //    MDT-TGC Correlations FOR THE ENDCAP
    // added by D. Levin,march 18, 2009
    if ( region=="Endcap") { 

      TH1F *htgc;
      TH2F *h2tgc;

      std::vector<TgcCalibHitBase*>::const_iterator tgc_hit_it = segment->tgcHOTBegin();
      std::vector<TgcCalibHitBase*>::const_iterator tgc_hit_it_end = segment->tgcHOTEnd();  
 
      for(; tgc_hit_it != tgc_hit_it_end; ++tgc_hit_it){
          
	//local
	double y = (*tgc_hit_it)->localPosition().y();
	double z = (*tgc_hit_it)->localPosition().z();
	
	MuonFixedId ID = (*tgc_hit_it)->identify();
	std::string cname = ID.stationNumberToFixedStationString( ID.stationName() ) ;

	if(segment->chi2() > CHI2CUT ) continue;

	int eta=ID.eta();
	int phi2=phiEtaConverter.phi_8to16(segStation,segPhi);
	std::string side="A";
	if(eta<0) side="C";
	std::string phisec=ts(phi2);
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
	double dist=(-1*aseg*z+y-bseg)/std::sqrt(aseg*aseg+1);
          

	histoType="MDT_TGC";
	htgc = (TH1F*) m_histoManager->GetTDaqHisto(histoType,region,side,phi2);
	if(htgc)htgc->Fill(dist);
	  
	histoType="z_MDT_TGC";
	htgc = (TH1F*) m_histoManager->GetMdtHisto(histoType,region,side,phi2);
	if(htgc)htgc->Fill(dist);

	histoType="MDT_TGC_Vs_Event";
	h2tgc = (TH2F*) m_histoManager->GetTDaqHisto(histoType,region,side,phi2);
	if(h2tgc) h2tgc->Fill((float)evtNumber,dist);

      } // end TGC hit loop
    } // end if region == Endcpa

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
      return flag;
    }
      
  }//loop

  return flag;
}  //end SegmentAnalysis::multiChambSeg

int SegmentAnalysis::segmentStation( MuonCalibSegment *segment  ){
  std::vector<MdtCalibHitBase*>::const_iterator hit_it = segment->mdtHOTBegin();
  MuonFixedId ID;
  ID =  (*hit_it)->identify() ;
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
