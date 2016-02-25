/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file InDetPerfPlot_Pt.cxx
 * @author shaun roe
**/

#include "InDetPerfPlot_duplicate.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Service.h"
#include "GaudiKernel/IToolSvc.h"
#include "xAODTracking/TrackingPrimitives.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthParticle.h"


InDetPerfPlot_duplicate::InDetPerfPlot_duplicate(PlotBase* pParent, const std::string & sDir):PlotBase(pParent, sDir),m_histDefSvc(nullptr){
  ISvcLocator* svcLoc = Gaudi::svcLocator( );
   IToolSvc* toolSvc;
   StatusCode sc = svcLoc->service( "ToolSvc",toolSvc  );
   sc = toolSvc->retrieveTool("HistogramDefinitionSvc",m_histDefSvc);
}


void 
InDetPerfPlot_duplicate::initializePlots() {
  const bool prependDirectory(false);
  m_duplicateDeltaPt  = Book1D("duplicateDeltaPt","p_{T} difference of duplicate reco tracks (in GeV);p_{T}(GeV/c)",50,0.,50, prependDirectory);
  m_duplicateDeltaPtZoomed  = Book1D("duplicateDeltaPtZoomed","p_{T} difference of duplicate reco tracks (in GeV);p_{T}(GeV/c)",50,0.,5, prependDirectory);
  m_duplicateDeltaEta = Book1D("duplicateDeltaEta","eta difference of duplicate reco tracks",60,-3,3,prependDirectory);
  m_duplicateDeltaPhi =  Book1D("duplicateDeltaPhi","phi difference of duplicate reco tracks",60,-3,3,prependDirectory);
 
 
  m_duplicateDeltaPTvsTruthPT = Book2D("duplicateDeltaPTvsTruthPT","delta pt vs truth pt ",100,0,50,100,0,50,prependDirectory);
  m_duplicateDeltaPTvsTruthPTZoomed = Book2D("duplicateDeltaPTvsTruthPTZoomed","delta pt vs truth pt ",50,0,10,50,0,10,prependDirectory);



  //spectrum plots lpt = lower pt track, hpt = higher pt track, truth = truth track spectrum
  m_duplicateLPT  = Book1D("duplicateLPT","p_{T} of the duplicate lower pt reco track (in GeV);p_{T}(GeV/c)",40,0.,40, prependDirectory);
  m_duplicateLEta = Book1D("duplicateLEta","eta of the duplicate lower pt reco track",60,-3,3,prependDirectory);
  m_duplicateLPhi =  Book1D("duplicateLPhi","phi of the duplicate lower pt reco track",60,-3,3,prependDirectory);

  m_duplicateHPT  = Book1D("duplicateHPT","p_{T} of the duplicate higher pt reco track (in GeV);p_{T}(GeV/c)",40,0.,40, prependDirectory);
  m_duplicateHEta = Book1D("duplicateHEta","eta of the duplicate higher pt reco track",60,-3,3,prependDirectory);
  m_duplicateHPhi =  Book1D("duplicateHPhi","phi of the duplicate higher pt reco track",60,-3,3,prependDirectory);

  m_duplicateTruthPT  = Book1D("duplicateAssociatedTruthPT","p_{T} of the associated truth track (in GeV);p_{T}(GeV/c)",40,0.,40, prependDirectory);
  m_duplicateTruthEta = Book1D("duplicateAssociatedTruthEta","eta of the associated truth track",60,-3,3,prependDirectory);
  m_duplicateTruthPhi =  Book1D("duplicateAssociatedTruthPhi","phi of the associated truth track",60,-3,3,prependDirectory);

  //resolutions hpt - truth, lpt - truth

  m_duplicateResLPT  = Book1D("duplicateResLPT","p_{T} of the lower pt track minus truth pt (in GeV);p_{T}(GeV/c)",60,-15.,15, prependDirectory);
  m_duplicateResLEta = Book1D("duplicateResLEta","eta of the lower pt track minus truth eta",60,-3,3,prependDirectory);
  m_duplicateResLPhi =  Book1D("duplicateResLPhi","phi of the lower pt track minus truth phi",60,-3,3,prependDirectory);
  m_duplicateResHPT  = Book1D("duplicateResHPT","p_{T} of the higher pt track minus truth pt (in GeV);p_{T}(GeV/c)",60,-15.,15, prependDirectory);
  m_duplicateResHEta = Book1D("duplicateResHEta","eta of the higher pt track minus truth eta",60,-3,3,prependDirectory);
  m_duplicateResHPhi =  Book1D("duplicateResHPhi","phi of the higher pt track minus truth phi",60,-3,3,prependDirectory);

  m_duplicateProbSpectrum = Book2D("duplicateProbSpectrum","duplicate probability spectrum",30,.8,1.1,30,.8,1.1, prependDirectory);


  // chisq/dof
  m_duplicateLPTHoles = Book1D("duplicateLPTvsHoles", " # of holes of the lower pt track", 10,0,10, prependDirectory);
  m_duplicateHPTHoles = Book1D("duplicateHPTvsHoles", " # of holes of the higher pt track", 10,0,10, prependDirectory);
  m_singleMatchPTHoles = Book1D("singleMatchPTvsHoles", "# of holes of a single matched reco track", 10,0,10, prependDirectory);

  m_duplicateHPixelvsSCTShared = Book2D("duplicateHPixelvsSCTShared", "# of Pixel Shared Hits vs # of SCT Shared Hits for the higher pt track",20,0,20,20,0,20, prependDirectory);
  m_duplicateLPixelvsSCTShared = Book2D("duplicateLPixelvsSCTShared", "# of Pixel Shared Hits vs # of SCT Shared Hits for the lower pt track",20,0,20,20,0,20, prependDirectory);
  m_singleMatchPixelvsSCTShared = Book2D("singleMatchPixelvsSCTShared", "# of Pixel Shared Hits vs # of SCT Shared Hits for a single matched track",20,0,20,20,0,20, prependDirectory);
  m_twoMatchHPixelvsSCTShared = Book2D("twoMatchHPixelvsSCTShared", "# of Pixel Shared Hits vs # of SCT Shared Hits for the higher pt track",20,0,20,20,0,20, prependDirectory);
  m_twoMatchLPixelvsSCTShared = Book2D("twoMatchLPixelvsSCTShared", "# of Pixel Shared Hits vs # of SCT Shared Hits for the lower pt track",20,0,20,20,0,20, prependDirectory);



}


