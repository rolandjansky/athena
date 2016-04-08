/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/GaudiException.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/AlgFactory.h"

#include "xAODEventInfo/EventInfo.h"

//#include "CLHEP/Units/PhysicalConstants.h"

#include "MissingETPerformance/BasicPlotsTool.h"

//------------------------------------------------------------------------------
BasicPlotsTool::BasicPlotsTool( const std::string& type,
                  const std::string& name, 
                  const IInterface* parent ) : AthAlgTool( type, name, parent ),
                                               _jetVariables ("JetVariablesTool"),
                                               _clusterVariables ("TopoClusterVariablesTool") {

  declareInterface<BasicPlotsTool>( this );
  declareProperty("plotJetVariables",     _plotJetVariables=false);
  declareProperty("plotClusterVariables", _plotClusterVariables=false);

  declareProperty("iset",                 m_iset=6020);
  declareProperty("nbinp",                m_nbinp=250);
  declareProperty("suptmi",               m_suptmi=500.);
  declareProperty("nbinE",                m_nbinE=200);
  declareProperty("lowET",                m_lowET=-5.);
  declareProperty("nbinpxy",              m_nbinpxy=500);
  declareProperty("suptmixy",             m_suptmixy=250.);
  declareProperty("nbinphi",              m_nbinphi=63);
  declareProperty("binphi",               m_binphi=3.15);
  declareProperty("nbinsig",              m_nbinsig=100);
  declareProperty("binsig",               m_binsig=10.);
  declareProperty("nbineta",              m_nbineta=50);
  declareProperty("bineta",               m_bineta=5.0);
  declareProperty("regions_ncells",       m_regions_ncells=60000);
  declareProperty("regions_nbins_ncells", m_regions_nbins_ncells=6000);
  declareProperty("suET",                 m_suET=200.+m_lowET);
  declareProperty("nbinJetEt",            m_nbinJetEt=500);
  declareProperty("lowerbinJetEt",        m_lowerbinJetEt=0);
  declareProperty("upperbinJetEt",        m_upperbinJetEt=500.);
  declareProperty("nbinClusterEt",        m_nbinClusterEt=500);
  declareProperty("lowerbinClusterEt",    m_lowerbinClusterEt=0);
  declareProperty("upperbinClusterEt",    m_upperbinClusterEt=500.);
  declareProperty("nbinBCID",             m_nbinBCID=10000);
  declareProperty("lowBCID",              m_lowBCID=0.);
  declareProperty("upBCID",               m_upBCID=500.);
  declareProperty("nbinMU",               m_nbinMU=100);
  declareProperty("lowMU",                m_lowMU=0.);
  declareProperty("upMU",                 m_upMU=50.);

  declareProperty("EventInfoKey",         m_evtInfoKey="EventInfo");
  declareProperty("FolderName",           m_folderName="");
}


void BasicPlotsTool::book1d (const std::string& title,
                             int nbins,
                             float xlo,
                             float xhi,
                             TH1D*& var,
                             const char* subfolder)
{
  std::string name = std::string("h_") + title;
  var = new TH1D (name.c_str(), title.c_str(), nbins, xlo, xhi);
  StatusCode sc = m_thistSvc->regHist
    ("/AANT/" + m_folderName + "BasicPlots/" + 
     subfolder + (subfolder[0] ? "/" : "") + title, var);
  if (sc.isFailure()) {
    msg() << MSG::WARNING << "BasicPlotsTool CBNT_initialize() fail "
          << title << endreq;
  }
}


void BasicPlotsTool::book2d (const std::string& title,
                             int nbinsx,
                             float xlo,
                             float xhi,
                             int nbinsy,
                             float ylo,
                             float yhi,
                             TH2D*& var,
                             const char* subfolder)
{
  std::string name = std::string("h_") + title;
  var = new TH2D (name.c_str(), title.c_str(), nbinsx, xlo, xhi, nbinsy, ylo, yhi);
  StatusCode sc = m_thistSvc->regHist
    ("/AANT/" + m_folderName + "BasicPlots/" + 
     subfolder + (subfolder[0] ? "/" : "") + title, var);
  if (sc.isFailure()) {
    msg() << MSG::WARNING << "BasicPlotsTool CBNT_initialize() fail "
          << title << endreq;
  }
}


void BasicPlotsTool::book_missingETHists (const char* name, BasicPlotsTool::MissingETHists& v)
{
  std::string sname = name;
  sname += "/";
  sname += name;

  book1d (sname + "SET",m_nbinE,m_lowET,m_suET,  v.SET);
  book1d (sname + "MET",m_nbinp,0.,m_suptmi, v.MET);
  book1d (sname + "METphi",m_nbinphi,-m_binphi,m_binphi , v.METphi);
  book2d (sname + "MET_v_METphi",m_nbinp,0.,m_suptmi, m_nbinphi,-m_binphi,m_binphi, v.MET_v_METphi);
  book1d (sname + "METx",m_nbinpxy,-m_suptmixy,m_suptmixy ,v.METx);
  book1d (sname + "METy",m_nbinpxy,-m_suptmixy,m_suptmixy ,v.METy);
}


