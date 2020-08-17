/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/GaudiException.h"
#include "Gaudi/Property.h"

//#include "CLHEP/Units/PhysicalConstants.h"

#include "MissingETPerformance/LinearityTool.h"
#include "TCanvas.h"
#include "TF1.h"
#include "TLine.h"
#include "TLatex.h"
#include "TMarker.h"
#include "TGraphErrors.h"
#include "AthenaKernel/Units.h"

using Athena::Units::GeV;

//------------------------------------------------------------------------------
LinearityTool::LinearityTool( const std::string& type,
                  const std::string& name, 
                  const IInterface* parent )
  : AthAlgTool( type, name, parent ) {
  declareInterface<LinearityTool>( this );

  declareProperty("linplot_x",         m_linplot_x    = 0.72);
  declareProperty("linplot_y",         m_linplot_y    = 0.85);
  declareProperty("linplot_text",      m_linplot_text);

  declareProperty("lnumptt",      m_lnumptt = 20);
  declareProperty("lstartpt",   m_lstartpt=0.);  
  declareProperty("lbinpt",	  m_lbinpt=5.);    
  declareProperty("fitrangel",  m_fitrangel=2);  
  declareProperty("lnbini",	  m_lnbini=100);    
  declareProperty("percerr",	  m_percerr=0.15); 
  declareProperty("entries",	  m_entries=50);   
  declareProperty("curvaslin",  m_curvaslin=0.3);
  declareProperty("fitr",	  m_fitr=2.5);     
  declareProperty("linorm",  m_linorm=1);
  declareProperty("bin_rp3",  m_bin_rp3=1.);
  declareProperty("suptmi",  m_suptmi=50.);
  declareProperty("etmi_threshold",  m_etmi_threshold=10.);
  declareProperty("FolderName",           m_folderName="");

//if m_linorm=1 Linearity 
//if m_linorm=0 Bias (set: m_curvaslin=10., m_bin_rp3=50.)
}

//------------------------------------------------------------------------------

