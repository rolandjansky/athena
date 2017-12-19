/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <sstream>
#include "GaudiKernel/IJobOptionsSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"

#include "StoreGate/StoreGateSvc.h"
#include "InDetDiMuonMonitoring/DiMuMon.h"
//#include "muonEvent/MuonContainer.h"
#include "xAODMuon/Muon.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

#include <math.h>

#include "TProfile.h"
#include "TMath.h"
#include "TF1.h"
#include "TH1.h"
#include "TH2.h"
#include "TCanvas.h"
#include "TString.h"
#include "TLorentzVector.h"
#include "TPostScript.h"
#include "TStyle.h"

#include "RooRealVar.h"
#include "RooDataHist.h"
#include "RooPlot.h"
#include "RooBreitWigner.h"
#include "RooCBShape.h"
#include "RooFFTConvPdf.h"
#include "RooGlobalFunc.h"
#include "RooArgList.h"


//using namespace Analysis;
//using namespace Rec;

DiMuMon::DiMuMon( const std::string & type, const std::string & name, const IInterface* parent )
  : ManagedMonitorToolBase( type, name, parent )
  , m_triggerChainName("NoTrig")
  , m_muonCollection("Muons")
{
   declareProperty( "resonName", m_resonName = "Zmumu" );
   declareProperty( "triggerChainName", m_triggerChainName = "NoTrig" );
   declareProperty( "muonCollection", m_muonCollection = "Muons" );
   declareProperty( "setDebug", m_setDebug = false );
   declareProperty( "minInvmass", m_minInvmass = 60.);
   declareProperty( "maxInvmass", m_maxInvmass = 120.);
   declareProperty( "regions", m_regions );
   declareProperty( "nVarBins", m_nVarBins = 10);
   declareProperty( "nPtBins", m_nPtBins = 100);
   declareProperty( "nEtaBins", m_nEtaBins = 32);
   declareProperty( "nPhiBins", m_nPhiBins = 36);
   declareProperty( "nMassBins", m_nMassBins = 60);
   declareProperty( "varsVSmean", m_varsVSmean);
   declareProperty( "varsVSwidth", m_varsVSwidth);
   declareProperty( "varsDistr", m_varsDistr);
   declareProperty( "doSaveFits", m_doSaveFits = false );
   declareProperty( "doFits", m_doFits = false );
   declareProperty( "ptCut", m_ptCut = 0.);//GeV
   declareProperty( "etaCut", m_etaCut = 2.5);
}


DiMuMon::~DiMuMon()
{
}

StatusCode DiMuMon::initialize(){

  ATH_CHECK( ManagedMonitorToolBase::initialize() );

  if (m_regions.empty()) {
    m_regions.push_back("All");
  }
  // m_variables[] = {"eta","etaAll","etaPos","etaNeg","phi","phiAll","phiPos","phiNeg","pt","ptAll","ptPos","ptNeg","etaDiff","etaSumm","phiDiff","phiSumm","crtDiff"};
  m_varLabels["eta"] = "Dimuon #eta";
  m_varLabels["etaAll"] = "#eta (all #mu)";
  m_varLabels["etaPos"] = "#eta(+)";
  m_varLabels["etaNeg"] = "#eta(-)";

  m_varLabels["phi"] = "Dimuon #phi";
  m_varLabels["phiAll"] = "#phi (all #mu)";
  m_varLabels["phiPos"] = "#phi(+)";
  m_varLabels["phiNeg"] = "#phi(-)";

  m_varLabels["pt"] = "Dimuon p_{T}";
  m_varLabels["ptAll"] = "p_{T} (all #mu)";
  m_varLabels["ptPos"] = "p_{T}(+) [GeV]";
  m_varLabels["ptNeg"] = "p_{T}(-) [GeV]";

  m_varLabels["etaDiff"] = "#eta(+)-#eta(-)";
  m_varLabels["etaSumm"] = "#eta(+)+#eta(-)";
  m_varLabels["phiDiff"] = "#phi(+)-#phi(-)";
  m_varLabels["phiSumm"] = "#phi(+)+#phi(-)";
  m_varLabels["crtDiff"] = "1/p_{T}(+) - 1/p_{T}(-) [GeV^{-1}]";

  //resonance independent
  // for eta these are filled as the histograms are declared due to the dependence between region and eta
  m_varRanges["phi"] = std::make_pair(-CLHEP::pi,CLHEP::pi);
  m_varRanges["phiAll"] = std::make_pair(-CLHEP::pi,CLHEP::pi);
  m_varRanges["phiPos"] = std::make_pair(-CLHEP::pi,CLHEP::pi);
  m_varRanges["phiNeg"] = std::make_pair(-CLHEP::pi,CLHEP::pi);
  m_varRanges["etaSumm"] = std::make_pair(-5.,5.);

  //resonance dependent
  double ptMax = 0.;
  if (m_resonName=="Jpsi" || m_resonName=="Upsi"){
    m_varRanges["eta"] = std::make_pair(-3.,3.);
    m_varRanges["phiDiff"] = std::make_pair(0.,1.);
    m_varRanges["etaDiff"] = std::make_pair(-1.,1.);
    m_varRanges["crtDiff"] = std::make_pair(-0.15,0.15);
    m_varRanges["phiSumm"] = std::make_pair(-6.,6.);
    if (m_ptCut == 0.) m_ptCut = 4.0;
    ptMax = 18.;
  } else if (m_resonName=="Zmumu") {
    m_varRanges["eta"] = std::make_pair(-5.,5.);
    m_varRanges["phiDiff"] = std::make_pair(0.,CLHEP::pi);
    m_varRanges["etaDiff"] = std::make_pair(-3.,3.);
    m_varRanges["crtDiff"] = std::make_pair(-0.03,0.03);
    m_varRanges["phiSumm"] = std::make_pair(-3.5,3.5);
    if (m_ptCut == 0.) m_ptCut = 20.0;
    ptMax = 100.;
  }
  m_varRanges["pt"] = std::make_pair(0.,ptMax);
  m_varRanges["ptAll"] = std::make_pair(m_ptCut,ptMax);
  m_varRanges["ptPos"] = std::make_pair(m_ptCut,ptMax);
  m_varRanges["ptNeg"] = std::make_pair(m_ptCut,ptMax);

  //retrieving track isolation tool
  //  ISvcLocator* serviceLocator = Gaudi::svcLocator();
  // IToolSvc* toolSvc;
  // IAlgTool* ptool;
  //sc = serviceLocator->service("ToolSvc", toolSvc, true);
  //if ( sc.isFailure() ) {
  //msg(MSG::WARNING) << "Could not locate toolSvc"<<endmsg;
  //return sc;
  //}
  //sc = toolSvc->retrieveTool("TrackIsolationTool", ptool);
  //if ( sc.isFailure() ) {
  //msg(MSG::WARNING) << "Could not retrieve track isolation tool!" << endmsg;
  //return sc;
  //}
  //m_isolationTool = dynamic_cast<TrackIsolationTool*>(ptool);
  m_coneSize = 0.4;
  m_isolationCut = 0.2;

  return StatusCode::SUCCESS;

}