void
BasicPlotsTool::book_missingETCaloHists (const char* name,
                                         BasicPlotsTool::MissingETCaloHists& v)
{
  book_missingETHists (name, v.base);

  const char* cnames[MissingEtCalo::Size] = {
    "PEMB", "EMB", "PEMEC", "EME",
    "TILE", "HEC", "FCAL",
  };
  std::string sname = name;

  for (size_t i = 0; i < MissingEtCalo::Size; i++) {
    book1d (sname + "SET_" + cnames[i], m_nbinE,m_lowET,m_suET,  v.SET[i]);
    book1d (sname + "METx_" + cnames[i], m_nbinpxy,-m_suptmixy,m_suptmixy ,v.METx[i]);
    book1d (sname + "METy_" + cnames[i], m_nbinpxy,-m_suptmixy,m_suptmixy ,v.METy[i]);
    book1d (sname + "NCELLS_" + cnames[i], m_regions_nbins_ncells,0,m_regions_ncells,v.NCELLS[i]);
  }
}


void BasicPlotsTool::fill_missingETHists (MissingETHists& h, const MissingET* m)
{
  h.SET->Fill(m->sumet()/1000.);
  h.MET->Fill( sqrt(pow(m->etx(),2)+pow(m->ety(),2))/1000. );
  h.METphi->Fill( m->phi() );
  h.METx->Fill( m->etx()/1000. );
  h.METy->Fill( m->ety()/1000. );
  h.MET_v_METphi->Fill( sqrt(pow(m->etx(),2)+pow(m->ety(),2))/1000. , m->phi() );
}


void BasicPlotsTool::fill_missingETCaloHists (MissingETCaloHists& h,
                                              const MissingEtCalo* m)
{
  fill_missingETHists (h.base, m);
  for (size_t i = 0; i < MissingEtCalo::Size; i++) {
    MissingEtCalo::CaloIndex ci = static_cast<MissingEtCalo::CaloIndex>(i);
    h.SET [i]->Fill (m->etSumCalo(ci)/1000.);
    h.METx[i]->Fill (m->exCalo(ci)/1000.);
    h.METy[i]->Fill (m->eyCalo(ci)/1000.);
    h.NCELLS[i]->Fill (m->ncellCalo(ci));
  }
}


//------------------------------------------------------------------------------

