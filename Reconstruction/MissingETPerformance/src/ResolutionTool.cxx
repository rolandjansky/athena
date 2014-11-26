/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/GaudiException.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/AlgFactory.h"

#include "CLHEP/Units/PhysicalConstants.h"

#include "MissingETPerformance/ResolutionTool.h"

#include "TCanvas.h"
#include "TLegend.h"
#include "TF1.h"
#include "TGraphErrors.h"

//------------------------------------------------------------------------------
ResolutionTool::ResolutionTool( const std::string& type,
                  const std::string& name, 
                  const IInterface* parent )
  : AthAlgTool( type, name, parent ) {
  declareInterface<ResolutionTool>( this );

  declareProperty("numptt",          m_numptt=20);
  declareProperty("startpt",         m_startpt=5.);
  declareProperty("binpt",	     m_binpt=10.);
  declareProperty("scares",	     m_scares=50.);
  declareProperty("curvasres",       m_curvasres=15.);
  declareProperty("fitrange",	     m_fitrange=3);
  declareProperty("ns",	             m_ns=2.);
  declareProperty("percerr",         m_percerr=0.15);
  declareProperty("entries",         m_entries=100);

  declareProperty("iset",            m_iset=6020);
  declareProperty("nbinres1",        m_nbinres1=100);
  declareProperty("scares1",         m_scares1=50.);
  declareProperty("nbinres2",        m_nbinres2=100);
  declareProperty("scares2",         m_scares2=500.);
  declareProperty("nbinres3",        m_nbinres3=200);
  declareProperty("scares3",         m_scares3=100.);
  declareProperty("nbinphires1",     m_nbinphires1=315);

  declareProperty("lowET",           m_lowET=-5.);
  declareProperty("suET",            m_suET=995.);

  declareProperty("useTruth",        m_useTruth=1);
  declareProperty("usesumet",        m_use_sumet=1);
  declareProperty("FolderName",      m_folderName="");
}

//------------------------------------------------------------------------------

