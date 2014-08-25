/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MissingETPerformance/PileUpTool.h"

#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ITHistSvc.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TProfile.h"
#include "MissingETPerformance/MissingETData.h"

#include <string>
#include <sstream>
#include "CLHEP/Units/SystemOfUnits.h"
#include "VxVertex/VxContainer.h"
#include "MissingETEvent/MissingET.h"
#include "TF1.h"


using CLHEP::GeV;


PileUpTool::PileUpTool( const std::string& type,
                        const std::string& name, 
                        const IInterface*  parent )
          : AthAlgTool( type, name, parent )
{
  declareInterface<PileUpTool>( this );
  declareProperty("VxCandidateContainerName",   m_vtxContName        = "VxPrimaryCandidate" );
  declareProperty("MinNumberOfTracksPerVertex", m_minTracksPerVertex = 2 );
  declareProperty("VxRange",                    m_vtxRange           = 12 );
  declareProperty("VxNumberOfBins",             m_vtxBins            = 6 );
  declareProperty("EtRatioRange",               m_rtRange            = 20.0 );
  declareProperty("EtRatioNumberOfBins",        m_rtBins             = 100 );
  declareProperty("EtRange",                    m_etRange            = 100.0 );
  declareProperty("EtNumberOfBins",             m_etBins             = 500 );
  declareProperty("StRange",                    m_stRange            = 1000.0 );
  declareProperty("StNumberOfBins",             m_stBins             = 25 );
  declareProperty("UseGaussianFit",             m_doFit              = false );
  declareProperty("FolderName",           m_folderName="");

}