//StatusCode DiMuMon::bookHistograms( bool isNewEventsBlock, bool isNewLumiBlock, bool isNewRun )
StatusCode DiMuMon::bookHistograms()
{

  //    MonGroup dimuMonObj_shift( this, "DiMuMon/"+m_resonName+"/" + m_triggerChainName, shift, run );
  //   MonGroup dimuMonObj_expert( this, "DiMuMon/"+m_resonName+"/" + m_triggerChainName + "_detail", expert, run );


    MonGroup dimuMonObj_shift( this, "DiMuMon/"+m_resonName+"/" + m_triggerChainName,  run );
    MonGroup dimuMonObj_expert( this, "DiMuMon/"+m_resonName+"/" + m_triggerChainName + "_detail", run );

    if ( AthenaMonManager::environment() == AthenaMonManager::online ) {
      // book histograms that are only made in the online environment...
    }

    if ( AthenaMonManager::dataType() == AthenaMonManager::cosmics ) {
      // book histograms that are only relevant for cosmics data...
    }


//    const EventInfo* thisEventInfo;
//    sc=evtStore()->retrieve(thisEventInfo);
//    if (sc != StatusCode::SUCCESS) {
//      if(msgLvl(MSG::WARNING)) msg(MSG::WARNING)  << "No EventInfo object found" << endmsg;
//    }
//    else {
//      m_lumiBlockNum = thisEventInfo->event_ID()->lumi_block();
//    }

//   if (isNewEventsBlock || isNewLumiBlock){
     // do nothing
    // }
   //if ( newLowStatFlag() || newLumiBlockFlag() ) {  }


   if( newRunFlag() ) {

     //   if( isNewRun ) {

     m_chi2 = new TH1F("chi2","chi2",100,0.,10.);
     RegisterHisto(dimuMonObj_expert,m_chi2);

     const int nCuts = 8;
     m_stat = new TH1F("statistics","Statistics",nCuts, 0., double(nCuts));
     TString binLabels[] ={"MuonIdTrk","TrkIsoPt40/Pt<.2","CombMuon","MCPgoodTrk","MuPtOK","eta<2.5","OppChargePair","InvMassOK"};
     for(int ilabel=0;ilabel<nCuts;ilabel++){
       m_stat->GetXaxis()->SetBinLabel(ilabel+1,binLabels[ilabel]);
     }
     RegisterHisto(dimuMonObj_expert,m_stat);

     //create histos for each region
     std::vector<std::string> ::iterator ireg = m_regions.begin();
     for (ireg = m_regions.begin(); ireg != m_regions.end(); ireg++){

       //declare region-dependent eta range
       if (*ireg == "BB") {
	 m_varRanges["etaAll"] = std::make_pair(-1.05,1.05);
	 m_varRanges["etaPos"] = std::make_pair(-1.05,1.05);
	 m_varRanges["etaNeg"] = std::make_pair(-1.05,1.05);
       } else if (*ireg == "EAEA") {
	 m_varRanges["etaAll"] = std::make_pair(1.05,2.5);
	 m_varRanges["etaPos"] = std::make_pair(1.05,2.5);
	 m_varRanges["etaNeg"] = std::make_pair(1.05,2.5);
       } else if (*ireg == "ECEC") {
	 m_varRanges["etaAll"] = std::make_pair(-2.5,-1.05);
	 m_varRanges["etaPos"] = std::make_pair(-2.5,-1.05);
	 m_varRanges["etaNeg"] = std::make_pair(-2.5,-1.05);
       } else {
	 m_varRanges["etaAll"] = std::make_pair(-2.5,2.5);
	 m_varRanges["etaPos"] = std::make_pair(-2.5,2.5);
	 m_varRanges["etaNeg"] = std::make_pair(-2.5,2.5);
       }

       //mass plots
       TString hname = m_resonName + "_invmass_" + *ireg;
       TString htitle = hname + "; Invmass[GeV/c^{2}]";
       m_invmass[*ireg] = new TH1F(hname, htitle, m_nMassBins, m_minInvmass, m_maxInvmass);
       RegisterHisto(dimuMonObj_shift,m_invmass[*ireg]);

       //for each var vs mass plot
       std::vector<std::string> ::iterator ivarM = m_varsVSmean.begin();
       for (ivarM=m_varsVSmean.begin(); ivarM!=m_varsVSmean.end();ivarM++){
	 hname = m_resonName + "_2DinvmassVS" + *ivarM + "_" + *ireg;
	 htitle = hname + ";" + m_varLabels[*ivarM] + ";Invmass[GeV/c^{2}]";
	 m_2DinvmassVSx[*ireg][*ivarM] = new TH2F(hname, htitle ,m_nVarBins,m_varRanges[*ivarM].first, m_varRanges[*ivarM].second, m_nMassBins, m_minInvmass, m_maxInvmass);
	 RegisterHisto(dimuMonObj_shift,m_2DinvmassVSx[*ireg][*ivarM]);

	 hname = m_resonName + "_invmassVS" + *ivarM + "_" + *ireg;
	 htitle = hname + ";" + m_varLabels[*ivarM] + ";Invmass[GeV/c^{2}]";
	 m_invmassVSx[*ireg][*ivarM] = new TH1F(hname, htitle, m_nVarBins,m_varRanges[*ivarM].first, m_varRanges[*ivarM].second);
	 RegisterHisto(dimuMonObj_shift,m_invmassVSx[*ireg][*ivarM]);
       }

       //for each var vs width plot
       std::vector<std::string>::iterator ivarW = m_varsVSwidth.begin();
       for (ivarW=m_varsVSwidth.begin(); ivarW!=m_varsVSwidth.end();ivarW++){
	 //book the corresponding 2D histo if it hasn't already been booked
	 if (m_2DinvmassVSx[*ireg].find(*ivarW)==m_2DinvmassVSx[*ireg].end()){
	   hname = m_resonName + "_2DinvmassVS" + *ivarW + "_" + *ireg;
	   htitle = hname + ";" + m_varLabels[*ivarW] + ";Invmass[GeV/c^{2}]";
	   m_2DinvmassVSx[*ireg][*ivarW] = new TH2F(hname, htitle ,m_nVarBins, m_varRanges[*ivarW].first, m_varRanges[*ivarW].second, m_nMassBins, m_minInvmass, m_maxInvmass);
	   RegisterHisto(dimuMonObj_shift,m_2DinvmassVSx[*ireg][*ivarW]);
	 }

	 hname = m_resonName + "_widthVS" + *ivarW + "_" + *ireg;
	 htitle = hname + ";" + m_varLabels[*ivarW] + ";Width[GeV/c^{2}]";
	 m_widthVSx[*ireg][*ivarW] = new TH1F(hname,htitle ,m_nVarBins,m_varRanges[*ivarW].first, m_varRanges[*ivarW].second);
	 RegisterHisto(dimuMonObj_expert,m_widthVSx[*ireg][*ivarW]);

       }

       //for the each variable's distribution
       std::vector<std::string>::iterator ivarD = m_varsDistr.begin();
       for (ivarD=m_varsDistr.begin(); ivarD!=m_varsDistr.end();ivarD++){
	 hname = m_resonName + "_" + *ivarD + "_" + *ireg;
	 htitle = hname + ";" + m_varLabels[*ivarD];
	 if (*ivarD == "eta" || *ivarD == "etaAll" || *ivarD == "etaPos" || *ivarD == "etaNeg" ){
	   m_xDistr[*ireg][*ivarD] = new TH1F(hname, htitle, m_nEtaBins, m_varRanges[*ivarD].first, m_varRanges[*ivarD].second);
	 } else if (*ivarD == "pt" || *ivarD == "ptAll" || *ivarD == "ptPos" || *ivarD == "ptNeg" ){
	   m_xDistr[*ireg][*ivarD] = new TH1F(hname, htitle, m_nPtBins, m_varRanges[*ivarD].first, m_varRanges[*ivarD].second);
	 } else if (*ivarD == "phi" || *ivarD == "phiAll" || *ivarD == "phiPos" || *ivarD == "phiNeg" ){
	   m_xDistr[*ireg][*ivarD] = new TH1F(hname, htitle, m_nPhiBins, m_varRanges[*ivarD].first, m_varRanges[*ivarD].second);
	 } else {
	   m_xDistr[*ireg][*ivarD] = new TH1F(hname, htitle, m_nVarBins, m_varRanges[*ivarD].first, m_varRanges[*ivarD].second);
	 }
	 RegisterHisto(dimuMonObj_expert,m_xDistr[*ireg][*ivarD]);
       }
     }
   }

   return StatusCode::SUCCESS;
}