StatusCode LinearityTool::CBNT_initialize() {
  StatusCode sc = service("THistSvc", m_thistSvc);

  if (sc.isFailure()) {
     msg() << MSG::ERROR
          << "Unable to retrieve pointer to THistSvc"
          << endmsg;
     return sc;
  }

  //book histos for linearity vs ETmiss
  std::string name3;
  std::string name32;
  std::string name33;
  std::string name34;
  std::string name35;
  std::string name36;
  std::string name37;

 m_hl_tot = new TH1D("hl_tot","hl_tot", 100,-m_bin_rp3,m_bin_rp3);
    sc = m_thistSvc->regHist("/AANT/" + m_folderName + "Linearity/hltot",m_hl_tot);
 m_hl2_tot = new TH1D("hl2_tot","hl2_tot", 100,-m_bin_rp3,m_bin_rp3);
    sc = m_thistSvc->regHist("/AANT/" + m_folderName + "Linearity/hl2tot",m_hl2_tot);
 m_hl3_tot = new TH1D("hl3_tot","hl3_tot", 100,-m_bin_rp3,m_bin_rp3);
    sc = m_thistSvc->regHist("/AANT/" + m_folderName + "Linearity/hl3tot",m_hl3_tot);
 m_hl4_tot = new TH1D("hl4_tot","hl4_tot", 100,-m_bin_rp3,m_bin_rp3);
    sc = m_thistSvc->regHist("/AANT/" + m_folderName + "Linearity/hl4tot",m_hl4_tot);
 m_hl5_tot = new TH1D("hl5_tot","hl5_tot", 100,-m_bin_rp3,m_bin_rp3);
    sc = m_thistSvc->regHist("/AANT/" + m_folderName + "Linearity/hl5tot",m_hl5_tot);
 m_hl6_tot = new TH1D("hl6_tot","hl6_tot", 100,-m_bin_rp3,m_bin_rp3);
    sc = m_thistSvc->regHist("/AANT/" + m_folderName + "Linearity/hl6tot",m_hl6_tot);
 m_hl7_tot = new TH1D("hl7_tot","hl7_tot", 100,-m_bin_rp3,m_bin_rp3);
    sc = m_thistSvc->regHist("/AANT/" + m_folderName + "Linearity/hl7tot",m_hl7_tot);

 m_hletmi = new TH1D("hletmi","hletmi", 100,0.,m_suptmi);
    sc = m_thistSvc->regHist("/AANT/" + m_folderName + "Linearity/hletm",m_hletmi);
 m_hl2etmi = new TH1D("hl2etmi","hl2etmi", 100,0.,m_suptmi);
    sc = m_thistSvc->regHist("/AANT/" + m_folderName + "Linearity/hl2etm",m_hl2etmi);
 m_hl3etmi = new TH1D("hl3etmi","hl3etmi", 100,0.,m_suptmi);
    sc = m_thistSvc->regHist("/AANT/" + m_folderName + "Linearity/hl3etm",m_hl3etmi);
 m_hl4etmi = new TH1D("hl4etmi","hl4etmi", 100,0.,m_suptmi);
    sc = m_thistSvc->regHist("/AANT/" + m_folderName + "Linearity/hl4etm",m_hl4etmi);
 m_hl5etmi = new TH1D("hl5etmi","hl5etmi", 100,0.,m_suptmi);
    sc = m_thistSvc->regHist("/AANT/" + m_folderName + "Linearity/hl5etm",m_hl5etmi);
 m_hl6etmi = new TH1D("hl6etmi","hl6etmi", 100,0.,m_suptmi);
    sc = m_thistSvc->regHist("/AANT/" + m_folderName + "Linearity/hl6etm",m_hl6etmi);
 m_hl7etmi = new TH1D("hl7etmi","hl7etmi", 100,0.,m_suptmi);
    sc = m_thistSvc->regHist("/AANT/" + m_folderName + "Linearity/hl7etm",m_hl7etmi);

  m_h_RefEle_METup = new TH1D("h_RefEle_METup","RefEle_METup",100,0.,m_suptmi);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "Linearity/RefEle_METup",m_h_RefEle_METup);
  m_h_RefJet_METup = new TH1D("h_RefJet_METup","RefJet_METup",100,0.,m_suptmi);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "Linearity/RefJet_METup",m_h_RefJet_METup);
  m_h_CellOut_METup = new TH1D("h_CellOut_METup","CellOut_METup",100,0.,m_suptmi);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "Linearity/CellOut_METup",m_h_CellOut_METup);
  m_h_RefEle_METlow = new TH1D("h_RefEle_METlow","RefEle_METlow",100,0.,m_suptmi);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "Linearity/RefEle_METlow",m_h_RefEle_METlow);
  m_h_RefJet_METlow = new TH1D("h_RefJet_METlow","RefJet_METlow",100,0.,m_suptmi);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "Linearity/RefJet_METlow",m_h_RefJet_METlow);
  m_h_CellOut_METlow = new TH1D("h_CellOut_METlow","CellOut_METlow",100,0.,m_suptmi);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "Linearity/CellOut_METlow",m_h_CellOut_METlow);
  m_h_refjetpoutup = new TH1D("h_refjetpoutup","h_refjetpoutup",100,0.,m_suptmi);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "Linearity/h_refjetpoutup",m_h_refjetpoutup);
  m_h_refjetpoutlow = new TH1D("h_refjetpoutlow","h_refjetpoutlow",100,0.,m_suptmi);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "Linearity/h_refjetpoutlow",m_h_refjetpoutlow);

  m_hcount_threshold= new TH1D("hcount_threshold","hcount_threshold", 10,-0.5,9.5);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "Linearity/hcount_threshold",m_hcount_threshold);

  for (int ih= 0; ih< m_lnumptt ; ih++){
    name3 = Form("hl_%d",ih);
    m_hl.push_back(new TH1F(name3.c_str(), name3.c_str(), m_lnbini,-m_bin_rp3,m_bin_rp3));
    sc = m_thistSvc->regHist("/AANT/" + m_folderName + "Linearity/Binned/" + name3,m_hl.at(ih));
    name32 = Form("hl2_%d",ih);
    m_hl2.push_back(new TH1F(name32.c_str(), name32.c_str(), m_lnbini,-m_bin_rp3,m_bin_rp3));
    sc = m_thistSvc->regHist("/AANT/" + m_folderName + "Linearity/Binned/" + name32,m_hl2.at(ih));
    name33 = Form("hl3_%d",ih);
    m_hl3.push_back(new TH1F(name33.c_str(), name33.c_str(), m_lnbini,-m_bin_rp3,m_bin_rp3));
    sc = m_thistSvc->regHist("/AANT/" + m_folderName + "Linearity/Binned/" + name33,m_hl3.at(ih));
    name34 = Form("hl4_%d",ih);
    m_hl4.push_back(new TH1F(name34.c_str(), name34.c_str(), m_lnbini,-m_bin_rp3,m_bin_rp3));
    sc = m_thistSvc->regHist("/AANT/" + m_folderName + "Linearity/Binned/" + name34,m_hl4.at(ih));
    name35 = Form("hl5_%d",ih);
    m_hl5.push_back(new TH1F(name35.c_str(), name35.c_str(), m_lnbini,-m_bin_rp3,m_bin_rp3));
    sc = m_thistSvc->regHist("/AANT/" + m_folderName + "Linearity/Binned/" + name35,m_hl5.at(ih));
    name36 = Form("hl6_%d",ih);
    m_hl6.push_back(new TH1F(name36.c_str(), name36.c_str(), m_lnbini,-m_bin_rp3,m_bin_rp3));
    sc = m_thistSvc->regHist("/AANT/" + m_folderName + "Linearity/Binned/" + name36,m_hl6.at(ih));
    name37 = Form("hl7%d",ih);
    m_hl7.push_back(new TH1F(name37.c_str(), name37.c_str(), m_lnbini,-m_bin_rp3,m_bin_rp3));
    sc = m_thistSvc->regHist("/AANT/" + m_folderName + "Linearity/Binned/" + name37,m_hl7.at(ih));
  }

  if (sc.isFailure()) { msg() << MSG::WARNING << "LinearityTool CBNT_initialize() fail" << endmsg; }

  return StatusCode::SUCCESS;
}

