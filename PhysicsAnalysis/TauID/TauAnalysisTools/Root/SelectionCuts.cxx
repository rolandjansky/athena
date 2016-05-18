// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


// local include(s)
#include "TauAnalysisTools/SelectionCuts.h"
#include "TauAnalysisTools/TauSelectionTool.h"

// ROOT include(s)
#include "TFile.h"

using namespace TauAnalysisTools;

//______________________________________________________________________________
SelectionCut::SelectionCut(std::string sName, TauAnalysisTools::TauSelectionTool* tTST)
  : m_sName(sName)
  , m_hHistCutPre(0)
  , m_hHistCut(0)
  , m_tTST(tTST)
{
}

//______________________________________________________________________________
SelectionCut::~SelectionCut()
{
  delete m_hHistCutPre;
  delete m_hHistCut;
}

//______________________________________________________________________________
void SelectionCut::writeControlHistograms()
{
  m_hHistCutPre->Write();
  m_hHistCut->Write();
}

//______________________________________________________________________________
TH1F* SelectionCut::CreateControlPlot(const char* sName, const char* sTitle, int iBins, double dXLow, double dXUp)
{
  // hHist.SetDirectory(0);
  if (m_tTST->m_bCreateControlPlots)
  {
    TH1F* hHist = new TH1F(sName, sTitle, iBins, dXLow, dXUp);
    hHist->SetDirectory(0);
    return hHist;
  }

  return NULL;
}

//______________________________________________________________________________
void SelectionCut::fillHistogramCutPre(const xAOD::TauJet& xTau)
{
  fillHistogram(xTau, *m_hHistCutPre);
}

//______________________________________________________________________________
void SelectionCut::fillHistogramCut(const xAOD::TauJet& xTau)
{
  fillHistogram(xTau, *m_hHistCut);
}

//_______________________________SelectionCutPt_________________________________
//______________________________________________________________________________
SelectionCutPt::SelectionCutPt(TauSelectionTool* tTST)
  : SelectionCut("CutPt", tTST)
{
  m_hHistCutPre = CreateControlPlot("hPt_pre","Pt_pre;#tau-p_{T} [GeV]; events",100,0,100);
  m_hHistCut = CreateControlPlot("hPt_cut","Pt_cut;#tau-p_{T} [GeV]; events",100,0,100);
}

//______________________________________________________________________________
void SelectionCutPt::fillHistogram(const xAOD::TauJet& xTau, TH1F& hHist)
{
  hHist.Fill(xTau.pt()/1000.);
}

//______________________________________________________________________________
bool SelectionCutPt::accept(const xAOD::TauJet& xTau)
{
  m_tTST->m_aAccept.addCut( "Pt",
                            "Selection of taus according to their transverse momentum" );
  // save tau pt in GeV
  double pt = xTau.pt() / 1000.;
  // in case of only one entry in vector, run for lower limits
  if (m_tTST->m_vPtRegion.size() == 1)
  {
    if ( pt >= m_tTST->m_vPtRegion.at(0) )
    {
      m_tTST->m_aAccept.setCutResult( "Pt", true );
      return true;
    }
  }
  unsigned int iNumPtRegion = m_tTST->m_vPtRegion.size()/2;
  for( unsigned int iPtRegion = 0; iPtRegion < iNumPtRegion; iPtRegion++ )
  {
    if ( pt >= m_tTST->m_vPtRegion.at(iPtRegion*2) and pt <= m_tTST->m_vPtRegion.at(iPtRegion*2+1))
    {
      m_tTST->m_aAccept.setCutResult( "Pt", true );
      return true;
    }
  }
  m_tTST->msg() << MSG::VERBOSE << "Tau failed pt requirement, tau pt [GeV]: " << pt << endmsg;
  return false;
}

//_____________________________SelectionCutAbsEta_______________________________
//______________________________________________________________________________
SelectionCutAbsEta::SelectionCutAbsEta(TauSelectionTool* tTST)
  : SelectionCut("CutAbsEta", tTST)
{
  m_hHistCutPre = CreateControlPlot("hEta_pre","Eta_pre;#tau-#eta; events",100,-3,3);
  m_hHistCut = CreateControlPlot("hEta_cut","Eta_cut;#tau-#eta; events",100,-3,3);
}

