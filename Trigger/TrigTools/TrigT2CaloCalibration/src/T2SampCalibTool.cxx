/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     T2SampCalibTool.cxx
// PACKAGE:  Trigger/TrigTools/TrigT2CaloCalibration
//
// AUTHORS:   C. Osuna <Carlos.Osuna@ifae.es>
//            P. Conde <Patricia,Conde.Muino@cern.ch>
// MODIFIED:  M. Schram <malachi.schram@cern.ch>
// COMMENT: Code need to be modified and made more dynamic!
//
//   Tool to apply a sampling calibration for jets/taus. 
//   It is based on what is done in JetCalib package for offline.
//   Weights are being specified through jobO.
//  
//   Energy arguments to public function, c_energy, must be specified
//     as transverse energy in MeV.
//
//   New conditional database functionality added via DataHandle & handle() method
// ********************************************************************

#include "TrigT2CaloCalibration/T2SampCalibTool.h"
#include "TrigT2CaloCalibration/T2CaloJetCalib_dBObj.h"
#include <cmath>

// Athena includes
#include "StoreGate/StoreGate.h"
#include "AthenaKernel/IAthenaOutputStreamTool.h"

// Gaudi includes
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/GaudiException.h" 
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/Property.h"

// Needed to register dB object
#include "RegistrationServices/IIOVRegistrationSvc.h"

T2SampCalibTool::T2SampCalibTool(const std::string& t, const std::string& n,
				 const IInterface*  p ) : AthAlgTool(t,n,p),						m_log(msgSvc(), n)	
{
 
 declareInterface<IT2HadCalibTool>(this);
 m_nbin_eta = 44;
 m_etcut = 5;
 m_etmid = 35.;
 m_nreg = 0;
 m_ient = 0;
 m_ieta = 0; 
 m_nSamp = 2;
 m_nEReg = 4;
 m_createDbObj = false;
 m_printDbObj = false;
 m_readFromDb = false;
 declareProperty("nEtaBin",   m_nbin_eta);
 declareProperty("EtaReg",    m_etareg );
 declareProperty("VECut",      m_vecut );
 declareProperty("Weights", m_Weights);
 declareProperty("MinEtCut",m_etcut);
 declareProperty("EtRegionLimit", m_etmid);
 //New dB variables
 declareProperty("CreateDbObject",m_createDbObj);
 declareProperty("PrintDbObject",m_printDbObj);
 declareProperty("ReadFromDB",m_readFromDb);
 declareProperty("L2CalibDbObjectKey", m_obj_key="T2CaloJetCalib");
}

T2SampCalibTool::~T2SampCalibTool() {
}


StatusCode T2SampCalibTool::initialize()
{
  StatusCode sc;

  // Initialize MsgStream
  m_log.setLevel(msgLevel());
  
  //Locate the ptr to the conditions store - needed for dB
  sc = service("DetectorStore", m_detStore);
  if (!sc.isSuccess() || 0 == m_detStore)  {
    m_log << MSG::ERROR <<"Could not find DetStore" <<endmsg;
    return StatusCode::FAILURE;
  }
  
  /////////////////////////////////////////
  // Setup in Svc/Tools needed to use dB //
  /////////////////////////////////////////
  
  //Create dB object needed for POOL/root
  if(m_createDbObj){
    sc = createCondObject();
    //Read dB object info.
    if (sc.isFailure()) {
      m_log << MSG::ERROR
            << " Failed to create T2SampCalibTool dB object "
            << endmsg;
      return StatusCode::FAILURE;
    }
  }
  //End of move (Nov 22th '09)
  if(m_readFromDb){
    sc = readFromDb();
    if (sc.isFailure()) {
        m_log << MSG::ERROR
              << " Failed to read T2SampCalibTool dB object "
              << endmsg;
        //return StatusCode::FAILURE;
    }
  }

 
  
  IIncidentSvc* incSvc;
  sc = service( "IncidentSvc", incSvc );
  if (sc.isFailure()) {
    m_log << MSG::ERROR << "Unable to get the IncidentSvc" << endmsg;
    return StatusCode::FAILURE;
  }
  incSvc->addListener(this, "BeginRun");    
  

  return StatusCode::SUCCESS;  
}


