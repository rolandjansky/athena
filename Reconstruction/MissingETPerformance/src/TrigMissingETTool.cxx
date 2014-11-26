/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ITHistSvc.h"

#include "StoreGate/StoreGateSvc.h"

//#include "CLHEP/Units/PhysicalConstants.h"
#include "CLHEP/Units/SystemOfUnits.h"

#include "MissingETPerformance/TrigMissingETTool.h"
#include "MissingETPerformance/MissingETData.h"

#include "boost/tokenizer.hpp"

#include "TH1F.h"
#include "TH2F.h"


using CLHEP::GeV;
using boost::tokenizer;
using boost::char_separator;

//------------------------------------------------------------------------------
TrigMissingETTool::TrigMissingETTool( const std::string& type,
                  const std::string& name, 
                  const IInterface* parent )
  : AthAlgTool( type, name, parent ) {
  declareInterface<TrigMissingETTool>( this );

  declareProperty("ToolName", m_name="");
  declareProperty("FolderName",           m_folderName="");

  m_name = (m_name == "") ? this->name() : m_name;
  size_t pos;
  if( (pos = m_name.find("ToolSvc.")) != std::string::npos ) m_name = m_name.erase(pos,8);
  m_curdir = "/AANT/" + m_folderName + m_name + '/';
}

//------------------------------------------------------------------------------

StatusCode TrigMissingETTool::CBNT_initialize() {
  StatusCode sc = service("THistSvc", m_thistSvc);

  msg() << MSG::DEBUG << "Res Tool CBNT_initialize() has been called" << endreq;

  if (sc.isFailure()) {
    msg() << MSG::ERROR
          << "Unable to retrieve pointer to THistSvc"
          << endreq;
     return sc;
  }

  msg() << MSG::DEBUG << "Dir after setDir() = " << getDir() << endreq;
  
  setDir("L1");
  sc = bookL1hists();
  
  setDir("L2");
  sc = bookL2hists();
  
  setDir("EF");
  sc = bookEFhists();
  
  return StatusCode::SUCCESS;
}

StatusCode TrigMissingETTool::addHistogram(TH1 *h1) {

  if(!h1) return StatusCode::FAILURE;

  StatusCode sc = StatusCode::SUCCESS;
  std::string m_dir = m_curdir;
  std::string m_name = h1->GetName();
  std::string m_reg = m_dir + m_name;
  if(m_thistSvc->regHist(m_reg,h1).isFailure()) {
    msg() << MSG::ERROR << "Couldn't register 1d histogram \"" << m_name << "\" in " << m_dir << endreq;
    sc = StatusCode::FAILURE;
  }
  return sc;
}

StatusCode TrigMissingETTool::addHistogram(TH2 *h2) {

  if(!h2) return StatusCode::FAILURE;

  StatusCode sc = StatusCode::SUCCESS;
  std::string m_dir = m_curdir;
  std::string m_name = h2->GetName();
  std::string m_reg = m_dir + m_name;
  if(m_thistSvc->regHist(m_reg,h2).isFailure()) {
    msg() << MSG::ERROR << "Couldn't register 2d histogram \"" << m_name << "\" in " << m_dir << endreq;
  }
  return sc;
}

TH1* TrigMissingETTool::hist(const std::string& m_hist) {
  std::string m_dir = m_curdir;
  m_dir += m_hist;
  TH1 *h(0);
  if (m_thistSvc->getHist(m_dir,h).isSuccess()) {
  } else {
    msg() << MSG::ERROR << "Couldn't retrieve 1d histogram \"" << m_hist << "\" from " << m_dir << endreq;
  }
  return h;
}

TH2* TrigMissingETTool::hist2(const std::string& m_hist) {
  std::string m_dir = m_curdir;
  m_dir += m_hist;
  TH2 *h(0);
  if (m_thistSvc->getHist(m_dir,h).isSuccess()) {
  } else {
    msg() << MSG::ERROR << "Couldn't retrieve 2d histogram \"" << m_hist << "\" from " << m_dir << endreq;
  }
  return h;
}

std::string& TrigMissingETTool::getDir() {
  return m_curdir;
}