//______________________________________________________________________________
void SelectionCutAbsEta::fillHistogram(const xAOD::TauJet& xTau, TH1F& hHist)
{
  hHist.Fill(xTau.eta());
}

//______________________________________________________________________________
bool SelectionCutAbsEta::accept(const xAOD::TauJet& xTau)
{
  // check regions of eta, if tau is in one region then return true; false otherwise
  m_tTST->m_aAccept.addCut( "AbsEta",
                            "Selection of taus according to their absolute pseudorapidity" );
  unsigned int iNumEtaRegion = m_tTST->m_vAbsEtaRegion.size()/2;
  for( unsigned int iEtaRegion = 0; iEtaRegion < iNumEtaRegion; iEtaRegion++ )
  {
    if ( std::abs( xTau.eta() ) >= m_tTST->m_vAbsEtaRegion.at(iEtaRegion*2) and std::abs( xTau.eta() ) <= m_tTST->m_vAbsEtaRegion.at(iEtaRegion*2+1))
    {
      m_tTST->m_aAccept.setCutResult( "AbsEta", true );
      return true;
    }
  }
  m_tTST->msg() << MSG::VERBOSE << "Tau failed eta requirement, tau eta: " << xTau.eta() << endmsg;
  return false;
}

//____________________________SelectionCutAbsCharge_____________________________
//______________________________________________________________________________
SelectionCutAbsCharge::SelectionCutAbsCharge(TauSelectionTool* tTST)
  : SelectionCut("CutAbsCharge", tTST)
{
  m_hHistCutPre = CreateControlPlot("hCharge_pre","Charge_pre;charge; events",7,-3.5,3.5);
  m_hHistCut = CreateControlPlot("hCharge_cut","Charge_cut;charge; events",7,-3.5,3.5);
}

//______________________________________________________________________________
void SelectionCutAbsCharge::fillHistogram(const xAOD::TauJet& xTau, TH1F& hHist)
{
  hHist.Fill(xTau.charge());
}

//______________________________________________________________________________
bool SelectionCutAbsCharge::accept(const xAOD::TauJet& xTau)
{
  // check charge, if tau has one of the charges requiered then return true; false otherwise
  m_tTST->m_aAccept.addCut( "AbsCharge",
                            "Selection of taus according to their absolute charge" );
  for( unsigned int iCharge = 0; iCharge < m_tTST->m_vAbsCharges.size(); iCharge++ )
  {
    if ( std::abs( xTau.charge() ) == m_tTST->m_vAbsCharges.at(iCharge) )
    {
      m_tTST->m_aAccept.setCutResult( "AbsCharge", true );
      return true;
    }
  }
  m_tTST->msg() << MSG::VERBOSE << "Tau failed charge requirement, tau charge: " << xTau.charge() << endmsg;
  return false;
}

//_____________________________SelectionCutNTracks______________________________
//______________________________________________________________________________
SelectionCutNTracks::SelectionCutNTracks(TauSelectionTool* tTST)
  : SelectionCut("CutNTrack", tTST)
{
  m_hHistCutPre = CreateControlPlot("hNTrack_pre","NTrack_pre;number of tracks; events",22,-1.5,20.5);
  m_hHistCut = CreateControlPlot("hNTrack_cut","NTrack_cut;number of tracks; events",22,-1.5,20.5);
}

//______________________________________________________________________________
void SelectionCutNTracks::fillHistogram(const xAOD::TauJet& xTau, TH1F& hHist)
{
  hHist.Fill(xTau.nTracks());
}

