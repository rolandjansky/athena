/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "PhotonAnalysisUtils/PAUhggUserDataTool.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IToolSvc.h"

#include <algorithm>
#include <math.h>
#include <functional>

// constructor
PAUhggUserDataTool::PAUhggUserDataTool(const std::string& t, const std::string& n, const IInterface* p) : AthAlgTool(t,n,p)
{
  declareInterface<IPAUhggUserDataTool>(this);
  
  m_ud = IPAUhggUserDataTool::UserData() ;
  m_udtree = 0 ;
  m_firstEvt = true ;

  declareProperty("TreeName",m_TreeName="HggUserData");
  declareProperty("StreamName",m_StreamName="AANT");
  declareProperty("DumpNtuple",m_dumpNtuple=true);
}

// destructor
PAUhggUserDataTool::~PAUhggUserDataTool()
{}

// initialize
StatusCode PAUhggUserDataTool::initialize() {
  MsgStream log( msgSvc(), name() );
  log << MSG::INFO << "Initializing..." << endreq;

  log << MSG::DEBUG << "... seeking hist service" << endreq ;
  /** get a handle on the NTuple and histogramming service */
  StatusCode sc = service("THistSvc", m_thistSvc);
  if (sc.isFailure()) {
    log << MSG::ERROR << "Unable to retrieve pointer to THistSvc" << endreq;
    return sc;
  }

  return StatusCode::SUCCESS;
}

// finalize
StatusCode PAUhggUserDataTool::finalize() {
  return StatusCode::SUCCESS;
}

IPAUhggUserDataTool::UserData & PAUhggUserDataTool::accessUserData() { 
  MsgStream log(msgSvc(), name());
  static bool firstTime = true ;
  if ( firstTime ) {
    if(init_tree() != StatusCode::SUCCESS){
      log << MSG::WARNING << "Could not initialize user data tree." << endreq;
    }
    firstTime = false ;
  }
  m_ud.Reset();
  return m_ud ; 
} 

void PAUhggUserDataTool::saveUserData() {
if ( m_udtree )  m_udtree->Fill();
}