StatusCode DiMuMon::fillHistograms()
{

  //  if (m_lumiBlockNum<402 || m_lumiBlockNum>1330) return StatusCode::SUCCESS;

  double muonMass = 105.66*CLHEP::MeV;
  //retrieve all muons
  const xAOD::MuonContainer* muons(0);
  StatusCode sc = evtStore()->retrieve(muons, m_muonCollection);
  if(sc.isFailure()){
    ATH_MSG_WARNING("Could not retrieve muon container");
    return sc;
  } else ATH_MSG_DEBUG("Muon container successfully retrieved.");

  //make a new container
  xAOD::MuonContainer* goodMuons = new xAOD::MuonContainer( SG::VIEW_ELEMENTS );
  sc = evtStore()->record ( goodMuons, "myGoodMuons" + m_triggerChainName + m_resonName);
  if (!sc.isSuccess()) {
    ATH_MSG_WARNING("Could not record good muon tracks container.");
    return StatusCode::FAILURE;
  }

  //pick out the good muon tracks and store in the new container
  //  MuonContainer::const_iterator iMuon = muons->begin();
  for(const auto* muon : *muons ) {
    //  for (;iMuon!=muons->end();iMuon++){

    const xAOD::TrackParticle *idTrk = muon->trackParticle(xAOD::Muon::InnerDetectorTrackParticle);
    if (!idTrk) continue;
    m_stat->Fill("MuonIdTrk",1);
    double idTrkPt(0),ptSum(0);
    float iso_pt40(0);
    if( !muon->isolation(iso_pt40, xAOD::Iso::ptcone40) ) {
      return StatusCode::FAILURE;
    }
    else {
      idTrkPt = idTrk->pt();
      ptSum = xAOD::Iso::ptcone40;
    }
    if(m_resonName=="Zmumu" && ptSum/idTrkPt > m_isolationCut) continue;
    m_stat->Fill("TrkIsoPt40/Pt<.2",1);
    if (!muon->trackParticle(xAOD::Muon::CombinedTrackParticle)) continue;
    m_stat->Fill("CombMuon",1);

    if (!trackQuality(idTrk)) continue;
    m_stat->Fill("MCPgoodTrk",1);

    if (idTrkPt < m_ptCut*1000) continue;
    m_stat->Fill("MuPtOK",1);

    double idTrkEta = idTrk->eta();
    if (fabs(idTrkEta)>2.5) continue;
    m_stat->Fill("eta<2.5",1);

    //    goodMuons->push_back(const_cast<Analysis::Muon*>(*iMuon));
    goodMuons->push_back(const_cast<xAOD::Muon*>(muon));
  }
  sc = evtStore()->setConst( goodMuons );
  if (!sc.isSuccess()) {
    ATH_MSG_ERROR("Could not set good muon track collection to const");
    return StatusCode::FAILURE;
  }

  //pair up the tracks of the good muons and fill histograms
  int nMuons = goodMuons->size();

  if (nMuons>1){
    //    ATH_MSG_DEBUG("There are at least two good muons");
    xAOD::MuonContainer::const_iterator mu1 = goodMuons->begin();
    xAOD::MuonContainer::const_iterator muEnd = goodMuons->end();
    for (; mu1!=muEnd;mu1++){
      const xAOD::TrackParticle *id1 = (*mu1)->trackParticle(xAOD::Muon::InnerDetectorTrackParticle);
      xAOD::MuonContainer::const_iterator mu2 = mu1+1;
      for (; mu2!=muEnd; mu2++){
	const xAOD::TrackParticle *id2 = (*mu2)->trackParticle(xAOD::Muon::InnerDetectorTrackParticle);
	//consider only opposite sign muons
	double q1 = id1->charge();
	double q2 = id2->charge();
	if (q1*q2>0) continue;
	m_stat->Fill("OppChargePair",1);
	const xAOD::TrackParticle *idPos = id1;
	const xAOD::TrackParticle *idNeg = id2;
	if (q1<0){
	  idPos = id2;
	  idNeg = id1;
	}

	//cut on the pair invariant mass
	double invmass = getInvmass(id1,id2,muonMass);
	if (invmass<m_minInvmass || invmass>m_maxInvmass) continue;
	m_stat->Fill("InvMassOK",1);

	//compute variables
	m_varValues["eta"] = getEta(idPos,idNeg);
	double etaPos = idPos->eta();
	m_varValues["etaPos"] = etaPos;
	double etaNeg = idNeg->eta();
	m_varValues["etaNeg"] = etaNeg;
	m_varValues["phi"] = getPhi(idPos,idNeg);
	double phiPos = idPos->phi();
	m_varValues["phiPos"] = phiPos;
	double phiNeg = idNeg->phi();
	m_varValues["phiNeg"] = phiNeg;
	m_varValues["pt"] = getPt(idPos,idNeg);
	double ptPos = idPos->pt()/CLHEP::GeV;
	m_varValues["ptPos"] = ptPos;
	double ptNeg = idNeg->pt()/CLHEP::GeV;
	m_varValues["ptNeg"] = ptNeg;

	m_varValues["crtDiff"] = getCrtDiff(idPos,idNeg);
	m_varValues["etaDiff"] = etaPos - etaNeg;
	double phiDiff = fabs(phiPos - phiNeg);
	if (phiDiff>CLHEP::pi) phiDiff = 2*(CLHEP::pi) - phiDiff;
	m_varValues["phiDiff"] = phiDiff;
	m_varValues["etaSumm"] = etaPos + etaNeg;
	m_varValues["phiSumm"] = phiPos + phiNeg;

	//determine which region muons are in
	std::string region = "";
	if ((fabs(etaPos)<1.05 || fabs(etaPos)==1.05) && (fabs(etaNeg)<1.05 || fabs(etaNeg)==1.05)) region="BB";
	else if ((etaPos>1.05 && etaPos<2.5) && (etaNeg>1.05 && etaNeg<2.5)) region="EAEA";
	else if ((etaPos<-1.05 && etaPos>-2.5) && (etaNeg<-1.05 && etaNeg>-2.5)) region="ECEC";

	//do we care about hese muons?
	bool fillAll = m_invmass.find("All")!=m_invmass.end();
	bool fillReg = false;
	if (region!=""){
	  if (m_invmass.find(region)!=m_invmass.end()) fillReg=true;
	}
	if (!fillAll && !fillReg) continue;


	//fill invmass histos
	if (fillAll) m_invmass["All"]->Fill(invmass);
	if (fillReg) m_invmass[region]->Fill(invmass);

	//fill 2D histos
	//first, retrieve the overlap of the width and mean variable lists which by construction is the list of vars in the 2D histos map
	std::map<std::string, TH2F*>* allVars = 0;
	if (fillAll){
	  allVars = &m_2DinvmassVSx["All"];
	} else {  //fillReg=true
	  allVars = &m_2DinvmassVSx[region];
	}

	std::map<std::string, TH2F*>::iterator ivar2D = allVars->begin();
	for (ivar2D = allVars->begin();ivar2D != allVars->end(); ivar2D++){
	  if ((*ivar2D).first!="etaAll" && (*ivar2D).first!="phiAll" && (*ivar2D).first!="ptAll"){
	    if (fillAll) m_2DinvmassVSx["All"][(*ivar2D).first]->Fill(m_varValues[(*ivar2D).first],invmass);
	    if (fillReg) m_2DinvmassVSx[region][(*ivar2D).first]->Fill(m_varValues[(*ivar2D).first],invmass);
	  } else if ((*ivar2D).first=="etaAll"){
	    if (fillAll){
	      m_2DinvmassVSx["All"]["etaAll"]->Fill(etaPos,invmass);
	      m_2DinvmassVSx["All"]["etaAll"]->Fill(etaNeg,invmass);
	    }
	    if (fillReg){
	      m_2DinvmassVSx[region]["etaAll"]->Fill(etaPos,invmass);
	      m_2DinvmassVSx[region]["etaAll"]->Fill(etaNeg,invmass);
	    }
	  } else if ((*ivar2D).first=="phiAll"){
	    if (fillAll){
	      m_2DinvmassVSx["All"]["phiAll"]->Fill(phiPos,invmass);
	      m_2DinvmassVSx["All"]["phiAll"]->Fill(phiNeg,invmass);
	    }
	    if (fillReg){
	      m_2DinvmassVSx[region]["phiAll"]->Fill(phiPos,invmass);
	      m_2DinvmassVSx[region]["phiAll"]->Fill(phiNeg,invmass);
	    }
	  } else if ((*ivar2D).first=="ptAll"){
	    if (fillAll){
	      m_2DinvmassVSx["All"]["ptAll"]->Fill(ptPos,invmass);
	      m_2DinvmassVSx["All"]["ptAll"]->Fill(ptNeg,invmass);
	    }
	    if (fillReg){
	      m_2DinvmassVSx[region]["ptAll"]->Fill(ptPos,invmass);
	      m_2DinvmassVSx[region]["ptAll"]->Fill(ptNeg,invmass);
	    }
	  }
	}

	//fill var distributions
	//here we already know the list of variables so no need for gymnastics
	std::vector<std::string>::iterator ivarD = m_varsDistr.begin();
	for (ivarD = m_varsDistr.begin();ivarD != m_varsDistr.end(); ivarD++){
	  if (*ivarD!="etaAll" && *ivarD!="phiAll" && *ivarD!="ptAll"){
	    if (fillAll) m_xDistr["All"][*ivarD]->Fill(m_varValues[*ivarD]);
	    if (fillReg) m_xDistr[region][*ivarD]->Fill(m_varValues[*ivarD]);
	  } else if (*ivarD=="etaAll"){
	    if (fillAll){
	      m_xDistr["All"]["etaAll"]->Fill(etaPos);
	      m_xDistr["All"]["etaAll"]->Fill(etaNeg);
	    }
	    if (fillReg){
	      m_xDistr[region]["etaAll"]->Fill(etaPos);
	      m_xDistr[region]["etaAll"]->Fill(etaNeg);
	    }
	  } else if (*ivarD=="phiAll"){
	    if (fillAll){
	      m_xDistr["All"]["phiAll"]->Fill(phiPos);
	      m_xDistr["All"]["phiAll"]->Fill(phiNeg);
	    }
	    if (fillReg){
	      m_xDistr[region]["phiAll"]->Fill(phiPos);
	      m_xDistr[region]["phiAll"]->Fill(phiNeg);
	    }
	  } else if (*ivarD=="ptAll"){
	    if (fillAll){
	      m_xDistr["All"]["ptAll"]->Fill(ptPos);
	      m_xDistr["All"]["ptAll"]->Fill(ptNeg);
	    }
	    if (fillReg){
	      m_xDistr[region]["ptAll"]->Fill(ptPos);
	      m_xDistr[region]["ptAll"]->Fill(ptNeg);
	    }
	  }
	}


      }// mu2 loop
    }// mu1 loop
  }// do we have more than 1 good muon?
   return StatusCode::SUCCESS;
}