//______________________________________________________________________________
bool SelectionCutNTracks::accept(const xAOD::TauJet& xTau)
{
  // check track multiplicity, if tau has one of the number of tracks requiered then return true; false otherwise
  m_tTST->m_aAccept.addCut( "NTrack",
                            "Selection of taus according to their number of associated tracks" );
  for( size_t iNumTrack = 0; iNumTrack < m_tTST->m_vNTracks.size(); iNumTrack++ )
  {
    if ( xTau.nTracks() == m_tTST->m_vNTracks.at(iNumTrack) )
    {
      m_tTST->m_aAccept.setCutResult( "NTrack", true );
      return true;
    }
  }
  m_tTST->msg() << MSG::VERBOSE << "Tau failed nTracks requirement, tau number of tracks: " << xTau.nTracks() << endmsg;
  return false;
}

//___________________________SelectionCutBDTJetScore____________________________
//______________________________________________________________________________
SelectionCutBDTJetScore::SelectionCutBDTJetScore(TauSelectionTool* tTST)
  : SelectionCut("CutJetBDTScore", tTST)
{
  m_hHistCutPre = CreateControlPlot("hJetBDT_pre","JetBDT_pre;BDTJetScore; events",100,0,1);
  m_hHistCut = CreateControlPlot("hJetBDT_cut","JetBDT_cut;BDTJetScore; events",100,0,1);
}

//______________________________________________________________________________
void SelectionCutBDTJetScore::fillHistogram(const xAOD::TauJet& xTau, TH1F& hHist)
{
  hHist.Fill(xTau.discriminant(xAOD::TauJetParameters::BDTJetScore));
}

//______________________________________________________________________________
bool SelectionCutBDTJetScore::accept(const xAOD::TauJet& xTau)
{
  // check JetBDTscore, if tau has a JetBDT score in one of the regions requiered then return true; false otherwise
  m_tTST->m_aAccept.addCut( "JetBDTScore",
                            "Selection of taus according to their JetBDTScore" );
  double dJetBDTScore = xTau.discriminant(xAOD::TauJetParameters::BDTJetScore);
  unsigned int iNumJetBDTRegion = m_tTST->m_vJetBDTRegion.size()/2;
  for( unsigned int iJetBDTRegion = 0; iJetBDTRegion < iNumJetBDTRegion; iJetBDTRegion++ )
  {
    if ( dJetBDTScore >= m_tTST->m_vJetBDTRegion.at(iJetBDTRegion*2) and dJetBDTScore <= m_tTST->m_vJetBDTRegion.at(iJetBDTRegion*2+1))
    {
      m_tTST->m_aAccept.setCutResult( "JetBDTScore", true );
      return true;
    }
  }
  m_tTST->msg() << MSG::VERBOSE << "Tau failed JetBDTScore requirement, tau JetBDTScore: " << dJetBDTScore << endmsg;
  return false;
}

//_____________________________SelectionCutJetIDWP______________________________
//______________________________________________________________________________
SelectionCutJetIDWP::SelectionCutJetIDWP(TauSelectionTool* tTST)
  : SelectionCut("CutJetIDWP", tTST)
{
  m_hHistCutPre = CreateControlPlot("hJetIDWP_pre","JetIDWP_pre;; events",6,-.5,5.5);
  m_hHistCut = CreateControlPlot("hJetIDWP_cut","JetIDWP_cut;; events",6,-.5,5.5);
  // only proceed if histograms are defined
  if (!m_hHistCutPre or !m_hHistCut)
    return;
  m_hHistCutPre->GetXaxis()->SetBinLabel(1,"!Loose");
  m_hHistCutPre->GetXaxis()->SetBinLabel(2,"Loose");
  m_hHistCutPre->GetXaxis()->SetBinLabel(3,"!Medium");
  m_hHistCutPre->GetXaxis()->SetBinLabel(4,"Medium");
  m_hHistCutPre->GetXaxis()->SetBinLabel(5,"!Tight");
  m_hHistCutPre->GetXaxis()->SetBinLabel(6,"Tight");
  m_hHistCut->GetXaxis()->SetBinLabel(1,"!Loose");
  m_hHistCut->GetXaxis()->SetBinLabel(2,"Loose");
  m_hHistCut->GetXaxis()->SetBinLabel(3,"!Medium");
  m_hHistCut->GetXaxis()->SetBinLabel(4,"Medium");
  m_hHistCut->GetXaxis()->SetBinLabel(5,"!Tight");
  m_hHistCut->GetXaxis()->SetBinLabel(6,"Tight");
}