StatusCode LinearityTool::initialize() {
  return StatusCode::SUCCESS;
}

StatusCode LinearityTool::execute(MissingETData *data) {

  /** check that the input and the output containers are defined */
  StatusCode sc = StatusCode::SUCCESS;

  msg() << MSG::DEBUG << "execute() has been called" << endmsg;

  sc = linearity(data);

  if (sc.isFailure()) { msg() << MSG::WARNING << "LinearityTool execute() fail" << endmsg; }

  return sc;
}

StatusCode LinearityTool::CBNT_finalize() {
  if(m_hl.size() == 0) return StatusCode::SUCCESS;
  StatusCode sc = StatusCode::SUCCESS;
  msg() << MSG::DEBUG << "Res Tool CBNT_finalize() has been called" << endmsg;

  int cp=int(sqrt((float)m_lnumptt)+1);
  m_lstartpt=0.;

  m_curvasETmi=m_lstartpt+m_lnumptt*m_lbinpt;
  //double con[100];
  double mean[100];
  double sigma[100];
  //double con_err[100];
  double mean_err[100];
  double sigma_err[100];
  double ex[100];
  double nxbin[100];
  int nbinplot=0;

  float xbin;
  // book histo curva resol vs SumET 
  m_he_linvset = new TH2F("linvset","linvset",100,0.,1000.,100,0.,20.);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "Linearity/he_linvset",m_he_linvset);

  TCanvas *c1 = new TCanvas("c1", "c1",293,139,700,500);
  c1->Update();
  c1->Divide(cp,cp);

  int ih=0;
  for (int i= 0; i< m_lnumptt ; i++){
    c1->cd(i+1);
    TH1F* hist  = m_hl.at(i);

    int entries = (int)(hist->GetEntries());
    sigma[ih]=-10005.;
    sigma_err[ih]=1000.;
    if  ( entries> m_entries) {

      xbin = m_lstartpt+m_lbinpt/2.+ i * m_lbinpt;

      if (m_fitrangel == 1) {
	hist->Fit( "gaus","","",-m_fitr,m_fitr );
      }
      //fit on the whole plot range............................
      if (m_fitrangel == 2) {
	hist->Fit( "gaus","","");
      }

      TF1 *fitsf = hist->GetFunction("gaus");

      nbinplot++;
      if(!fitsf) continue;
      //con[ih] = fitsf->GetParameter(0);
      mean[ih] = fitsf->GetParameter(1);
      sigma[ih] = fitsf->GetParameter(2);
      //con_err[ih] = fitsf->GetParError(0);
      mean_err[ih] = fitsf->GetParError(1);
      sigma_err[ih] = fitsf->GetParError(2);
     
      nxbin[ih]=xbin;
      ex[ih]=0.;
      m_he_linvset->Fill(xbin, mean[ih]);
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

  TCanvas* c2 = new TCanvas("c2"," Resolution vs Sumet from Topo (A0300)",293,139,700,500);
  c2->Update();
  c2->Divide(1,1);
  c2->cd( 1 );

  //Linearity
  m_grlin = new TGraphErrors(nbinplot++,nxbin,mean,ex,mean_err);
  sc = m_thistSvc->regGraph("/AANT/" + m_folderName + "Linearity/grlin",m_grlin);

  m_grlin->SetTitle();

  m_grlin->SetMarkerColor(1);
  m_grlin->SetMarkerStyle(21);
  m_grlin->Draw("AP"); 
  m_grlin->SetMaximum(m_curvaslin);
  m_grlin->SetMinimum(-m_curvaslin);
  m_grlin->GetYaxis()->SetTitle("Linearity of response");
  m_grlin->GetXaxis()->SetTitle("True E_{T}^{miss}  (GeV)");

  Double_t tsize=0.06;
  TMarker *marker = new TMarker(0.40-(0.4*tsize),0.25,8);
  marker->SetMarkerColor(1);  marker->SetNDC();
  marker->SetMarkerStyle(21);
  marker->SetMarkerSize(2.0);
  marker->Draw();

  TLatex l; l.SetTextAlign(12); //l.SetTextSize(tsize); 
  l.SetNDC();
  l.DrawLatex(0.40,0.25,"E_{T}^{miss} refined calibration");

  TLatex l2;
  l2.SetNDC();
  l2.SetTextColor(1);
  l2.DrawLatex(m_linplot_x,m_linplot_y,m_linplot_text.c_str());

  TLine b22(0.,0.,105.,0.);
  //blue
  b22.SetLineColor(4);
  b22.SetLineStyle(1); // 1 = linea tratteggiata
  b22.Draw();

  TLine b23(0.,-4.,105.,-4.);
  //blue
  b23.SetLineColor(4);
  b23.SetLineStyle(2); // 1 = linea tratteggiata
  b23.Draw();

  TLine b24(0.,4.,105.,4.);
  //blue
  b24.SetLineColor(4);
  b24.SetLineStyle(2); // 1 = linea tratteggiata
  b24.Draw();

  c2->Update();

  if (sc.isFailure()) { msg() << MSG::WARNING << "LinearityTool CBNT_finalize() fail" << endmsg; }

  return sc;
}

StatusCode LinearityTool::linearity(MissingETData *data) {

  msg() << MSG::DEBUG << "in linearity() " << endmsg;

  StatusCode sc = StatusCode::SUCCESS;

  //linearity---------------------------------
  float etmi_per_curve;
  float EtMissRefFinal;
  float EtMissEmScale;
  float EtMissGlobCalib;
  float EtMissFinal;
  float EtMissFinalLocHad;
  float EtMissBaseEmScale;
  float EtMissBaseGlobCalib;

  int ih;

  float refele;
  float refjet;
  float cellout;
  float refjetpout;
  float refjetpoutx;
  float refjetpouty;

  etmi_per_curve=sqrt(pow(data->truth_nonInt()->etx(),2)+pow(data->truth_nonInt()->ety(),2))/GeV;

  EtMissRefFinal= sqrt(pow(data->refFinal()->etx(),2)+pow(data->refFinal()->ety(),2))/GeV;
  EtMissEmScale= sqrt( pow( data->topo()->etx()+data->MuonBoy()->etx() ,2)
+pow( data->topo()->ety()+data->MuonBoy()->ety() ,2) )/GeV;
  EtMissGlobCalib= sqrt( pow(data->corrTopo()->etx()+data->MuonBoy()->etx(),2) 
+pow(data->corrTopo()->ety()+data->MuonBoy()->ety(),2) )/GeV;
  EtMissFinal= sqrt(pow(data->final()->etx(),2)+pow(data->final()->ety(),2))/GeV;
  EtMissFinalLocHad= sqrt( pow(data->locHadTopo()->etx()+data->MuonBoy()->etx(),2) 
+pow(data->locHadTopo()->ety()+data->MuonBoy()->ety(),2) )/GeV;
  EtMissBaseEmScale= sqrt( pow( data->base()->etx()+data->MuonBoy()->etx() ,2)
+pow( data->base()->ety()+data->MuonBoy()->ety() ,2) )/GeV;
  EtMissBaseGlobCalib= sqrt( pow(data->calib()->etx()+data->MuonBoy()->etx(),2) 
+pow(data->calib()->ety()+data->MuonBoy()->ety(),2) )/GeV;


    //m_linorm=1
    //true Linearity: (EtMissTruth-EtMissRefFinal)/EtMissTruth
    //m_linorm=0
    //Bias: (EtMissTruth-EtMissRefFinal)

    const float etmi_per_curve1 = m_linorm == 0 ? 1 : etmi_per_curve;
    const float inv_etmi_per_curve1 = 1. / etmi_per_curve1;
   
    m_hl_tot->Fill( (etmi_per_curve-EtMissRefFinal) * inv_etmi_per_curve1 );
    m_hl2_tot->Fill( (etmi_per_curve-EtMissEmScale) * inv_etmi_per_curve1 );
    m_hl3_tot->Fill( (etmi_per_curve-EtMissGlobCalib) * inv_etmi_per_curve1 );
    m_hl4_tot->Fill( (etmi_per_curve-EtMissFinal) * inv_etmi_per_curve1 );
    m_hl5_tot->Fill( (etmi_per_curve-EtMissFinalLocHad) * inv_etmi_per_curve1 );
    m_hl6_tot->Fill( (etmi_per_curve-EtMissBaseEmScale) * inv_etmi_per_curve1 );
    m_hl7_tot->Fill( (etmi_per_curve-EtMissBaseGlobCalib) *inv_etmi_per_curve1 );

    m_hletmi->Fill(EtMissRefFinal);
    m_hl2etmi->Fill(EtMissEmScale);
    m_hl3etmi->Fill(EtMissGlobCalib);
    m_hl4etmi->Fill(EtMissFinal);
    m_hl5etmi->Fill(EtMissFinalLocHad);
    m_hl6etmi->Fill(EtMissBaseEmScale);
    m_hl7etmi->Fill(EtMissBaseGlobCalib);

//check plots
   refele= sqrt(pow(data->refEle()->etx(),2)+pow(data->refEle()->ety(),2))/GeV ;
   refjet= sqrt(pow(data->refJet()->etx(),2)+pow(data->refJet()->ety(),2))/GeV ;
   cellout= sqrt(pow(data->cellOut()->etx(),2)+pow(data->cellOut()->ety(),2))/GeV ;
   refjetpoutx= data->refJet()->etx()+data->cellOut()->etx();
   refjetpouty= data->refJet()->ety()+data->cellOut()->ety();
   refjetpout= sqrt(pow(refjetpoutx,2)+pow(refjetpouty,2))/GeV ;

   if(etmi_per_curve > 40.) {
   m_h_RefEle_METup->Fill(refele);
   m_h_RefJet_METup->Fill(refjet);
   m_h_CellOut_METup->Fill(cellout);
   m_h_refjetpoutup->Fill(refjetpout);
   }
   if(etmi_per_curve <= 40.) {
   m_h_RefEle_METlow->Fill(refele);
   m_h_RefJet_METlow->Fill(refjet);
   m_h_CellOut_METlow->Fill(cellout);
   m_h_refjetpoutlow->Fill(refjetpout);
   }

//for syst evaluation.......
      float count=0.;
      m_hcount_threshold->Fill(count);
    if(EtMissRefFinal>m_etmi_threshold) {
      count=1.;
      m_hcount_threshold->Fill(count);
}
   if(EtMissEmScale>m_etmi_threshold) {
      count=2.;
      m_hcount_threshold->Fill(count);
}
   if(EtMissGlobCalib>m_etmi_threshold) {
      count=3.;
      m_hcount_threshold->Fill(count);
}
   if(EtMissFinal>m_etmi_threshold) {
      count=4.;
      m_hcount_threshold->Fill(count);
}
   if(EtMissFinalLocHad>m_etmi_threshold) {
      count=5.;
      m_hcount_threshold->Fill(count);
}
   if(EtMissBaseEmScale>m_etmi_threshold) {
      count=6.;
      m_hcount_threshold->Fill(count);
}
   if(EtMissBaseGlobCalib>m_etmi_threshold) {
      count=7.;
      m_hcount_threshold->Fill(count);
}

  ih =(int)((etmi_per_curve-m_lstartpt)/m_lbinpt);
  if (etmi_per_curve < m_lstartpt) {ih=-1;}
  if ( ih >=0 && ih <  m_lnumptt) {

    m_hl.at(ih)->Fill( (etmi_per_curve-EtMissRefFinal) * inv_etmi_per_curve1 );

   //Linearity for various steps of EtMiss calculation (as plot in CSC Note)
    m_hl2.at(ih)->Fill( (etmi_per_curve-EtMissEmScale) * inv_etmi_per_curve1 );

    m_hl3.at(ih)->Fill( (etmi_per_curve-EtMissGlobCalib) * inv_etmi_per_curve1 );

    m_hl4.at(ih)->Fill( (etmi_per_curve-EtMissFinal) * inv_etmi_per_curve1 );

   //Linearity for EtMiss with Localhadron calibration
    m_hl5.at(ih)->Fill( (etmi_per_curve-EtMissFinalLocHad) * inv_etmi_per_curve1 );

   //Linearity for EtMiss very basic
    m_hl6.at(ih)->Fill( (etmi_per_curve-EtMissBaseEmScale) * inv_etmi_per_curve1 );

    m_hl7.at(ih)->Fill( (etmi_per_curve-EtMissBaseGlobCalib) * inv_etmi_per_curve1 );
  }

  if (sc.isFailure()) { msg() << MSG::WARNING << "LinearityTool linearity() fail" << endmsg; }

  return sc;
}

//------------------------------------------------------------------------------
StatusCode LinearityTool::finalize() {
  return StatusCode::SUCCESS;
}