void TrigMissingETTool::setDir(const std::string& m_grp) {

  m_curdir  = "/AANT/" + m_folderName + m_name + '/';
  std::string m_add = "";
  
  // Parse input string and extract folder names
  std::vector<std::string> m_dirList;
  const char* separator="/"; 
  typedef  tokenizer<char_separator<char> > Tokenizer;
  Tokenizer tokComp(m_grp, char_separator<char>(separator));
  Tokenizer::iterator it = tokComp.begin();
  
  // Set directory to m_grp (relative to basedir)
  while(it != tokComp.end()) {
    msg() << MSG::DEBUG << *(it) << ";"; 
    m_dirList.push_back(*(it++));
  }
  size_t m_dsize = m_dirList.size();
  if(!m_dsize) return ;
  else {
    for(size_t j = 0; j < m_dsize; j++) {
      if (j == 0 && m_dirList[j] == "AANT") ;
      else if((/*j == 1 &&*/ m_dirList[j] == m_name)) ;
      else {
        m_add += m_dirList[j];
        m_add += '/';
      }
    }
  }
  m_curdir += m_add;
}


/*---------------------------------------------------------*/
StatusCode TrigMissingETTool::bookL1hists() 
/*---------------------------------------------------------*/
{
 
  StatusCode sc = StatusCode::SUCCESS;
  
  msg() << MSG::DEBUG << "TrigMissingETTool : in bookL1hists()" << endreq;

  
  addHistogram(new TH1F(Form("L1_METx"),     "LVL1 METx (GeV);METx (GeV)",        199, -298.5, 298.5));
  addHistogram(new TH1F(Form("L1_METy"),     "LVL1 METy (GeV);METy (GeV)",        199, -298.5, 298.5));
  addHistogram(new TH1F(Form("L1_MET"),      "LVL1 MET (GeV);MET (GeV)",          205, -13.5, 601.5));
  addHistogram(new TH1F(Form("L1_SumEt"),    "LVL1 SumEt (GeV);SumEt (GeV)",      305, -27., 1803.));
  addHistogram(new TH1F(Form("L1_MET_phi"),  "LVL1 MET #phi (rad);MET #phi (rad)",64, -3.1416, 3.1416));
  
  return sc;
  
}

/*---------------------------------------------------------*/
StatusCode TrigMissingETTool::bookL2hists()
/*---------------------------------------------------------*/
{

  StatusCode sc = StatusCode::SUCCESS;

  msg() << MSG::DEBUG << "TrigMissingETTool : in bookL2hists()" << endreq;

  addHistogram(new TH1F(Form("L2_METx"),     "LVL2 METx (GeV);METx (GeV)",        199, -298.5, 298.5));
  addHistogram(new TH1F(Form("L2_METy"),     "LVL2 METy (GeV);METy (GeV)",        199, -298.5, 298.5));
  addHistogram(new TH1F(Form("L2_MET"),      "LVL2 MET (GeV);MET (GeV)",          205, -13.5, 601.5));
  addHistogram(new TH1F(Form("L2_SumEt"),    "LVL2 SumEt (GeV);SumEt (GeV)",      305, -27., 1803.));
  addHistogram(new TH1F(Form("L2_MET_phi"),  "LVL2 MET #phi (rad);MET #phi (rad)",64, -3.1416, 3.1416));
  return sc;

}