//______________________________________________________________________________
void SelectionCutJetIDWP::fillHistogram(const xAOD::TauJet& xTau, TH1F& hHist)
{
  hHist.Fill(xTau.isTau(xAOD::TauJetParameters::JetBDTSigLoose));
  hHist.Fill(xTau.isTau(xAOD::TauJetParameters::JetBDTSigMedium)+2);
  hHist.Fill(xTau.isTau(xAOD::TauJetParameters::JetBDTSigTight)+4);
}

//______________________________________________________________________________
bool SelectionCutJetIDWP::accept(const xAOD::TauJet& xTau)
{
  // check Jet ID working point, if tau passes JetID working point then return true; false otherwise
  m_tTST->m_aAccept.addCut( "JetIDWP",
                            "Selection of taus according to their JetIDScore" );
  bool bPass = false;
  switch (m_tTST->m_iJetIDWP)
  {
  case JETIDNONE:
    bPass = true;
    break;
  case JETIDNONEUNCONFIGURED:
    bPass = true;
    break;
  case JETIDBDTLOOSE:
    if (xTau.isTau(xAOD::TauJetParameters::JetBDTSigLoose)) bPass = true;
    break;
  case JETIDBDTMEDIUM:
    if (xTau.isTau(xAOD::TauJetParameters::JetBDTSigMedium)) bPass = true;
    break;
  case JETIDBDTTIGHT:
    if (xTau.isTau(xAOD::TauJetParameters::JetBDTSigTight)) bPass = true;
    break;
  case JETIDBDTLOOSENOTMEDIUM:
    if (xTau.isTau(xAOD::TauJetParameters::JetBDTSigLoose) and !xTau.isTau(xAOD::TauJetParameters::JetBDTSigMedium)) bPass = true;
    break;
  case JETIDBDTLOOSENOTTIGHT:
    if (xTau.isTau(xAOD::TauJetParameters::JetBDTSigLoose) and !xTau.isTau(xAOD::TauJetParameters::JetBDTSigTight)) bPass = true;
    break;
  case JETIDBDTMEDIUMNOTTIGHT:
    if (xTau.isTau(xAOD::TauJetParameters::JetBDTSigMedium) and !xTau.isTau(xAOD::TauJetParameters::JetBDTSigTight)) bPass = true;
    break;
  case JETIDBDTNOTLOOSE:
    if (!xTau.isTau(xAOD::TauJetParameters::JetBDTSigLoose)) bPass = true;
    break;
  default:
    m_tTST->msg() << MSG::WARNING << "The jet ID working point with the enum "<<m_tTST->m_iJetIDWP<<" is not available" << endmsg;
    break;
  }
  if (bPass)
  {
    m_tTST->m_aAccept.setCutResult( "JetIDWP", true );
    return true;
  }
  m_tTST->msg() << MSG::VERBOSE << "Tau failed JetBDTWP requirement, tau JetBDTScore: " << xTau.discriminant(xAOD::TauJetParameters::BDTJetScore) << endmsg;
  return false;
}

//___________________________SelectionCutBDTEleScore____________________________
//______________________________________________________________________________
SelectionCutBDTEleScore::SelectionCutBDTEleScore(TauSelectionTool* tTST)
  : SelectionCut("CutEleBDTScore", tTST)
{
  m_hHistCutPre = CreateControlPlot("hEleBDT_pre","EleBDT_pre;BDTEleScore; events",100,0,1);
  m_hHistCut = CreateControlPlot("hEleBDT_cut","EleBDT_cut;BDTEleScore; events",100,0,1);
}

//______________________________________________________________________________
void SelectionCutBDTEleScore::fillHistogram(const xAOD::TauJet& xTau, TH1F& hHist)
{
  hHist.Fill(xTau.discriminant(xAOD::TauJetParameters::BDTEleScore));
}