StatusCode BasicPlotsTool::CBNT_initialize() {
  StatusCode sc = service("THistSvc", m_thistSvc);

  if (!mLog) mLog = new MsgStream(msgSvc(), name() );

  msg() << MSG::DEBUG << "BasicPlotsTool CBNT_initialize() has been called" << endreq;

  h_MET_Topo_METx_V_lbn = new TH2F("h_MET_Topo_METx_V_lbn","MET_Topo (METx ) V lbn",2000,0,2000,m_nbinpxy,-m_suptmixy,m_suptmixy );
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "BasicPlots/MET_Topo_METx_V_lbn", h_MET_Topo_METx_V_lbn);

  h_MET_Topo_METy_V_lbn = new TH2F("h_MET_Topo_METy_V_lbn","MET_Topo (METy) V lbn",2000,0,2000,m_nbinpxy,-m_suptmixy,m_suptmixy );
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "BasicPlots/MET_Topo_METy_V_lbn", h_MET_Topo_METy_V_lbn);

  h_MET_RefFinal_METx_V_lbn = new TH2F("h_MET_RefFinal_METx_V_lbn","MET_RefFinal (METx) V lbn",2000,0,2000,m_nbinpxy,-m_suptmixy,m_suptmixy );
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "BasicPlots/MET_RefFinal_METx_V_lbn",h_MET_RefFinal_METx_V_lbn);

  h_MET_RefFinal_METy_V_lbn = new TH2F("h_MET_RefFinal_METy_V_lbn","MET_RefFinal (METy) V lbn",2000,0,2000,m_nbinpxy,-m_suptmixy,m_suptmixy );
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "BasicPlots/MET_RefFinal_METy_V_lbn",h_MET_RefFinal_METy_V_lbn);

  if (_plotClusterVariables) {
    /// get a handle on the TopoClusterVariablesTool
    sc = _clusterVariables.retrieve();
    if ( sc.isFailure() ) {
      msg() << MSG::ERROR << "Can't get handle on TopoClusterVariablesTool in BasicPlotsTool" << endreq;
      return sc;
    }

    h_cluster_n = new TH1F("h_cluster_n","h_cluster_n",500,0,500);
    sc = m_thistSvc->regHist("/AANT/" + m_folderName + "BasicPlots/ClusterVariables/cluster_n", h_cluster_n);

    h_cluster_et = new TH1F("h_cluster_et","h_cluster_et",m_nbinClusterEt, m_lowerbinClusterEt, m_upperbinClusterEt);
    sc = m_thistSvc->regHist("/AANT/" + m_folderName + "BasicPlots/ClusterVariables/cluster_et", h_cluster_et);

    h_cluster_eta = new TH1F("h_cluster_eta","h_cluster_eta",2*m_nbineta,-1*m_bineta,m_bineta);
    sc = m_thistSvc->regHist("/AANT/" + m_folderName + "BasicPlots/ClusterVariables/cluster_eta", h_cluster_eta);

    h_cluster_phi = new TH1F("h_cluster_phi","h_cluster_phi",m_nbinphi,-1*m_binphi,m_binphi);
    sc = m_thistSvc->regHist("/AANT/" + m_folderName + "BasicPlots/ClusterVariables/cluster_phi", h_cluster_phi);

    h_cluster_n_V_MET_Topo_MET = new TH2F("h_cluster_n_V_MET_Topo_MET","N clusters V MET_Topo (MET)",500,0,500,m_nbinp,0.,m_suptmi);
    sc = m_thistSvc->regHist("/AANT/" + m_folderName + "BasicPlots/ClusterVariables/cluster_n_V_MET_Topo_MET", h_cluster_n_V_MET_Topo_MET);

    h_cluster_n_V_MET_RefFinal_MET = new TH2F("h_cluster_n_V_MET_RefFinal_MET","N clusters V MET_RefFinal (MET)",500,0,500,m_nbinp,0.,m_suptmi);
    sc = m_thistSvc->regHist("/AANT/" + m_folderName + "BasicPlots/ClusterVariables/cluster_n_V_MET_RefFinal_MET", h_cluster_n_V_MET_RefFinal_MET);

    h_cluster_n_V_MET_Topo_SET = new TH2F("h_cluster_n_V_MET_Topo_SET","N clusters V MET_Topo (SET)",500,0,500,m_nbinE,m_lowET,m_suET);
    sc = m_thistSvc->regHist("/AANT/" + m_folderName + "BasicPlots/ClusterVariables/cluster_n_V_MET_Topo_SET", h_cluster_n_V_MET_Topo_SET);

    h_cluster_n_V_MET_RefFinal_SET = new TH2F("h_cluster_n_V_MET_RefFinal_SET","N clusters V MET_RefFinal (SET)",500,0,500,m_nbinE,m_lowET,m_suET);
    sc = m_thistSvc->regHist("/AANT/" + m_folderName + "BasicPlots/ClusterVariables/cluster_n_V_MET_RefFinal_SET", h_cluster_n_V_MET_RefFinal_SET);

    h_cluster_eta_V_phi = new TH2F("h_cluster_eta_V_phi","cluster eta V phi",2*m_nbineta,-1*m_bineta,m_bineta,m_nbinphi,-1*m_binphi,m_binphi);
    sc = m_thistSvc->regHist("/AANT/" + m_folderName + "BasicPlots/ClusterVariables/cluster_eta_V_phi", h_cluster_eta_V_phi);

    h_cluster_et_V_eta = new TH2F("h_cluster_et_V_eta","cluster et V eta",2*m_nbineta,-1*m_bineta,m_bineta,m_nbinClusterEt, m_lowerbinClusterEt, m_upperbinClusterEt);
    sc = m_thistSvc->regHist("/AANT/" + m_folderName + "BasicPlots/ClusterVariables/cluster_et_V_eta", h_cluster_et_V_eta);

    h_cluster_et_V_phi = new TH2F("h_cluster_et_V_phi","cluster et V phi",m_nbinphi,-1*m_binphi,m_binphi,m_nbinClusterEt, m_lowerbinClusterEt, m_upperbinClusterEt);
    sc = m_thistSvc->regHist("/AANT/" + m_folderName + "BasicPlots/ClusterVariables/cluster_et_V_phi", h_cluster_et_V_phi);

  }//_plotClusterVariables

  //do we run this tool?
  if (_plotJetVariables) {
    /// get a handle on the JetVariablesTool
    sc = _jetVariables.retrieve();
    if ( sc.isFailure() ) {
      msg() << MSG::ERROR << "Can't get handle on JetVariablesTool in BasicPlotsTool" << endreq;
      return sc;
    }

    book1d ("jetPtWeightedEventEMfraction",200, -0.5,1.5, h_jetPtWeightedEventEMfraction, "JetVariables");

    book1d ("jetPtWeightedNumAssociatedTracks",500, 0.,50.,h_jetPtWeightedNumAssociatedTracks, "JetVariables");

    book1d ("jetPtWeightedSize",500, 0.,50., h_jetPtWeightedSize, "JetVariables");

    h_RefFinal_simple_Significance_v_leadJetEta = new TH2D("h_RefFinal_simple_Significance_v_leadJetEta",
							   "RefFinal simple Significance v lead reco Jet |Eta|",m_nbineta,0.,m_bineta,m_nbinsig,0.,m_binsig);
    sc = m_thistSvc->regHist("/AANT/" + m_folderName + "BasicPlots/JetVariables/RefFinal_simple_Significance_v_leadJetEta", h_RefFinal_simple_Significance_v_leadJetEta);

    h_RefFinal_simple_Significance_v_secondJetEta = new TH2D("h_RefFinal_simple_Significance_v_secondJetEta",
							   "RefFinal simple Significance v second reco Jet |Eta|",m_nbineta,0.,m_bineta,m_nbinsig,0.,m_binsig);
    sc = m_thistSvc->regHist("/AANT/" + m_folderName + "BasicPlots/JetVariables/RefFinal_simple_Significance_v_secondJetEta", h_RefFinal_simple_Significance_v_secondJetEta);

    h_jet_n = new TH1F("h_jet_n","h_jet_n",50,0,50);
    sc = m_thistSvc->regHist("/AANT/" + m_folderName + "BasicPlots/JetVariables/jet_n", h_jet_n);

    h_jet_et = new TH1F("h_jet_et","h_jet_et",m_nbinJetEt, m_lowerbinJetEt, m_upperbinJetEt);
    sc = m_thistSvc->regHist("/AANT/" + m_folderName + "BasicPlots/JetVariables/jet_et", h_jet_et);

    h_jet_eta = new TH1F("h_jet_eta","h_jet_eta",2*m_nbineta,-1*m_bineta,m_bineta);
    sc = m_thistSvc->regHist("/AANT/" + m_folderName + "BasicPlots/JetVariables/jet_eta", h_jet_eta);

    h_jet_phi = new TH1F("h_jet_phi","h_jet_phi",m_nbinphi,-1*m_binphi,m_binphi);
    sc = m_thistSvc->regHist("/AANT/" + m_folderName + "BasicPlots/JetVariables/jet_phi", h_jet_phi);

    h_jet_n_V_MET_Topo_MET = new TH2F("h_jet_n_V_MET_Topo_MET","N jets V MET_Topo (MET)",20,0,20,m_nbinp,0.,m_suptmi);
    sc = m_thistSvc->regHist("/AANT/" + m_folderName + "BasicPlots/JetVariables/jet_n_V_MET_Topo_MET", h_jet_n_V_MET_Topo_MET);

    h_jet_n_V_MET_Topo_SET = new TH2F("h_jet_n_V_MET_Topo_SET","N jets V MET_Topo (SET)",20,0,20,m_nbinE,m_lowET,m_suET);
    sc = m_thistSvc->regHist("/AANT/" + m_folderName + "BasicPlots/JetVariables/jet_n_V_MET_Topo_SET", h_jet_n_V_MET_Topo_SET);

    h_jet_leadjet_et_V_MET_Topo_MET = new TH2F("h_jet_leadjet_et_V_MET_Topo (MET)","Lead jet jet Et V MET_Topo (MET)",
					   m_nbinJetEt, m_lowerbinJetEt, m_upperbinJetEt,m_nbinp,0.,m_suptmi);
    sc = m_thistSvc->regHist("/AANT/" + m_folderName + "BasicPlots/JetVariables/jet_leadjet_et_V_MET_Topo_MET", h_jet_leadjet_et_V_MET_Topo_MET);

    h_jet_leadjet_eta_V_MET_Topo_MET = new TH2F("h_jet_leadjet_eta_V_MET_Topo_MET","Lead jet jet eta V MET_Topo (MET)",
					   2*m_nbineta,-1*m_bineta,m_bineta,m_nbinp,0.,m_suptmi);
    sc = m_thistSvc->regHist("/AANT/" + m_folderName + "BasicPlots/JetVariables/jet_leadjet_eta_V_MET_Topo_MET", h_jet_leadjet_eta_V_MET_Topo_MET);

    h_jet_leadjet_phi_V_MET_Topo_MET = new TH2F("h_jet_leadjet_phi_V_MET_Topo_MET","Lead jet jet phi V MET_Topo (MET)",
					   m_nbinphi,-1*m_binphi,m_binphi,m_nbinp,0.,m_suptmi);
    sc = m_thistSvc->regHist("/AANT/" + m_folderName + "BasicPlots/JetVariables/jet_leadjet_phi_V_MET_Topo_MET", h_jet_leadjet_phi_V_MET_Topo_MET);

    h_jet_secondjet_et_V_MET_Topo_MET = new TH2F("h_jet_secondjet_et_V_MET_Topo_MET","Second jet jet Et V MET_Topo (MET)",
					   m_nbinJetEt, m_lowerbinJetEt, m_upperbinJetEt,m_nbinp,0.,m_suptmi);
    sc = m_thistSvc->regHist("/AANT/" + m_folderName + "BasicPlots/JetVariables/jet_secondjet_et_V_MET_Topo_MET", h_jet_secondjet_et_V_MET_Topo_MET);

    h_jet_secondjet_eta_V_MET_Topo_MET = new TH2F("h_jet_secondjet_eta_V_MET_Topo_MET","Second jet jet eta V MET_Topo (MET)",
					   2*m_nbineta,-1*m_bineta,m_bineta,m_nbinp,0.,m_suptmi);
    sc = m_thistSvc->regHist("/AANT/" + m_folderName + "BasicPlots/JetVariables/jet_secondjet_eta_V_MET_Topo_MET", h_jet_secondjet_eta_V_MET_Topo_MET);

    h_jet_secondjet_phi_V_MET_Topo_MET = new TH2F("h_jet_secondjet_phi_V_MET_Topo_MET","Second jet jet phi V MET_Topo (MET)",
					   m_nbinphi,-1*m_binphi,m_binphi,m_nbinp,0.,m_suptmi);
    sc = m_thistSvc->regHist("/AANT/" + m_folderName + "BasicPlots/JetVariables/jet_secondjet_phi_V_MET_Topo_MET", h_jet_secondjet_phi_V_MET_Topo_MET);

    h_jet_n_V_MET_RefFinal_MET = new TH2F("h_jet_n_V_MET_RefFinal_MET","N jets V MET_RefFinal (MET)",20,0,20,m_nbinp,0.,m_suptmi);
    sc = m_thistSvc->regHist("/AANT/" + m_folderName + "BasicPlots/JetVariables/jet_n_V_MET_RefFinal_MET", h_jet_n_V_MET_RefFinal_MET);

    h_jet_n_V_MET_RefFinal_SET = new TH2F("h_jet_n_V_MET_RefFinal_SET","N jets V MET_RefFinal (SET)",20,0,20,m_nbinE,m_lowET,m_suET);
    sc = m_thistSvc->regHist("/AANT/" + m_folderName + "BasicPlots/JetVariables/jet_n_V_MET_RefFinal_SET", h_jet_n_V_MET_RefFinal_SET);

    h_jet_leadjet_et_V_MET_RefFinal_MET = new TH2F("h_jet_leadjet_et_V_MET_RefFinal_MET","Lead jet jet Et V MET_RefFinal (MET)",
					   m_nbinJetEt, m_lowerbinJetEt, m_upperbinJetEt,m_nbinp,0.,m_suptmi);
    sc = m_thistSvc->regHist("/AANT/" + m_folderName + "BasicPlots/JetVariables/jet_leadjet_et_V_MET_RefFinal_MET", h_jet_leadjet_et_V_MET_RefFinal_MET);

    h_jet_leadjet_eta_V_MET_RefFinal_MET = new TH2F("h_jet_leadjet_eta_V_MET_RefFinal_MET","Lead jet jet eta V MET_RefFinal (MET)",
					   2*m_nbineta,-1*m_bineta,m_bineta,m_nbinp,0.,m_suptmi);
    sc = m_thistSvc->regHist("/AANT/" + m_folderName + "BasicPlots/JetVariables/jet_leadjet_eta_V_MET_RefFinal_MET", h_jet_leadjet_eta_V_MET_RefFinal_MET);

    h_jet_leadjet_phi_V_MET_RefFinal_MET = new TH2F("h_jet_leadjet_phi_V_MET_RefFinal_MET","Lead jet jet phi V MET_RefFinal (MET)",
					   m_nbinphi,-1*m_binphi,m_binphi,m_nbinp,0.,m_suptmi);
    sc = m_thistSvc->regHist("/AANT/" + m_folderName + "BasicPlots/JetVariables/jet_leadjet_phi_V_MET_RefFinal_MET", h_jet_leadjet_phi_V_MET_RefFinal_MET);

    h_jet_secondjet_et_V_MET_RefFinal_MET = new TH2F("h_jet_secondjet_et_V_MET_RefFinal_MET","Second jet jet Et V MET_RefFinal (MET)",
					   m_nbinJetEt, m_lowerbinJetEt, m_upperbinJetEt,m_nbinp,0.,m_suptmi);
    sc = m_thistSvc->regHist("/AANT/" + m_folderName + "BasicPlots/JetVariables/jet_secondjet_et_V_MET_RefFinal_MET", h_jet_secondjet_et_V_MET_RefFinal_MET);

    h_jet_secondjet_eta_V_MET_RefFinal_MET = new TH2F("h_jet_secondjet_eta_V_MET_RefFinal_MET","Second jet jet eta V MET_RefFinal (MET)",
					   2*m_nbineta,-1*m_bineta,m_bineta,m_nbinp,0.,m_suptmi);
    sc = m_thistSvc->regHist("/AANT/" + m_folderName + "BasicPlots/JetVariables/jet_secondjet_eta_V_MET_RefFinal_MET", h_jet_secondjet_eta_V_MET_RefFinal_MET);

    h_jet_secondjet_phi_V_MET_RefFinal_MET = new TH2F("h_jet_secondjet_phi_V_MET_RefFinal_MET","Second jet jet phi V MET_RefFinal (MET)",
					   m_nbinphi,-1*m_binphi,m_binphi,m_nbinp,0.,m_suptmi);
    sc = m_thistSvc->regHist("/AANT/" + m_folderName + "BasicPlots/JetVariables/jet_secondjet_phi_V_MET_RefFinal_MET", h_jet_secondjet_phi_V_MET_RefFinal_MET);

  }//_plotJetVariables

  if (sc.isFailure()) {
     msg() << MSG::ERROR
          << "Unable to retrieve pointer to THistSvc"
          << endreq;
     return sc;
  }