//StatusCode DiMuMon::procHistograms( bool isEndOfEventsBlock, bool isEndOfLumiBlock, bool isEndOfRun )
StatusCode DiMuMon::procHistograms()
{

   //if (endOfLowStatFlag() || endOfLumiBlockFlag()){ }

   if(endOfRunFlag() && m_doFits) {
     std::vector<std::string> ::iterator ireg = m_regions.begin();
     for (ireg = m_regions.begin(); ireg != m_regions.end(); ireg++){
       std::map<std::string, TH2F*>::iterator ivar2D = m_2DinvmassVSx[*ireg].begin();
       for (ivar2D = m_2DinvmassVSx[*ireg].begin();ivar2D != m_2DinvmassVSx[*ireg].end(); ivar2D++){
	 int mode = 999.; //0 = both, 1 = mean only, 2 = wdth only
	 std::vector<TH1F*> hout;//1D histograms for fit results
	 if (m_invmassVSx[*ireg].find((*ivar2D).first)!=m_invmassVSx[*ireg].end()){
	   if (m_widthVSx[*ireg].find((*ivar2D).first)!=m_widthVSx[*ireg].end()) {
	     mode =0;
	     hout.push_back(m_invmassVSx[*ireg][(*ivar2D).first]);
	     hout.push_back(m_widthVSx[*ireg][(*ivar2D).first]);
	   } else {
	     mode = 1;
	     hout.push_back(m_invmassVSx[*ireg][(*ivar2D).first]);
	   }
	 } else {
	   mode = 2;
	   hout.push_back(m_widthVSx[*ireg][(*ivar2D).first]);
	 }
	 iterativeGausFit(m_2DinvmassVSx[*ireg][(*ivar2D).first],hout,mode);
       } //variables
     } //regions
   } //isEndOfRun
   return StatusCode::SUCCESS;
}