//Allows the calibration weights to be assigned before the first event at "initialize"
void T2SampCalibTool::handle(const Incident& inc) {
  if (inc.type()=="BeginRun") {
    StatusCode sc;
    //Read dB object info.
    /*if(m_readFromDb){
      sc = readFromDb();
      if (sc.isFailure()) {
	m_log << MSG::ERROR
              << " Failed to read T2SampCalibTool dB object "
              << endmsg;
	//return StatusCode::FAILURE;
      }
    }*/
    //Assign weights
    sc = assignWeights();
    if (sc.isFailure()) {
      m_log << MSG::ERROR
            << " Failed to assign T2SampCalibTool calibration weights "
            << endmsg;
      //return StatusCode::FAILURE;
    }
  }
}

StatusCode T2SampCalibTool::finalize()
{
 return StatusCode::SUCCESS;
}


double T2SampCalibTool::c_energy(double EMe,double HADe, double eta)
{

  //AssignWeights during the running
  ///if(m_readFromDb){assignWeights();}

// Internally we must work in GeV
// but the function arguments are specified in MeV
// and the returned value as well in MeV


  double EMenergy = EMe / 1000.;
  double HADenergy = HADe / 1000.;

//  double etf = 1./( (exp(eta)+exp(-eta))/2. );
  double etf = 1./cosh(eta);

  double eR = EMenergy + HADenergy;
  double etR = etf * eR;

  if ( etR < m_etcut ) {
    if ( msgLevel() <= MSG::DEBUG )
      m_log << MSG::DEBUG << "Cluster transv. energy :" << etR << ", below threshold :" 
            << m_etcut << endmsg;
    return 0.0;
  }
  m_ieta = -1;
  for(int i=0; i < m_nbin_eta;++i) {
    if( fabs(eta) >= m_etareg[i] && fabs(eta) < m_etareg[i+1] ) {
      m_ieta = i;
      break;
    }
  } 
  m_nreg = 2;

  // Remove difference between FCal and other detector jets
  //if(m_ieta > 31) m_nreg = 1;

  m_ecut = m_vecut[m_ieta];


  double e2c = m_etmid / etf;

  double f = 1.0;
  int count = 0;
  m_ient = -1;
  double eC = 0;
  double CaloWeight[2] ;

  do
  {
    if ( eR < e2c ) m_ient = 0;
    if ( eR >= e2c ) m_ient = 1;

    get_weight( eR , CaloWeight ) ;

    eC = 0;
    if ( m_nreg == 2)
      eC += CaloWeight[0] * EMenergy + CaloWeight[1] * HADenergy;
    else
      eC += CaloWeight[0] * (EMenergy + HADenergy);


    f = std::fabs(eC - eR ) /eR;
    eR = eC;
    if( eR < m_ecut || etf*eR < m_etcut) {
      if ( msgLevel() <= MSG::DEBUG )
        m_log << MSG::DEBUG << "At iteration: " << count << ", cluster transv. energy :" 
              << etR << ", below threshold :"   << m_etcut << endmsg;

      return 0.0; 
    }
    ++count;

  } while ( f > 0.005 && count < 10 );

  return eR * 1000.; //return in MeV


}