/*---------------------------------------------------------*/
StatusCode TrigMissingETTool::bookEFhists()
/*---------------------------------------------------------*/
{

  StatusCode sc = StatusCode::SUCCESS;

  msg() << MSG::DEBUG << "TrigMissingETTool : in bookEFhists()" << endreq;
  
  // default 
  addHistogram(new TH1F("EF_METx_log_def",  "EF : sgn(METx)*log_10(METx/GeV);sgn(METx)*log_10(METx/GeV)", 175, -3.5, 3.5));
  addHistogram(new TH1F("EF_METy_log_def",  "EF : sgn(METy)*log_10(METy/GeV);sgn(METy)*log_10(METy/GeV)", 175, -3.5, 3.5));
  addHistogram(new TH1F("EF_MET_log_def",   "EF : log_10(MET/GeV);log_10(MET/GeV)", 100, -0.5, 3.5));
  addHistogram(new TH1F("EF_SumEt_log_def", "EF : log_10(SumET/GeV);log_10(SumET/GeV)", 100, -0.5, 3.5));
  
  addHistogram(new TH1F("EF_METx_lin_def",  "EF : METx (GeV);METx (GeV)", 199, -298.5, 298.5));
  addHistogram(new TH1F("EF_METy_lin_def",  "EF : METy (GeV);METy (GeV)", 199, -298.5, 298.5));
  addHistogram(new TH1F("EF_MET_lin_def",   "EF : MET (GeV);MET (GeV)", 205, -13.5, 601.5));
  addHistogram(new TH1F("EF_SumEt_lin_def", "EF : SumET (GeV);SumET (GeV)", 305, -27., 1803.));
  addHistogram(new TH1F("EF_MET_phi_def",   "EF : MET #phi (rad);#phi (rad)", 64, -3.1416, 3.1416));

  // noise suppressed
  addHistogram(new TH1F("EF_METx_log_nsup",  "EF : sgn(METx)*log_10(METx/GeV);sgn(METx)*log_10(METx/GeV)", 175, -3.5, 3.5));
  addHistogram(new TH1F("EF_METy_log_nsup",  "EF : sgn(METy)*log_10(METy/GeV);sgn(METy)*log_10(METy/GeV)", 175, -3.5, 3.5));
  addHistogram(new TH1F("EF_MET_log_nsup",   "EF : log_10(MET/GeV);log_10(MET/GeV)", 100, -0.5, 3.5));
  addHistogram(new TH1F("EF_SumEt_log_nsup", "EF : log_10(SumET/GeV);log_10(SumET/GeV)", 100, -0.5, 3.5));
  
  addHistogram(new TH1F("EF_METx_lin_nsup",  "EF : METx (GeV);METx (GeV)", 199, -298.5, 298.5));
  addHistogram(new TH1F("EF_METy_lin_nsup",  "EF : METy (GeV);METy (GeV)", 199, -298.5, 298.5));
  addHistogram(new TH1F("EF_MET_lin_nsup",   "EF : MET (GeV);MET (GeV)", 205, -13.5, 601.5));
  addHistogram(new TH1F("EF_SumEt_lin_nsup", "EF : SumET (GeV);SumET (GeV)", 305, -27., 1803.));
  addHistogram(new TH1F("EF_MET_phi_nsup",   "EF : MET #phi (rad);#phi (rad)", 64, -3.1416, 3.1416));
  
  // feb header
  addHistogram(new TH1F("EF_METx_log_feb",  "EF : sgn(METx)*log_10(METx/GeV);sgn(METx)*log_10(METx/GeV)", 175, -3.5, 3.5));
  addHistogram(new TH1F("EF_METy_log_feb",  "EF : sgn(METy)*log_10(METy/GeV);sgn(METy)*log_10(METy/GeV)", 175, -3.5, 3.5));
  addHistogram(new TH1F("EF_MET_log_feb",   "EF : log_10(MET/GeV);log_10(MET/GeV)", 100, -0.5, 3.5));
  addHistogram(new TH1F("EF_SumEt_log_feb", "EF : log_10(SumET/GeV);log_10(SumET/GeV)", 100, -0.5, 3.5));
  
  addHistogram(new TH1F("EF_METx_lin_feb",  "EF : METx (GeV);METx (GeV)", 199, -298.5, 298.5));
  addHistogram(new TH1F("EF_METy_lin_feb",  "EF : METy (GeV);METy (GeV)", 199, -298.5, 298.5));
  addHistogram(new TH1F("EF_MET_lin_feb",   "EF : MET (GeV);MET (GeV)", 205, -13.5, 601.5));
  addHistogram(new TH1F("EF_SumEt_lin_feb", "EF : SumET (GeV);SumET (GeV)", 305, -27., 1803.));
  addHistogram(new TH1F("EF_MET_phi_feb",   "EF : MET #phi (rad);#phi (rad)", 64, -3.1416, 3.1416));

  return sc;

}


