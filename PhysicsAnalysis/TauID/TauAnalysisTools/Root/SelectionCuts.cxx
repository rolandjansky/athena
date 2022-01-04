// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


// local include(s)
#include "TauAnalysisTools/SelectionCuts.h"
#include "TauAnalysisTools/TauSelectionTool.h"

// ROOT include(s)
#include "TFile.h"

using namespace TauAnalysisTools;

//______________________________________________________________________________
SelectionCut::SelectionCut(const std::string& sName, TauAnalysisTools::TauSelectionTool* tTST)
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


//______________________________________________________________________________
void SelectionCut::setProperty(const std::string& name, const std::string& value)
{
  std::map<std::string, std::string&>::iterator it = m_mProperties.find(name);
  if(it == m_mProperties.end() )
    throw std::runtime_error (("Undeclared property: " + name + "\n").c_str());
  it->second = value;
}

//______________________________________________________________________________
void SelectionCut::declareProperty(const std::string& name, std::string& loc)
{
  std::pair<std::string, std::string&> p(name, loc);
  m_mProperties.insert(p);
}

//______________________________________________________________________________
std::string SelectionCut::getProperty(const std::string& name)
{
  std::map<std::string, std::string&>::iterator it = m_mProperties.find(name);
  if(it == m_mProperties.end() )
    throw std::runtime_error (("Undeclared property: " + name + "\n").c_str());

  return it->second;
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
void SelectionCutPt::setAcceptInfo(asg::AcceptInfo& info) const
{
  info.addCut( "Pt",
               "Selection of taus according to their transverse momentum" );
}
//______________________________________________________________________________
bool SelectionCutPt::accept(const xAOD::TauJet& xTau,
                            asg::AcceptData& acceptData)
{
  // save tau pt in GeV
  double pt = xTau.pt() / 1000.;
  // in case of only one entry in vector, run for lower limits
  if (m_tTST->m_vPtRegion.size() == 1)
  {
    if ( pt >= m_tTST->m_vPtRegion.at(0) )
    {
      acceptData.setCutResult( "Pt", true );
      return true;
    }
  }
  unsigned int iNumPtRegion = m_tTST->m_vPtRegion.size()/2;
  for( unsigned int iPtRegion = 0; iPtRegion < iNumPtRegion; iPtRegion++ )
  {
    if ( pt >= m_tTST->m_vPtRegion.at(iPtRegion*2) and pt <= m_tTST->m_vPtRegion.at(iPtRegion*2+1))
    {
      acceptData.setCutResult( "Pt", true );
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
void SelectionCutAbsEta::setAcceptInfo(asg::AcceptInfo& info) const
{
  info.addCut( "AbsEta",
               "Selection of taus according to their absolute pseudorapidity" );
}
//______________________________________________________________________________
bool SelectionCutAbsEta::accept(const xAOD::TauJet& xTau,
                                asg::AcceptData& acceptData)
{
  // check regions of eta, if tau is in one region then return true; false otherwise
  unsigned int iNumEtaRegion = m_tTST->m_vAbsEtaRegion.size()/2;
  for( unsigned int iEtaRegion = 0; iEtaRegion < iNumEtaRegion; iEtaRegion++ )
  {
    if ( std::abs( xTau.eta() ) >= m_tTST->m_vAbsEtaRegion.at(iEtaRegion*2) and std::abs( xTau.eta() ) <= m_tTST->m_vAbsEtaRegion.at(iEtaRegion*2+1))
    {
      acceptData.setCutResult( "AbsEta", true );
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
void SelectionCutAbsCharge::setAcceptInfo(asg::AcceptInfo& info) const
{
  info.addCut( "AbsCharge",
               "Selection of taus according to their absolute charge" );
}
//______________________________________________________________________________
bool SelectionCutAbsCharge::accept(const xAOD::TauJet& xTau,
                            asg::AcceptData& acceptData)
{
  // check charge, if tau has one of the charges requiered then return true; false otherwise
  for( unsigned int iCharge = 0; iCharge < m_tTST->m_vAbsCharges.size(); iCharge++ )
  {
    if ( std::abs( xTau.charge() ) == m_tTST->m_vAbsCharges.at(iCharge) )
    {acceptData.setCutResult( "AbsCharge", true );
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
void SelectionCutNTracks::setAcceptInfo(asg::AcceptInfo& info) const
{
  info.addCut( "NTrack",
               "Selection of taus according to their number of associated tracks" );
}
//______________________________________________________________________________
bool SelectionCutNTracks::accept(const xAOD::TauJet& xTau,
                                 asg::AcceptData& acceptData)
{
  // check track multiplicity, if tau has one of the number of tracks requiered then return true; false otherwise
  for( size_t iNumTrack = 0; iNumTrack < m_tTST->m_vNTracks.size(); iNumTrack++ )
  {
    if ( static_cast<unsigned> (xTau.nTracks()) == m_tTST->m_vNTracks.at(iNumTrack) )
    {
      acceptData.setCutResult( "NTrack", true );
      return true;
    }
  }
  m_tTST->msg() << MSG::VERBOSE << "Tau failed nTracks requirement, tau number of tracks: " << xTau.nTracks() << endmsg;
  return false;
}

//___________________________SelectionCutRNNJetScoreSigTrans____________________________
//______________________________________________________________________________
SelectionCutRNNJetScoreSigTrans::SelectionCutRNNJetScoreSigTrans(TauSelectionTool* tTST)
  : SelectionCut("CutJetRNNScoreSigTrans", tTST)
{
  m_hHistCutPre = CreateControlPlot("hJetRNNSigTrans_pre","JetRNNSigTrans_pre;RNNJetSigTransScore; events",100,0,1);
  m_hHistCut = CreateControlPlot("hJetRNNSigTrans_cut","JetRNNSigTrans_cut;RNNJetSigTransScore; events",100,0,1);
}
//______________________________________________________________________________
void SelectionCutRNNJetScoreSigTrans::fillHistogram(const xAOD::TauJet& xTau, TH1F& hHist)
{
  hHist.Fill(xTau.discriminant(xAOD::TauJetParameters::RNNJetScoreSigTrans));
}
//______________________________________________________________________________
void SelectionCutRNNJetScoreSigTrans::setAcceptInfo(asg::AcceptInfo& info) const
{
  info.addCut( "JetRNNScoreSigTrans",
               "Selection of taus according to their JetRNNScore" );
}
//______________________________________________________________________________
bool SelectionCutRNNJetScoreSigTrans::accept(const xAOD::TauJet& xTau,
                                             asg::AcceptData& acceptData)
{
  // check JetRNNscore, if tau has a JetRNN score in one of the regions requiered then return true; false otherwise
  double dJetRNNScoreSigTrans = xTau.discriminant(xAOD::TauJetParameters::RNNJetScoreSigTrans);
  unsigned int iNumJetRNNSigTransRegion = m_tTST->m_vJetRNNSigTransRegion.size()/2;
  for( unsigned int iJetRNNSigTransRegion = 0; iJetRNNSigTransRegion < iNumJetRNNSigTransRegion; iJetRNNSigTransRegion++ )
  {
    if ( dJetRNNScoreSigTrans >= m_tTST->m_vJetRNNSigTransRegion.at(iJetRNNSigTransRegion*2) and dJetRNNScoreSigTrans <= m_tTST->m_vJetRNNSigTransRegion.at(iJetRNNSigTransRegion*2+1))
    {
      acceptData.setCutResult( "JetRNNScoreSigTrans", true );
      return true;
    }
  }
  m_tTST->msg() << MSG::VERBOSE << "Tau failed JetRNNScore requirement, tau JetRNNScore: " << dJetRNNScoreSigTrans << endmsg;
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
  hHist.Fill(xTau.isTau(xAOD::TauJetParameters::JetRNNSigLoose));
  hHist.Fill(xTau.isTau(xAOD::TauJetParameters::JetRNNSigMedium)+2);
  hHist.Fill(xTau.isTau(xAOD::TauJetParameters::JetRNNSigTight)+4);
}

//______________________________________________________________________________
void SelectionCutJetIDWP::setAcceptInfo(asg::AcceptInfo& info) const
{
  info.addCut( "JetIDWP",
               "Selection of taus according to their JetIDScore" );
}
//______________________________________________________________________________
bool SelectionCutJetIDWP::accept(const xAOD::TauJet& xTau,
                                 asg::AcceptData& acceptData)
{
  // check Jet ID working point, if tau passes JetID working point then return true; false otherwise
  bool bPass = false;
  switch (m_tTST->m_iJetIDWP)
  {
  case JETIDNONE:
    bPass = true;
    break;
  case JETIDNONEUNCONFIGURED:
    bPass = true;
    break;
  case JETIDRNNVERYLOOSE:
    if (xTau.isTau(xAOD::TauJetParameters::JetRNNSigVeryLoose)) bPass = true;
    break;
  case JETIDRNNLOOSE:
    if (xTau.isTau(xAOD::TauJetParameters::JetRNNSigLoose)) bPass = true;
    break;
  case JETIDRNNMEDIUM:
    if (xTau.isTau(xAOD::TauJetParameters::JetRNNSigMedium)) bPass = true;
    break;
  case JETIDRNNTIGHT:
    if (xTau.isTau(xAOD::TauJetParameters::JetRNNSigTight)) bPass = true;
    break;
  default:
    m_tTST->msg() << MSG::WARNING << "The jet ID working point with the enum "<<m_tTST->m_iJetIDWP<<" is not available" << endmsg;
    break;
  }
  if (bPass)
  {
    acceptData.setCutResult( "JetIDWP", true );
    return true;
  }
  m_tTST->msg() << MSG::VERBOSE << "Tau failed JetRNNWP requirement, tau transformed RNN score: " << xTau.discriminant(xAOD::TauJetParameters::RNNJetScoreSigTrans) << endmsg;
  return false;
}

//___________________________SelectionCutRNNEleScore____________________________
//______________________________________________________________________________
SelectionCutRNNEleScore::SelectionCutRNNEleScore(TauSelectionTool* tTST)
  : SelectionCut("CutEleRNNScore", tTST)
{
  m_hHistCutPre = CreateControlPlot("hEleRNN_pre","EleRNN_pre;RNNEleScore; events",100,0,1);
  m_hHistCut = CreateControlPlot("hEleRNN_cut","EleRNN_cut;RNNEleScore; events",100,0,1);
}

//______________________________________________________________________________
void SelectionCutRNNEleScore::fillHistogram(const xAOD::TauJet& xTau, TH1F& hHist)
{
 hHist.Fill(xTau.discriminant(xAOD::TauJetParameters::RNNEleScoreSigTrans));
}

//______________________________________________________________________________
void SelectionCutRNNEleScore::setAcceptInfo(asg::AcceptInfo& info) const
{
  info.addCut( "EleRNNScore",
               "Selection of taus according to their EleRNNScore" );
}
//______________________________________________________________________________
bool SelectionCutRNNEleScore::accept(const xAOD::TauJet& xTau,
                                     asg::AcceptData& acceptData)
{
  double fEleRNNScore = xTau.discriminant(xAOD::TauJetParameters::RNNEleScoreSigTrans);
  unsigned int iNumEleRNNRegion = m_tTST->m_vEleRNNRegion.size()/2;
  for( unsigned int iEleRNNRegion = 0; iEleRNNRegion < iNumEleRNNRegion; iEleRNNRegion++ )
  {
    if ( fEleRNNScore >= m_tTST->m_vEleRNNRegion.at(iEleRNNRegion*2) and fEleRNNScore <= m_tTST->m_vEleRNNRegion.at(iEleRNNRegion*2+1))
    {
      acceptData.setCutResult("EleRNNScore", true );
      return true;
    }
  }
  m_tTST->msg() << MSG::VERBOSE << "Tau failed EleRNNScore requirement, tau EleRNNScore: " << fEleRNNScore << endmsg;
  return false;
}

//____________________________SelectionCutEleRNNWP______________________________
//______________________________________________________________________________
SelectionCutEleRNNWP::SelectionCutEleRNNWP(TauSelectionTool* tTST)
  : SelectionCut("CutEleRNNWP", tTST)
{
  m_hHistCutPre = CreateControlPlot("hEleRNNWP_pre","EleRNNWP_pre;; events",6,-.5,5.5);
  m_hHistCut = CreateControlPlot("hEleRNNWP_cut","EleRNNWP_cut;; events",6,-.5,5.5);
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
void SelectionCutEleRNNWP::fillHistogram(const xAOD::TauJet& xTau, TH1F& hHist)
{
  hHist.Fill(xTau.isTau(xAOD::TauJetParameters::EleRNNLoose));
  hHist.Fill(xTau.isTau(xAOD::TauJetParameters::EleRNNMedium)+2);
  hHist.Fill(xTau.isTau(xAOD::TauJetParameters::EleRNNTight)+4);
}

//______________________________________________________________________________
void SelectionCutEleRNNWP::setAcceptInfo(asg::AcceptInfo& info) const
{
  info.addCut( "EleRNNWP",
               "Selection of taus according to their EleRNNScore" );
}
//______________________________________________________________________________
bool SelectionCutEleRNNWP::accept(const xAOD::TauJet& xTau,
                                  asg::AcceptData& acceptData)
{
  // check EleRNNscore, if tau passes EleRNN working point then return true; false otherwise
  bool bPass = false;
  switch (m_tTST->m_iEleRNNWP)
  {
  case ELEIDNONE:
    bPass = true;
    break;
  case ELEIDNONEUNCONFIGURED:
    bPass = true;
    break;
  case ELEIDRNNLOOSE:
    if (xTau.isTau(xAOD::TauJetParameters::EleRNNLoose)) bPass = true;
    break;
  case ELEIDRNNMEDIUM:
    if (xTau.isTau(xAOD::TauJetParameters::EleRNNMedium)) bPass = true;
    break;
  case ELEIDRNNTIGHT:
    if (xTau.isTau(xAOD::TauJetParameters::EleRNNTight)) bPass = true;
    break;
  default:
    m_tTST->msg() << MSG::WARNING << "The electron ID working point with the enum "<<m_tTST->m_iEleRNNWP<<" is not available" << endmsg;
    break;
  }
    
  if (bPass)
  {
    acceptData.setCutResult( "EleRNNWP", true );
    return true;
  }
  m_tTST->msg() << MSG::VERBOSE << "Tau failed EleRNN requirement, tau EleRNNScore: " << xTau.discriminant(xAOD::TauJetParameters::RNNEleScoreSigTrans) << endmsg;
  return false;
}

//added by Li-Gang Xia < ligang.xia@cern.ch >
//____________________________SelectionCutMuonOLR______________________________
//______________________________________________________________________________
SelectionCutMuonOLR::SelectionCutMuonOLR(TauSelectionTool* tTST)
  : SelectionCut("CutMuonOLR", tTST)
  , m_bTauMuonOLR(true)
  , m_xMuonContainer(0)
{
  //ATH_MSG_INFO("Construct SelectionCutMuonOLR");
  m_hHistCutPre = CreateControlPlot("hMuonOLR_pre","MuonOLR_pre;; events",2,-.5,1.5);
  m_hHistCut = CreateControlPlot("hMuonOLR_cut","MuonOLR_cut;; events",2,-.5,1.5);
  // only proceed if histograms are defined
  if (!m_hHistCutPre or !m_hHistCut)
    return;
  m_hHistCutPre->GetXaxis()->SetBinLabel(1,"!MuonOLR");
  m_hHistCutPre->GetXaxis()->SetBinLabel(2,"MuonOLR");
  m_hHistCut->GetXaxis()->SetBinLabel(1,"!MuonOLR");
  m_hHistCut->GetXaxis()->SetBinLabel(2,"MuonOLR");
}

//______________________________________________________________________________
void SelectionCutMuonOLR::fillHistogram(const xAOD::TauJet& xTau, TH1F& hHist)
{
  (void)xTau;
  hHist.Fill(m_bTauMuonOLR);
}

//______________________________________________________________________________
void SelectionCutMuonOLR::setAcceptInfo(asg::AcceptInfo& info) const
{
  info.addCut( "MuonOLR",
               "Selection of taus according to their MuonOLR" );
}
//______________________________________________________________________________
bool SelectionCutMuonOLR::accept(const xAOD::TauJet& xTau,
                                 asg::AcceptData& acceptData)
{
  if (!m_tTST->m_bMuonOLR)
  {
    acceptData.setCutResult( "MuonOLR", true );
    return true;
  }
  // MuonOLR : removing tau overlapped with muon satisfying pt>2GeV and not calo-tagged
  m_bTauMuonOLR = true;
  if(!m_tTST->evtStore()->contains<xAOD::MuonContainer>(m_tTST->m_sMuonContainerName))
    m_tTST->msg() << MSG::FATAL << "Muon container with name " << m_tTST->m_sMuonContainerName << " is not available" << endmsg;
  else if(m_tTST->evtStore()->retrieve(m_xMuonContainer,m_tTST->m_sMuonContainerName).isFailure())
    m_tTST->msg() << MSG::FATAL << "Muon container with name " << m_tTST->m_sMuonContainerName << " could not be retrieved from event store" << endmsg;
  if(!m_xMuonContainer)
    return false;

  for( auto xMuon : *(m_xMuonContainer) )
  {
    if(xMuon->pt() < 2000.) continue;// pt > 2GeV
    if(xMuon->muonType() == xAOD::Muon::CaloTagged) continue; // not calo-tagged
    if(xMuon->p4().DeltaR( xTau.p4() ) > 0.2 ) continue; //delta R < 0.2
    m_bTauMuonOLR = false;// muon-tau overlapped
    break;
  }
  if(m_bTauMuonOLR == true)
  {
    acceptData.setCutResult( "MuonOLR", true );
    return true;
  }

  m_tTST->msg() << MSG::VERBOSE << "Tau failed MuonOLR requirement" << endmsg;
  return false;
}