StatusCode PileUpTool::CBNT_initialize()
{
  // get pointer to THistSvc
  StatusCode sc = service("THistSvc", m_thistSvc);
  if ( sc.isFailure() ) {
    msg() << MSG::ERROR << "Unable to retrieve pointer to THistSvc" << endreq;
    return sc;
  }
  
  // Allocate dynamic arrays
  h2_RefFinal_SumEt_RatioEt_vtx = new TH2F*[m_vtxBins];
  h2_RefFinal_SumEt_DeltaEt_vtx = new TH2F*[m_vtxBins];
  h2_RefFinal_SumEt_DeltaEx_vtx = new TH2F*[m_vtxBins];
  h2_RefFinal_SumEt_DeltaEy_vtx = new TH2F*[m_vtxBins];
  h1_RefFinal_SumEt_ResponseEt_vtx   = new TH1F*[m_vtxBins];
  h1_RefFinal_SumEt_OffsetEt_vtx     = new TH1F*[m_vtxBins];
  h1_RefFinal_SumEt_ResolutionEt_vtx = new TH1F*[m_vtxBins];
  h1_RefFinal_SumEt_ResolutionEx_vtx = new TH1F*[m_vtxBins];
  h1_RefFinal_SumEt_ResolutionEy_vtx = new TH1F*[m_vtxBins];
  
  // Book histograms
  
  // Total number of reconstructed primary vertices
  h1_Nvtx = new TH1F("h1_Nvtx","",m_vtxRange,-0.5,m_vtxRange-0.5);
  h1_Nvtx->GetXaxis()->SetTitle("Vertex multiplicity");
  h1_Nvtx->GetYaxis()->SetTitle("Events (arbitrary units)");
  m_thistSvc->regHist("/AANT/" + m_folderName + "PileUp/h1_Nvtx",h1_Nvtx).ignore();
  
  // Histograms as function of Number of vertices
  h2_RefFinal_Nvtx_RatioEt = new TH2F("h2_RefFinal_Nvtx_RatioEt","",m_vtxRange,-0.5,m_vtxRange-0.5,m_rtBins,0.0,m_rtRange);
  h2_RefFinal_Nvtx_RatioEt->GetXaxis()->SetTitle("Vertex multiplicity");
  h2_RefFinal_Nvtx_RatioEt->GetYaxis()->SetTitle("E_{T}^{miss}/True E_{T}^{miss}");
  m_thistSvc->regHist("/AANT/" + m_folderName + "PileUp/h2_RefFinal_Nvtx_RatioEt",h2_RefFinal_Nvtx_RatioEt).ignore();
  
  h2_RefFinal_Nvtx_DeltaEt = new TH2F("h2_RefFinal_Nvtx_DeltaEt","",m_vtxRange,-0.5,m_vtxRange-0.5,m_etBins,-m_etRange,m_etRange);
  h2_RefFinal_Nvtx_DeltaEt->GetXaxis()->SetTitle("Vertex multiplicity");
  h2_RefFinal_Nvtx_DeltaEt->GetYaxis()->SetTitle("E_{T}^{miss} - True E_{T}^{miss} (GeV)");
  m_thistSvc->regHist("/AANT/" + m_folderName + "PileUp/h2_RefFinal_Nvtx_DeltaEt",h2_RefFinal_Nvtx_DeltaEt).ignore();
  
  h2_RefFinal_Nvtx_DeltaEx = new TH2F("h2_RefFinal_Nvtx_DeltaEx","",m_vtxRange,-0.5,m_vtxRange-0.5,m_etBins,-m_etRange,m_etRange);
  h2_RefFinal_Nvtx_DeltaEx->GetXaxis()->SetTitle("Vertex multiplicity");
  h2_RefFinal_Nvtx_DeltaEx->GetYaxis()->SetTitle("E_{x}^{miss} - True E_{x}^{miss} (GeV)");
  m_thistSvc->regHist("/AANT/" + m_folderName + "PileUp/h2_RefFinal_Nvtx_DeltaEx",h2_RefFinal_Nvtx_DeltaEx).ignore();
  
  h2_RefFinal_Nvtx_DeltaEy = new TH2F("h2_RefFinal_Nvtx_DeltaEy","",m_vtxRange,-0.5,m_vtxRange-0.5,m_etBins,-m_etRange,m_etRange);
  h2_RefFinal_Nvtx_DeltaEy->GetXaxis()->SetTitle("Vertex multiplicity");
  h2_RefFinal_Nvtx_DeltaEy->GetYaxis()->SetTitle("E_{y}^{miss} - True E_{y}^{miss} (GeV)");
  m_thistSvc->regHist("/AANT/" + m_folderName + "PileUp/h2_RefFinal_Nvtx_DeltaEy",h2_RefFinal_Nvtx_DeltaEy).ignore();
  
  h1_RefFinal_Nvtx_ResponseEt = new TH1F("h1_RefFinal_Nvtx_ResponseEt","",m_vtxRange,-0.5,m_vtxRange-0.5);
  h1_RefFinal_Nvtx_ResponseEt->GetXaxis()->SetTitle("Vertex multiplicity");
  h1_RefFinal_Nvtx_ResponseEt->GetYaxis()->SetTitle("E_{T}^{miss} response");
  m_thistSvc->regHist("/AANT/" + m_folderName + "PileUp/h1_RefFinal_Nvtx_ResponseEt",h1_RefFinal_Nvtx_ResponseEt).ignore();
    
  h1_RefFinal_Nvtx_OffsetEt = new TH1F("h1_RefFinal_Nvtx_OffsetEt","",m_vtxRange,-0.5,m_vtxRange-0.5);
  h1_RefFinal_Nvtx_OffsetEt->GetXaxis()->SetTitle("Vertex multiplicity");
  h1_RefFinal_Nvtx_OffsetEt->GetYaxis()->SetTitle("E_{T}^{miss} offset (GeV)");
  m_thistSvc->regHist("/AANT/" + m_folderName + "PileUp/h1_RefFinal_Nvtx_OffsetEt",h1_RefFinal_Nvtx_OffsetEt).ignore();
  
  h1_RefFinal_Nvtx_ResolutionEt = new TH1F("h1_RefFinal_Nvtx_ResolutionEt","",m_vtxRange,-0.5,m_vtxRange-0.5);
  h1_RefFinal_Nvtx_ResolutionEt->GetXaxis()->SetTitle("Vertex multiplicity");
  h1_RefFinal_Nvtx_ResolutionEt->GetYaxis()->SetTitle("E_{T}^{miss} resolution (GeV)");
  m_thistSvc->regHist("/AANT/" + m_folderName + "PileUp/h1_RefFinal_Nvtx_ResolutionEt",h1_RefFinal_Nvtx_ResolutionEt).ignore();
  
  h1_RefFinal_Nvtx_ResolutionEx = new TH1F("h1_RefFinal_Nvtx_ResolutionEx","",m_vtxRange,-0.5,m_vtxRange-0.5);
  h1_RefFinal_Nvtx_ResolutionEx->GetXaxis()->SetTitle("Vertex multiplicity");
  h1_RefFinal_Nvtx_ResolutionEx->GetYaxis()->SetTitle("E_{x}^{miss} resolution (GeV)");
  m_thistSvc->regHist("/AANT/" + m_folderName + "PileUp/h1_RefFinal_Nvtx_ResolutionEx",h1_RefFinal_Nvtx_ResolutionEx).ignore();
  
  h1_RefFinal_Nvtx_ResolutionEy = new TH1F("h1_RefFinal_Nvtx_ResolutionEy","",m_vtxRange,-0.5,m_vtxRange-0.5);
  h1_RefFinal_Nvtx_ResolutionEy->GetXaxis()->SetTitle("Vertex multiplicity");
  h1_RefFinal_Nvtx_ResolutionEy->GetYaxis()->SetTitle("E_{y}^{miss} resolution (GeV)");
  m_thistSvc->regHist("/AANT/" + m_folderName + "PileUp/h1_RefFinal_Nvtx_ResolutionEy",h1_RefFinal_Nvtx_ResolutionEy).ignore();
  
  // Histograms as function of SumEt --- In bins of vertex multiplicities
  for ( int i = 0; i < m_vtxBins; i++ ) {
    std::ostringstream oss;
    std::string name;
    
    oss.str("");
    oss << "h2_RefFinal_SumEt_RatioEt_vtx" << i;
    name = oss.str();
    h2_RefFinal_SumEt_RatioEt_vtx[i] = new TH2F(name.c_str(),"",m_stBins,0.0,m_stRange,m_rtBins,0.0,m_rtRange);
    h2_RefFinal_SumEt_RatioEt_vtx[i]->GetXaxis()->SetTitle("#Sigma E_{T} (GeV)");
    h2_RefFinal_SumEt_RatioEt_vtx[i]->GetYaxis()->SetTitle("E_{T}^{miss}/True E_{T}^{miss}");
    name.insert(0,"/AANT/" + m_folderName + "PileUp/");
    m_thistSvc->regHist(name.c_str(),h2_RefFinal_SumEt_RatioEt_vtx[i]).ignore();
    
    oss.str("");
    oss << "h2_RefFinal_SumEt_DeltaEt_vtx" << i;
    name = oss.str();
    h2_RefFinal_SumEt_DeltaEt_vtx[i] = new TH2F(name.c_str(),"",m_stBins,0.0,m_stRange,m_etBins,-m_etRange,m_etRange);
    h2_RefFinal_SumEt_DeltaEt_vtx[i]->GetXaxis()->SetTitle("#Sigma E_{T} (GeV)");
    h2_RefFinal_SumEt_DeltaEt_vtx[i]->GetYaxis()->SetTitle("E_{T}^{miss} - True E_{T}^{miss} (GeV)");
    name.insert(0,"/AANT/" + m_folderName + "PileUp/");
    m_thistSvc->regHist(name.c_str(),h2_RefFinal_SumEt_DeltaEt_vtx[i]).ignore();
    
    oss.str("");
    oss << "h2_RefFinal_SumEt_DeltaEx_vtx" << i;
    name = oss.str();
    h2_RefFinal_SumEt_DeltaEx_vtx[i] = new TH2F(name.c_str(),"",m_stBins,0.0,m_stRange,m_etBins,-m_etRange,m_etRange);
    h2_RefFinal_SumEt_DeltaEx_vtx[i]->GetXaxis()->SetTitle("#Sigma E_{T} (GeV)");
    h2_RefFinal_SumEt_DeltaEx_vtx[i]->GetYaxis()->SetTitle("E_{x}^{miss} - True E_{x}^{miss} (GeV)");
    name.insert(0,"/AANT/" + m_folderName + "PileUp/");
    m_thistSvc->regHist(name.c_str(),h2_RefFinal_SumEt_DeltaEx_vtx[i]).ignore();
    
    oss.str("");
    oss << "h2_RefFinal_SumEt_DeltaEy_vtx" << i;
    name = oss.str();
    h2_RefFinal_SumEt_DeltaEy_vtx[i] = new TH2F(name.c_str(),"",m_stBins,0.0,m_stRange,m_etBins,-m_etRange,m_etRange);
    h2_RefFinal_SumEt_DeltaEy_vtx[i]->GetXaxis()->SetTitle("#Sigma E_{T} (GeV)");
    h2_RefFinal_SumEt_DeltaEy_vtx[i]->GetYaxis()->SetTitle("E_{y}^{miss} - True E_{y}^{miss} (GeV)");
    name.insert(0,"/AANT/" + m_folderName + "PileUp/");
    m_thistSvc->regHist(name.c_str(),h2_RefFinal_SumEt_DeltaEy_vtx[i]).ignore();
    
    oss.str("");
    oss << "h1_RefFinal_SumEt_ResponseEt_vtx" << i;
    name = oss.str();
    h1_RefFinal_SumEt_ResponseEt_vtx[i] = new TH1F(name.c_str(),"",m_stBins,0.0,m_stRange);
    h1_RefFinal_SumEt_ResponseEt_vtx[i]->GetXaxis()->SetTitle("#Sigma E_{T} (GeV)");
    h1_RefFinal_SumEt_ResponseEt_vtx[i]->GetYaxis()->SetTitle("E_{T}^{miss} response");
    name.insert(0,"/AANT/" + m_folderName + "PileUp/");
    m_thistSvc->regHist(name.c_str(),h1_RefFinal_SumEt_ResponseEt_vtx[i]).ignore();
        
    oss.str("");
    oss << "h1_RefFinal_SumEt_OffsetEt_vtx" << i;
    name = oss.str();
    h1_RefFinal_SumEt_OffsetEt_vtx[i] = new TH1F(name.c_str(),"",m_stBins,0.0,m_stRange);
    h1_RefFinal_SumEt_OffsetEt_vtx[i]->GetXaxis()->SetTitle("#Sigma E_{T} (GeV)");
    h1_RefFinal_SumEt_OffsetEt_vtx[i]->GetYaxis()->SetTitle("E_{T}^{miss} offset (GeV)");
    name.insert(0,"/AANT/" + m_folderName + "PileUp/");
    m_thistSvc->regHist(name.c_str(),h1_RefFinal_SumEt_OffsetEt_vtx[i]).ignore();
    
    oss.str("");
    oss << "h1_RefFinal_SumEt_ResolutionEt_vtx" << i;
    name = oss.str();
    h1_RefFinal_SumEt_ResolutionEt_vtx[i] = new TH1F(name.c_str(),"",m_stBins,0.0,m_stRange);
    h1_RefFinal_SumEt_ResolutionEt_vtx[i]->GetXaxis()->SetTitle("#Sigma E_{T} (GeV)");
    h1_RefFinal_SumEt_ResolutionEt_vtx[i]->GetYaxis()->SetTitle("E_{T}^{miss} resolution (GeV)");
    name.insert(0,"/AANT/" + m_folderName + "PileUp/");
    m_thistSvc->regHist(name.c_str(),h1_RefFinal_SumEt_ResolutionEt_vtx[i]).ignore();
    
    oss.str("");
    oss << "h1_RefFinal_SumEt_ResolutionEx_vtx" << i;
    name = oss.str();
    h1_RefFinal_SumEt_ResolutionEx_vtx[i] = new TH1F(name.c_str(),"",m_stBins,0.0,m_stRange);
    h1_RefFinal_SumEt_ResolutionEx_vtx[i]->GetXaxis()->SetTitle("#Sigma E_{T} (GeV)");
    h1_RefFinal_SumEt_ResolutionEx_vtx[i]->GetYaxis()->SetTitle("E_{x}^{miss} resolution (GeV)");
    name.insert(0,"/AANT/" + m_folderName + "PileUp/");
    m_thistSvc->regHist(name.c_str(),h1_RefFinal_SumEt_ResolutionEx_vtx[i]).ignore();
    
    oss.str("");
    oss << "h1_RefFinal_SumEt_ResolutionEy_vtx" << i;
    name = oss.str();
    h1_RefFinal_SumEt_ResolutionEy_vtx[i] = new TH1F(name.c_str(),"",m_stBins,0.0,m_stRange);
    h1_RefFinal_SumEt_ResolutionEy_vtx[i]->GetXaxis()->SetTitle("#Sigma E_{T} (GeV)");
    h1_RefFinal_SumEt_ResolutionEy_vtx[i]->GetYaxis()->SetTitle("E_{y}^{miss} resolution (GeV)");
    name.insert(0,"/AANT/" + m_folderName + "PileUp/");
    m_thistSvc->regHist(name.c_str(),h1_RefFinal_SumEt_ResolutionEy_vtx[i]).ignore();
    
  }
  
  // Profile of SumEt as function of vertex multiplicity
  hp_RefFinal_Nvtx_SumEt = new TProfile("hp_RefFinal_Nvtx_SumEt","",m_vtxRange,-0.5,m_vtxRange-0.5);
  hp_RefFinal_Nvtx_SumEt->GetXaxis()->SetTitle("Vertex multiplicity");
  hp_RefFinal_Nvtx_SumEt->GetYaxis()->SetTitle("#Sigma E_{T} (GeV)");
  m_thistSvc->regHist("/AANT/" + m_folderName + "PileUp/hp_RefFinal_Nvtx_SumEt",hp_RefFinal_Nvtx_SumEt).ignore();
  
  return StatusCode::SUCCESS;
}