//______________________________________________________________________________
bool SelectionCutBDTEleScore::accept(const xAOD::TauJet& xTau)
{
  // check EleBDTscore, if tau has a EleBDT score in one of the regions requiered then return true; false otherwise
  m_tTST->m_aAccept.addCut( "EleBDTScore",
                            "Selection of taus according to their EleBDTScore" );
  double dEleBDTScore = xTau.discriminant(xAOD::TauJetParameters::BDTEleScore);
  unsigned int iNumEleBDTRegion = m_tTST->m_vEleBDTRegion.size()/2;
  for( unsigned int iEleBDTRegion = 0; iEleBDTRegion < iNumEleBDTRegion; iEleBDTRegion++ )
  {
    if ( dEleBDTScore >= m_tTST->m_vEleBDTRegion.at(iEleBDTRegion*2) and dEleBDTScore <= m_tTST->m_vEleBDTRegion.at(iEleBDTRegion*2+1))
    {
      m_tTST->m_aAccept.setCutResult( "EleBDTScore", true );
      return true;
    }
  }
  m_tTST->msg() << MSG::VERBOSE << "Tau failed EleBDTScore requirement, tau EleBDTScore: " << dEleBDTScore << endmsg;
  return false;
}

//____________________________SelectionCutEleBDTWP______________________________
//______________________________________________________________________________
SelectionCutEleBDTWP::SelectionCutEleBDTWP(TauSelectionTool* tTST)
  : SelectionCut("CutEleBDTWP", tTST)
{
  m_hHistCutPre = CreateControlPlot("hEleBDTWP_pre","EleBDTWP_pre;; events",6,-.5,5.5);
  m_hHistCut = CreateControlPlot("hEleBDTWP_cut","EleBDTWP_cut;; events",6,-.5,5.5);
  // only proceed if histograms are defined
  if (!m_hHistCutPre or !m_hHistCut)
    return;
  m_hHistCutPre->GetXaxis()->SetBinLabel(1,"!Loose");
  m_hHistCutPre->GetXaxis()->SetBinLabel(2,"Loose");
  m_hHistCutPre->GetXaxis()->SetBinLabel(3,"!Medium");
  m_hHistCutPre->GetXaxis()->SetBinLabel(4,"Medium");
  m_hHistCutPre->GetXaxis()->SetBinLabel(5,"!Tight");
  m_hHistCutPre->GetXaxis()->SetBinLabel(6,"Tight");
  m_hHistCut->GetXaxis()->SetBinLabel(1,"!Loose");
  m_hHistCut->GetXaxis()->SetBinLabel(2,"Loose");
  m_hHistCut->GetXaxis()->SetBinLabel(3,"!Medium");
  m_hHistCut->GetXaxis()->SetBinLabel(4,"Medium");
  m_hHistCut->GetXaxis()->SetBinLabel(5,"!Tight");
  m_hHistCut->GetXaxis()->SetBinLabel(6,"Tight");
}

//______________________________________________________________________________
void SelectionCutEleBDTWP::fillHistogram(const xAOD::TauJet& xTau, TH1F& hHist)
{
#ifndef XAODTAU_VERSIONS_TAUJET_V3_H
  hHist.Fill(!xTau.isTau(xAOD::TauJetParameters::ElectronVetoLoose));
  hHist.Fill(!xTau.isTau(xAOD::TauJetParameters::ElectronVetoMedium)+2);
  hHist.Fill(!xTau.isTau(xAOD::TauJetParameters::ElectronVetoTight)+4);
#else
  (void)xTau;
  (void)hHist;
#endif
}