StatusCode ResolutionTool::CBNT_initialize() {
  StatusCode sc = service("THistSvc", m_thistSvc);

  msg() << MSG::DEBUG << "Res Tool CBNT_initialize() has been called" << endreq;

  if (sc.isFailure()) {
     msg() << MSG::ERROR
          << "Unable to retrieve pointer to THistSvc"
          << endreq;
     return sc;
  }

  // beamdata
  if(m_iset >87000) {
    m_nbinres1=200;
    m_scares1=100.;
  }

 h_DetOnlyRefFinalResolution_x = new TH1D("h_DetOnlyRefFinalResolution_x","DetOnlyRefFinalResolution(x)",m_nbinres1,-m_scares1,m_scares1 );
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "Resolution/DetOnlyRefFinalResolution_x",h_DetOnlyRefFinalResolution_x);

  h_DetOnlyRefFinalResolution_y = new TH1D("h_DetOnlyRefFinalResolution_y","DetOnlyRefFinalResolution(y)",m_nbinres1,-m_scares1,m_scares1);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "Resolution/DetOnlyRefFinalResolution_y",h_DetOnlyRefFinalResolution_y);

  h_DetOnlyRefFinalResolution_EtMiss = new TH1D("h_DetOnlyRefFinalResolution_EtMiss","DetOnlyRefFinalResolution(EtMiss)",m_nbinres1,-m_scares1,m_scares1);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "Resolution/DetOnlyRefFinalResolution_EtMiss",h_DetOnlyRefFinalResolution_EtMiss);

  h_DetOnlyRefFinalResolution_V_truth_x = new TH2D("h_DetOnlyRefFinalResolution_V_truth_x","DetOnlyRefFinalResolution_V_truth(x)",m_nbinres2,-m_scares2,m_scares2,m_nbinres3,-m_scares3,m_scares3 );
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "Resolution/DetOnlyRefFinalResolution_V_truth_x",h_DetOnlyRefFinalResolution_V_truth_x);

  h_DetOnlyRefFinalResolution_V_truth_y = new TH2D("h_DetOnlyRefFinalResolution_V_truth_y","DetOnlyRefFinalResolution_V_truth(y)",m_nbinres2,-m_scares2,m_scares2,m_nbinres3,-m_scares3,m_scares3 );
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "Resolution/DetOnlyRefFinalResolution_V_truth_y",h_DetOnlyRefFinalResolution_V_truth_y);

  h_DetOnlyRefFinalReco_V_truth_x = new TH2D("h_DetOnlyRefFinalReco_V_truth_x","DetOnlyRefFinalReco_V_truth(x)",m_nbinres2,-m_scares2,m_scares2,m_nbinres2,-m_scares2,m_scares2 );
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "Resolution/DetOnlyRefFinalReco_V_truth_x",h_DetOnlyRefFinalReco_V_truth_x);

  h_DetOnlyRefFinalReco_V_truth_y = new TH2D("h_DetOnlyRefFinalReco_V_truth_y","DetOnlyRefFinalReco_V_truth(y)",m_nbinres2,-m_scares2,m_scares2,m_nbinres2,-m_scares2,m_scares2 );
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "Resolution/DetOnlyRefFinalReco_V_truth_y",h_DetOnlyRefFinalReco_V_truth_y);

  h_RefFinalResolution_x = new TH1D("h_RefFinalResolution_x","RefFinalResolution(x)",m_nbinres1,-m_scares1,m_scares1);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "Resolution/RefFinalResolution_x",h_RefFinalResolution_x);

  h_RefFinalResolution_y = new TH1D("h_RefFinalResolution_y","RefFinalResolution(y)",m_nbinres1,-m_scares1,m_scares1);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "Resolution/RefFinalResolution_y",h_RefFinalResolution_y);

  h_RefFinalResolution_EtMiss = new TH1D("h_RefFinalResolution_EtMiss","RefFinalResolution(EtMiss)",m_nbinres1,-m_scares1,m_scares1);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "Resolution/RefFinalResolution_EtMiss",h_RefFinalResolution_EtMiss);

  h_RefFinalResolution_phi = new TH1D("h_RefFinalResolution_phi","RefFinalResolution(phi)",m_nbinphires1,0.,3.15);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "Resolution/RefFinalResolution_phi",h_RefFinalResolution_phi);

  h_RefFinalResolution_phi_V_truth_EtMiss = new TH2D("h_RefFinalResolution_phi_V_truth_EtMiss","RefFinalResolution(phi) vs truth EtMiss",
						     m_nbinres2,0.,m_scares2,m_nbinphires1,0.,3.15);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "Resolution/RefFinalResolution_phi_V_truth_EtMiss", h_RefFinalResolution_phi_V_truth_EtMiss); 

  h_RefFinalResolution_V_truth_x = new TH2D("h_RefFinalResolution_V_truth_x","RefFinalResolution_V_truth(x)",m_nbinres2,-m_scares2,m_scares2,m_nbinres3,-m_scares3,m_scares3 );
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "Resolution/RefFinalResolution_V_truth_x",h_RefFinalResolution_V_truth_x);

  h_RefFinalResolution_V_truth_y = new TH2D("h_RefFinalResolution_V_truth_y","RefFinalResolution_V_truth(y)",m_nbinres2,-m_scares2,m_scares2,m_nbinres3,-m_scares3,m_scares3 );
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "Resolution/RefFinalResolution_V_truth_y",h_RefFinalResolution_V_truth_y);

  h_RefFinalReco_V_truth_x = new TH2D("h_RefFinalReco_V_truth_x","RefFinalReco_V_truth(x)",m_nbinres2,-m_scares2,m_scares2,m_nbinres2,-m_scares2,m_scares2 );
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "Resolution/RefFinalReco_V_truth_x",h_RefFinalReco_V_truth_x);

  h_RefFinalReco_V_truth_y = new TH2D("h_RefFinalReco_V_truth_y","RefFinalReco_V_truth(y)",m_nbinres2,-m_scares2,m_scares2,m_nbinres2,-m_scares2,m_scares2 );
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "Resolution/RefFinalReco_V_truth_y",h_RefFinalReco_V_truth_y);

  h_MuonBoyResolution_x = new TH1D("h_MuonBoyResolution_x","MuonBoyResolution(x)",m_nbinres1,-m_scares1,m_scares1);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "Resolution/MuonBoyResolution_x",h_MuonBoyResolution_x);

  h_MuonBoyResolution_y = new TH1D("h_MuonBoyResolution_y","MuonBoyResolution(y)",m_nbinres1,-m_scares1,m_scares1);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "Resolution/MuonBoyResolution_y",h_MuonBoyResolution_y);

  h_MuonBoyResolution_EtMiss = new TH1D("h_MuonBoyResolution_EtMiss","MuonBoyResolution(EtMiss)",m_nbinres1,-m_scares1,m_scares1);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "Resolution/MuonBoyResolution_EtMiss",h_MuonBoyResolution_EtMiss);

  h_MuonBoyResolution_V_truth_x = new TH2D("h_MuonBoyResolution_V_truth_x","MuonBoyResolution_V_truth(x)",m_nbinres2,-m_scares2,m_scares2,m_nbinres3,-m_scares3,m_scares3 );
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "Resolution/MuonBoyResolution_V_truth_x",h_MuonBoyResolution_V_truth_x);

  h_MuonBoyResolution_V_truth_y = new TH2D("h_MuonBoyResolution_V_truth_y","MuonBoyResolution_V_truth(y)",m_nbinres2,-m_scares2,m_scares2,m_nbinres3,-m_scares3,m_scares3 );
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "Resolution/MuonBoyResolution_V_truth_y",h_MuonBoyResolution_V_truth_y);

  h_MuonBoyReco_V_truth_x = new TH2D("h_MuonBoyReco_V_truth_x","MuonBoyReco_V_truth(x)",m_nbinres2,-m_scares2,m_scares2,m_nbinres2,-m_scares2,m_scares2 );
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "Resolution/MuonBoyReco_V_truth_x",h_MuonBoyReco_V_truth_x);

  h_MuonBoyReco_V_truth_y = new TH2D("h_MuonBoyReco_V_truth_y","MuonBoyReco_V_truth(y)",m_nbinres2,-m_scares2,m_scares2,m_nbinres2,-m_scares2,m_scares2 );
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "Resolution/MuonBoyReco_V_truth_y",h_MuonBoyReco_V_truth_y);

  h_DetOnlyCorrResolution_x = new TH1D("h_DetOnlyCorrResolution_x","DetOnlyCorrResolution(x)",m_nbinres1,-m_scares1,m_scares1 );
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "Resolution/DetOnlyCorrResolution_x",h_DetOnlyCorrResolution_x);

  h_DetOnlyCorrResolution_y = new TH1D("h_DetOnlyCorrResolution_y","DetOnlyCorrResolution(y)",m_nbinres1,-m_scares1,m_scares1);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "Resolution/DetOnlyCorrResolution_y",h_DetOnlyCorrResolution_y);

  h_FinalCorrResolution_x = new TH1D("h_FinalCorrResolution_x","FinalCorrResolution(x)",m_nbinres1,-m_scares1,m_scares1);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "Resolution/FinalCorrResolution_x",h_FinalCorrResolution_x);

  h_FinalCorrResolution_y = new TH1D("h_FinalCorrResolution_y","FinalCorrResolution(y)",m_nbinres1,-m_scares1,m_scares1);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "Resolution/FinalCorrResolution_y",h_FinalCorrResolution_y);

  h_DetOnlyCorrResolution_EtMiss = new TH1D("h_DetOnlyCorrResolution_EtMiss","DetOnlyCorrResolution(EtMiss)",m_nbinres1,-m_scares1,m_scares1);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "Resolution/DetOnlyCorrResolution_EtMiss",h_DetOnlyCorrResolution_EtMiss);

  h_FinalCorrResolution_EtMiss = new TH1D("h_FinalCorrResolution_EtMiss","FinalCorrResolution(EtMiss)",m_nbinres3,-m_scares3,m_scares3);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "Resolution/FinalCorrResolution_EtMiss",h_FinalCorrResolution_EtMiss);