StatusCode TrigMissingETTool::initialize() {
  msg() << MSG::DEBUG << "Res Tool initialize() has been called" << endreq;

  return StatusCode::SUCCESS;
}

StatusCode TrigMissingETTool::execute(MissingETData *data) {

  /** check that the input and the output containers are defined */
  StatusCode sc = StatusCode::SUCCESS;

  msg() << MSG::DEBUG << "execute() has been called" << endreq;

  sc = trigMissingETPlots(data);

  return sc;
}

StatusCode TrigMissingETTool::trigMissingETPlots(MissingETData *data) {

  msg() << MSG::DEBUG << "in trigMissingET() " << endreq;


  StatusCode sc = StatusCode::SUCCESS;

  const LVL1_ROI *m_L1EsumROI = data->getL1ROI();
  setDir("L1");
  if(!m_L1EsumROI) ; // do nothing
  else sc = fillL1hists(m_L1EsumROI);

  const  TrigMissingETContainer *m_L2MET = data->getL2MET();
  setDir("L2");
  if(!m_L2MET) ; // do nothing
  else sc = fillL2hists(m_L2MET);

  const  TrigMissingETContainer *m_EFMET_Default = data->getEFMET_default();
  setDir("EF");
  if(!m_EFMET_Default) ; // do nothing
  else sc = fillEFhists(m_EFMET_Default, "DEFAULT");
  
  const  TrigMissingETContainer *m_EFMET_noiseSupp = data->getEFMET_noiseSupp();
  if(!m_EFMET_noiseSupp) ; // do nothing
  else sc = fillEFhists(m_EFMET_noiseSupp, "noiseSupp");

  const  TrigMissingETContainer *m_EFMET_FEB = data->getEFMET_FEBHeader();
  if(!m_EFMET_FEB) ; // do nothing
  else sc = fillEFhists(m_EFMET_FEB, "FEB");
  
  return sc;

}

StatusCode TrigMissingETTool::fillL1hists(const LVL1_ROI *m_L1EsumROI) {
  
  StatusCode sc = StatusCode::SUCCESS;

  LVL1_ROI::energysums_type::const_iterator itL1Esum = m_L1EsumROI->getEnergySumROIs().begin();
  LVL1_ROI::energysums_type::const_iterator itL1Esum_e = m_L1EsumROI->getEnergySumROIs().end();

  TH1 *h(0);
  for( LVL1_ROI::energysums_type::const_iterator it=itL1Esum; it!=itL1Esum_e;++it){
    float m_lvl1_mex = it->getEnergyX();
    float m_lvl1_mey = it->getEnergyY();
    float m_lvl1_met = sqrt(m_lvl1_mex*m_lvl1_mex + m_lvl1_mey*m_lvl1_mey);
    float m_lvl1_phi = atan2f(m_lvl1_mey,m_lvl1_mex);
    float m_lvl1_sumEt = it->getEnergyT(); 
    if((h = hist("L1_METx")))    h->Fill(m_lvl1_mex/GeV);
    if((h = hist("L1_METy")))    h->Fill(m_lvl1_mey/GeV);
    if((h = hist("L1_MET")))     h->Fill(m_lvl1_met/GeV);
    if((h = hist("L1_MET_phi"))) h->Fill(m_lvl1_phi);
    if((h = hist("L1_SumEt")))   h->Fill(m_lvl1_sumEt/GeV);
  }
 
  return sc;
}