void
InDetPerfPlot_duplicate::fillSingleMatch(const xAOD::TrackParticle & trackParticle){
  //float smpt(trackParticle.pt()/1000.);
  //float cSquare1 = trackParticle.chiSquared();
  //float dof1 = trackParticle.numberDoF();
  //float csOverDoF1 = cSquare1 / dof1;

  uint8_t iPixHoles, iSCTHoles, iPixShared, iSCTShared;
  //uint8_t iPixHits,iSCTHits;
  //int pixHits = 0;
  //int sctHits = 0;
  //int totalHits = 0;
  int pixHoles = 0;
  int sctHoles = 0;
  
  int pixShared = 0;
  int sctShared = 0;
  if (trackParticle.summaryValue(iPixHoles,xAOD::numberOfPixelHoles)) pixHoles = iPixHoles;
  if (trackParticle.summaryValue(iSCTHoles,xAOD::numberOfSCTHoles)) sctHoles = iSCTHoles;
  if (trackParticle.summaryValue(iPixShared,xAOD::numberOfPixelSharedHits)) pixShared = iPixShared;
  if (trackParticle.summaryValue(iSCTShared,xAOD::numberOfSCTSharedHits)) sctShared = iSCTShared;
  //if (trackParticle.summaryValue(iPixHits,xAOD::numberOfPixelHits)) pixHits = iPixHits;
  //if (trackParticle.summaryValue(iSCTHits,xAOD::numberOfSCTHits)) sctHits = iSCTHits;


 
  m_singleMatchPixelvsSCTShared->Fill(pixShared,sctShared);

  m_singleMatchPTHoles->Fill(sctHoles+pixHoles);



}