// Z/W datasets
  if(m_iset == 6052 || m_iset == 6020 || m_iset == 6021) {
    m_nbinp=100;
    m_suptmi=100.;
    m_nbinE=100;
    m_suET=1000.+m_lowET;
    m_nbinpxy=100;
    m_suptmixy=50.;
   }
// beamdata
  if(m_iset >87000) {
    m_nbinp=100;
    m_suptmi=1000.;
    m_nbinE=100;
    m_suET=1000.+m_lowET;
    m_nbinpxy=100;
    m_suptmixy=50.;
   }

  book1d ("ETmissTruthInt_plus_Muons",m_nbinp,0.,m_suptmi,h_ETmissTruthInt_plus_Muons);

  ////////////////////////////////////////////////////////////////////////////////////////////////
  book1d ("ETmissCorrFinal",m_nbinp,0.,m_suptmi,h_ETmissCorrFinal);

  book1d ("SumETCorrFinal",m_nbinE,m_lowET,m_suET,h_SumETCorrFinal);

  h_EtMissH1calibrated = new TH1D("h_EtMissH1calibrated","h_EtMissH1-calibrated",m_nbinp,0.,m_suptmi);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "BasicPlots/EtMissH1calibrated",h_EtMissH1calibrated);

  h_RefFinal_simple_Significance = new TH1D("h_RefFinal_simple_Significance","RefFinal simple significance (met / sqrt(SET))",m_nbinsig,0.,m_binsig);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "BasicPlots/RefFinal/RefFinal_simple_Significance", h_RefFinal_simple_Significance);

  book_missingETHists ("RefFinal",      h_RefFinal);
  book_missingETHists ("RefEle",        h_RefEle);
  book_missingETHists ("RefJet",        h_RefJet);
  book_missingETHists ("SoftJets",      h_SoftJets);
  book_missingETHists ("MuonBoy",       h_MuonBoy);
  book_missingETHists ("MuID",          h_MuID);
  book_missingETHists ("RefMuon",       h_RefMuon);
  book_missingETHists ("RefMuon_Track", h_RefMuon_Track);
  book_missingETHists ("RefGamma",      h_RefGamma);
  book_missingETHists ("CellOut",       h_CellOut);
  book_missingETHists ("CellOutEFlow",  h_CellOutEFlow);
  book_missingETCaloHists  ("Base",     h_Base);
  book_missingETCaloHists  ("CorrTopo", h_CorrTopo);
  book_missingETCaloHists  ("Calib",    h_Calib);
  book_missingETHists ("CryoCone",      h_CryoCone);
  book_missingETHists ("TruthInt",      h_TruthInt);
  book_missingETHists ("TruthNonInt",   h_TruthNonInt);
  book_missingETHists ("TruthMuons",    h_TruthMuons);
  book_missingETHists ("Cryo",          h_Cryo);
  book_missingETHists ("DM_All",        h_DM_All);
  book_missingETHists ("DM_Crack1",     h_DM_Crack1);
  book_missingETHists ("DM_Crack2",     h_DM_Crack2);
  book_missingETHists ("DM_Cryo",       h_DM_Cryo);
  book_missingETHists ("Final",         h_Final);
  book_missingETHists ("LocHadTopoObj", h_LocHadTopoObj);
  book_missingETHists ("TopoObj",       h_TopoObj);
  book_missingETHists ("MuonBoy_Spectro", h_MuonBoy_Spectro);
  book_missingETHists ("MuonBoy_Track", h_MuonBoy_Track);
  book_missingETHists ("Muon",          h_Muon);
  book_missingETHists ("MuonMuons",     h_MuonMuons);

  h_METBase_EMfraction = new TH1F("h_METBase_EMfraction","METBase_EMfraction",200, -0.5,1.5);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "BasicPlots/METBase_EMfraction",h_METBase_EMfraction);

  //////////////////////////////////// MU & BCID plots //////////////////////////////////////////////

  h_MU = new TH1F("h_MU","h_MU",m_nbinMU,m_lowMU,m_upMU);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "BasicPlots/h_MU", h_MU);

  h_BCID = new TH1F("h_BCID","h_BCID",m_nbinBCID,m_lowBCID,m_upBCID);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "BasicPlots/h_BCID", h_BCID);

  //////////////////////////////////////////////// Resolution plots //////////////////////////////////////////////////////

  h_Resolution_TruthNonInt_RefFinal_METx = new TH1D("h_Resolution_TruthNonInt_RefFinal_METx","Resolution_TruthNonInt_RefFinal_METx",1.5*m_nbinpxy,-1.5*m_suptmixy,1.5*m_suptmixy );
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "BasicPlots/Resolution/h_Resolution_TruthNonInt_RefFinal_METx", h_Resolution_TruthNonInt_RefFinal_METx);

  h_Resolution_TruthNonInt_RefFinal_METy = new TH1D("h_Resolution_TruthNonInt_RefFinal_METy","Resolution_TruthNonInt_RefFinal_METy",1.5*m_nbinpxy,-1.5*m_suptmixy,1.5*m_suptmixy );
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "BasicPlots/Resolution/h_Resolution_TruthNonInt_RefFinal_METy", h_Resolution_TruthNonInt_RefFinal_METy);

  if (sc.isFailure()) { msg() << MSG::WARNING << "BasicPlotsTool CBNT_initialize() fail" << endreq; }

  return sc;
}

