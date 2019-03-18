/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TrigEFCaloHypo.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigEFCaloHypo
//
// AUTHOR:   P. Conde Muino
//           Performs the Hypothesis cuts on the Event Filter
//           Calorimeter variables. Create a photon-like object to pass to SelectorTools.
//
// MAINTAINED BY: R. M. White
// MODIFIED: Oct 2014, Update to xAOD addition of egamma Tools
// ********************************************************************
//
#include <list>
#include <iterator>
#include <sstream>
//
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ListItem.h"
#include "GaudiKernel/EventContext.h"
//
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrigEgammaHypo/TrigEFCaloHypo.h"
//

#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODEgamma/PhotonContainer.h"
//#include "xAODEgammaCnv/xAODPhotonMonFuncs.h" // Cannot use macros in more than one place
#include "CaloEvent/CaloCellContainer.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "xAODTrigger/TrigPassBits.h"
#include "PATCore/AcceptData.h"

class ISvcLocator;

/////////////////////////////////////////////////////////////////////
// CONSTRUCTOR:
/////////////////////////////////////////////////////////////////////
//
TrigEFCaloHypo::TrigEFCaloHypo(const std::string& name, ISvcLocator* pSvcLocator):
    HLT::HypoAlgo(name, pSvcLocator),
    m_lumiBlockMuTool("LumiBlockMuTool/LumiBlockMuTool")
{

  declareProperty("AcceptAll",      m_acceptAll=true);
  declareProperty("emEt",   m_emEt = -3.*CLHEP::GeV); 
  //isEM offline
  declareProperty("ApplyIsEM",m_applyIsEM = false);
  declareProperty("ApplyPhotonIsEM",m_applyPhotonIsEM = false);
  declareProperty("ApplyLH",m_applyLH = false);
  declareProperty("IsEMrequiredBits",m_IsEMrequiredBits = 0xF2);

  declareProperty("ShowerBuilderTool", m_showerBuilder, "Handle of instance of EMShowerBuilder");
  declareProperty("FourMomBuilderTool", m_fourMomBuilder, "Handle of instance of EMFourBuilder");
  declareProperty("SelectorToolName", m_SelectorToolName, "Name for selector tool");
  declareProperty("PhotonSelectorToolName", m_PhSelectorToolName, "Name for selector tool");
  declareProperty("LHSelectorToolName", m_LHSelectorToolName, "Name for LH selector tool");

  /** Luminosity tool */
  declareProperty("LuminosityTool", m_lumiBlockMuTool, "Luminosity Tool");
  
  //Monitor collections
  declareMonitoredStdContainer("EnergyBE0",m_EBE0);
  declareMonitoredStdContainer("EnergyBE1",m_EBE1);
  declareMonitoredStdContainer("EnergyBE2",m_EBE2);
  declareMonitoredStdContainer("EnergyBE3",m_EBE3);
  declareMonitoredStdContainer("Eta",m_Eta);
  declareMonitoredStdContainer("Phi",m_Phi);
  declareMonitoredStdContainer("EtaCalo",m_EtaCalo);
  declareMonitoredStdContainer("PhiCalo",m_PhiCalo);
  declareMonitoredStdContainer("E",m_E);
  declareMonitoredStdContainer("ECalib",m_ECalib);
  declareMonitoredStdContainer("ERes",m_ERes);
  declareMonitoredStdContainer("mu",m_avgmu);
  declareMonitoredStdContainer("LikelihoodRatio",m_lhval);

  //Initialize pointers
  m_totalTimer = nullptr;
}

/////////////////////////////////////////////////////////////////////
// DESTRUCTOR:
/////////////////////////////////////////////////////////////////////
//
TrigEFCaloHypo::~TrigEFCaloHypo()
{  }

/////////////////////////////////////////////////////////////////////
// INITIALIZE:
// The initialize method will create all the required algorithm objects
// Note that it is NOT NECESSARY to run the initialize of individual
// sub-algorithms.  The framework takes care of it.
/////////////////////////////////////////////////////////////////////
//