//______________________________________________________________________________
bool SelectionCutEleBDTWP::accept(const xAOD::TauJet& xTau)
{
  // check EleBDTscore, if tau passes EleBDT working point then return true; false otherwise
  m_tTST->m_aAccept.addCut( "EleBDTWP",
                            "Selection of taus according to their EleBDTScore" );
  bool bPass = false;
  switch (m_tTST->m_iEleBDTWP)
  {
  case ELEIDNONE:
    bPass = true;
    break;
  case ELEIDNONEUNCONFIGURED:
    bPass = true;
    break;
  case ELEIDBDTLOOSE:
    if (!xTau.isTau(xAOD::TauJetParameters::EleBDTLoose )) bPass = true;
    break;
  case ELEIDBDTMEDIUM:
    if (!xTau.isTau(xAOD::TauJetParameters::EleBDTMedium )) bPass = true;
    break;
  case ELEIDBDTTIGHT:
    if (!xTau.isTau(xAOD::TauJetParameters::EleBDTTight )) bPass = true;
    break;
  default:
    m_tTST->msg() << MSG::WARNING << "The electron ID working point with the enum "<<m_tTST->m_iJetIDWP<<" is not available" << endmsg;
    break;
  }
  if (bPass)
  {
    m_tTST->m_aAccept.setCutResult( "EleBDTWP", true );
    return true;
  }
  m_tTST->msg() << MSG::VERBOSE << "Tau failed EleBDT requirement, tau EleBDTScore: " << xTau.discriminant(xAOD::TauJetParameters::BDTEleScore) << endmsg;
  return false;
}

//___________________________SelectionCutEleOLR____________________________
//______________________________________________________________________________
SelectionCutEleOLR::SelectionCutEleOLR(TauSelectionTool* tTST)
  : SelectionCut("CutEleOLR", tTST)
  , m_tTOELLHDecorator(0)
  , m_bCheckEleMatchPassAvailable(true)
  , m_bEleMatchPassAvailable(true)
#ifndef XAODTAU_VERSIONS_TAUJET_V3_H
  , m_sEleOlrLhScoreDecorationName("ele_match_lhscore")
#else
  , m_sEleOlrLhScoreDecorationName("EleMatchLikelihoodScore")
#endif

{
  m_hHistCutPre = CreateControlPlot("hEleOLR_pre","EleOLR_pre;Electron Likelihood Score; events",100,-4,4);
  m_hHistCut = CreateControlPlot("hEleOLR_cut","EleOLR_cut;Electron Likelihood Score; events",100,-4,4);
}

SelectionCutEleOLR::~SelectionCutEleOLR()
{
#ifdef ROOTCORE
  if (m_tTOELLHDecorator and asg::ToolStore::remove(m_tTOELLHDecorator).isFailure())
    m_tTST->msg() << MSG::ERROR << "An error occured while trying to remove "<<m_tTOELLHDecorator->name()<<" from tool store";
#endif  // ROOTCORE
  delete m_tTOELLHDecorator;
}

//______________________________________________________________________________
void SelectionCutEleOLR::fillHistogram(const xAOD::TauJet& xTau, TH1F& hHist)
{
  // run this to get ele_match_lhscore decoration
  getEvetoPass(xTau);
  static SG::AuxElement::ConstAccessor<float> accEleMatchLhscore(m_sEleOlrLhScoreDecorationName.c_str());
  hHist.Fill(accEleMatchLhscore(xTau));
}

//______________________________________________________________________________
bool SelectionCutEleOLR::accept(const xAOD::TauJet& xTau)
{
  m_tTST->m_aAccept.addCut( "EleOLR",
                            "Selection of taus according to the LH score of a matched electron" );

  if (!m_tTST->m_bEleOLR)
  {
    m_tTST->m_aAccept.setCutResult( "EleOLR", true );
    return true;
  }

  if (getEvetoPass(xTau))
  {
    m_tTST->m_aAccept.setCutResult( "EleOLR", true );
    return true;
  }

  static SG::AuxElement::ConstAccessor<float> accEleMatchLhscore(m_sEleOlrLhScoreDecorationName.c_str());
  m_tTST->msg() << MSG::VERBOSE << "Tau failed EleOLR requirement, tau overlapping electron llh score: " << accEleMatchLhscore(xTau) << endmsg;
  return false;
}