//effect of not adding CellOut
  h_RefFinalHTResolution_x = new TH1D("h_RefFinalHTResolution_x","RefFinalHTResolution(x)",m_nbinres1,-m_scares1,m_scares1);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "Resolution/RefFinalHTResolution_x",h_RefFinalHTResolution_x);

  h_RefFinalHTResolution_y = new TH1D("h_RefFinalHTResolution_y","RefFinalHTResolution(y)",m_nbinres1,-m_scares1,m_scares1);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "Resolution/RefFinalHTResolution_y",h_RefFinalHTResolution_y);

  h_RefFinalHTResolution_EtMiss = new TH1D("h_RefFinalHTResolution_EtMiss","RefFinalHTResolution(EtMiss)",m_nbinres1,-m_scares1,m_scares1);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "Resolution/RefFinalHTResolution_EtMiss",h_RefFinalHTResolution_EtMiss);

 h_tot = new TH1D("h_tot","h_tot", 100,-m_scares,m_scares);
 sc = m_thistSvc->regHist("/AANT/" + m_folderName + "Resolution/htot",h_tot);
 h2_tot = new TH1D("h2_tot","h2_tot", 100,-m_scares,m_scares);
 sc = m_thistSvc->regHist("/AANT/" + m_folderName + "Resolution/h2tot",h2_tot);
 h3_tot = new TH1D("h3_tot","h3_tot", 100,-m_scares,m_scares);
 sc = m_thistSvc->regHist("/AANT/" + m_folderName + "Resolution/h3tot",h3_tot);
 h4_tot = new TH1D("h4_tot","h4_tot", 100,-m_scares,m_scares);
 sc = m_thistSvc->regHist("/AANT/" + m_folderName + "Resolution/h4tot",h4_tot);
 h5_tot = new TH1D("h5_tot","h5_tot", 100,-m_scares,m_scares);
 sc = m_thistSvc->regHist("/AANT/" + m_folderName + "Resolution/h5tot",h5_tot);
 h6_tot = new TH1D("h6_tot","h6_tot", 100,-m_scares,m_scares);
 sc = m_thistSvc->regHist("/AANT/" + m_folderName + "Resolution/h6tot",h6_tot);
 h7_tot = new TH1D("h7_tot","h7_tot", 100,-m_scares,m_scares);
 sc = m_thistSvc->regHist("/AANT/" + m_folderName + "Resolution/h7tot",h7_tot);

  h_sumetT = new TH1D("h_sumetT","h_sumetT", 100,m_lowET,m_suET);
 sc = m_thistSvc->regHist("/AANT/" + m_folderName + "Resolution/hsumetT",h_sumetT);
  h_sumetHT = new TH1D("h_sumetHT","h_sumetHT", 100,m_lowET,m_suET);
 sc = m_thistSvc->regHist("/AANT/" + m_folderName + "Resolution/hsumetHT",h_sumetHT);
  h_sumet = new TH1D("h_sumet","h_sumet", 100,m_lowET,m_suET);
 sc = m_thistSvc->regHist("/AANT/" + m_folderName + "Resolution/hsumet",h_sumet);
 h2_sumet = new TH1D("h2_sumet","h2_sumet", 100,m_lowET,m_suET);
 sc = m_thistSvc->regHist("/AANT/" + m_folderName + "Resolution/h2sumet",h2_sumet);
 h3_sumet = new TH1D("h3_sumet","h3_sumet", 100,m_lowET,m_suET);
 sc = m_thistSvc->regHist("/AANT/" + m_folderName + "Resolution/h3sumet",h3_sumet);
 h4_sumet = new TH1D("h4_sumet","h4_sumet", 100,m_lowET,m_suET);
 sc = m_thistSvc->regHist("/AANT/" + m_folderName + "Resolution/h4sumet",h4_sumet);
 h5_sumet = new TH1D("h5_sumet","h5_sumet", 100,m_lowET,m_suET);
 sc = m_thistSvc->regHist("/AANT/" + m_folderName + "Resolution/h5sumet",h5_sumet);
 h6_sumet = new TH1D("h6_sumet","h6_sumet", 100,m_lowET,m_suET);
 sc = m_thistSvc->regHist("/AANT/" + m_folderName + "Resolution/h6sumet",h6_sumet);
 h7_sumet = new TH1D("h7_sumet","h7_sumet", 100,m_lowET,m_suET);
 sc = m_thistSvc->regHist("/AANT/" + m_folderName + "Resolution/h7sumet",h7_sumet);

  msg() << MSG:: WARNING <<"m_lowET "<< m_lowET << "m_suET "<< m_suET <<endreq;
  msg() << MSG:: WARNING <<"m_useTruth "<<m_useTruth<< "m_use_sumet "<<m_use_sumet<<endreq;

 //donatella Resolution curves
  //curvaresol---------------------------------
  m_curvasET=m_startpt+m_numptt*m_binpt;

  //book histos for curvaresol vs SumET 
  std::string name1;
  std::string name12;
  std::string name13;
  std::string name14;
  std::string name15;
  std::string name16;
  std::string name17;

  for (int ih= 0; ih< m_numptt ; ih++){
    name1 = Form("h_%d",ih);
    h_.push_back(new TH1F(name1.c_str(), name1.c_str() , 100,-m_scares,m_scares));
    sc = m_thistSvc->regHist("/AANT/" + m_folderName + "Resolution/Binned/" + name1,h_.at(ih));
    name12 = Form("h2_%d",ih);
    h2_.push_back(new TH1F(name12.c_str(), name12.c_str() , 100,-m_scares,m_scares));
    sc = m_thistSvc->regHist("/AANT/" + m_folderName + "Resolution/Binned/" + name12,h2_.at(ih));
    name13 = Form("h3_%d",ih);
    h3_.push_back(new TH1F(name13.c_str(), name13.c_str() , 100,-m_scares,m_scares));
    sc = m_thistSvc->regHist("/AANT/" + m_folderName + "Resolution/Binned/" + name13,h3_.at(ih));
    name14 = Form("h4_%d",ih);
    h4_.push_back(new TH1F(name14.c_str(), name14.c_str() , 100,-m_scares,m_scares));
    sc = m_thistSvc->regHist("/AANT/" + m_folderName + "Resolution/Binned/" + name14,h4_.at(ih));
    name15 = Form("h5_%d",ih);
    h5_.push_back(new TH1F(name15.c_str(), name15.c_str() , 100,-m_scares,m_scares));
    sc = m_thistSvc->regHist("/AANT/" + m_folderName + "Resolution/Binned/" + name15,h5_.at(ih));
    name16 = Form("h6_%d",ih);
    h6_.push_back(new TH1F(name16.c_str(), name16.c_str() , 100,-m_scares,m_scares));
    sc = m_thistSvc->regHist("/AANT/" + m_folderName + "Resolution/Binned/" + name16,h6_.at(ih));
    name17 = Form("h7_%d",ih);
    h7_.push_back(new TH1F(name17.c_str(), name17.c_str() , 100,-m_scares,m_scares));
    sc = m_thistSvc->regHist("/AANT/" + m_folderName + "Resolution/Binned/" + name17,h7_.at(ih));

  }

  // book histo curva resol vs SumET 
  he_resvset = new TH2F("resvset","resolution vs set",100,0.,m_curvasET,100,0.,m_curvasres);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "Resolution/resvset",he_resvset);

  //curvaresol---------------------------------

  if (sc.isFailure()) {
     msg() << MSG::ERROR
          << "ERROR in CBNT_initialize"
          << endreq;
     return sc;
  }

  return sc;
}