/*---------------------------------------------------------*/
StatusCode TrigMissingETTool::fillL2hists(const TrigMissingETContainer* trigMETcont)
  /*---------------------------------------------------------*/
{
  
  msg() << MSG::DEBUG << "TrigMissingETTool : in fillL2hists()" << endreq;

  TrigMissingETContainer::const_iterator trigMETfirst  = trigMETcont->begin();
  TrigMissingETContainer::const_iterator trigMETlast = trigMETcont->end();

  TH1 *h(0);

  for ( ; trigMETfirst !=  trigMETlast; trigMETfirst++ ) {

    const TrigMissingET *trigMET = *trigMETfirst;
    if(!trigMET) continue;

    //float m_etx_log = -9e9;
    //float m_ety_log = -9e9;
    //float m_met_log = -9e9;
    //float m_set_log = -9e9;
    float m_etx_lin = -9e9;
    float m_ety_lin = -9e9;
    float m_met_lin = -9e9;
    float m_set_lin = -9e9;
    float m_phi = -9e9;

    float m_ETx     = trigMET->ex();
    float m_ETy     = trigMET->ey();
    float m_MET     = trigMET->et();
    float m_SumEt   = trigMET->sumEt();
    //float m_ETz     = trigMET->ez();
    //float m_SumE    = trigMET->sumE();
    //float m_E       = trigMET->e();
    //int   m_Flag    = trigMET->getFlag();
    //long  m_RoiWord = trigMET->RoIword();

    m_etx_lin = m_ETx/GeV; 
    m_ety_lin = m_ETy/GeV; 
    m_met_lin = m_MET/GeV; 
    m_set_lin = m_SumEt/GeV; 
    m_phi = atan2f(m_ETy, m_ETx);

    //m_etx_log = copysign(log10(fabsf(m_ETx))-3, m_ETx); // signed, GeV
    //m_ety_log = copysign(log10(fabsf(m_ETy))-3, m_ETy); // signed, GeV
    //m_met_log = log10(fabsf(m_MET))-3;    // positive, GeV
    //m_set_log = log10(fabsf(m_SumEt))-3;    // positive, GeV


    if((h = hist("L2_METx")))     h->Fill(m_etx_lin);
    if((h = hist("L2_METy")))     h->Fill(m_ety_lin);
    if((h = hist("L2_MET")))      h->Fill(m_met_lin);
    if((h = hist("L2_SumEt")))    h->Fill(m_set_lin);
    if((h = hist("L2_MET_phi")))  h->Fill(m_phi);


  }     // loop over TrigMissingET objects

  return StatusCode::SUCCESS; 
}