StatusCode PileUpTool::initialize()
{
  return StatusCode::SUCCESS;
}


StatusCode PileUpTool::execute(MissingETData *data)
{
  msg() << MSG::DEBUG << "PileUpTool execute() has been called" << endreq;
  
  StatusCode sc = pileUp(data);
  
  return sc;
}


StatusCode PileUpTool::pileUp(MissingETData *data)
{
  msg() << MSG::DEBUG << "in pileUp()" << endreq;
  
  // Get vertex multiplicity
  
  // Retrieve vertex container
  const VxContainer *vtxCont = 0;
  StatusCode sc = evtStore()->retrieve( vtxCont, m_vtxContName );
  if ( sc.isFailure() ) {
    msg() << MSG::ERROR << "Could not retrieve VxCandidate container" << endreq;
    return sc;
  }
  msg() << MSG::DEBUG << "VxCandidate container successfully retrieved" << endreq;
  
  // Loop on vertices and count valide ones
  int nVtx = 0;
  VxContainer::const_iterator vtxItr = vtxCont->begin();
  VxContainer::const_iterator vtxEnd = vtxCont->end();
  for ( ; vtxItr != vtxEnd; vtxItr++ ) {
    if ( (*vtxItr)->vxTrackAtVertex()->size() >= m_minTracksPerVertex ) nVtx++;
  }
  msg() << MSG::DEBUG << "Numer of valid vertices = " << nVtx << endreq;
  msg() << MSG::DEBUG << "Numer of total vertices = " << vtxCont->size() << endreq;
  
  // Vertex bin index
  int iVtx = nVtx*m_vtxBins/m_vtxRange;
  if ( iVtx >= m_vtxBins ) iVtx = m_vtxBins-1;
  
  // Fill 1-D histograms
  h1_Nvtx->Fill( float(nVtx) );
  
  // Fill 2-D histograms
  const MissingET *missingET = data->refFinal();
  if ( missingET == 0 || data->truth_nonInt() == 0 ) {
    msg() << MSG::ERROR << "Could not retrieve MissingET data" << endreq;
    return StatusCode::FAILURE;
  }
  float exNonInt = data->truth_nonInt()->etx();
  float eyNonInt = data->truth_nonInt()->ety();
  float etNonInt = sqrt( exNonInt*exNonInt+eyNonInt*eyNonInt );
  if ( etNonInt > 1.0E-05*GeV ) {
    h2_RefFinal_Nvtx_RatioEt->Fill( float(nVtx), (missingET->et()/etNonInt) );
    h2_RefFinal_SumEt_RatioEt_vtx[iVtx]->Fill( missingET->sumet()/GeV, (missingET->et()/etNonInt) );
  }
  h2_RefFinal_Nvtx_DeltaEt->Fill( float(nVtx), (missingET->et()-etNonInt)/GeV );
  h2_RefFinal_Nvtx_DeltaEx->Fill( float(nVtx), (missingET->etx()-exNonInt)/GeV );
  h2_RefFinal_Nvtx_DeltaEy->Fill( float(nVtx), (missingET->ety()-eyNonInt)/GeV );
  h2_RefFinal_SumEt_RatioEt_vtx[iVtx]->Fill( missingET->sumet()/GeV, (missingET->et()/etNonInt) );
  h2_RefFinal_SumEt_DeltaEt_vtx[iVtx]->Fill( missingET->sumet()/GeV, (missingET->et()-etNonInt)/GeV );
  h2_RefFinal_SumEt_DeltaEx_vtx[iVtx]->Fill( missingET->sumet()/GeV, (missingET->etx()-exNonInt)/GeV );
  h2_RefFinal_SumEt_DeltaEy_vtx[iVtx]->Fill( missingET->sumet()/GeV, (missingET->ety()-eyNonInt)/GeV );
  
  //Fill profile histograms
  hp_RefFinal_Nvtx_SumEt->Fill( float(nVtx), missingET->sumet()/GeV );
  
  return StatusCode::SUCCESS;
}