StatusCode ResolutionTool::initialize() {
  msg() << MSG::DEBUG << "Res Tool initialize() has been called" << endreq;

  return StatusCode::SUCCESS;
}

StatusCode ResolutionTool::execute(MissingETData *data) {

  /** check that the input and the output containers are defined */
  StatusCode sc = StatusCode::SUCCESS;

  msg() << MSG::DEBUG << "execute() has been called" << endreq;

  sc = resolution(data);

  if (sc.isFailure()) {msg() << MSG::ERROR << "Error in ResolutionTool::execute()" << endreq;}

  return sc;
}

StatusCode ResolutionTool::resolution(MissingETData *data) {

  msg() << MSG::DEBUG << "in resolution() " << endreq;

  StatusCode sc = StatusCode::SUCCESS;

  h_DetOnlyRefFinalResolution_x->Fill( (data->truth_int()->etx()+data->truth_muons()->etx()-data->refFinal()->etx()) / 1000. );
  h_DetOnlyRefFinalResolution_y->Fill( (data->truth_int()->ety()+data->truth_muons()->ety()-data->refFinal()->ety()) / 1000. );
  h_DetOnlyRefFinalResolution_EtMiss->Fill( (sqrt(pow(data->truth_int()->etx()+data->truth_muons()->etx(),2)+pow(data->truth_int()->ety()+data->truth_muons()->ety(),2))-
					  sqrt(pow(data->refFinal()->etx(),2)+pow(data->refFinal()->ety(),2)))
					 /1000. );

  h_DetOnlyRefFinalResolution_V_truth_x->Fill( (data->truth_int()->etx()+data->truth_muons()->etx())/1000. , 
					       (data->truth_int()->etx()+data->truth_muons()->etx()-data->refFinal()->etx()) / 1000. );
  h_DetOnlyRefFinalResolution_V_truth_y->Fill( (data->truth_int()->ety()+data->truth_muons()->ety())/1000. , 
					       (data->truth_int()->ety()+data->truth_muons()->ety()-data->refFinal()->ety()) / 1000. );
  h_DetOnlyRefFinalReco_V_truth_x->Fill( (data->truth_int()->etx()+data->truth_muons()->etx())/1000. , data->refFinal()->etx() / 1000. );
  h_DetOnlyRefFinalReco_V_truth_y->Fill( (data->truth_int()->ety()+data->truth_muons()->ety())/1000. , data->refFinal()->ety() / 1000. );

  h_RefFinalResolution_x->Fill((data->truth_nonInt()->etx() - data->refFinal()->etx())/1000.0);
  h_RefFinalResolution_y->Fill((data->truth_nonInt()->ety() - data->refFinal()->ety())/1000.0);

  h_RefFinalResolution_EtMiss->Fill( (sqrt(pow(data->truth_nonInt()->etx(),2)+pow(data->truth_nonInt()->ety(),2))-sqrt(pow(data->refFinal()->etx(),2)+pow(data->refFinal()->ety(),2)))/1000. );

  float dphi = fabs(data->truth_nonInt()->phi() - data->refFinal()->phi());
  if (dphi > M_PI) dphi = fabs(dphi - 2*M_PI);
  h_RefFinalResolution_phi->Fill(dphi);
  h_RefFinalResolution_phi_V_truth_EtMiss->Fill(sqrt(pow(data->truth_nonInt()->etx(),2)+pow(data->truth_nonInt()->ety(),2))/1000.0,dphi);

  h_RefFinalResolution_V_truth_x->Fill( (data->truth_nonInt()->etx())/1000. , 
					       (data->truth_nonInt()->etx()-data->refFinal()->etx()) / 1000. );
  h_RefFinalResolution_V_truth_y->Fill( (data->truth_nonInt()->ety())/1000. , 
					       (data->truth_nonInt()->ety()-data->refFinal()->ety()) / 1000. );
  h_RefFinalReco_V_truth_x->Fill( (data->truth_nonInt()->etx())/1000. , data->refFinal()->etx() / 1000. );
  h_RefFinalReco_V_truth_y->Fill( (data->truth_nonInt()->ety())/1000. , data->refFinal()->ety() / 1000. );

  h_DetOnlyCorrResolution_x->Fill( (data->truth_int()->etx()-(data->calib()->etx()+data->cryoCone()->etx()))/1000. );
  h_DetOnlyCorrResolution_y->Fill( (data->truth_int()->ety()-(data->calib()->ety()+data->cryoCone()->ety()))/1000. );
  h_FinalCorrResolution_x->Fill( (data->truth_nonInt()->etx()-(data->final()->etx()-data->corrTopo()->etx()+data->calib()->etx()))/1000. );
  h_FinalCorrResolution_y->Fill( (data->truth_nonInt()->ety()-(data->final()->ety()-data->corrTopo()->ety()+data->calib()->ety()))/1000. );

  h_DetOnlyCorrResolution_EtMiss->Fill( (sqrt(pow(data->truth_int()->etx(),2)+pow(data->truth_int()->ety(),2))-
					 sqrt(pow(data->calib()->etx()+data->cryoCone()->etx(),2) +
					      pow(data->calib()->ety()+data->cryoCone()->ety(),2)))
					/1000.);
  h_FinalCorrResolution_EtMiss->Fill( (sqrt(pow(data->truth_nonInt()->etx(),2)+pow(data->truth_nonInt()->ety(),2))-
				      sqrt(pow(data->final()->etx()-data->corrTopo()->etx()+data->calib()->etx(),2)+
					   pow(data->final()->ety()-data->corrTopo()->ety()+data->calib()->ety(),2)))
				     /1000.);

  h_MuonBoyResolution_x->Fill((data->truth_muons()->etx() - data->MuonBoy()->etx())/1000.0);
  h_MuonBoyResolution_y->Fill((data->truth_muons()->ety() - data->MuonBoy()->ety())/1000.0);
  h_MuonBoyResolution_EtMiss->Fill( (sqrt(pow(data->truth_muons()->etx(),2)+pow(data->truth_muons()->ety(),2))-
				     sqrt(pow(data->MuonBoy()->etx(),2)+pow(data->MuonBoy()->ety(),2)))
				    /1000. );

  h_MuonBoyResolution_V_truth_x->Fill( data->truth_muons()->etx()/1000. , 
				       (data->truth_muons()->etx()-data->MuonBoy()->etx()) / 1000. );
  h_MuonBoyResolution_V_truth_y->Fill( data->truth_muons()->ety()/1000. , 
				       (data->truth_muons()->ety()-data->MuonBoy()->ety()) / 1000. );

  h_MuonBoyReco_V_truth_x->Fill( data->truth_muons()->etx()/1000. , data->MuonBoy()->etx() / 1000. );
  h_MuonBoyReco_V_truth_y->Fill( data->truth_muons()->ety()/1000. , data->MuonBoy()->ety() / 1000. );

  float Sumet_per_curve;
  float Sumet_per_curveHT = 0.;
  float Sumet_per_curve1 = 0.;
  float Sumet_per_curve2 = 0.;
  float Sumet_per_curve3 = 0.;
  float Sumet_per_curve4 = 0.;
  float Sumet_per_curve5 = 0.;
  float Sumet_per_curve6 = 0.;
  float Sumet_per_curve7 = 0.;
  float ExMissEmScale;
  float EyMissEmScale;
  float ExMissGlobCalib;
  float EyMissGlobCalib;
  float ExMissFinalLocHad;
  float EyMissFinalLocHad;
  float ExMissRefFinalHT;
  float EyMissRefFinalHT;
  float EtMissRefFinalHT;
  float SumetRefFinalHT;
  float ExMissBaseEmScale;
  float ExMissBaseGlobCalib;
  float EyMissBaseEmScale;
  float EyMissBaseGlobCalib;
  float verx;
  float very;
  float vert;
  float versumet;
  int ih;

  //Truth etmiss and sumet
  verx=data->truth_nonInt()->etx();
  very=data->truth_nonInt()->ety();
  vert=sqrt(verx*verx+very*very);
  versumet=data->truth_int()->sumet();

  Sumet_per_curve=versumet/1000.0;
  //  msg() << MSG:: WARNING <<"Sumet_per_curve "<< Sumet_per_curve << endreq;
  if(m_use_sumet == 1 || m_useTruth == 1) {
    Sumet_per_curve1= Sumet_per_curve;
    Sumet_per_curve2= Sumet_per_curve;
    Sumet_per_curve3= Sumet_per_curve;
    Sumet_per_curve4= Sumet_per_curve;
    Sumet_per_curve5= Sumet_per_curve;
    Sumet_per_curve6= Sumet_per_curve;
    Sumet_per_curve7= Sumet_per_curve;
  }

  ExMissRefFinalHT=data->refFinal()->etx()-data->cellOut()->etx();
  EyMissRefFinalHT=data->refFinal()->ety()-data->cellOut()->ety();
  EtMissRefFinalHT= sqrt(pow(ExMissRefFinalHT,2)+pow(EyMissRefFinalHT,2));
  SumetRefFinalHT=data->refFinal()->sumet()-data->cellOut()->sumet();

  if(m_useTruth == 0) {
    verx=0.;
    very=0.; 
    vert=0.;
    //different reco Sumet.......  used if (m_use_sumet == 2)
    Sumet_per_curve1=data->refFinal()->sumet()/1000.0;
    Sumet_per_curveHT=SumetRefFinalHT/1000.0;
    //  msg() << MSG:: WARNING <<"Sumet_per_curveHT "<< Sumet_per_curveHT << endreq;
    Sumet_per_curve2=data->topo()->sumet()/1000.0;
    Sumet_per_curve3=data->corrTopo()->sumet()/1000.0;
    Sumet_per_curve4=data->final()->sumet()/1000.0;
    Sumet_per_curve5=data->locHadTopo()->sumet()/1000.0;
    Sumet_per_curve6=data->base()->sumet()/1000.0;
    Sumet_per_curve7=data->calib()->sumet()/1000.0;
    //use Sumet EM scale.......
    if(m_use_sumet == 3) {
      Sumet_per_curve1= Sumet_per_curve2;
      //    Sumet_per_curve2= Sumet_per_curve2;
      Sumet_per_curve3= Sumet_per_curve2;
      Sumet_per_curve4= Sumet_per_curve2;
      Sumet_per_curve5= Sumet_per_curve2;
      Sumet_per_curve6= Sumet_per_curve2;
      Sumet_per_curve7= Sumet_per_curve2;
    }
    //use Sumet LH.......
    if(m_use_sumet == 4) {
      Sumet_per_curve1= Sumet_per_curve5;
      Sumet_per_curve2= Sumet_per_curve5;
      Sumet_per_curve3= Sumet_per_curve5;
      Sumet_per_curve4= Sumet_per_curve5;
      //    Sumet_per_curve5= Sumet_per_curve5;
      Sumet_per_curve6= Sumet_per_curve5;
      Sumet_per_curve7= Sumet_per_curve5;
    }
    //use Sumet HT.......
    if(m_use_sumet == 5) {
      Sumet_per_curve1= Sumet_per_curveHT;
      Sumet_per_curve2= Sumet_per_curveHT;
      Sumet_per_curve3= Sumet_per_curveHT;
      Sumet_per_curve4= Sumet_per_curveHT;
      Sumet_per_curve5= Sumet_per_curveHT;
      Sumet_per_curve6= Sumet_per_curveHT;
      Sumet_per_curve7= Sumet_per_curveHT;
    }
    //use Sumet corrTopo.......
    if(m_use_sumet == 6) {
      Sumet_per_curve1= Sumet_per_curve3;
      Sumet_per_curve2= Sumet_per_curve3;
      //Sumet_per_curve3= Sumet_per_curve3;
      Sumet_per_curve4= Sumet_per_curve3;
      Sumet_per_curve5= Sumet_per_curve3;
      Sumet_per_curve6= Sumet_per_curve3;
      Sumet_per_curve7= Sumet_per_curve3;
    }
  }

  h_sumetT->Fill(Sumet_per_curve);
  h_sumetHT->Fill(Sumet_per_curveHT);
  h_sumet->Fill(Sumet_per_curve1);
  h2_sumet->Fill(Sumet_per_curve2);
  h3_sumet->Fill(Sumet_per_curve3);
  h4_sumet->Fill(Sumet_per_curve4);
  h5_sumet->Fill(Sumet_per_curve5);
  h6_sumet->Fill(Sumet_per_curve6);
  h7_sumet->Fill(Sumet_per_curve7);

  ExMissEmScale= data->topo()->etx()+data->MuonBoy()->etx();
  EyMissEmScale= data->topo()->ety()+data->MuonBoy()->ety();
  ExMissGlobCalib= data->corrTopo()->etx()+data->MuonBoy()->etx();
  EyMissGlobCalib= data->corrTopo()->ety()+data->MuonBoy()->ety();
  ExMissFinalLocHad= data->locHadTopo()->etx()+data->MuonBoy()->etx(); 
  EyMissFinalLocHad= data->locHadTopo()->ety()+data->MuonBoy()->ety();
  ExMissBaseEmScale= data->base()->etx()+data->MuonBoy()->etx();
  ExMissBaseGlobCalib= data->calib()->etx()+data->MuonBoy()->etx(); 
  EyMissBaseEmScale= data->base()->ety()+data->MuonBoy()->ety();
  EyMissBaseGlobCalib= data->calib()->ety()+data->MuonBoy()->ety(); 

  h_tot->Fill((verx - data->refFinal()->etx())/1000.0);
  h_tot->Fill((very - data->refFinal()->ety())/1000.0);
  h2_tot->Fill((verx - ExMissEmScale)/1000.0);
  h2_tot->Fill((very - EyMissEmScale)/1000.0);
  h3_tot->Fill((verx - ExMissGlobCalib)/1000.0);
  h3_tot->Fill((very - EyMissGlobCalib)/1000.0);
  h4_tot->Fill((verx - data->final()->etx())/1000.0);
  h4_tot->Fill((very - data->final()->ety())/1000.0);
  h5_tot->Fill((verx - ExMissFinalLocHad)/1000.0);
  h5_tot->Fill((very - EyMissFinalLocHad)/1000.0);
  h6_tot->Fill((verx - ExMissBaseEmScale)/1000.0);
  h6_tot->Fill((very - EyMissBaseEmScale)/1000.0);
  h7_tot->Fill((verx - ExMissBaseGlobCalib)/1000.0);
  h7_tot->Fill((very - EyMissBaseGlobCalib)/1000.0);

  h_RefFinalHTResolution_x->Fill((verx- ExMissRefFinalHT)/1000.0);
  h_RefFinalHTResolution_y->Fill((very- EyMissRefFinalHT)/1000.0);
  h_RefFinalHTResolution_EtMiss->Fill((vert- EtMissRefFinalHT)/1000.0);

  ih =(int)((Sumet_per_curve1-m_startpt)/m_binpt);
  if (Sumet_per_curve1 < m_startpt) {ih = -1;}
  if ( ih >=0 && ih <  m_numptt) {
    h_.at(ih)->Fill((verx - data->refFinal()->etx())/1000.0);
    h_.at(ih)->Fill((very - data->refFinal()->ety())/1000.0);
  }

  //Resolution for various steps of EtMiss calculation
  ih =(int)((Sumet_per_curve2-m_startpt)/m_binpt);
  if (Sumet_per_curve2 < m_startpt) {ih = -1;}
  if ( ih >=0 && ih <  m_numptt) {
    h2_.at(ih)->Fill((verx - ExMissEmScale)/1000.0);
    h2_.at(ih)->Fill((very - EyMissEmScale)/1000.0);
  }
  ih =(int)((Sumet_per_curve3-m_startpt)/m_binpt);
  if (Sumet_per_curve3 < m_startpt) {ih = -1;}
  if ( ih >=0 && ih <  m_numptt) {
    h3_.at(ih)->Fill((verx - ExMissGlobCalib)/1000.0);
    h3_.at(ih)->Fill((very - EyMissGlobCalib)/1000.0);
  }
  ih =(int)((Sumet_per_curve4-m_startpt)/m_binpt);
  if (Sumet_per_curve4 < m_startpt) {ih = -1;}
  if ( ih >=0 && ih <  m_numptt) {
    h4_.at(ih)->Fill((verx - data->final()->etx())/1000.0);
    h4_.at(ih)->Fill((very - data->final()->ety())/1000.0);
  }
  //Resolution for EtMiss with Localhadron calibration
  ih =(int)((Sumet_per_curve5-m_startpt)/m_binpt);
  if (Sumet_per_curve5 < m_startpt) {ih = -1;}
  if ( ih >=0 && ih <  m_numptt) {
    h5_.at(ih)->Fill((verx - ExMissFinalLocHad)/1000.0);
    h5_.at(ih)->Fill((very - EyMissFinalLocHad)/1000.0);
  }
  //very basic EtMiss
  ih =(int)((Sumet_per_curve6-m_startpt)/m_binpt);
  if (Sumet_per_curve6 < m_startpt) {ih = -1;}
  if ( ih >=0 && ih <  m_numptt) {
    h6_.at(ih)->Fill((verx - ExMissBaseEmScale)/1000.0);
    h6_.at(ih)->Fill((very - EyMissBaseEmScale)/1000.0);
  }
  ih =(int)((Sumet_per_curve7-m_startpt)/m_binpt);
  if (Sumet_per_curve7 < m_startpt) {ih = -1;}
  if ( ih >=0 && ih <  m_numptt) {
    h7_.at(ih)->Fill((verx - ExMissBaseGlobCalib)/1000.0);
    h7_.at(ih)->Fill((very - EyMissBaseGlobCalib)/1000.0);
  }

  if (sc.isFailure()) {msg() << MSG::ERROR << "Error in ResolutionTool::resolution()" << endreq;}

  return sc;
}

