/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//#####################################################
//#                                                   #
//# File   : TBBeamQualityEMFractionTool.cxx          #
//# Package: TBRec                                    #
//#                                                   #
//# Author : Marco Bieri <mbieri@sfu.ca>              #
//#                                                   #
//#                                                   #
//#####################################################

#include "TBBeamQualityEMFractionTool.h"

#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloEvent/CaloCellContainer.h"

#include "Identifier/Identifier.h"

TBBeamQualityEMFractionTool::TBBeamQualityEMFractionTool(const std::string& name,
							 const std::string& type,
							 const IInterface* parent)
  : TBBeamQualityTool(name,type,parent),
    m_StoreGate(nullptr),
    m_detStore(nullptr),
    m_emecID_help(nullptr),
    m_hecID_help(nullptr),
    m_fcalID_help(nullptr),
    m_em_frac(0)
{  
  declareInterface<TBBeamQualityTool>(this);
  declareProperty("EM_fraccut",m_em_fraccut=0.2);
  declareProperty("EM_fracnumsamp",m_em_fracnumsamp);
  declareProperty("EM_fracdensamp",m_em_fracdensamp);
  declareProperty("EM_fracnumdet",m_em_fracnumdet="none");
  declareProperty("EM_fracdendet",m_em_fracdendet="none");
}

TBBeamQualityEMFractionTool::~TBBeamQualityEMFractionTool()
{ }

StatusCode TBBeamQualityEMFractionTool::initializeTool()
{ //init
  MsgStream log(msgSvc(),name());
  log << MSG::DEBUG
      << "initialize"
      << endmsg;
  
  StatusCode sc;
  
  // accessing Store Gate
  sc = service( "StoreGateSvc", m_StoreGate);
  if( sc.isFailure() ){
    log << MSG::FATAL << "Unable to locate the StoreGateSvc Service" <<endmsg;
    return sc;
  }
  
  sc = service("DetectorStore", m_detStore);
  if (sc.isFailure())
    {
      log << MSG::FATAL << " Cannot locate DetectorStore " << std::endl;
      return StatusCode::FAILURE;
    }
  
  m_hecID_help = NULL;
  m_emecID_help = NULL;
  m_fcalID_help = NULL;
  
  /*
  // retrieve detector description manager for LAr subsystem
  const DataHandle<LArDetDescrManager> m_larMgr;
  sc = m_detStore->retrieve(m_larMgr);
  if (sc.isFailure()) {
    log << MSG::ERROR << "unable to retrieve LArDetDescrManager from detector store"<< endmsg;
    return sc;
  }
  
  //obtainine Identifier helpers
  m_hecID_help = m_larMgr->get_hec_id();
  if (!m_hecID_help) {
    log << MSG::ERROR << "unable to obtain hec id " << endmsg;
    return StatusCode::FAILURE;
  }
  m_emecID_help = m_larMgr->get_em_id();
  if (!m_emecID_help) {
    log << MSG::ERROR << "unable to obtain emec id " << endmsg;
    return StatusCode::FAILURE;
  }
  
  m_fcalID_help = m_larMgr->get_fcal_id();
  if (!m_fcalID_help) {
    log << MSG::ERROR << "unable to obtain fcal id " << endmsg;
    return StatusCode::FAILURE;
  }
  */
  
  
  // retrieve detector description manager for LAr subsystem
  const CaloCell_ID* idHelper = nullptr;
  ATH_CHECK( detStore()->retrieve (idHelper, "CaloCell_ID") );
  
  //obtainine Identifier helpers
  m_hecID_help = idHelper->hec_idHelper();
  if (!m_hecID_help) {
    log << MSG::ERROR << "unable to obtain hec id " << endmsg;
    return StatusCode::FAILURE;
  }
  m_emecID_help = idHelper->em_idHelper();
  if (!m_emecID_help) {
    log << MSG::ERROR << "unable to obtain emec id " << endmsg;
    return StatusCode::FAILURE;
  }
  
  m_fcalID_help = idHelper->fcal_idHelper();
  if (!m_fcalID_help) {
    log << MSG::ERROR << "unable to obtain fcal id " << endmsg;
    return StatusCode::FAILURE;
  }
  
  
  
  log<<MSG::DEBUG<<"Got to before p_toolSvc"<<endmsg;
  
  if (m_em_fracnumdet=="HEC") {
    m_em_fracnumsamp.push_back(8);
    m_em_fracnumsamp.push_back(9);
    m_em_fracnumsamp.push_back(10);
  }
  if (m_em_fracdendet=="HEC") {
    m_em_fracdensamp.push_back(8);
    m_em_fracdensamp.push_back(9);
    m_em_fracdensamp.push_back(10);
  }
  if (m_em_fracnumdet=="EMEC") {
    m_em_fracnumsamp.push_back(6);
    m_em_fracnumsamp.push_back(7);
  }
  if (m_em_fracdendet=="EMEC") {
    m_em_fracdensamp.push_back(6);
    m_em_fracdensamp.push_back(7);
  }
  if (m_em_fracnumdet=="FCAL") {
    m_em_fracnumsamp.push_back(21);
    m_em_fracnumsamp.push_back(22);
    m_em_fracnumsamp.push_back(23);
  }
  if (m_em_fracdendet=="FCAL") {
    m_em_fracdensamp.push_back(21);
    m_em_fracdensamp.push_back(22);
    m_em_fracdensamp.push_back(23);
  }
  
  return StatusCode::SUCCESS;
}//init

