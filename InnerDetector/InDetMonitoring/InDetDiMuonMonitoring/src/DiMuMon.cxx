/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include <sstream>
#include "GaudiKernel/PhysicalConstants.h"
#include "AthContainers/ConstDataVector.h"

#include "DiMuMon.h"

#include "CxxUtils/checker_macros.h"

#include <cmath>

#include "TF1.h"
#include "TCanvas.h"
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
{
   declareProperty( "resonName", m_resonName = "Zmumu" );
   declareProperty( "triggerChainName", m_triggerChainName = "NoTrig" );
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
  ATH_CHECK( m_muonCollection.initialize() );

  if (m_regions.empty()) {
    m_regions.emplace_back("All");
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
  m_varRanges["phi"] = std::make_pair(-Gaudi::Units::pi,Gaudi::Units::pi);
  m_varRanges["phiAll"] = std::make_pair(-Gaudi::Units::pi,Gaudi::Units::pi);
  m_varRanges["phiPos"] = std::make_pair(-Gaudi::Units::pi,Gaudi::Units::pi);
  m_varRanges["phiNeg"] = std::make_pair(-Gaudi::Units::pi,Gaudi::Units::pi);
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
    m_varRanges["phiDiff"] = std::make_pair(0.,Gaudi::Units::pi);
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

  
  m_coneSize = 0.4;
  m_isolationCut = 0.2;

  return StatusCode::SUCCESS;

}

StatusCode DiMuMon::bookHistograms()
{


    MonGroup dimuMonObj_shift( this, "DiMuMon/"+m_resonName+"/" + m_triggerChainName,  run );
    MonGroup dimuMonObj_expert( this, "DiMuMon/"+m_resonName+"/" + m_triggerChainName + "_detail", run );

    if ( AthenaMonManager::environment() == AthenaMonManager::online ) {
      // book histograms that are only made in the online environment...
    }

    if ( AthenaMonManager::dataType() == AthenaMonManager::cosmics ) {
      // book histograms that are only relevant for cosmics data...
    }




   if( newRunFlag() ) {


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
     for (const std::string& reg : m_regions) {

       //declare region-dependent eta range
       if (reg == "BB") {
	 m_varRanges["etaAll"] = std::make_pair(-1.05,1.05);
	 m_varRanges["etaPos"] = std::make_pair(-1.05,1.05);
	 m_varRanges["etaNeg"] = std::make_pair(-1.05,1.05);
       } else if (reg == "EAEA") {
	 m_varRanges["etaAll"] = std::make_pair(1.05,2.5);
	 m_varRanges["etaPos"] = std::make_pair(1.05,2.5);
	 m_varRanges["etaNeg"] = std::make_pair(1.05,2.5);
       } else if (reg == "ECEC") {
	 m_varRanges["etaAll"] = std::make_pair(-2.5,-1.05);
	 m_varRanges["etaPos"] = std::make_pair(-2.5,-1.05);
	 m_varRanges["etaNeg"] = std::make_pair(-2.5,-1.05);
       } else {
	 m_varRanges["etaAll"] = std::make_pair(-2.5,2.5);
	 m_varRanges["etaPos"] = std::make_pair(-2.5,2.5);
	 m_varRanges["etaNeg"] = std::make_pair(-2.5,2.5);
       }

       //mass plots
       TString hname = m_resonName + "_invmass_" + reg;
       TString htitle = hname + "; Invmass[GeV/c^{2}]";
       m_invmass[reg] = new TH1F(hname, htitle, m_nMassBins, m_minInvmass, m_maxInvmass);
       RegisterHisto(dimuMonObj_shift,m_invmass[reg]);

       //for each var vs mass plot
       for (const std::string& varM : m_varsVSmean) {
	 hname = m_resonName + "_2DinvmassVS" + varM + "_" + reg;
	 htitle = hname + ";" + m_varLabels[varM] + ";Invmass[GeV/c^{2}]";
	 m_2DinvmassVSx[reg][varM] = new TH2F(hname, htitle ,m_nVarBins,m_varRanges[varM].first, m_varRanges[varM].second, m_nMassBins, m_minInvmass, m_maxInvmass);
	 RegisterHisto(dimuMonObj_shift,m_2DinvmassVSx[reg][varM]);

	 hname = m_resonName + "_invmassVS" + varM + "_" + reg;
	 htitle = hname + ";" + m_varLabels[varM] + ";Invmass[GeV/c^{2}]";
	 m_invmassVSx[reg][varM] = new TH1F(hname, htitle, m_nVarBins,m_varRanges[varM].first, m_varRanges[varM].second);
	 RegisterHisto(dimuMonObj_shift,m_invmassVSx[reg][varM]);
       }

       //for each var vs width plot
       for (const std::string& varW : m_varsVSwidth) {
	 //book the corresponding 2D histo if it hasn't already been booked
	 if (m_2DinvmassVSx[reg].find(varW)==m_2DinvmassVSx[reg].end()){
	   hname = m_resonName + "_2DinvmassVS" + varW + "_" + reg;
	   htitle = hname + ";" + m_varLabels[varW] + ";Invmass[GeV/c^{2}]";
	   m_2DinvmassVSx[reg][varW] = new TH2F(hname, htitle ,m_nVarBins, m_varRanges[varW].first, m_varRanges[varW].second, m_nMassBins, m_minInvmass, m_maxInvmass);
	   RegisterHisto(dimuMonObj_shift,m_2DinvmassVSx[reg][varW]);
	 }

	 hname = m_resonName + "_widthVS" + varW + "_" + reg;
	 htitle = hname + ";" + m_varLabels[varW] + ";Width[GeV/c^{2}]";
	 m_widthVSx[reg][varW] = new TH1F(hname,htitle ,m_nVarBins,m_varRanges[varW].first, m_varRanges[varW].second);
	 RegisterHisto(dimuMonObj_expert,m_widthVSx[reg][varW]);

       }

       //for the each variable's distribution
       for (const std::string& varD: m_varsDistr) {
	 hname = m_resonName + "_" + varD + "_" + reg;
	 htitle = hname + ";" + m_varLabels[varD];
	 if (varD == "eta" || varD == "etaAll" || varD == "etaPos" || varD == "etaNeg" ){
	   m_xDistr[reg][varD] = new TH1F(hname, htitle, m_nEtaBins, m_varRanges[varD].first, m_varRanges[varD].second);
	 } else if (varD == "pt" || varD == "ptAll" || varD == "ptPos" || varD == "ptNeg" ){
	   m_xDistr[reg][varD] = new TH1F(hname, htitle, m_nPtBins, m_varRanges[varD].first, m_varRanges[varD].second);
	 } else if (varD == "phi" || varD == "phiAll" || varD == "phiPos" || varD == "phiNeg" ){
	   m_xDistr[reg][varD] = new TH1F(hname, htitle, m_nPhiBins, m_varRanges[varD].first, m_varRanges[varD].second);
	 } else {
	   m_xDistr[reg][varD] = new TH1F(hname, htitle, m_nVarBins, m_varRanges[varD].first, m_varRanges[varD].second);
	 }
	 RegisterHisto(dimuMonObj_expert,m_xDistr[reg][varD]);
       }
     }
   }

   return StatusCode::SUCCESS;
}


StatusCode DiMuMon::fillHistograms()
{

  const double muonMass = 105.66*Gaudi::Units::MeV;
  //retrieve all muons
  SG::ReadHandle<xAOD::MuonContainer> muons{m_muonCollection};
  if(!muons.isValid()){
    ATH_MSG_WARNING("Could not retrieve muon container");
    return StatusCode::FAILURE;
  } else ATH_MSG_DEBUG("Muon container successfully retrieved.");

  //make a new container
  ConstDataVector<xAOD::MuonContainer> goodMuons( SG::VIEW_ELEMENTS );

  //pick out the good muon tracks and store in the new container
  for(const auto* muon : *muons ) {

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
    if (std::abs(idTrkEta)>2.5) continue;
    m_stat->Fill("eta<2.5",1);

    goodMuons.push_back(muon);
  }

  //pair up the tracks of the good muons and fill histograms
  int nMuons = goodMuons.size();

  if (nMuons>1){
    xAOD::MuonContainer::const_iterator mu1 = goodMuons.begin();
    xAOD::MuonContainer::const_iterator muEnd = goodMuons.end();
    for (; mu1!=muEnd;++mu1){
      const xAOD::TrackParticle *id1 = (*mu1)->trackParticle(xAOD::Muon::InnerDetectorTrackParticle);
      xAOD::MuonContainer::const_iterator mu2 = mu1+1;
      for (; mu2!=muEnd; ++mu2){
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
	double ptPos = idPos->pt()/Gaudi::Units::GeV;
	m_varValues["ptPos"] = ptPos;
	double ptNeg = idNeg->pt()/Gaudi::Units::GeV;
	m_varValues["ptNeg"] = ptNeg;

	m_varValues["crtDiff"] = getCrtDiff(idPos,idNeg);
	m_varValues["etaDiff"] = etaPos - etaNeg;
	double phiDiff = std::abs(phiPos - phiNeg);
	if (phiDiff>Gaudi::Units::pi) phiDiff = 2*(Gaudi::Units::pi) - phiDiff;
	m_varValues["phiDiff"] = phiDiff;
	m_varValues["etaSumm"] = etaPos + etaNeg;
	m_varValues["phiSumm"] = phiPos + phiNeg;

	//determine which region muons are in
	std::string region = "";
	if ((std::abs(etaPos)<1.05 || std::abs(etaPos)==1.05) && (std::abs(etaNeg)<1.05 || std::abs(etaNeg)==1.05)) region="BB";
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
	std::map<std::string, TH2F*>* allVars = nullptr;
	if (fillAll){
	  allVars = &m_2DinvmassVSx["All"];
	} else {  //fillReg=true
	  allVars = &m_2DinvmassVSx[region];
	}

        for (const std::pair<const std::string, TH2F*>& p : *allVars) {
	  if (p.first!="etaAll" && p.first!="phiAll" && p.first!="ptAll"){
	    if (fillAll) m_2DinvmassVSx["All"][p.first]->Fill(m_varValues[p.first],invmass);
	    if (fillReg) m_2DinvmassVSx[region][p.first]->Fill(m_varValues[p.first],invmass);
	  } else if (p.first=="etaAll"){
	    if (fillAll){
	      m_2DinvmassVSx["All"]["etaAll"]->Fill(etaPos,invmass);
	      m_2DinvmassVSx["All"]["etaAll"]->Fill(etaNeg,invmass);
	    }
	    if (fillReg){
	      m_2DinvmassVSx[region]["etaAll"]->Fill(etaPos,invmass);
	      m_2DinvmassVSx[region]["etaAll"]->Fill(etaNeg,invmass);
	    }
	  } else if (p.first=="phiAll"){
	    if (fillAll){
	      m_2DinvmassVSx["All"]["phiAll"]->Fill(phiPos,invmass);
	      m_2DinvmassVSx["All"]["phiAll"]->Fill(phiNeg,invmass);
	    }
	    if (fillReg){
	      m_2DinvmassVSx[region]["phiAll"]->Fill(phiPos,invmass);
	      m_2DinvmassVSx[region]["phiAll"]->Fill(phiNeg,invmass);
	    }
	  } else if (p.first=="ptAll"){
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
        for (const std::string& varD : m_varsDistr) {
	  if (varD!="etaAll" && varD!="phiAll" && varD!="ptAll"){
	    if (fillAll) m_xDistr["All"][varD]->Fill(m_varValues[varD]);
	    if (fillReg) m_xDistr[region][varD]->Fill(m_varValues[varD]);
	  } else if (varD=="etaAll"){
	    if (fillAll){
	      m_xDistr["All"]["etaAll"]->Fill(etaPos);
	      m_xDistr["All"]["etaAll"]->Fill(etaNeg);
	    }
	    if (fillReg){
	      m_xDistr[region]["etaAll"]->Fill(etaPos);
	      m_xDistr[region]["etaAll"]->Fill(etaNeg);
	    }
	  } else if (varD=="phiAll"){
	    if (fillAll){
	      m_xDistr["All"]["phiAll"]->Fill(phiPos);
	      m_xDistr["All"]["phiAll"]->Fill(phiNeg);
	    }
	    if (fillReg){
	      m_xDistr[region]["phiAll"]->Fill(phiPos);
	      m_xDistr[region]["phiAll"]->Fill(phiNeg);
	    }
	  } else if (varD=="ptAll"){
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


StatusCode DiMuMon::procHistograms()
{


   if(endOfRunFlag() && m_doFits) {
     for (const std::string& reg : m_regions) {
       for (const std::pair<const std::string, TH2F*>& p : m_2DinvmassVSx[reg]) {
	 int mode = 999.; //0 = both, 1 = mean only, 2 = wdth only
	 std::vector<TH1F*> hout;//1D histograms for fit results
	 if (m_invmassVSx[reg].find(p.first)!=m_invmassVSx[reg].end()){
	   if (m_widthVSx[reg].find(p.first)!=m_widthVSx[reg].end()) {
	     mode =0;
	     hout.push_back(m_invmassVSx[reg][p.first]);
	     hout.push_back(m_widthVSx[reg][p.first]);
	   } else {
	     mode = 1;
	     hout.push_back(m_invmassVSx[reg][p.first]);
	   }
	 } else {
	   mode = 2;
	   hout.push_back(m_widthVSx[reg][p.first]);
	 }
	 iterativeGausFit(m_2DinvmassVSx[reg][p.first],hout,mode);
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
	//gStyle->SetOptStat(1); // not thread-safe
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
	RooDataHist *data = nullptr;
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
    ATH_MSG_WARNING( "Cannot book histogram:" );
  }
}

bool DiMuMon::trackQuality(const xAOD::TrackParticle *idTrk){
  int countPass = 0;
  // https://twiki.cern.ch/twiki/bin/view/AtlasProtected/MCPAnalysisGuidelinesEPS2011
  if (idTrk) {
    uint8_t dummy(-1);
    bool eBLhits = idTrk->summaryValue( dummy, xAOD::expectInnermostPixelLayerHit )? dummy :false;
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
  if (std::abs(idTrk->eta())<1.9){
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
  double invmass = v.Mag()/Gaudi::Units::GeV;
  return invmass;
}

double DiMuMon::getPt(const xAOD::TrackParticle* id1, const xAOD::TrackParticle* id2 ) const {
  double transmom;
  double px = id1->p4().Px()+id2->p4().Px();
  double py = id1->p4().Py()+id2->p4().Py();
  transmom=sqrt(px*px+py*py);
  return transmom/Gaudi::Units::GeV;  //Gev
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
  asym=std::abs(qoverpt1)-std::abs(qoverpt2);
  return asym*1000;
}