StatusCode ResolutionTool::CBNT_finalize() {
  if(h_.size() == 0) return StatusCode::SUCCESS;
  StatusCode sc = StatusCode::SUCCESS;
  msg() << MSG::DEBUG << "Res Tool CBNT_finalize() has been called" << endreq;

  if (sc.isFailure()) { msg() << MSG::WARNING << "Res Tool CBNT_finalize() fail" << endreq; }

  const int array_size = 100;
  //double mean[array_size];
  double sigma[array_size];
  //double mean_err[array_size];
  double sigma_err[array_size];
  double ex[array_size];
  double nxbin[array_size];
  //float expsigmaf[array_size];
  int nbinplot=0;
  float xbin;

  int cp=(int)(sqrt((float)m_numptt)+1);
  TCanvas *c1 = new TCanvas("c1", "c1",293,139,700,500);
  c1->Update();
  c1->Divide(cp,cp);
  //

  int ih=0;
  for (int i= 0; i< m_numptt ; i++){
    c1->cd(i+1);
    TH1F* hist  = h_.at(i);

    int entries = (int)(hist->GetEntries());
    sigma[ih]=-10005.;
    sigma_err[ih]=1000.;
    xbin = m_startpt+m_binpt/2.+ i * m_binpt;
    nxbin[ih]=xbin;
    ex[ih]=0.;

    float expsigma=0.5*sqrt(xbin);

    if  ( entries> m_entries) {

      //fit on the whole plot range............................
      if (m_fitrange == 2) {
	hist->Fit( "gaus","","");
      }
      //fit in m_ns sigma (sigma=0.5*sqrt(SumET bin)..........
      if (m_fitrange == 3) {
	hist->Fit( "gaus","","",-m_ns*expsigma,m_ns*expsigma );
      }
      TF1 *fitsf = hist->GetFunction("gaus");

      nbinplot++;
      //mean[ih] = fitsf->GetParameter(1);
      sigma[ih] = fitsf->GetParameter(2);
      //mean_err[ih] = fitsf->GetParError(1);
      sigma_err[ih] = fitsf->GetParError(2);
      //expsigmaf[ih] =m_ns*expsigma;

      he_resvset->Fill(xbin, sigma[ih]);
      //choice of points.....................
      if(sigma[ih] > 0.) {
	double rappo= sigma_err[ih]/sigma[ih];
	if ( rappo > m_percerr ) {
	  sigma[ih]=-10005.;
	  sigma_err[ih]=1000.;
	}
      }
      ih++;
    }
  }

  if(nbinplot == 0) return StatusCode::SUCCESS;
  TCanvas* c2 = new TCanvas("c2"," Resolution vs Sumet from Topo (A0300)",293,139,700,500);
  c2->Update();
  c2->Divide(1,1);
  c2->cd( 1 );

  grres = new TGraphErrors(nbinplot++,nxbin,sigma,ex,sigma_err);
  sc = m_thistSvc->regGraph("/AANT/" + m_folderName + "Resolution/grres",grres);

  if (sc.isFailure()) { msg() << MSG::WARNING << "Res Tool CBNT_finalize() fail" << endreq; }

  grres->SetTitle();

  grres->SetMarkerColor(4);
  grres->SetMarkerStyle(21);
  grres->Draw("AP");
  grres->SetMaximum(m_curvasres);
  grres->SetMinimum(0);
  grres->GetYaxis()->SetTitle("Resolution (GeV)");
  grres->GetXaxis()->SetTitle("#Sigma E_{T}  (GeV)");

  TF1 *f1 = new TF1("f1","[0]*sqrt(x)",0, m_curvasET);
  f1->SetLineColor(4);
  if (nbinplot > 1) {
    grres->Fit("f1","R");
  }

  TLegend *leg = new TLegend(0.55, 0.75, 0.85, 0.95);
  char message[50];
  //sprintf(message,"W#rightarrow e#nu: fit %3.2f #sqrt{Sumet} " , f1->GetParameter(0) );
  sprintf(message,"fit %3.2f #sqrt{Sumet} " , f1->GetParameter(0) );
  leg->AddEntry(grres,message,"pl");

  c2->Update();
  leg->Draw();

  if (sc.isFailure()) { msg() << MSG::WARNING << "Res Tool CBNT_finalize() fail" << endreq; }

  return sc;
}


//------------------------------------------------------------------------------
StatusCode ResolutionTool::finalize() {
  msg() << MSG::DEBUG << "finalize() has been called" << endreq;

  return StatusCode::SUCCESS;
}

//------------------------------------------------------------------------------
ResolutionTool::~ResolutionTool() {}


