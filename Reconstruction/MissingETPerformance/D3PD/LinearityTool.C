/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LinearityTool.h"
#include "TCanvas.h"
#include "TF1.h"
#include "TLine.h"
#include "TLatex.h"
#include "TMarker.h"
#include "TGraphErrors.h"

LinearityTool::LinearityTool() {
  m_lnumptt = 40;
  m_lstartpt=0.;  
  m_lbinpt=5.;    
  m_fitrangel=2;  
  m_lnbini=100;    
  m_percerr=0.15; 
  m_entries=50;   
  m_curvaslin=0.3;
  m_fitr=2.5;     
  m_linorm=1;
  m_bin_rp3=1.;
  m_suptmi=300.;
  m_etmi_threshold=50.;
  m_folderName="";
}

void LinearityTool::initialize(TFile *outfile) {

  //if m_linorm=1 Linearity 
  //if m_linorm=0 Bias (set: m_curvaslin=10., m_bin_rp3=50.)

  //book histos for linearity vs ETmiss
  std::string name3;
  std::string name32;
  std::string name33;
  std::string name34;
  std::string name35;
  std::string name36;
  std::string name37;

  TDirectory *dir=0;
  if (m_folderName!="") {
    if ( (dir = outfile->GetDirectory(m_folderName.c_str())) == 0) {
      dir = outfile->mkdir(m_folderName.c_str());
    }
    outfile->cd(m_folderName.c_str());
    dir = dir->mkdir("Linearity");
    outfile->cd((m_folderName+"/Linearity").c_str());
  } else {
    dir = outfile->mkdir("Linearity");
    outfile->cd("Linearity");
  }

  hl_tot = new TH1D("hl_tot","hl_tot", 100,-m_bin_rp3,m_bin_rp3);
  hl2_tot = new TH1D("hl2_tot","hl2_tot", 100,-m_bin_rp3,m_bin_rp3);
  hl3_tot = new TH1D("hl3_tot","hl3_tot", 100,-m_bin_rp3,m_bin_rp3);
  hl4_tot = new TH1D("hl4_tot","hl4_tot", 100,-m_bin_rp3,m_bin_rp3);
  hl5_tot = new TH1D("hl5_tot","hl5_tot", 100,-m_bin_rp3,m_bin_rp3);
  hl6_tot = new TH1D("hl6_tot","hl6_tot", 100,-m_bin_rp3,m_bin_rp3);
  hl7_tot = new TH1D("hl7_tot","hl7_tot", 100,-m_bin_rp3,m_bin_rp3);

  hletmi = new TH1D("hletmi","hletmi", 100,0.,m_suptmi);
  hl2etmi = new TH1D("hl2etmi","hl2etmi", 100,0.,m_suptmi);
  hl3etmi = new TH1D("hl3etmi","hl3etmi", 100,0.,m_suptmi);
  hl4etmi = new TH1D("hl4etmi","hl4etmi", 100,0.,m_suptmi);
  hl5etmi = new TH1D("hl5etmi","hl5etmi", 100,0.,m_suptmi);
  hl6etmi = new TH1D("hl6etmi","hl6etmi", 100,0.,m_suptmi);
  hl7etmi = new TH1D("hl7etmi","hl7etmi", 100,0.,m_suptmi);

  h_RefEle_METup = new TH1D("h_RefEle_METup","RefEle_METup",100,0.,m_suptmi);
  h_RefJet_METup = new TH1D("h_RefJet_METup","RefJet_METup",100,0.,m_suptmi);
  h_CellOut_METup = new TH1D("h_CellOut_METup","CellOut_METup",100,0.,m_suptmi);
  h_RefEle_METlow = new TH1D("h_RefEle_METlow","RefEle_METlow",100,0.,m_suptmi);
  h_RefJet_METlow = new TH1D("h_RefJet_METlow","RefJet_METlow",100,0.,m_suptmi);
  h_CellOut_METlow = new TH1D("h_CellOut_METlow","CellOut_METlow",100,0.,m_suptmi);
  h_refjetpoutup = new TH1D("h_refjetpoutup","h_refjetpoutup",100,0.,m_suptmi);
  h_refjetpoutlow = new TH1D("h_refjetpoutlow","h_refjetpoutlow",100,0.,m_suptmi);

  hcount_threshold= new TH1D("hcount_threshold","hcount_threshold", 10,-0.5,9.5);

  dir = dir->mkdir("Binned");
  if (m_folderName!="") {
    outfile->cd((m_folderName+"/Linearity/Binned").c_str());
  } else {
    outfile->cd("Linearity/Binned");
  }

  for (int ih= 0; ih< m_lnumptt ; ih++){
    name3 = Form("hl_%d",ih);
    hl_.push_back(new TH1F(name3.c_str(), name3.c_str(), m_lnbini,-m_bin_rp3,m_bin_rp3));
    name32 = Form("hl2_%d",ih);
    hl2_.push_back(new TH1F(name32.c_str(), name32.c_str(), m_lnbini,-m_bin_rp3,m_bin_rp3));
    name33 = Form("hl3_%d",ih);
    hl3_.push_back(new TH1F(name33.c_str(), name33.c_str(), m_lnbini,-m_bin_rp3,m_bin_rp3));
    name34 = Form("hl4_%d",ih);
    hl4_.push_back(new TH1F(name34.c_str(), name34.c_str(), m_lnbini,-m_bin_rp3,m_bin_rp3));
    name35 = Form("hl5_%d",ih);
    hl5_.push_back(new TH1F(name35.c_str(), name35.c_str(), m_lnbini,-m_bin_rp3,m_bin_rp3));
    name36 = Form("hl6_%d",ih);
    hl6_.push_back(new TH1F(name36.c_str(), name36.c_str(), m_lnbini,-m_bin_rp3,m_bin_rp3));
    name37 = Form("hl7_%d",ih);
    hl7_.push_back(new TH1F(name37.c_str(), name37.c_str(), m_lnbini,-m_bin_rp3,m_bin_rp3));
  }
}

