/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     T2CaloJetCalibTool.cxx
PACKAGE:  Trigger/TrigAlgorithms/TrigT2CaloJet

AUTHORS:  Patricia Conde Muino <Patricia.Conde.Muino@cern.ch>
          Nuno Anjos <Nuno.Anjos@cern.ch>
CREATED:  May 2006

PURPOSE: Calibration tool for LVL2 jets.

********************************************************************/
#include "TrigT2CaloJet/T2CaloJetCalibTool.h"
#include "TrigCaloEvent/TrigT2Jet.h"
#include "CaloGeoHelpers/CaloSampling.h"
#include "AthenaKernel/Timeout.h"

#include "GaudiKernel/IToolSvc.h"

//-----------------------------------------------------------------
//  Constructor
//-----------------------------------------------------------------
T2CaloJetCalibTool::T2CaloJetCalibTool(const std::string& type,
				       const std::string& name,
				       const IInterface* parent):
  T2CaloJetBaseTool(type, name, parent),
  m_log(NULL),
  m_outputLevel(0),
  m_t2JetCalibTool ("T2JetCalibTool",this),
  m_t2JetCorrToolTile1  ("T2JetCorrToolTile1" ,this),
  m_t2JetCorrToolFcal1  ("T2JetCorrToolFcal1" ,this),
  m_t2JetCorrToolEm3Bar ("T2JetCorrToolEm3Bar",this),
  m_t2JetCorrToolHec0   ("T2JetCorrToolHec0"  ,this),
  m_t2JetCorrToolEm3Fwd ("T2JetCorrToolEm3Fwd",this),
  m_t2JetCorrToolPsBar  ("T2JetCorrToolPsBar" ,this)//,m_layer_info(0)
{  
  declareProperty("useFebJets",          m_useFEBJets      = false);
//declareProperty("fillLayerInfo",       m_fillLayerInfo   = false);
  declareProperty("doJetCalib",          m_doJetCalib      = false);
  declareProperty("doJetCorrTile1",      m_doJetCorrTile1  = false);
  declareProperty("doJetCorrFcal1",      m_doJetCorrFcal1  = false);
  declareProperty("doJetCorrEm3Bar",     m_doJetCorrEm3Bar = false);
  declareProperty("doJetCorrHec0",       m_doJetCorrHec0   = false);
  declareProperty("doJetCorrEm3Fwd",     m_doJetCorrEm3Fwd = false);
  declareProperty("doJetCorrPsBar",      m_doJetCorrPsBar  = false);
  declareProperty("T2JetCalibTool",      m_t2JetCalibTool, "Tool for L2 jet trigger Jet calibration");
  declareProperty("T2JetCorrToolTile1",  m_t2JetCorrToolTile1 , "Tool for L2 jet trigger Tile1 corr");
  declareProperty("T2JetCorrToolFcal1",  m_t2JetCorrToolFcal1 , "Tool for L2 jet trigger Fcal1 corr");
  declareProperty("T2JetCorrToolEm3Bar", m_t2JetCorrToolEm3Bar, "Tool for L2 jet trigger Em3Bar corr");
  declareProperty("T2JetCorrToolHec0",   m_t2JetCorrToolHec0  , "Tool for L2 jet trigger Hec0 corr");
  declareProperty("T2JetCorrToolEm3Fwd", m_t2JetCorrToolEm3Fwd, "Tool for L2 jet trigger Em3Fwd corr");
  declareProperty("T2JetCorrToolPsBar",  m_t2JetCorrToolPsBar , "Tool for L2 jet trigger PsBar corr");
}

//-----------------------------------------------------------------
//  Destructor
//-----------------------------------------------------------------
T2CaloJetCalibTool::~T2CaloJetCalibTool() 
{
}