StatusCode TBBeamQualityEMFractionTool::accept(std::vector<std::string> m_particles)
{//accept
  
  MsgStream log(msgSvc(),name());
  
  log << MSG::DEBUG
      << "TBBeamQualityEMFractionTool: Started Accept"
      << endmsg;
  
  StatusCode sc;
  
  log << MSG::DEBUG << "m_em_fracnumdet: "<<m_em_fracnumdet<<endmsg;
  log << MSG::DEBUG << "m_em_fracdendet: "<<m_em_fracdendet<<endmsg;
  log << MSG::DEBUG << " called for:  "<<m_particles.size()<<" particles"<<endmsg;
  
  // setting up layers (h6)
  
  // setting tmp variables
  // resetting the numerator and denominato to 0
  
  //long chan =0;
  float numenergy=0;
  float denenergy=0;
  for (int i=0;i<500;++i) {
    m_Mlayer[i]=0;
  }  
  
  //Accesing the CaloCellContainer
  const CaloCellContainer* cellContainer = nullptr;
  
  sc = m_StoreGate->retrieve(cellContainer);
  if (sc.isFailure()) {
    log << MSG::ERROR << "couldn't get the calo cells from storegate" << endmsg;
    log << MSG::ERROR << "here is what is in storegate: " << m_StoreGate->dump() << endmsg;
    return StatusCode::FAILURE;
  }
  
  CaloCellContainer::const_iterator ifirst = cellContainer->begin();
  CaloCellContainer::const_iterator ilast = cellContainer->end();
  
  for (; ifirst!=ilast; ifirst++){//iterating
    
    //obtaining cell pointer
    const CaloCell* cell_ptr = *ifirst;
    
    //obtaining dde pointer
    const CaloDetDescrElement* caloDDE_ptr = cell_ptr->caloDDE();
    
    //obtaining SubCalo
    //const CaloCell_ID::SUBCALO subcalo = caloDDE_ptr->getSubCalo();
    
    //obtaining sampling (layer)
    const CaloCell_ID::CaloSample sampling = caloDDE_ptr->getSampling();
    
    // This map will have the layers as an index and increment the energy
    // Does this for ALL layers ( all detectors )
    
    m_Mlayer[sampling]+=cell_ptr->energy();
    
  } //iterating
  
  // picking out layers of interest and filling numerator and denominator energies.
  
  for (unsigned int i=0;i<m_em_fracnumsamp.size();++i) {
    numenergy+=m_Mlayer.find(m_em_fracnumsamp[i])->second;
  }  
  
  for (unsigned int i=0;i<m_em_fracdensamp.size();++i) {
    denenergy+=m_Mlayer.find(m_em_fracdensamp[i])->second;
  } 
  
  if (denenergy!=0) {
    m_em_frac=numenergy/denenergy;
    
    if (m_em_frac<m_em_fraccut) {
      return StatusCode::SUCCESS;
    }
    else {
      return StatusCode::FAILURE;  
    }
  }
  else {
    return StatusCode::FAILURE;
  }
}//accept