StatusCode BasicPlotsTool::initialize() {
  return StatusCode::SUCCESS;
}

StatusCode BasicPlotsTool::execute(MissingETData *data) {

  /** check that the input and the output containers are defined */
  StatusCode sc = StatusCode::SUCCESS;

  msg() << MSG::DEBUG << "execute() has been called" << endreq;

  sc = basicPlots(data);

  if (sc.isFailure()) { msg() << MSG::WARNING << "BasicPlotsTool execute() fail" << endreq; }

  return sc;
}

StatusCode BasicPlotsTool::basicPlots(MissingETData *data) {

  msg() << MSG::DEBUG << "in basicPlots() " << endreq;

  StatusCode sc = StatusCode::SUCCESS;

  if (evtStore()->contains<xAOD::EventInfo>(m_evtInfoKey)) {
    const xAOD::EventInfo* pevt = 0;
    sc = evtStore()->retrieve(pevt, m_evtInfoKey);
    if( !sc.isFailure() ){
      unsigned int lbn = pevt->lumiBlock();

      h_MET_Topo_METx_V_lbn->Fill(lbn,data->topo()->etx()/1000.);
      h_MET_Topo_METy_V_lbn->Fill(lbn,data->topo()->ety()/1000.);
      h_MET_RefFinal_METx_V_lbn->Fill(lbn,data->refFinal()->etx()/1000.);
      h_MET_RefFinal_METy_V_lbn->Fill(lbn,data->refFinal()->ety()/1000.);

    }//!isFailure()
  }//container present

  double significance = 0.;
  if (data->refFinal()->sumet()!=0) {
    significance = (data->refFinal()->et()/1000.) / (sqrt(data->refFinal()->sumet()/1000.));//in GeV^1/2
  }

  if (_plotClusterVariables) {
    sc = _clusterVariables->retrieveContainers();
    if ( sc.isFailure() ) {
      msg() << MSG::WARNING << "TopoClusterVariablesTool retrieveContainers() Failed" << endreq;
      return sc;
    }

    h_cluster_n->Fill(_clusterVariables->clusterContainer()->size());
    h_cluster_n_V_MET_Topo_MET->Fill(_clusterVariables->clusterContainer()->size(),data->topo()->et()/1000.);
    h_cluster_n_V_MET_RefFinal_MET->Fill(_clusterVariables->clusterContainer()->size(),data->refFinal()->et()/1000.);

    h_cluster_n_V_MET_Topo_SET->Fill(_clusterVariables->clusterContainer()->size(),data->topo()->sumet()/1000.);
    h_cluster_n_V_MET_RefFinal_SET->Fill(_clusterVariables->clusterContainer()->size(),data->refFinal()->sumet()/1000.);

    const CaloClusterContainer *clustercont = _clusterVariables->clusterContainer();
    for (CaloClusterContainer::const_iterator it = clustercont->begin(); it!=clustercont->end(); ++it) {

      h_cluster_et->Fill((*it)->et()/1000.);
      h_cluster_eta->Fill((*it)->eta());
      h_cluster_phi->Fill((*it)->phi());

      h_cluster_eta_V_phi->Fill((*it)->eta(),(*it)->phi());
      h_cluster_et_V_eta->Fill((*it)->eta(),(*it)->et()/1000.);
      h_cluster_et_V_phi->Fill((*it)->phi(),(*it)->et()/1000.);
    }
  }//plotClusterVariables

  if (_plotJetVariables) {
    sc = _jetVariables->retrieveContainers();
    if ( sc.isFailure() ) {
      msg() << MSG::WARNING << "JetVariablesTool retrieveContainers() Failed" << endreq;
      return sc;
    }
    h_jetPtWeightedEventEMfraction->Fill(_jetVariables->JetPtWeightedEventEMfraction());
    h_jetPtWeightedNumAssociatedTracks->Fill(_jetVariables->JetPtWeightedNumAssociatedTracks());
    h_jetPtWeightedSize->Fill(_jetVariables->JetPtWeightedSize());
    h_RefFinal_simple_Significance_v_leadJetEta->Fill(fabs(_jetVariables->leadingJetEta()), significance);
    h_RefFinal_simple_Significance_v_secondJetEta->Fill(fabs(_jetVariables->secondJetEta()), significance);

    h_jet_n->Fill(_jetVariables->jetCollection()->size());
    h_jet_n_V_MET_Topo_MET->Fill(_jetVariables->jetCollection()->size(),data->topo()->et()/1000.);
    h_jet_n_V_MET_RefFinal_MET->Fill(_jetVariables->jetCollection()->size(),data->refFinal()->et()/1000.);
    h_jet_n_V_MET_Topo_SET->Fill(_jetVariables->jetCollection()->size(),data->topo()->sumet()/1000.);
    h_jet_n_V_MET_RefFinal_SET->Fill(_jetVariables->jetCollection()->size(),data->refFinal()->sumet()/1000.);

    h_jet_leadjet_et_V_MET_Topo_MET->Fill(_jetVariables->leadingJetEt()/1000.,data->topo()->et()/1000.);
    h_jet_leadjet_eta_V_MET_Topo_MET->Fill(_jetVariables->leadingJetEta(),data->topo()->et()/1000.);
    h_jet_leadjet_phi_V_MET_Topo_MET->Fill(_jetVariables->leadingJetPhi(),data->topo()->et()/1000.);

    h_jet_leadjet_et_V_MET_RefFinal_MET->Fill(_jetVariables->leadingJetEt()/1000.,data->refFinal()->et()/1000.);
    h_jet_leadjet_eta_V_MET_RefFinal_MET->Fill(_jetVariables->leadingJetEta(),data->refFinal()->et()/1000.);
    h_jet_leadjet_phi_V_MET_RefFinal_MET->Fill(_jetVariables->leadingJetPhi(),data->refFinal()->et()/1000.);

    h_jet_secondjet_et_V_MET_Topo_MET->Fill(_jetVariables->secondJetEt()/1000.,data->topo()->et()/1000.);
    h_jet_secondjet_eta_V_MET_Topo_MET->Fill(_jetVariables->secondJetEta(),data->topo()->et()/1000.);
    h_jet_secondjet_phi_V_MET_Topo_MET->Fill(_jetVariables->secondJetPhi(),data->topo()->et()/1000.);

    h_jet_secondjet_et_V_MET_RefFinal_MET->Fill(_jetVariables->secondJetEt()/1000.,data->refFinal()->et()/1000.);
    h_jet_secondjet_eta_V_MET_RefFinal_MET->Fill(_jetVariables->secondJetEta(),data->refFinal()->et()/1000.);
    h_jet_secondjet_phi_V_MET_RefFinal_MET->Fill(_jetVariables->secondJetPhi(),data->refFinal()->et()/1000.);

    const JetCollection *jetcoll = _jetVariables->jetCollection();
    for (JetCollection::const_iterator it = jetcoll->begin(); it!=jetcoll->end(); ++it) {

      h_jet_et->Fill((*it)->et()/1000.);
      h_jet_eta->Fill((*it)->eta());
      h_jet_phi->Fill((*it)->phi());
    }
  }//if _plotJetVariables



  h_ETmissTruthInt_plus_Muons->Fill( sqrt(pow(data->truth_int()->etx()+data->truth_muons()->etx(),2)+pow(data->truth_int()->ety()+data->truth_muons()->ety(),2))/1000. );

  h_ETmissCorrFinal->Fill( sqrt(pow(data->refFinal()->etx()-data->corrTopo()->etx()+data->calib()->etx(),2)+
			      pow(data->refFinal()->ety()-data->corrTopo()->ety()+data->calib()->ety(),2))
			 /1000. );
  h_SumETCorrFinal->Fill( (data->calib()->sumet())/1000. );

  h_EtMissH1calibrated->Fill( (sqrt(pow(data->corrTopo()->etx(),2)+pow(data->corrTopo()->ety(),2))) / 1000. );

  h_RefFinal_simple_Significance->Fill(significance);

  // HACK: substrach MuonBoy from RefFinal
  //float reff2 = pow(data->refFinal()->etx()-data->MuonBoy()->etx(), 2) + pow(data->refFinal()->ety()-data->MuonBoy()->ety(), 2);
  //h_RefFinal_SET->Fill((data->refFinal()->sumet() - data->MuonBoy()->sumet()) / 1000.);
  //h_RefFinal_MET->Fill( sqrt(reff2)/1000. );

  fill_missingETHists (h_RefFinal,      data->refFinal());
  fill_missingETHists (h_RefJet,        data->refJet());
  fill_missingETHists (h_SoftJets,      data->softJets());
  fill_missingETHists (h_RefEle,        data->refEle());
  fill_missingETHists (h_MuonBoy,       data->MuonBoy());
  fill_missingETHists (h_MuID,          data->MuID());
  fill_missingETHists (h_RefMuon,       data->refMuon());
  fill_missingETHists (h_RefMuon_Track, data->refMuon_Track());
  fill_missingETHists (h_RefGamma,      data->refGamma());
  fill_missingETHists (h_RefTau,        data->refTau());
  fill_missingETHists (h_CellOut,       data->cellOut());
  fill_missingETHists (h_CellOutEFlow,  data->cellOutEFlow());
  fill_missingETCaloHists (h_Base,      data->base());
  fill_missingETCaloHists (h_CorrTopo,  data->corrTopo());
  fill_missingETCaloHists (h_Calib,     data->calib());
  fill_missingETHists (h_CryoCone,      data->cryoCone());
  fill_missingETHists (h_TruthInt,      data->truth_int());
  fill_missingETHists (h_TruthNonInt,   data->truth_nonInt());
  fill_missingETHists (h_TruthMuons,    data->truth_muons());
  fill_missingETHists (h_Cryo,          data->cryo());
  fill_missingETHists (h_DM_All,        data->DM_All());
  fill_missingETHists (h_DM_Crack1,     data->DM_Crack1());
  fill_missingETHists (h_DM_Crack2,     data->DM_Crack2());
  fill_missingETHists (h_DM_Cryo,       data->DM_Cryo());
  fill_missingETHists (h_Final,         data->final());
  fill_missingETHists (h_LocHadTopoObj, data->locHadTopoObj());
  fill_missingETHists (h_TopoObj,       data->topoObj());
  fill_missingETHists (h_MuonBoy_Spectro, data->muonBoy_Spectro());
  fill_missingETHists (h_MuonBoy_Track, data->muonBoy_Track());
  fill_missingETHists (h_Muon,          data->muon());
  fill_missingETHists (h_MuonMuons,     data->muonMuons());
  fill_missingETCaloHists (h_Base0,     data->base0());
  fill_missingETCaloHists (h_Topo,      data->topo());
  fill_missingETCaloHists (h_LocHadTopo,data->locHadTopo());

  h_METBase_EMfraction->Fill( data->sumet_MissingET_Base_EMfraction() );

  h_MU->Fill(data->MU());
  h_BCID->Fill(data->BCID());

  h_Resolution_TruthNonInt_RefFinal_METx->Fill((data->truth_nonInt()->etx()- data->refFinal()->etx())/1000.);
  h_Resolution_TruthNonInt_RefFinal_METy->Fill((data->truth_nonInt()->ety()- data->refFinal()->ety())/1000.);

  if (sc.isFailure()) { msg() << MSG::WARNING << "BasicPlotsTool basicPlots() fail" << endreq; }

  return sc;
}

//------------------------------------------------------------------------------
StatusCode BasicPlotsTool::finalize() {
  return StatusCode::SUCCESS;
}

//------------------------------------------------------------------------------
BasicPlotsTool::~BasicPlotsTool() {}