//-----------------------------------------------------------------
//  Initialize:
//-----------------------------------------------------------------
StatusCode T2CaloJetCalibTool::initialize() 
{

  m_log = new MsgStream (msgSvc(), name());
  m_outputLevel = msgSvc()->outputLevel( name() );
  if(m_outputLevel <= MSG::DEBUG)
     (*m_log) << MSG::DEBUG << "  In initalize() " << endmsg;

  StatusCode sc;
  // Retrieve the jet calibration tool:
  ATH_CHECK( m_t2JetCalibTool.retrieve(      DisableTool {!m_doJetCalib}  ));
  
  // Retrieve the layer correction tools:
  ATH_CHECK( m_t2JetCorrToolTile1.retrieve(  DisableTool {!m_doJetCorrTile1} ));
  ATH_CHECK( m_t2JetCorrToolFcal1.retrieve(  DisableTool {!m_doJetCorrFcal1} ));
  ATH_CHECK( m_t2JetCorrToolEm3Bar.retrieve( DisableTool {!m_doJetCorrEm3Bar} ));
  ATH_CHECK( m_t2JetCorrToolHec0.retrieve(   DisableTool {!m_doJetCorrHec0} ));
  ATH_CHECK( m_t2JetCorrToolEm3Fwd.retrieve( DisableTool {!m_doJetCorrEm3Fwd} ));
  ATH_CHECK( m_t2JetCorrToolPsBar.retrieve(  DisableTool {!m_doJetCorrPsBar} ));

  // from IAlgToolCalo
  m_geometryTool.disable();
  m_data.disable();

//  if (m_fillLayerInfo) {
//    m_layer_info = new TrigCaloCluster(); 
//  }

  return StatusCode::SUCCESS;
}

//-----------------------------------------------------------------
//  Execute
//-----------------------------------------------------------------
StatusCode T2CaloJetCalibTool::execute()
{
  return StatusCode::SUCCESS;
}


//-----------------------------------------------------------------
//  Finalize:
//-----------------------------------------------------------------
StatusCode T2CaloJetCalibTool::finalize()
{
  delete m_log;

//  if (m_fillLayerInfo) {
//    delete m_layer_info;
//  }

  return StatusCode::SUCCESS;
}