void DiMuMon::iterativeGausFit (TH2F* hin, std::vector<TH1F*> hout, int mode){
  // a canvas may be needed when implmenting this into the post-processing file
  TString hname =  hin->GetName();
  TString psName = hname + m_triggerChainName + ".ps";
  TCanvas* ctemp = new TCanvas("ctemp","ctemp",500,500);
  if (m_doSaveFits) ctemp->Print(psName+"[");
  hin->SetMarkerSize(1.2);
  hin->Draw();
  if (m_doSaveFits) ctemp->Print(psName);
  int nbins=hin->GetNbinsX();
  for (int i=0; i<nbins;i++){
    std::ostringstream o; o<<i;
    TString projName = hname + o.str();
    TH1D* htemp = (TH1D*) (hin->ProjectionY(projName,i+1,i+1)->Clone());
    htemp->SetTitle(projName);
    htemp->Sumw2();
    if (htemp->GetEntries()>50){
      double mean = 999., meanErr = 999., sigma = 999., sigmaErr = 999., chi2=0;
      if (m_resonName == "Jpsi" || m_resonName == "Upsi"){
	mean = htemp->GetMean();
	sigma= htemp->GetRMS();
	TF1* fn = new TF1("fn","gaus",mean-2*sigma,mean+2*sigma);
	fn->SetParameters(float(htemp->GetEntries())/10.,mean,sigma);
	htemp->Fit("fn","RMLQN");
	mean = fn->GetParameter(1);
	sigma= fn->GetParameter(2);
	fn->SetRange(mean-1.2*sigma,mean+1.2*sigma);
	fn->SetParameters(float(htemp->GetEntries())/10.,mean,sigma);
	gStyle->SetOptStat(1);
	if (m_doSaveFits) {
	  htemp->Fit("fn","RML");
	  ctemp->Print(psName);
	} else htemp->Fit("fn","RMLQN");
	double frange = 2.4*sigma;
	double hrange = htemp->GetXaxis()->GetXmax()-htemp->GetXaxis()->GetXmin();
	double ndf = frange/hrange*(htemp->GetNbinsX()) - 3;//substract the fit parameters
	chi2 = (fn->GetChisquare())/ndf;
	//fill results
	mean = fn->GetParameter(1);
	meanErr = fn->GetParError(1);
	sigma = fn->GetParameter(2);
	sigmaErr = fn->GetParError(2);
	delete fn;
      } else {
	//fit Z peak with a convolution of BreitWigner and Crystal Ball fns, fit by Louise, implementation by Jike taken from IDPerfMon
	RooMsgService::instance().setGlobalKillBelow(RooFit::FATAL);
	RooRealVar  m("mass", "dimuon invariant mass", 91.2, 71., 111., "GeV");
	RooDataHist *data = 0;
	data = new RooDataHist("data", "data", m, htemp);
	RooRealVar bwm0("bw_#mu",   "bw_#mu",   91.2, 85.2, 97.2) ;
	RooRealVar bwsg("bw_#sigma","bw_#sigma", 2.4952) ;
	RooBreitWigner bw("bw","bw", m, bwm0, bwsg);

	RooRealVar cbm0("cb_#mu",    "cb_#mu",  0  ) ;
	RooRealVar cbsg("cb_#sigma", "cb_#sigma", 3., 1.,  10.) ;
	RooRealVar cbal("cb_#alpha", "cb_#alpha", 2.0) ;
	RooRealVar cbn( "cb_n",      "cb_n",      1., 0.05, 3.) ;
	RooCBShape cb(  "cb",        "cb", m, cbm0, cbsg, cbal, cbn);

	m.setBins(5000);
	RooFFTConvPdf bxc("bxc", "BW (X) CB", m, bw, cb) ;
	bxc.fitTo(*data, RooFit::PrintLevel(-1), RooFit::FitOptions("QN"), RooFit::PrintEvalErrors(-1),"Q");
	RooPlot* frame = m.frame();
	data->plotOn(frame, RooFit::MarkerSize(0.9));
	bxc.paramOn(frame,  RooFit::Format("NELU", RooFit::AutoPrecision(2)), RooFit::Layout(0.1,0.4,0.9));
	bxc.plotOn (frame,  RooFit::LineColor(kBlue));
	//	cb.plotOn(frame,    RooFit::LineColor(kRed));
	//bw.plotOn(frame,    RooFit::LineStyle(kDashed)) ;
	if (m_doSaveFits) {
	  frame->Draw();
	  ctemp->Print(psName);
	}
	mean = bwm0.getVal();
	meanErr = bwm0.getError();
	sigma = cbsg.getVal();
	sigmaErr = cbsg.getError();
	chi2 = frame->chiSquare();
	delete data;
      }
      //fill results
      m_chi2->Fill(chi2);
      if (mode == 0){//plot both invmass and width vs variable
	hout.at(0)->SetBinContent(i+1,mean);
	hout.at(0)->SetBinError(i+1,meanErr);
	hout.at(1)->SetBinContent(i+1,sigma);
	hout.at(1)->SetBinError(i+1,sigmaErr);
      } else if (mode==1){//plot only invmass vs variable
	hout.at(0)->SetBinContent(i+1,mean);
	hout.at(0)->SetBinError(i+1,meanErr);
      } else if (mode==2){//plot only width vs variable
	hout.at(0)->SetBinContent(i+1,sigma);
	hout.at(0)->SetBinError(i+1,sigmaErr);
      }

    }// more than 50 events

    delete htemp;
  }
  hout.at(0)->Sumw2();
  if (hout.size()==2) hout.at(1)->Sumw2();
  if (m_doSaveFits) ctemp->Print(psName+"]");
  delete ctemp;
}