double T2SampCalibTool::c_febenergy(double EMe,double HADe,
                           double eta)
{

  //AssignWeights during the running
  //  if(m_readFromDb){assignWeights();}
  //  assignWeights();
// Internally we must work in GeV
// but the function arguments are specified in MeV
// and the returned value as well in MeV


  double EMenergy = EMe / 1000.;
  double HADenergy = HADe / 1000.;

  double etf = 1./( (exp(eta)+exp(-eta))/2. );

  double eR = EMenergy + HADenergy;
  double etR = etf * eR;

  if ( etR < m_etcut ) {
    if ( msgLevel() <= MSG::DEBUG )
      m_log << MSG::DEBUG << "Cluster transv. energy :" << etR << ", below threshold :" 
            << m_etcut << endmsg;
    return 0.0;
  }
  m_ieta = -1;
  for(int i=0; i < m_nbin_eta;++i) {
    if( fabs(eta) >= m_etareg[i] && fabs(eta) < m_etareg[i+1] ) {
      m_ieta = i;
      break;
    }
  } 
  m_nreg = 2;

  // Remove difference between FCal and other detector jets
  //if(m_ieta > 31) m_nreg = 1;

  m_ecut = m_vecut[m_ieta];


  double e2c = m_etmid / etf;

  double f = 1.0;
  int count = 0;
  m_ient = -1;
  double eC = 0;
  double CaloWeight[2] ;

  do
  {
    if ( (eR < e2c) && (fabs(eta)<1.4) ) m_ient = 0;
    if ( (eR >= e2c) && (fabs(eta)<1.4) ) m_ient = 1;
    if ( fabs(eta) > 1.4 ) m_ient = 2;

    get_weight( eR , CaloWeight ) ;

    eC = 0;
    if ( m_nreg == 2)
      eC += CaloWeight[0] * EMenergy + CaloWeight[1] * HADenergy;
    else
      eC += CaloWeight[0] * (EMenergy + HADenergy);


    f = fabs(eC - eR ) /eR;
    eR = eC;
    if( eR < m_ecut || etf*eR < m_etcut) {
      if ( msgLevel() <= MSG::DEBUG )
        m_log << MSG::DEBUG << "At iteration: " << count << ", cluster transv. energy :" 
              << etR << ", below threshold :"   << m_etcut << endmsg;

      return 0.0; 
    }
    ++count;

  } while ( f > 0.005 && count < 10 );

  return eR * 1000.; //return in MeV


}
void T2SampCalibTool::get_weight(double e, double CaloWeight[2])
{
  //AssignWeights during the running
  //if(m_readFromDb){assignWeights();}

  double w=1;

  for( int i=0; i< m_nreg;++i)
  {
    if (m_ient == 0) w = m_wt[m_ieta][i][0] + (m_wt[m_ieta][i][1])*log(e/m_ecut);
    if ( m_ient == 1 ) w = m_wt[m_ieta][i][2] + (m_wt[m_ieta][i][3])*log(e/m_ecut);
    if ( m_ient == 2 ) w = m_wt[m_ieta][i][2]; 
//    if (m_ient == 0) w = m_Weights[m_ieta*m_nSamp*m_nEReg + i*m_nEReg + 0] + 
//                        (m_Weights[m_ieta*m_nSamp*m_nEReg + i*m_nEReg + 1])*log(e/m_ecut);
//    if (m_ient == 1) w = m_Weights[m_ieta*m_nSamp*m_nEReg + i*m_nEReg + 2] + 
//                        (m_Weights[m_ieta*m_nSamp*m_nEReg + i*m_nEReg + 3])*log(e/m_ecut);

    if ( e < m_ecut ) w = 1.0;

    CaloWeight[i] = w;

  }

  

}

StatusCode T2SampCalibTool::readFromDb(){
  StatusCode status;

  m_log << MSG::DEBUG <<"T2SampCalibTool::Reading from dB..." <<endmsg;

  //Register DataHandle
  status = m_detStore->regHandle(m_lvl2_calib_handle, m_obj_key);
  if (status.isFailure()) {
    m_log << MSG::ERROR <<"Could not retrieve DataHandle for T2CaloJetCalib_dBObj" <<endmsg;
    return( StatusCode::FAILURE);
  } else {
    m_log << MSG::INFO << "Retrieved DataHandle for T2CaloJetCalib_dBObj " << endmsg;
  }
  return StatusCode::SUCCESS;
}

//Method used to create the calibration dB object
StatusCode T2SampCalibTool::createCondObject(){
  StatusCode sc;

  m_log << MSG::INFO <<"Creating T2SampCalibTool dB object" <<endmsg;
  
  /////////////////////////////////
  // Create T2CaloJetCalib_dBObj //
  /////////////////////////////////
  T2CaloJetCalib_dBObj* lvl2_calib = new T2CaloJetCalib_dBObj();
  //Fill obj
  m_log << MSG::INFO << "Setup T2CaloJetCalib_dBObj... " << endmsg;
  //Use info from job option to fill dB objects
  lvl2_calib->set_eta_ranges(m_etareg);
  lvl2_calib->set_ve_cuts(m_vecut);
  lvl2_calib->set_calib_weights(m_Weights);
  //Provide a key which is used as the name to create the folder
  sc = m_detStore->record(lvl2_calib, m_obj_key);
  if (sc.isFailure()) {
    m_log <<MSG::ERROR <<"Could not record T2CaloJetCalib_dBObj" <<endmsg;
    return( StatusCode::FAILURE);
  }
    return StatusCode::SUCCESS;
}