StatusCode PileUpTool::CBNT_finalize()
{
  msg() << MSG::DEBUG << "PileUpTool CBNT_finalize() has been called" << endreq;
  
  getMeanProfile( h2_RefFinal_Nvtx_RatioEt, h1_RefFinal_Nvtx_ResponseEt );
  getMeanProfile( h2_RefFinal_Nvtx_DeltaEt, h1_RefFinal_Nvtx_OffsetEt );
  getResolutionProfile( h2_RefFinal_Nvtx_DeltaEt, h1_RefFinal_Nvtx_ResolutionEt );
  getResolutionProfile( h2_RefFinal_Nvtx_DeltaEx, h1_RefFinal_Nvtx_ResolutionEx );
  getResolutionProfile( h2_RefFinal_Nvtx_DeltaEy, h1_RefFinal_Nvtx_ResolutionEy );
  
  for ( int i = 0; i < m_vtxBins; i++ ) {
    getMeanProfile( h2_RefFinal_SumEt_RatioEt_vtx[i], h1_RefFinal_SumEt_ResponseEt_vtx[i] );
    getMeanProfile( h2_RefFinal_SumEt_DeltaEt_vtx[i], h1_RefFinal_SumEt_OffsetEt_vtx[i] );
    getResolutionProfile( h2_RefFinal_SumEt_DeltaEt_vtx[i], h1_RefFinal_SumEt_ResolutionEt_vtx[i] );
    getResolutionProfile( h2_RefFinal_SumEt_DeltaEx_vtx[i], h1_RefFinal_SumEt_ResolutionEx_vtx[i] );
    getResolutionProfile( h2_RefFinal_SumEt_DeltaEy_vtx[i], h1_RefFinal_SumEt_ResolutionEy_vtx[i] );
  }
  
  return StatusCode::SUCCESS;
}