template<class T>
void DiMuMon::RegisterHisto(MonGroup& mon, T* histo) {

  StatusCode sc = mon.regHist(histo);
  if (sc.isFailure() ) {
    msg(MSG::WARNING) << "Cannot book histogram:" << endmsg;
  }
}

bool DiMuMon::trackQuality(const xAOD::TrackParticle *idTrk){
  int countPass = 0;
  //  const Trk::TrackSummary *idTrkSumm = idTrk->trackSummary();
  // https://twiki.cern.ch/twiki/bin/view/AtlasProtected/MCPAnalysisGuidelinesEPS2011
  if (idTrk) {
    uint8_t dummy(-1);
    bool eBLhits = idTrk->summaryValue( dummy, xAOD::expectInnermostPixelLayerHit )? dummy :-1;
    int  nBLhits = idTrk->summaryValue( dummy, xAOD::numberOfInnermostPixelLayerHits )? dummy :-1;
    int nhitsPIX = idTrk->summaryValue( dummy, xAOD::numberOfPixelHits )? dummy :-1;
    int nPIXDS = idTrk->summaryValue( dummy, xAOD::numberOfPixelDeadSensors )? dummy :-1;
    int nhitsSCT = idTrk->summaryValue( dummy, xAOD::numberOfSCTHits )? dummy :-1;
    int nSCTDS = idTrk->summaryValue( dummy, xAOD::numberOfSCTDeadSensors )? dummy :-1;
    int nPIXH = idTrk->summaryValue( dummy, xAOD::numberOfPixelHoles )? dummy :-1;
    int nSCTH = idTrk->summaryValue( dummy, xAOD::numberOfSCTHoles )? dummy :-1;

  if (nBLhits>0 || !(eBLhits)) countPass+=1;
  if ((nhitsPIX+nPIXDS)>1) countPass+=1;
  if ((nhitsSCT+nSCTDS)>5) countPass+=1;
  if ((nPIXH+nSCTH)<3) countPass+=1;
  int nTRTout = idTrk->summaryValue( dummy, xAOD::numberOfTRTOutliers )? dummy :-1;
  int nTRThits = idTrk->summaryValue( dummy, xAOD::numberOfTRTHits )? dummy :-1;
  int n = nTRTout + nTRThits;
  if (fabs(idTrk->eta())<1.9){
    if (n>5 && nTRTout<(0.9*n)) countPass+=1;
  } else {
    if (n>5){
      if (nTRTout<(0.9*n)) countPass+=1;
    } else countPass+=1;
  }
  }
  else{
    ATH_MSG_WARNING("Trying to check trackquality but no xAOD::TrackParticle available!");
  }
  return countPass==5;
}