StatusCode PAUhggUserDataTool::init_tree() {
  MsgStream log( msgSvc(), name() );
  if ( m_dumpNtuple ) {
    log << MSG::INFO << "Booking the tree..." << endreq;
    m_udtree = new TTree(m_TreeName.c_str(),m_TreeName.c_str());
    log << MSG::INFO << "Registering the tree..." << endreq;
    StatusCode sc = m_thistSvc->regTree( "/"+m_StreamName+"/"+m_TreeName , m_udtree ) ;
    if ( sc.isFailure() ) {
      log << MSG::ERROR << "Could not register tree " << "/"+m_StreamName+"/"+m_TreeName << " into THistSvc" << endreq ;
      return sc ;
    }
    log << MSG::INFO << "Setting up tree..." << endreq;
    m_udtree = 0 ;
    sc = m_thistSvc->getTree( "/"+m_StreamName+"/"+m_TreeName , m_udtree ) ;
    if ( sc.isFailure() ) {
      log << MSG::WARNING << "Could not retrieve tree " << "/"+m_StreamName+"/"+m_TreeName << " from THistSvc" << endreq ;
      log << MSG::WARNING << "TTree " << m_TreeName << " will not be filled!" << endreq ;
      //return sc ;
      return StatusCode::SUCCESS ;
    }
    log << MSG::INFO << "Tree " << "/"+m_StreamName+"/"+m_TreeName << " successfully retrieved from THistSvc" << endreq ;
  } else {
    log << MSG::INFO << "You chose not to dump tree " << m_TreeName << endreq ;
    m_udtree = 0 ;
  }

  this->addBranch("RunNumber",           m_ud.ud_run   , "RunNumber/I" );
  this->addBranch("EventNumber",         m_ud.ud_event , "EventNumber/I" );
  this->addBranch("ud_Ph_eta",           m_ud.ud_Ph_eta);
  this->addBranch("ud_Ph_phi",           m_ud.ud_Ph_phi);
  this->addBranch("ud_Ph_pt",            m_ud.ud_Ph_pt);
  this->addBranch("ud_Ph_convFlag",      m_ud.ud_Ph_convFlag);
  this->addBranch("ud_Ph_conv_vtx_Rmin", m_ud.ud_Ph_conv_vtx_Rmin);
  this->addBranch("ud_Ph_conv_vtx_chi2", m_ud.ud_Ph_conv_vtx_chi2);
  this->addBranch("ud_Njets",            m_ud.ud_Njets,      "ud_Njets/I");
  this->addBranch("ud_Jet_eta",          m_ud.ud_Jet_eta);
  this->addBranch("ud_Jet_phi",          m_ud.ud_Jet_phi);
  this->addBranch("ud_Jet_pt",           m_ud.ud_Jet_pt);
  this->addBranch("ud_Jet_E",            m_ud.ud_Jet_E);
  this->addBranch("ud_Nleptons",         m_ud.ud_Nleptons,   "ud_Nleptons/I");
  this->addBranch("ud_Nelectrons",       m_ud.ud_Nelectrons, "ud_Nelectrons/I");
  this->addBranch("ud_El_eta",           m_ud.ud_El_eta);
  this->addBranch("ud_El_phi",           m_ud.ud_El_phi);
  this->addBranch("ud_El_pt",            m_ud.ud_El_pt);
  this->addBranch("ud_El_q",             m_ud.ud_El_q);
  this->addBranch("ud_Nmuons",           m_ud.ud_Nmuons,     "ud_Nmuons/I");
  this->addBranch("ud_Mu_eta",           m_ud.ud_Mu_eta);
  this->addBranch("ud_Mu_phi",           m_ud.ud_Mu_phi);
  this->addBranch("ud_Mu_pt",            m_ud.ud_Mu_pt);
  this->addBranch("ud_Mu_q",             m_ud.ud_Mu_q);
  this->addBranch("ud_Etmiss",           m_ud.ud_Etmiss, "ud_Etmiss/F");
  this->addBranch("ud_Etmiss_phi",       m_ud.ud_Etmiss_phi, "ud_Etmiss_phi/F");
  this->addBranch("ud_LeptonJets_pt",    m_ud.ud_LeptonJets_pt);
  this->addBranch("ud_LeptonJetsSumpt",  m_ud.ud_LeptonJetsSumpt, "ud_LeptonJetsSumpt/F");
  this->addBranch("ud_mgg",            m_ud.ud_mgg,            "ud_mgg/F") ;
  this->addBranch("ud_cosThetaStar",   m_ud.ud_cosThetaStar,   "ud_cosThetaStar/F") ;
  this->addBranch("ud_cosThetaStarCS",   m_ud.ud_cosThetaStarCS,   "ud_cosThetaStarCS/F") ;
  this->addBranch("ud_ptgg",           m_ud.ud_ptgg,           "ud_ptgg/F") ;
  this->addBranch("ud_detagg",         m_ud.ud_detagg,         "ud_detagg/F") ;
  this->addBranch("ud_dphigg",         m_ud.ud_dphigg,         "ud_dphigg/F") ;
  this->addBranch("ud_mggj",           m_ud.ud_mggj,           "ud_mggj/F") ;
  this->addBranch("ud_mjj",            m_ud.ud_mjj,            "ud_mjj/F") ;
  this->addBranch("ud_okHggInclusive", m_ud.ud_okHggInclusive, "ud_okHggInclusive/I") ;
  this->addBranch("ud_okHgg1j",        m_ud.ud_okHgg1j,        "ud_okHgg1j/I") ;
  this->addBranch("ud_okHgg2j",        m_ud.ud_okHgg2j,        "ud_okHgg2j/I") ;
  this->addBranch("ud_okHggZWjj",      m_ud.ud_okHggZWjj,      "ud_okHggZWjj/I");
  this->addBranch("ud_okHggEtmiss",    m_ud.ud_okHggEtmiss,    "ud_okHggEtmiss/I");
  this->addBranch("ud_okHggLepton",    m_ud.ud_okHggLepton,    "ud_okHggLepton/I");
  this->addBranch("ud_L1_2em18",       m_ud.ud_L1_2em18,       "ud_L1_2em18/I") ;
  this->addBranch("ud_L2_2g20",        m_ud.ud_L2_2g20,        "ud_L2_2g20/I") ;
  this->addBranch("ud_EF_2g20",        m_ud.ud_EF_2g20,        "ud_EF_2g20/I") ;
  //this->addBranch("ud_L1_em60",        m_ud.ud_L1_em60,        "ud_L1_em60/I") ;
  //this->addBranch("ud_L2_g60",         m_ud.ud_L2_g60,         "ud_L2_g60/I") ;
  //this->addBranch("ud_EF_g60",         m_ud.ud_EF_g60,         "ud_EF_g60/I") ;
  this->addBranch("ud_PAU_truth",      m_ud.ud_PAU_truth,      "ud_PAU_truth/I") ;
  this->addBranch("ud_Hmass_truth",    m_ud.ud_Hmass_truth,    "ud_Hmass_truth/F") ;
  this->addBranch("ud_brem_truth",     m_ud.ud_brem_truth,     "ud_brem_truth/I") ;
  this->addBranch("ud_mcweight",       m_ud.ud_mcweight,       "ud_mcweight/F"); 

  if ( m_udtree ) {
    log << MSG::INFO << "Content of tree " << m_TreeName << endreq ;
    m_udtree->Print();
    log << MSG::INFO << "End of content of tree " << m_TreeName << endreq ;
  }
  return StatusCode::SUCCESS ;
}