//-----------------------------------------------------------------
//  Execute method of the tool. Apply energy correction to the jet.
//-----------------------------------------------------------------
StatusCode T2CaloJetCalibTool::execute(TrigT2Jet* jet, double /*etamin*/, double /*etamax*/, double /*phimin*/, double /*phimax*/)
{
  // reset error
  m_error = 0x0;


  // Calculate Hadronic and Electromagnetic transverse energies:
//  double HadEn = 0, EMEn = 0;
  double EMEn = jet->eem0();
  double HadEn = jet->ehad0();
  double jeta=jet->eta();
//  sc = calculateHadEMEnergy(jet, &HadEn, &EMEn);
//  if(sc.isFailure()){
//    (*m_log) << MSG::WARNING << "It seems that the Had/EM energy was not properly calculated!" << endmsg;
//  }
//
//  if(m_outputLevel <= MSG::DEBUG){
//    (*m_log) << MSG::DEBUG << " EM, Had energy components: " << EMEn << ", " 
//             << HadEn << endmsg; 
//  }

  // Apply the calibration:
  double corrEnergy=0;
  if(m_doJetCalib){
    if(!m_useFEBJets){
      corrEnergy = m_t2JetCalibTool->c_energy(EMEn, HadEn, jeta);
    } else {
      corrEnergy = m_t2JetCalibTool->c_febenergy(EMEn, HadEn, jeta);
    }

    if(corrEnergy == 0 || corrEnergy!=corrEnergy){//corrEnergy!=corrEnergy checks for nan
      if(m_outputLevel <= MSG::DEBUG){
         (*m_log) << MSG::DEBUG 
                  << " JES Calibration failed. Jet energy is left uncalibrated: "
                  << endmsg;
      }
      corrEnergy = EMEn+HadEn ;
    } 
  } else {
    if(m_outputLevel <= MSG::DEBUG){
         (*m_log) << MSG::DEBUG 
                  << " Calibration not applied. Jet energy is left uncalibrated: "
                  << endmsg;
    }
    corrEnergy = EMEn+HadEn ;
  } 

  // Compute layer fractions:
  double fTile1=0, fFcal1=0, fEm3=0, fHec0=0, fPs=0;
  if( m_doJetCorrTile1  || 
      m_doJetCorrHec0   || 
      m_doJetCorrEm3Fwd || 
      m_doJetCorrFcal1  || 
      m_doJetCorrEm3Bar || 
      m_doJetCorrPsBar//|| m_fillLayerInfo
    ) {
    if (calculateLayerFracs(jet, &fTile1, &fFcal1, &fEm3, &fHec0, &fPs).isFailure()){
      (*m_log) << MSG::WARNING << "Layer Fractions not properly calculated!" << endmsg;
    }
    if(m_outputLevel <= MSG::DEBUG){
      (*m_log) << MSG::DEBUG << " Uncorr Jet E = " <<  EMEn+HadEn
                             << " Tile1 fraction = " << fTile1 
                             << " Fcal1 fraction = " << fFcal1
                             << " Em3 fraction = " << fEm3 
                             << " Hec0 fraction = " << fHec0
                             << " Ps fraction = " << fPs   
                             << " Jet E before layer corr = " << corrEnergy << endmsg; 
    }
  }

  // Apply layer corrections:
  double response=1;
  if(m_doJetCorrTile1) {
    response = m_t2JetCorrToolTile1->c_energy(fTile1, corrEnergy, jeta);
    if(response != 0 && response != 1 && response==response) corrEnergy = corrEnergy/response;
    if(m_outputLevel <= MSG::DEBUG){
      (*m_log) << MSG::DEBUG << "Tile1 response = " << response 
                             << " corrected E = " << corrEnergy << endmsg; 
    }
  }
  if(m_doJetCorrHec0) {
    response = m_t2JetCorrToolHec0->c_energy(fHec0, corrEnergy, jeta);
    if(response != 0 && response != 1 && response==response) corrEnergy = corrEnergy/response;
    if(m_outputLevel <= MSG::DEBUG){
      (*m_log) << MSG::DEBUG << "Hec0 response = " << response 
                             << " corrected E = " << corrEnergy << endmsg; 
    }
  }
  if(m_doJetCorrEm3Fwd) {
    response = m_t2JetCorrToolEm3Fwd->c_energy(fEm3, corrEnergy, jeta);
    if(response != 0 && response != 1 && response==response) corrEnergy = corrEnergy/response;
    if(m_outputLevel <= MSG::DEBUG){
      (*m_log) << MSG::DEBUG << "Em3Fwd response = " << response 
                             << " corrected E = " << corrEnergy << endmsg; 
    }
  }
  if(m_doJetCorrFcal1) {
    response = m_t2JetCorrToolFcal1->c_energy(fFcal1, corrEnergy, jeta);
    if(response != 0 && response != 1 && response==response) corrEnergy = corrEnergy/response;
    if(m_outputLevel <= MSG::DEBUG){
      (*m_log) << MSG::DEBUG << "Fcal1 response = " << response 
                             << " corrected E = " << corrEnergy << endmsg; 
    }
  }
  if(m_doJetCorrEm3Bar) {
    response = m_t2JetCorrToolEm3Bar->c_energy(fEm3, corrEnergy, jeta);
    if(response != 0 && response != 1 && response==response) corrEnergy = corrEnergy/response;
    if(m_outputLevel <= MSG::DEBUG){
      (*m_log) << MSG::DEBUG << "Em3Bar response = " << response 
                             << " corrected E = " << corrEnergy << endmsg; 
    }
  }
  if(m_doJetCorrPsBar) {
    response = m_t2JetCorrToolPsBar->c_energy(fPs, corrEnergy, jeta);
    if(response != 0 && response != 1 && response==response) corrEnergy = corrEnergy/response;
    if(m_outputLevel <= MSG::DEBUG){
      (*m_log) << MSG::DEBUG << "PsBar response = " << response 
                             << " corrected E = " << corrEnergy << endmsg; 
    }
  }


  // Set jet energy:
  jet->setE(corrEnergy);

  if(m_outputLevel <= MSG::DEBUG){
    (*m_log) << MSG::DEBUG << " Uncorrected E = " << EMEn+HadEn << ", Fully corrected E = " 
             << corrEnergy << ", eta = " << jet->eta() << endmsg; 
  }

  if ( Athena::Timeout::instance().reached() ) {
       (*m_log) << MSG::ERROR << "Timeout reached in T2CaloJetCalibTool" << endmsg;
       return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

//---------------------------------------------------------------
//  Calculate layer fractions of the jet energy:
//---------------------------------------------------------------
StatusCode T2CaloJetCalibTool::calculateLayerFracs(TrigT2Jet* jet, double *fTile1, double *fFcal1, double *fEm3, double *fHec0, double *fPs)
{

   // Loop in the jet cells:

  std::vector<Trig3Momentum>::iterator gridIt = jet->grid()->begin();
  std::vector<Trig3Momentum>::iterator gridEnd = jet->grid()->end();

  double eTile1=0, eFcal1=0, eEm3=0, eHec0=0, ePs= 0;
  int nCells = 0;

  for (; gridIt != gridEnd; ++gridIt){

     // consider only cells that are used in the cone:
    if(! (*gridIt).inCone()) continue;
    nCells++;

    const CaloSampling::CaloSample s = (*gridIt).caloSample();
    double cellE = (*gridIt).e();  

//    if (m_fillLayerInfo) {
//      m_layer_info->setRawEnergy(s,cellE);
//    }

    switch ( s ) {
      case CaloSampling::PreSamplerB:
      case CaloSampling::PreSamplerE:
         ePs += cellE;
         break;
      case CaloSampling::EMB3:
      case CaloSampling::EME3:
         eEm3 += cellE;
         break;
      case CaloSampling::FCAL0:
         eFcal1 += cellE;
         break;
      case CaloSampling::HEC0:
         eHec0 += cellE;
         break;
      case CaloSampling::TileBar0:
      case CaloSampling::TileExt0:
         eTile1 += cellE;
         break;
      default:
         /* do nothing */
         break;

      }// end switch - Jet Sampling Layers


  } // End of loop in cells;

//  if (m_fillLayerInfo) {
//    m_layer_info->setRawEnergy(jet->eem0()+jet->ehad0());
//    m_layer_info->setRawEt((jet->eem0()+jet->ehad0())/cosh(jet->eta()));
//    m_layer_info->setRawEta(jet->eta());
//    m_layer_info->setRawPhi(jet->phi());
//    m_layer_info->setRoIword(jet->RoIword());
//    m_layer_info->setNCells(nCells);
//  }

  if ((jet->eem0()+jet->ehad0())!=0){
    (*fTile1) = eTile1/(jet->eem0()+jet->ehad0());
    (*fFcal1) = eFcal1/(jet->eem0()+jet->ehad0());
    (*fEm3)   = eEm3  /(jet->eem0()+jet->ehad0());
    (*fHec0)  = eHec0 /(jet->eem0()+jet->ehad0());
    (*fPs)    = ePs   /(jet->eem0()+jet->ehad0());
  } else {
    (*fTile1) = 0;
    (*fFcal1) = 0;
    (*fEm3)   = 0;
    (*fHec0)  = 0;
    (*fPs)    = 0;
  }

  return StatusCode::SUCCESS;
}

//---------------------------------------------------------------
//  Calculate hadronic/Electromagnetic component of the jet energy:
//---------------------------------------------------------------
/*StatusCode T2CaloJetCalibTool::calculateHadEMEnergy(TrigT2Jet* jet, double *HadEnergy, double * EMEnergy)
{

   // Loop in the jet cells:

  std::vector<Trig3Momentum>::iterator gridIt = jet->grid()->begin();
  std::vector<Trig3Momentum>::iterator gridEnd = jet->grid()->end();

  (*HadEnergy) = 0;
  (*EMEnergy) = 0;
  for (; gridIt != gridEnd; ++gridIt){

     // consider only cells that are used in the cone:
    if(! (*gridIt).inCone()) continue;

    const CaloSampling::CaloSample s = (*gridIt).caloSample();
    double cellE = (*gridIt).e();  

    switch ( s ) {
      case CaloSampling::PreSamplerB:
      case CaloSampling::PreSamplerE:
      case CaloSampling::EMB1:
      case CaloSampling::EMB2:
      case CaloSampling::EMB3:
      case CaloSampling::EME1:
      case CaloSampling::EME2:
      case CaloSampling::EME3:
         (*EMEnergy) += cellE;
         break;
      case CaloSampling::HEC0:
      case CaloSampling::HEC1:
      case CaloSampling::HEC2:
      case CaloSampling::HEC3:
      case CaloSampling::TileBar0:
      case CaloSampling::TileBar1:
      case CaloSampling::TileBar2:
      case CaloSampling::TileExt0:
      case CaloSampling::TileExt1:
      case CaloSampling::TileExt2:
      case CaloSampling::TileGap1:
      case CaloSampling::TileGap2:
      case CaloSampling::TileGap3:
      case CaloSampling::FCAL0:
      case CaloSampling::FCAL1:
      case CaloSampling::FCAL2:
         (*HadEnergy) += cellE;
         break;
      case CaloSampling::Unknown:
        (*m_log) << MSG::WARNING
             << "Unknown CaloCell type "
             << s
             << endmsg;
        break;

      }// end switch - Jet Sampling Layers


  } // End of loop in cells;


   // Cross check that everything went fine:
//
//  double eSum = (*HadEnergy)+(*EMEnergy);
//  if( eSum != jet->e()){ 
//
//    (*m_log) << MSG::WARNING << " HadEnergy+EMEnergy != jet->e() !!! HadEnergy, EMEnergy, eSum, jetEnergy =" 
//             << (*HadEnergy) << ", " << (*EMEnergy) << ", " << eSum << ", " << jet->e() << endmsg;
//    std::cout << eSum << ", " << jet->e() << std::endl;
//    return StatusCode::FAILURE;
//  }
//

  return StatusCode::SUCCESS;
}
*/