void
InDetPerfPlot_duplicate::fillTwoMatchDuplicate(Float_t prob1,Float_t prob2,const xAOD::TrackParticle & trackParticle,const xAOD::TrackParticle & particle, const xAOD::TruthParticle&  tp ){
  float pt1(trackParticle.pt()/1000.);
  float pt2(particle.pt()/1000.);
  float truthPt(tp.pt()/1000);
  float eta1(trackParticle.eta());
  float eta2(particle.eta());
  float truthEta(tp.eta());
  float phi1(trackParticle.phi());
  float phi2(particle.phi());
  float truthPhi(tp.phi());

  uint8_t iPixHoles, iSCTHoles;
  int pixHoles1 = 0;
  int sctHoles1 = 0;
  int totalHoles1 =0;
  int pixHoles2 = 0;
  int sctHoles2 = 0;
  int totalHoles2 =0;

  uint8_t iPixShared,  iSCTShared;                                                                                                                                                   
  int pixShared1 = 0;                                                                                                                                                                                            
  int pixShared2 = 0;                                                                                                                                                                                          
  int sctShared1 = 0;                                                                                                                                                                                             int sctShared2 = 0;
    
  if (trackParticle.summaryValue(iPixShared,xAOD::numberOfPixelSharedHits)) pixShared1 = iPixShared;
  if (trackParticle.summaryValue(iSCTShared,xAOD::numberOfSCTSharedHits)) sctShared1 = iSCTShared;
  if (particle.summaryValue(iPixShared,xAOD::numberOfPixelSharedHits)) pixShared2 = iPixShared;
  if (particle.summaryValue(iSCTShared,xAOD::numberOfSCTSharedHits)) sctShared2 = iSCTShared;

  if (trackParticle.summaryValue(iPixHoles,xAOD::numberOfPixelHoles)) pixHoles1 = iPixHoles;
  if (trackParticle.summaryValue(iSCTHoles,xAOD::numberOfSCTHoles)) sctHoles1 = iSCTHoles;
  if (particle.summaryValue(iPixHoles,xAOD::numberOfPixelHoles)) pixHoles2 = iPixHoles;
  if (particle.summaryValue(iSCTHoles,xAOD::numberOfSCTHoles)) sctHoles2 = iSCTHoles;
  totalHoles1 = pixHoles1 + sctHoles1;
  totalHoles2 = pixHoles2 + sctHoles2;
  if(prob1 == prob2){
    if(pt1 > pt2)
      {
	


	m_duplicateLPTHoles->Fill(totalHoles2);
	m_duplicateHPTHoles->Fill(totalHoles1);
	m_duplicateHPixelvsSCTShared->Fill(pixShared1,sctShared1);
	m_duplicateLPixelvsSCTShared->Fill(pixShared2,sctShared2);
        m_duplicateHPTHoles->Fill(totalHoles1);

	float deltaPT1 = pt1 - pt2;
	float deltaEta1 = eta1 - eta2;
	float deltaPhi1 = phi1 - phi2;
     
	m_duplicateDeltaPTvsTruthPT->Fill(truthPt,deltaPT1);
	m_duplicateDeltaPTvsTruthPTZoomed->Fill(truthPt,deltaPT1);     

	//resolution plots
	float resHPT1 = pt1 - truthPt;
	float resHEta1 = eta1 - truthEta;
	float resHPhi1 = phi1 - truthPhi;
	float resLPT1 = pt2 - truthPt;
	float resLEta1 = eta2 - truthEta;
	float resLPhi1 = phi2 - truthPhi;
	
	m_duplicateResLPT->Fill(resLPT1);
	m_duplicateResLEta->Fill(resLEta1);
	m_duplicateResLPhi->Fill(resLPhi1);
	m_duplicateResHPT->Fill(resHPT1);
	m_duplicateResHEta->Fill(resHEta1);
	m_duplicateResHPhi->Fill(resHPhi1);
	
	
	//spectrum plots                                                                                                                                                                                           
	m_duplicateLPT->Fill(pt2);
	m_duplicateLEta->Fill(eta2);
	m_duplicateLPhi->Fill(phi2);
	m_duplicateHPT->Fill(pt1);
	m_duplicateHEta->Fill(eta1);
	m_duplicateHPhi->Fill(phi1);
	m_duplicateTruthPT->Fill(truthPt);
	m_duplicateTruthEta->Fill(truthEta);
	m_duplicateTruthPhi->Fill(truthPhi);
	// delta plots
	m_duplicateDeltaPt->Fill(deltaPT1);  
	m_duplicateDeltaPtZoomed->Fill(deltaPT1);
	m_duplicateDeltaEta->Fill(deltaEta1); 
	m_duplicateDeltaPhi->Fill(deltaPhi1); 
	//lpt vs hpt

	// hpt & lpt vs truth

	m_duplicateProbSpectrum->Fill(prob2,prob1); 
      }
    if( pt2 > pt1 || pt1 == pt2) 
      {    

	m_duplicateHPTHoles->Fill(totalHoles2);
	m_duplicateLPTHoles->Fill(totalHoles1);
	m_duplicateLPixelvsSCTShared->Fill(pixShared1,sctShared1);
        m_duplicateHPixelvsSCTShared->Fill(pixShared2,sctShared2);
	float deltaPT2 = pt2 - pt1;
	float deltaEta2 = eta2 - eta1;
	float deltaPhi2 = phi2 - phi1;
	// pt2 - pt1 vs truth                                                                                                                                                                                     

	
	m_duplicateDeltaPTvsTruthPT->Fill(truthPt,deltaPT2);
	m_duplicateDeltaPTvsTruthPTZoomed->Fill(truthPt,deltaPT2);



	
	//resolution plots
	float resLPT2 = pt1 - truthPt;
	float resLEta2 = eta1 - truthEta;
	float resLPhi2 = phi1 - truthPhi;
	float resHPT2 = pt2 - truthPt;
	float resHEta2 = eta2 - truthEta;
	float resHPhi2 = phi2 - truthPhi;
	m_duplicateResLPT->Fill(resLPT2);
	m_duplicateResLEta->Fill(resLEta2);
	m_duplicateResLPhi->Fill(resLPhi2);
	m_duplicateResHPT->Fill(resHPT2);
	m_duplicateResHEta->Fill(resHEta2);
	m_duplicateResHPhi->Fill(resHPhi2);
	
	//spectrum plots
	m_duplicateLPT->Fill(pt1);
	m_duplicateLEta->Fill(eta1);
	m_duplicateLPhi->Fill(phi1);
	m_duplicateHPT->Fill(pt2);
	m_duplicateHEta->Fill(eta2);
	m_duplicateHPhi->Fill(phi2);
	m_duplicateTruthPT->Fill(truthPt);
	m_duplicateTruthEta->Fill(truthEta);
	m_duplicateTruthPhi->Fill(truthPhi);
	// delta (hpt - lpt plots) & lpt vs hpt plts
	m_duplicateDeltaPt->Fill(deltaPT2);
	m_duplicateDeltaPtZoomed->Fill(deltaPT2);
	m_duplicateDeltaEta->Fill(deltaEta2);
	m_duplicateDeltaPhi->Fill(deltaPhi2);

	//lpt & hpt vs truth

	m_duplicateProbSpectrum->Fill(prob1,prob2);
      }
  }
  if(prob1 != prob2)
    { 
      if(pt1 > pt2)
	{                                                                                                                                                                                                 
	  m_twoMatchHPixelvsSCTShared->Fill(pixShared1,sctShared1);
	  m_twoMatchLPixelvsSCTShared->Fill(pixShared2,sctShared2);
	  
	  m_duplicateProbSpectrum->Fill(prob2,prob1);                                                                                                                                                                    
	}
      else
	{
	  m_twoMatchLPixelvsSCTShared->Fill(pixShared1,sctShared1);
	  m_twoMatchHPixelvsSCTShared->Fill(pixShared2,sctShared2);

	  m_duplicateProbSpectrum->Fill(prob1,prob2);
	} 
    }
}

   
      
      
      
      