/*---------------------------------------------------------*/
StatusCode TrigMissingETTool::fillEFhists(const TrigMissingETContainer* trigMETcont, const std::string& suf)
  /*---------------------------------------------------------*/
{
  msg() << MSG::DEBUG << "TrigMissingETTool : in fillEFhists()" << endreq;


  TrigMissingETContainer::const_iterator trigMETfirst  = trigMETcont->begin();
  TrigMissingETContainer::const_iterator trigMETlast = trigMETcont->end();

  TH1 *h(0);
  for ( ; trigMETfirst !=  trigMETlast; trigMETfirst++ ) {

    const TrigMissingET *trigMET = *trigMETfirst;
    if(!trigMET) continue;


    float m_etx_log = -9e9;
    float m_ety_log = -9e9;
    float m_met_log = -9e9;
    float m_set_log = -9e9;
    float m_etx_lin = -9e9;
    float m_ety_lin = -9e9;
    float m_met_lin = -9e9;
    float m_set_lin = -9e9;
    float m_phi = -9e9;

    float m_ETx     = trigMET->ex();
    float m_ETy     = trigMET->ey();
    float m_MET     = trigMET->et();
    float m_SumEt   = trigMET->sumEt();
    //float m_ETz     = trigMET->ez();
    //float m_SumE    = trigMET->sumE();
    //float m_E       = trigMET->e();
    //int   m_Flag    = trigMET->getFlag();
    //long  m_RoiWord = trigMET->RoIword();

    // monitoring values (in GeV)
    m_etx_lin = m_ETx/GeV; 
    m_ety_lin = m_ETy/GeV; 
    m_met_lin = m_MET/GeV; 
    m_set_lin = m_SumEt/GeV; 
    m_phi = atan2f(m_ETy, m_ETx);

    m_etx_log = copysign(log10(fabsf(m_ETx))-3, m_ETx); // signed, GeV
    m_ety_log = copysign(log10(fabsf(m_ETy))-3, m_ETy); // signed, GeV
    m_met_log = log10(fabsf(m_MET))-3;    // positive, GeV
    m_set_log = log10(fabsf(m_SumEt))-3;    // positive, GeV



    std::string sTrigType=" EF: ";

    if(suf == "DEFAULT") {
      if((h = hist("EF_METx_lin_def")))     h->Fill(m_etx_lin);
      if((h = hist("EF_METy_lin_def")))     h->Fill(m_ety_lin);
      if((h = hist("EF_MET_lin_def")))      h->Fill(m_met_lin);
      if((h = hist("EF_SumEt_lin_def")))    h->Fill(m_set_lin);
      if((h = hist("EF_MET_phi_def")))      h->Fill(m_phi);
      if((h = hist("EF_METx_log_def")))     h->Fill(m_etx_log);
      if((h = hist("EF_METy_log_def")))     h->Fill(m_ety_log);
      if((h = hist("EF_MET_log_def")))      h->Fill(m_met_log);
      if((h = hist("EF_SumEt_log_def")))    h->Fill(m_set_log);

    } else if (suf == "noiseSupp") {
      if((h = hist("EF_METx_lin_nsup")))     h->Fill(m_etx_lin);
      if((h = hist("EF_METy_lin_nsup")))     h->Fill(m_ety_lin);
      if((h = hist("EF_MET_lin_nsup")))      h->Fill(m_met_lin);
      if((h = hist("EF_SumEt_lin_nsup")))    h->Fill(m_set_lin);
      if((h = hist("EF_MET_phi_nsup")))      h->Fill(m_phi);
      if((h = hist("EF_METx_log_nsup")))     h->Fill(m_etx_log);
      if((h = hist("EF_METy_log_nsup")))     h->Fill(m_ety_log);
      if((h = hist("EF_MET_log_nsup")))      h->Fill(m_met_log);
      if((h = hist("EF_SumEt_log_nsup")))    h->Fill(m_set_log);
    } else if (suf == "FEB") {
      if((h = hist("EF_METx_lin_feb")))     h->Fill(m_etx_lin);
      if((h = hist("EF_METy_lin_feb")))     h->Fill(m_ety_lin);
      if((h = hist("EF_MET_lin_feb")))      h->Fill(m_met_lin);
      if((h = hist("EF_SumEt_lin_feb")))    h->Fill(m_set_lin);
      if((h = hist("EF_MET_phi_feb")))      h->Fill(m_phi);
      if((h = hist("EF_METx_log_feb")))     h->Fill(m_etx_log);
      if((h = hist("EF_METy_log_feb")))     h->Fill(m_ety_log);
      if((h = hist("EF_MET_log_feb")))      h->Fill(m_met_log);
      if((h = hist("EF_SumEt_log_feb")))    h->Fill(m_set_log);
    } 

    /** Access components - calo samplings / muon correction 
    unsigned int Nc = trigMET->getNumOfComponents();
    if (Nc > 0) {
      for (unsigned int i = 0; i < Nc; ++i) { // loop over components
        const char* name =              trigMET->getNameOfComponent(i).c_str();
        const short status =            trigMET->getStatus(i);
        const unsigned short usedChan = trigMET->getUsedChannels(i);
        const short sumOfSigns =        trigMET->getSumOfSigns(i);
        const float calib0 =            trigMET->getComponentCalib0(i);
        const float calib1 =            trigMET->getComponentCalib1(i);
        const float ex =                trigMET->getExComponent(i);
        const float ey =                trigMET->getEyComponent(i);
        const float ez =                trigMET->getEzComponent(i);
        const float sumE =              trigMET->getSumEComponent(i);
        const float sumEt =             trigMET->getSumEtComponent(i);
      } // loop over components
    } // if (Nc > 0)
    * end access components */

  } // loop over TrigMissingET objects

  return StatusCode::SUCCESS; 
}



//------------------------------------------------------------------------------
StatusCode TrigMissingETTool::finalize() {
  msg() << MSG::DEBUG << "finalize() has been called" << endreq;

  return StatusCode::SUCCESS;
}

//------------------------------------------------------------------------------
TrigMissingETTool::~TrigMissingETTool() {}