void LinearityTool::execute(MissingETData *data) {
  linearity(data);
}

void LinearityTool::linearity(MissingETData *data) {

  //linearity---------------------------------
  float etmi_per_curve;
  float EtMissRefFinal;
  float EtMissEmScale;
  float EtMissGlobCalib;
  float EtMissFinal;
  float EtMissFinalLocHad;
  float EtMissBaseEmScale;
  float EtMissBaseGlobCalib;

  float etmi_per_curve1;

  int ih;

  float refele;
  float refjet;
  float cellout;
  float refjetpout;
  float refjetpoutx;
  float refjetpouty;

  etmi_per_curve=sqrt(pow(data->truth_nonInt()->etx(),2)+pow(data->truth_nonInt()->ety(),2))/1000.;

  EtMissRefFinal= sqrt(pow(data->refFinal()->etx(),2)+pow(data->refFinal()->ety(),2))/1000.;
  EtMissEmScale= sqrt( pow( data->topo()->etx()+data->MuonBoy()->etx() ,2)
+pow( data->topo()->ety()+data->MuonBoy()->ety() ,2) )/1000.;
  EtMissGlobCalib= sqrt( pow(data->corrTopo()->etx()+data->MuonBoy()->etx(),2) 
+pow(data->corrTopo()->ety()+data->MuonBoy()->ety(),2) )/1000.;
  EtMissFinal= sqrt(pow(data->final()->etx(),2)+pow(data->final()->ety(),2))/1000.;
  EtMissFinalLocHad= sqrt( pow(data->locHadTopo()->etx()+data->MuonBoy()->etx(),2) 
+pow(data->locHadTopo()->ety()+data->MuonBoy()->ety(),2) )/1000.;
  EtMissBaseEmScale= sqrt( pow( data->base()->etx()+data->MuonBoy()->etx() ,2)
+pow( data->base()->ety()+data->MuonBoy()->ety() ,2) )/1000.;
  EtMissBaseGlobCalib= sqrt( pow(data->calib()->etx()+data->MuonBoy()->etx(),2) 
+pow(data->calib()->ety()+data->MuonBoy()->ety(),2) )/1000.;


    //m_linorm=1
    //true Linearity: (EtMissTruth-EtMissRefFinal)/EtMissTruth
    //m_linorm=0
    //Bias: (EtMissTruth-EtMissRefFinal)

    etmi_per_curve1=etmi_per_curve;
    if(m_linorm == 0) {etmi_per_curve1=1.;}
   
    hl_tot->Fill( (etmi_per_curve-EtMissRefFinal)/etmi_per_curve1 );
    hl2_tot->Fill( (etmi_per_curve-EtMissEmScale)/etmi_per_curve1 );
    hl3_tot->Fill( (etmi_per_curve-EtMissGlobCalib)/etmi_per_curve1 );
    hl4_tot->Fill( (etmi_per_curve-EtMissFinal)/etmi_per_curve1 );
    hl5_tot->Fill( (etmi_per_curve-EtMissFinalLocHad)/etmi_per_curve1 );
    hl6_tot->Fill( (etmi_per_curve-EtMissBaseEmScale)/etmi_per_curve1 );
    hl7_tot->Fill( (etmi_per_curve-EtMissBaseGlobCalib)/etmi_per_curve1 );

    hletmi->Fill(EtMissRefFinal);
    hl2etmi->Fill(EtMissEmScale);
    hl3etmi->Fill(EtMissGlobCalib);
    hl4etmi->Fill(EtMissFinal);
    hl5etmi->Fill(EtMissFinalLocHad);
    hl6etmi->Fill(EtMissBaseEmScale);
    hl7etmi->Fill(EtMissBaseGlobCalib);

//check plots
   refele= sqrt(pow(data->refEle()->etx(),2)+pow(data->refEle()->ety(),2))/1000. ;
   refjet= sqrt(pow(data->refJet()->etx(),2)+pow(data->refJet()->ety(),2))/1000. ;
   cellout= sqrt(pow(data->cellOut()->etx(),2)+pow(data->cellOut()->ety(),2))/1000. ;
   refjetpoutx= data->refJet()->etx()+data->cellOut()->etx();
   refjetpouty= data->refJet()->ety()+data->cellOut()->ety();
   refjetpout= sqrt(pow(refjetpoutx,2)+pow(refjetpouty,2))/1000. ;

   if(etmi_per_curve > 40.) {
   h_RefEle_METup->Fill(refele);
   h_RefJet_METup->Fill(refjet);
   h_CellOut_METup->Fill(cellout);
   h_refjetpoutup->Fill(refjetpout);
   }
   if(etmi_per_curve <= 40.) {
   h_RefEle_METlow->Fill(refele);
   h_RefJet_METlow->Fill(refjet);
   h_CellOut_METlow->Fill(cellout);
   h_refjetpoutlow->Fill(refjetpout);
   }

//for syst evaluation.......
      float count=0.;
      hcount_threshold->Fill(count);
    if(EtMissRefFinal>m_etmi_threshold) {
      count=1.;
      hcount_threshold->Fill(count);
}
   if(EtMissEmScale>m_etmi_threshold) {
      count=2.;
      hcount_threshold->Fill(count);
}
   if(EtMissGlobCalib>m_etmi_threshold) {
      count=3.;
      hcount_threshold->Fill(count);
}
   if(EtMissFinal>m_etmi_threshold) {
      count=4.;
      hcount_threshold->Fill(count);
}
   if(EtMissFinalLocHad>m_etmi_threshold) {
      count=5.;
      hcount_threshold->Fill(count);
}
   if(EtMissBaseEmScale>m_etmi_threshold) {
      count=6.;
      hcount_threshold->Fill(count);
}
   if(EtMissBaseGlobCalib>m_etmi_threshold) {
      count=7.;
      hcount_threshold->Fill(count);
}

  ih =(int)((etmi_per_curve-m_lstartpt)/m_lbinpt);
  if (etmi_per_curve < m_lstartpt) {ih=-1;}
  if ( ih >=0 && ih <  m_lnumptt) {

    hl_.at(ih)->Fill( (etmi_per_curve-EtMissRefFinal)/etmi_per_curve1 );

   //Linearity for various steps of EtMiss calculation (as plot in CSC Note)
    hl2_.at(ih)->Fill( (etmi_per_curve-EtMissEmScale)/etmi_per_curve1 );

    hl3_.at(ih)->Fill( (etmi_per_curve-EtMissGlobCalib)/etmi_per_curve1 );

    hl4_.at(ih)->Fill( (etmi_per_curve-EtMissFinal)/etmi_per_curve1 );

   //Linearity for EtMiss with Localhadron calibration
    hl5_.at(ih)->Fill( (etmi_per_curve-EtMissFinalLocHad)/etmi_per_curve1 );

   //Linearity for EtMiss very basic
    hl6_.at(ih)->Fill( (etmi_per_curve-EtMissBaseEmScale)/etmi_per_curve1 );

    hl7_.at(ih)->Fill( (etmi_per_curve-EtMissBaseGlobCalib)/etmi_per_curve1 );
  }
}