// ----------------------------------------------------------------------
HLT::ErrorCode TrigEFCaloHypo::hltInitialize()
// ----------------------------------------------------------------------
{
    ATH_MSG_DEBUG("in initialize()");

  // Initialize timing service
  //------------------------------
  
  if (timerSvc()){
   m_totalTimer  = addTimer("TrigEFCaloHypoTot");
  } 

  if (m_SelectorToolName=="") {
    ATH_MSG_DEBUG("Electron IsEM PID is disabled, no tool specified "); 
    m_SelectorTool=ToolHandle<IAsgElectronIsEMSelector>();
  } 
  else {
      m_SelectorTool=ToolHandle<IAsgElectronIsEMSelector>(m_SelectorToolName);
      if(m_SelectorTool.retrieve().isFailure()) {
          ATH_MSG_ERROR("Unable to retrieve " << m_SelectorTool  << " tool ");
          return HLT::BAD_JOB_SETUP; 
      } 
      else ATH_MSG_DEBUG("Tool " << m_SelectorTool << " retrieved");
  }
 
  if (m_PhSelectorToolName=="") {
    ATH_MSG_DEBUG("Photon IsEM PID is disabled, no tool specified "); 
    m_PhSelectorTool=ToolHandle<IAsgPhotonIsEMSelector>();
  } 
  else {
      m_PhSelectorTool=ToolHandle<IAsgPhotonIsEMSelector>(m_PhSelectorToolName);
      if(m_PhSelectorTool.retrieve().isFailure()) {
          ATH_MSG_ERROR("Unable to retrieve " << m_PhSelectorTool  << " tool ");
          return HLT::BAD_JOB_SETUP; 
      } 
      else ATH_MSG_DEBUG("Tool " << m_PhSelectorTool << " retrieved");
  }
 
  if (m_LHSelectorToolName=="") {
    ATH_MSG_DEBUG("Electron LH PID is disabled, no tool specified "); 
    m_LHSelectorTool=ToolHandle<IAsgElectronLikelihoodTool>();
  } 
  else {
      m_LHSelectorTool=ToolHandle<IAsgElectronLikelihoodTool>(m_LHSelectorToolName);
      if(m_LHSelectorTool.retrieve().isFailure()) {
          ATH_MSG_ERROR("Unable to retrieve " << m_LHSelectorTool  << " tool ");
          return HLT::BAD_JOB_SETUP; 
      } 
      else ATH_MSG_DEBUG("Tool " << m_LHSelectorTool << " retrieved");
  }
  

  if (m_showerBuilder.empty()) {
      ATH_MSG_INFO("ShowerBuilder is empty");
      return HLT::BAD_JOB_SETUP;
  }
  if(m_showerBuilder.retrieve().isFailure()) {
      ATH_MSG_ERROR("Unable to retrieve "<<m_showerBuilder);
      return HLT::BAD_JOB_SETUP; 
  }
  else ATH_MSG_DEBUG("Retrieved Tool " << m_showerBuilder);
  
  if (m_fourMomBuilder.empty()) {
      ATH_MSG_INFO("FourMomBuilder is empty");
      return HLT::BAD_JOB_SETUP;
  }
  if(m_fourMomBuilder.retrieve().isFailure()) {
      ATH_MSG_ERROR("Unable to retrieve "<<m_fourMomBuilder);
      return HLT::BAD_JOB_SETUP; 
  }
  else ATH_MSG_DEBUG("Retrieved Tool " << m_fourMomBuilder);
  
  // For now, just try to retrieve the lumi tool
  if (m_lumiBlockMuTool.retrieve().isFailure()) {
      ATH_MSG_FATAL("Unable to retrieve Luminosity Tool");
      return HLT::ERROR;
  } else {
      ATH_MSG_DEBUG("Successfully retrieved Luminosity Tool");
  }
    
  ATH_MSG_DEBUG("IsEM " << m_IsEMrequiredBits);
  ATH_MSG_DEBUG("ApplyIsEM " << m_applyIsEM);
  ATH_MSG_DEBUG("ApplyLH " << m_applyLH);
  ATH_MSG_DEBUG("EtCut = " << m_emEt);
  ATH_MSG_DEBUG("AcceptAll " << m_acceptAll);
  
  ATH_MSG_DEBUG("Initialization of TrigEFCaloHypo completed successfully");

  return HLT::OK;
}