//______________________________________________________________________________
StatusCode SelectionCutEleOLR::createTOELLHDecorator()
{
  if (!m_tTOELLHDecorator)
  {
    m_tTOELLHDecorator = new TauOverlappingElectronLLHDecorator(m_tTST->name()+"_TOELLHDecorator");
    m_tTOELLHDecorator->msg().setLevel( m_tTST->msg().level() );
    if (m_tTOELLHDecorator->setProperty( "EleOLRFilePath", m_tTST->m_sEleOLRFilePath ).isFailure())
      return StatusCode::FAILURE;
    return m_tTOELLHDecorator->initialize();
  }
  return StatusCode::SUCCESS;
}

//______________________________________________________________________________
bool SelectionCutEleOLR::getEvetoPass(const xAOD::TauJet& xTau)
{

#ifndef XAODTAU_VERSIONS_TAUJET_V3_H
  if (m_bCheckEleMatchPassAvailable)
  {
    m_bCheckEleMatchPassAvailable = false;
    if (!xTau.isAvailable<char>("ele_olr_pass"))
    {
      m_bEleMatchPassAvailable = false;
      if (createTOELLHDecorator().isFailure())
        throw std::runtime_error ("TOELLHDecorator constructor failed\n");
    }
  }
  if (!m_bEleMatchPassAvailable)
    if (m_tTOELLHDecorator->decorate(xTau).isFailure())
      throw std::runtime_error ("TOELLHDecorator decoration failed\n");
  static SG::AuxElement::ConstAccessor<char> accEleOlrPass("ele_olr_pass");
  return (bool)accEleOlrPass(xTau);
#else
  return xTau.isTau(xAOD::TauJetParameters::PassEleOLR);
#endif

}

//______________________________________________________________________________
StatusCode SelectionCutEleOLR::beginEvent()
{
  if (createTOELLHDecorator().isFailure())
    return StatusCode::FAILURE;
  if (m_tTOELLHDecorator->beginEvent().isFailure())
    return StatusCode::FAILURE;
  return StatusCode::SUCCESS;
}

//____________________________SelectionCutMuonVeto______________________________
//______________________________________________________________________________
SelectionCutMuonVeto::SelectionCutMuonVeto(TauSelectionTool* tTST)
  : SelectionCut("CutMuonVeto", tTST)
{
  m_hHistCutPre = CreateControlPlot("hMuonVeto_pre","MuonVeto_pre;; events",2,-.5,1.5);
  m_hHistCut = CreateControlPlot("hMuonVeto_cut","MuonVeto_cut;; events",2,-.5,1.5);
  // only proceed if histograms are defined
  if (!m_hHistCutPre or !m_hHistCut)
    return;
  m_hHistCutPre->GetXaxis()->SetBinLabel(1,"!MuonVeto");
  m_hHistCutPre->GetXaxis()->SetBinLabel(2,"MuonVeto");
  m_hHistCut->GetXaxis()->SetBinLabel(1,"!MuonVeto");
  m_hHistCut->GetXaxis()->SetBinLabel(2,"MuonVeto");
}

//______________________________________________________________________________
void SelectionCutMuonVeto::fillHistogram(const xAOD::TauJet& xTau, TH1F& hHist)
{
  hHist.Fill(!xTau.isTau(xAOD::TauJetParameters::MuonVeto));
}

//______________________________________________________________________________
bool SelectionCutMuonVeto::accept(const xAOD::TauJet& xTau)
{
  // check EleBDTscore, if tau passes EleBDT working point then return true; false otherwise
  m_tTST->m_aAccept.addCut( "MuonVeto",
                            "Selection of taus according to their MuonVeto" );
  if (!m_tTST->m_bMuonVeto)
  {
    m_tTST->m_aAccept.setCutResult( "MuonVeto", true );
    return true;
  }

  if (!xTau.isTau(xAOD::TauJetParameters::MuonVeto ))
  {
    m_tTST->m_aAccept.setCutResult( "MuonVeto", true );
    return true;
  }
  m_tTST->msg() << MSG::VERBOSE << "Tau failed MuonVeto requirement" << endmsg;
  return false;
}