bool PileUpTool::getProfile( TH2F *hIn, TH1F *hOut, bool doMean )
{
  // Get histogram binning informations
  int   nBinsX = hIn->GetNbinsX();
  float minY = hIn->GetYaxis()->GetXmin();
  float maxY = hIn->GetYaxis()->GetXmax();
  
  // Check consistency of output histogram
  if ( nBinsX != hOut->GetNbinsX() ) {
    msg() << MSG::WARNING << "in getProfile(): input and output histograms have inconsitent binnings" << endreq;
    return false;
  }
  
  // Temporary variables
  TH1F *hTmp = 0;
  TF1  *fGaus = 0;
  if ( m_doFit ) fGaus = new TF1("fGaus","gaus",minY,maxY);
  
  for ( int i = 0; i < nBinsX; i++ ) {
    // Get 1d-slice of a bin in X
    hTmp = (TH1F*)hIn->ProjectionY("_py",i+1,i+1);
    
    // Get its standard deviation
    if ( hTmp->GetEntries() > 0 ) {
      float content = 0.0, error = 0.0;
      
      // Do a Gaussian fit
      if ( m_doFit ) {
        hTmp->Fit(fGaus,"QN");
        if ( doMean ) {
          content = fGaus->GetParameter(1);
          error   = fGaus->GetParError(1);
        }
        else {
          content = fGaus->GetParameter(2);
          error   = fGaus->GetParError(2);
        }
      }
      // Or get mean and RMS
      else {
        if ( doMean ) {
          content = hTmp->GetMean();
          error   = hTmp->GetMeanError();
        }
        else {
          content = hTmp->GetRMS();
          error   = hTmp->GetRMSError();
        }
      }
      
      // Set to output histogram
      hOut->SetBinContent(i+1,content);
      hOut->SetBinError(i+1,error);
    }
    
    // Delete histogram created by ProjectionY()
    delete hTmp;
  }
  
  if ( m_doFit ) delete fGaus;
  return true;
}


StatusCode PileUpTool::finalize()
{
  // Deallocate dynamic arrays
  delete [] h2_RefFinal_SumEt_RatioEt_vtx;
  delete [] h2_RefFinal_SumEt_DeltaEt_vtx;
  delete [] h2_RefFinal_SumEt_DeltaEx_vtx;
  delete [] h2_RefFinal_SumEt_DeltaEy_vtx;
  delete [] h1_RefFinal_SumEt_ResponseEt_vtx;
  delete [] h1_RefFinal_SumEt_OffsetEt_vtx;
  delete [] h1_RefFinal_SumEt_ResolutionEt_vtx;
  delete [] h1_RefFinal_SumEt_ResolutionEx_vtx;
  delete [] h1_RefFinal_SumEt_ResolutionEy_vtx;
  
  return StatusCode::SUCCESS;
}


PileUpTool::~PileUpTool()
{
}