//methods from IDPerformanceMoniroting by Weina
double DiMuMon::getInvmass(const xAOD::TrackParticle* id1, const xAOD::TrackParticle* id2, double Mass) const {
  TLorentzVector particle1,particle2,v;
  particle1.SetPtEtaPhiE(id1->pt(),id1->eta(),id1->phi(),sqrt(pow(Mass,2)+pow(id1->p4().Px(),2)+pow(id1->p4().Py(),2)+pow(id1->p4().Pz(),2)));
  particle2.SetPtEtaPhiE(id2->pt(),id2->eta(),id2->phi(),sqrt(pow(Mass,2)+pow(id2->p4().Px(),2)+pow(id2->p4().Py(),2)+pow(id2->p4().Pz(),2)));
  v=particle1+particle2;
  double invmass = v.Mag()/CLHEP::GeV;
  return invmass;
}

double DiMuMon::getPt(const xAOD::TrackParticle* id1, const xAOD::TrackParticle* id2 ) const {
  double transmom;
  double px = id1->p4().Px()+id2->p4().Px();
  double py = id1->p4().Py()+id2->p4().Py();
  transmom=sqrt(px*px+py*py);
  return transmom/CLHEP::GeV;  //Gev
}

double DiMuMon::getEta(const xAOD::TrackParticle* id1, const xAOD::TrackParticle* id2 ) const {
  double eta;
  double px=id1->p4().Px()+id2->p4().Px();
  double py = id1->p4().Py()+id2->p4().Py();
  double pz = id1->p4().Pz()+id2->p4().Pz();
  double p=sqrt(px*px+py*py+pz*pz);
  eta=0.5*log((p+pz)/(p-pz));
  return eta;
}

double DiMuMon::getPhi(const xAOD::TrackParticle* id1, const xAOD::TrackParticle* id2 ) const {
  double px=id1->p4().Px()+id2->p4().Px();
  double py = id1->p4().Py()+id2->p4().Py();
  double p=sqrt(px*px+py*py);
  double phi=acos(px/p);
  if (py<0.) phi=-1.*phi;
  return phi;
}
//curvature difference unit 1/GeV
double DiMuMon::getCrtDiff(const xAOD::TrackParticle* id1, const xAOD::TrackParticle* id2 ) const {
  double qoverpt1=id1->charge()/id1->pt();
  double qoverpt2=id2->charge()/id2->pt();
  double asym;
  asym=fabs(qoverpt1)-fabs(qoverpt2);
  return asym*1000;
}