// ----------------------------------------------------------------------
HLT::ErrorCode TrigEFCaloHypo::hltFinalize(){
// ----------------------------------------------------------------------
  ATH_MSG_INFO("in finalize()");
  return HLT::OK;
}


// ----------------------------------------------------------------------
HLT::ErrorCode TrigEFCaloHypo::hltExecute(const HLT::TriggerElement* outputTE,
					    bool& pass){
    // ----------------------------------------------------------------------
    const EventContext ctx = Gaudi::Hive::currentContext(); 

    // Time total TrigEFCaloHypo execution time.
  if (timerSvc()) m_totalTimer->start();    

  //clear the monitoring vectors
  m_EBE0.clear();
  m_EBE1.clear();
  m_EBE2.clear();
  m_EBE3.clear();
  m_Eta.clear();
  m_Phi.clear();
  m_EtaCalo.clear();
  m_PhiCalo.clear();
  m_E.clear();
  m_ECalib.clear();
  m_ERes.clear();
  m_lhval.clear();
  m_avgmu.clear();

  ATH_MSG_DEBUG(": in execute()");
 
  if(m_applyIsEM)
      ATH_MSG_DEBUG("Apply Electron isEM PID with tool " << m_SelectorTool);

  else if(m_applyPhotonIsEM)
      ATH_MSG_DEBUG("Apply Photon isEM PID with tool " << m_PhSelectorTool);

  else if(m_applyLH)
      ATH_MSG_DEBUG("Apply Electron LH PID with tool " << m_LHSelectorTool);
      
  // get CaloClusterContainer from the trigger element:
  //--------------------------------------------------

  bool accepted=false;
  // AcceptAll property = true means selection cuts should not be applied
  if (m_acceptAll) accepted=true;

  std::vector<const xAOD::CaloClusterContainer*> vectorClusterContainer; 
  HLT::ErrorCode stat = getFeatures(outputTE,vectorClusterContainer , "");

  if (stat != HLT::OK ) {
     ATH_MSG_WARNING( " Failed to get vectorClusterContainers from the trigger element"); 
     if (timerSvc())m_totalTimer ->stop();
     return HLT::OK;
  } 

  ATH_MSG_DEBUG(" Got " << vectorClusterContainer.size()  << " vectorClusterContainer's associated to the TE ");
  // Get the last ClusterContainer
  const xAOD::CaloClusterContainer* clusContainer = vectorClusterContainer.back();
  if(!clusContainer){
      if ( msgLvl() <= MSG::ERROR )
          msg() << MSG::ERROR
              << " REGTEST: Retrieval of CaloClusterContainer from vector failed"
              << endmsg;
      //return HLT::BAD_JOB_SETUP;
      return HLT::OK;
  }

  if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG
      << clusContainer->size() << " calo clusters in container" << endmsg;

  if(clusContainer->size() < 1){
      return HLT::OK;
  }
  
  // All this only to retrieve the key :
  std::string clusCollKey;
  if ( getStoreGateKey( clusContainer, clusCollKey) != HLT::OK) {
      ATH_MSG_ERROR("Failed to get key for CaloClusterContainer");
  }
  else ATH_MSG_DEBUG("REGTEST:: Retrieved ClusterContainer with key " << clusCollKey);
  
  // Shower Shape & CaloCellContainer
  const CaloCellContainer* pCaloCellContainer = 0;

  // Get vector of pointers to all CaloCellContainers from TE
  std::vector<const CaloCellContainer*> vectorCellContainer;

  stat = getFeatures(outputTE, vectorCellContainer);
  if ( stat != HLT::OK ) {
      ATH_MSG_ERROR("REGTEST: No CaloCellContainers retrieved for the trigger element");
      return HLT::OK;
  } else{
      ATH_MSG_DEBUG(" REGTEST: Got " << vectorCellContainer.size() << " CaloCellContainers associated to the TE ");
      // Check that there is only one CellContainer in the RoI
      if (vectorCellContainer.size() != 1){
          ATH_MSG_ERROR("REGTEST: Size of calo cell container vector is not 1 but " << vectorCellContainer.size());
          return HLT::ERROR;
      } else{
          // Get the last CellContainer if ShowerShapeTool is going to be run
          pCaloCellContainer = vectorCellContainer.back();
          if(!pCaloCellContainer){
              ATH_MSG_ERROR("Retrieval of CaloCellContainer from vector failed");
              return HLT::ERROR;
          } 
      }
  }

  // Create pass bits for clusters
  std::unique_ptr<xAOD::TrigPassBits> xBits = xAOD::makeTrigPassBits<xAOD::CaloClusterContainer>(clusContainer);
  double mu = 0.;
  double avg_mu = 0.;
  bool useLumiTool=false;
  if(m_lumiBlockMuTool){
      useLumiTool=true;
      mu = m_lumiBlockMuTool->actualInteractionsPerCrossing(); // (retrieve mu for the current BCID)
      avg_mu = m_lumiBlockMuTool->averageInteractionsPerCrossing();
      ATH_MSG_DEBUG("REGTEST: Retrieved Mu Value : " << mu);
      ATH_MSG_DEBUG("REGTEST: Average Mu Value   : " << avg_mu);
      m_avgmu.push_back(avg_mu);
  }
  
  unsigned int iclus=0;
  for(const auto clus : *clusContainer){
      if(m_acceptAll){
          xBits->markPassing(clus,clusContainer,true);
          continue;
      }
      unsigned int isEMTrig = 0;
      bool isLHAcceptTrig = false;
      float lhval=0.;
        
      // Now creating the photon objects (dummy EDM for Calo selection at EF)
      const ElementLink<xAOD::CaloClusterContainer> clusterLink(*clusContainer,iclus);
      std::vector< ElementLink<xAOD::CaloClusterContainer> > clLinks;
      clLinks.push_back(clusterLink);
      xAOD::Photon eg;
      eg.makePrivateStore();
      eg.setCaloClusterLinks(clLinks);

      // Without this, AsgElectronLikelihoodTool::accept will throw
      // an exception due to the missing xAOD variable.
      eg.setAuthor (xAOD::EgammaParameters::AuthorUnknown);

      // Run the tools for each object
      ATH_MSG_DEBUG("REGTEST: Run the tools on eg object");
      if(m_fourMomBuilder->hltExecute(&eg));
      else ATH_MSG_DEBUG("Problem with FourMomBuilder");
      if(m_showerBuilder->recoExecute(&eg,pCaloCellContainer));
      else ATH_MSG_DEBUG("Problem with ShowerBuilder");
      ATH_MSG_DEBUG("REGTEST: cluster e " << clus->e());
      ATH_MSG_DEBUG("REGTEST: e " << eg.e());
      ATH_MSG_DEBUG("REGTEST: eg cluster e " << eg.caloCluster()->e());
      ATH_MSG_DEBUG("REGTEST: eta " << eg.eta());
      ATH_MSG_DEBUG("REGTEST: phi " << eg.phi());
      // Increment counter for next cluster before selection
      iclus++;
      
      // Apply selection
      if(m_applyIsEM){
          ATH_MSG_DEBUG("REGTEST: Check Object, eta2 = " << fabsf(eg.caloCluster()->etaBE(2)) << " e = " << eg.caloCluster()->e());
          unsigned int isEM = 0;
          if(m_SelectorTool->execute(ctx, &eg, isEM).isFailure())
              ATH_MSG_DEBUG("REGTEST:: Problem in isEM Selector");
          else isEMTrig = isEM;
      }
      else if(m_applyPhotonIsEM){
          ATH_MSG_DEBUG("REGTEST: Check Object, eta2 = " << fabsf(eg.caloCluster()->etaBE(2)) << " e = " << eg.caloCluster()->e());
          unsigned int isEM = 0;
          if(m_PhSelectorTool->execute(ctx, &eg, isEM).isFailure())
              ATH_MSG_DEBUG("REGTEST:: Problem in isEM Selector");
          else isEMTrig = isEM;
      }
      else if(m_applyLH){
          if(useLumiTool){
              asg::AcceptData acc = m_LHSelectorTool->accept(ctx, &eg, avg_mu);
              lhval = m_LHSelectorTool->calculate(ctx, &eg);
              ATH_MSG_DEBUG("LHValue with mu " << lhval);
              m_lhval.push_back(lhval);
              isLHAcceptTrig = (bool) (acc);
          }
          else {
              ATH_MSG_DEBUG("Lumi tool returns mu = 0, do not pass mu");
              asg::AcceptData lhacc = m_LHSelectorTool->accept(ctx, &eg); // use method for calo-only
              lhval = m_LHSelectorTool->calculate(ctx, &eg);
              ATH_MSG_DEBUG("LHValue without mu " << lhval);
              m_lhval.push_back(lhval);
              isLHAcceptTrig = (bool) (lhacc);
          }

          ATH_MSG_DEBUG("REGTEST: Applying LH pid selection " << isLHAcceptTrig);
      }
      // Monitoring
      m_EBE0.push_back(clus->energyBE(0));
      m_EBE1.push_back(clus->energyBE(1));
      m_EBE2.push_back(clus->energyBE(2));
      m_EBE3.push_back(clus->energyBE(3));
      m_Eta.push_back(clus->eta());
      m_Phi.push_back(clus->phi());
      double tmpeta = -999.;
      clus->retrieveMoment(xAOD::CaloCluster::ETACALOFRAME,tmpeta);
      double tmpphi = -999.;
      clus->retrieveMoment(xAOD::CaloCluster::ETACALOFRAME,tmpphi);
      m_EtaCalo.push_back(tmpeta);
      m_PhiCalo.push_back(tmpphi);
      m_ECalib.push_back(clus->e());
      const xAOD::CaloCluster *origClus=nullptr;
      static const SG::AuxElement::Accessor<ElementLink<xAOD::CaloClusterContainer> > orig ("originalCaloCluster");
      if (!orig.isAvailable(*clus) || !orig(*clus).isValid()){
          ATH_MSG_DEBUG("Problem with original cluster link");
      }
      else {
          origClus = *orig(*clus); 
      }
      if(origClus != NULL){
          m_E.push_back(origClus->e());
          m_ERes.push_back(clus->e()-origClus->e());
      }
     
      if(m_applyIsEM || m_applyPhotonIsEM){
          if( (isEMTrig & m_IsEMrequiredBits)!=0 ) {
              ATH_MSG_DEBUG("REGTEST IsEM = " << std::hex << isEMTrig 
                      << " cut not satisfied for pattern:" << std::hex << m_IsEMrequiredBits);
              continue;
          }
      }
      if(m_applyLH){
          if(!isLHAcceptTrig) {
              ATH_MSG_DEBUG("REGTEST: Fails LH");
              continue;
          }
      }
      if(clus->et() < m_emEt){
          ATH_MSG_DEBUG("REGTEST::Et cut no satisfied: "<< clus->et() << "< cut: " << m_emEt);
          continue;
      }
      accepted=true;
      xBits->markPassing(clus,clusContainer,true);
   
  }

  // Set event to pass 
  pass=accepted;
  ATH_MSG_DEBUG("REGTEST Result = " <<(accepted ? "accepted" : "not accepted"));
  ATH_MSG_DEBUG("REGTEST AcceptAll= " <<(m_acceptAll ? "true (no cuts)" : "false (selection applied)"));
  ATH_MSG_DEBUG("REGTEST Result = " <<(pass ? "passed" : "failed"));
 
  // Time total TrigEFCaloHypo execution time.
  // -------------------------------------
  // store TrigPassBits result
  if(attachFeature(outputTE, xBits.release(),"passbits") != HLT::OK)
      ATH_MSG_ERROR("Could not store TrigPassBits! ");

  if (timerSvc()) m_totalTimer->stop();

  return HLT::OK;
}