//Method used to print out dB info
StatusCode T2SampCalibTool::printCondObjects(){
  StatusCode sc;

  m_log << MSG::INFO << "Printing T2SampCalibTool dB Calibration Info." << endmsg;
  
  //Retrieve T2CaloJetCalib_dBObj
  sc = m_detStore->retrieve(m_lvl2_calib, m_obj_key);
  if (sc.isFailure()) {
    m_log << MSG::ERROR <<"Could not retrieve T2CaloJetCalib_dBObj" <<endmsg;
    return( StatusCode::FAILURE);
  } else {
    m_log << MSG::INFO << "Retrieved T2CaloJetCalib_dBObj " << endmsg;
  }
  if (m_lvl2_calib==0) {
    m_log <<MSG::ERROR <<"T2CaloJetCalib_dBObj ptr is 0" <<endmsg;
    return( StatusCode::FAILURE);
  }
  if(msgLevel() <= MSG::DEBUG){
    int eta_size=(m_lvl2_calib->eta_ranges().size());
    m_log << MSG::DEBUG << "T2SampCalibTool::Number of eta bins: " <<  eta_size << endmsg;
    for(int i=0;i<eta_size;++i){
      m_log << MSG::DEBUG << "T2SampCalibTool::EtaRange["<<i<<"]: " 
            << (m_lvl2_calib->eta_ranges()).at(i) 
            << endmsg;
    }
    int ve_size=(m_lvl2_calib->ve_cuts().size());
    m_log << MSG::DEBUG << "T2SampCalibTool::Number of ve bins: " <<  ve_size << endmsg;
    for(int i=0;i<ve_size;++i){
      m_log << MSG::DEBUG << "T2SampCalibTool::VECut["<<i<<"]: " 
            << (m_lvl2_calib->ve_cuts()).at(i) 
            << endmsg;
    }
    int weight_size=(m_lvl2_calib->calib_weights().size());
    m_log << MSG::DEBUG << "T2SampCalibTool::Number of ve bins: " <<  ve_size << endmsg;
    for(int i=0;i<weight_size;++i){
      m_log << MSG::DEBUG << "T2SampCalibTool::Weights["<<i<<"]: " 
            << (m_lvl2_calib->calib_weights()).at(i) 
            << endmsg;
    }
  }

  delete m_lvl2_calib;
  return StatusCode::SUCCESS;
}

StatusCode T2SampCalibTool::assignWeights(){

  ////////////////////////////
  // Print calibration data //
  ////////////////////////////
  if(m_readFromDb){
    if(msgLevel() <= MSG::INFO){
      int eta_size=(m_lvl2_calib_handle->eta_ranges().size());
      m_etareg.resize(eta_size,0);
      for(int i=0;i<eta_size;++i){
	m_etareg.at(i)=(m_lvl2_calib_handle->eta_ranges()).at(i);
	m_log <<  MSG::DEBUG << "T2SampCalibTool::EtaRanges["<<i<<"]: " 
              << (m_lvl2_calib_handle->eta_ranges()).at(i) 
              << endmsg;
      }
      int ve_size=(m_lvl2_calib_handle->ve_cuts().size());
      m_vecut.resize(ve_size,0);
      for(int i=0;i<ve_size;++i){
	m_vecut.at(i) = (m_lvl2_calib_handle->ve_cuts()).at(i); 
	m_log << MSG::DEBUG << "T2SampCalibTool::VECuts["<<i<<"]: " 
              << (m_lvl2_calib_handle->ve_cuts()).at(i) 
              << endmsg;
      }
      int weight_size=(m_lvl2_calib_handle->calib_weights().size());
      m_Weights.resize(weight_size,0);
      for(int i=0;i<weight_size;++i){
	m_Weights.at(i) = (m_lvl2_calib_handle->calib_weights()).at(i);
	m_log << MSG::DEBUG << "T2SampCalibTool::Weights["<<i<<"]: " 
              << (m_lvl2_calib_handle->calib_weights()).at(i) 
              << endmsg;
      }
    }
  }

  if(msgLevel() <= MSG::DEBUG)
    m_log << MSG::DEBUG <<  "T2SampCalib Weights: " << endmsg;
  
  for(int i=0; i<m_nbin_eta; ++i)
    {
      for(int j=0; j<m_nSamp; ++j)
	{
	  for(int k=0;k<m_nEReg; ++k)
	    {
	      m_wt[i][j][k]=m_Weights[i*m_nSamp*m_nEReg + j*m_nEReg + k];
	      if(msgLevel() <= MSG::DEBUG)
		m_log << MSG::DEBUG << "  m_wt["<<i<<"]["<<j<<"]["<<k<<"] =" << m_wt[i][j][k];
	    }
	  if(msgLevel() <= MSG::DEBUG)
	    m_log << MSG::DEBUG << endmsg;
	}
      if(msgLevel() <= MSG::DEBUG)
	m_log << MSG::DEBUG << endmsg;
    }
  return StatusCode::SUCCESS;
}
