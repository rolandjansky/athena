/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/IToolSvc.h"

#include "TrigL2LongLivedParticles/TrigJetSplitter.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrigSteeringEvent/TrigOperationalInfo.h"
#include "TrigSteeringEvent/PhiHelper.h"
#include "TrigNavigation/TriggerElement.h"

#include "JetEvent/Jet.h"
#include "JetEvent/JetCollection.h"
#include "FourMomUtils/P4DescendingSorters.h"
#include "TrigSteeringEvent/PhiHelper.h"

#include "CaloEvent/CaloCellContainer.h"
#include "CaloEvent/CaloCell.h"

#include "xAODJet/JetContainer.h"
#include "AthContainers/ConstDataVector.h"
#include "CxxUtils/fpcompare.h"

//** ----------------------------------------------------------------------------------------------------------------- **//


struct DescendingEt:std::binary_function<const xAOD::Jet*, const xAOD::Jet*, bool> {
  bool operator()(const xAOD::Jet* l, const xAOD::Jet* r)  const {
    return l->p4().Et() > r->p4().Et();
  }
};


//** ----------------------------------------------------------------------------------------------------------------- **//


TrigJetSplitter::TrigJetSplitter(const std::string & name, ISvcLocator* pSvcLocator) :
  HLT::AllTEAlgo(name, pSvcLocator)
{
  declareProperty ("JetInputKey",  m_jetInputKey  = "TrigJetRec");
  declareProperty ("JetOutputKey", m_jetOutputKey = "SplitJet");
  declareProperty ("EtaHalfWidth", m_etaHalfWidth = 0.4);
  declareProperty ("PhiHalfWidth", m_phiHalfWidth = 0.4);
  declareProperty ("ZHalfWidth",   m_zHalfWidth   = 20.0);// in mm?
  declareProperty ("JetMinEt",     m_minJetEt     = 30.0); // in GeV (increase from 15 GeV to be same as vertex threshold)
  declareProperty ("JetMaxEta",    m_maxJetEta    = 2.5+m_etaHalfWidth);  // tracker acceptance + jet half-width
  declareProperty ("JetLogRatio",  m_logRatio    = 1.2);  // minimum/maximum log ratio for being selected
  declareProperty("Reversed",      m_reversedCut = false, "reversed cut for collimated photons");
}


//** ----------------------------------------------------------------------------------------------------------------- **//


HLT::ErrorCode TrigJetSplitter::hltInitialize() {

  if (msgLvl() <= MSG::INFO) 
    msg() << MSG::INFO << "Initializing TrigJetSplitter, version " << PACKAGE_VERSION << endreq;

  //* declareProperty overview *//
  if (msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG << "declareProperty review:" << endreq;
    msg() << MSG::DEBUG << " JetInputKey  = "  << m_jetInputKey << endreq; 
    msg() << MSG::DEBUG << " JetOutputKey = " << m_jetOutputKey << endreq; 
    msg() << MSG::DEBUG << " EtaHalfWidth = " << m_etaHalfWidth << endreq; 
    msg() << MSG::DEBUG << " PhiHalfWidth = " << m_phiHalfWidth << endreq; 
    msg() << MSG::DEBUG << " ZHalfWidth   = " << m_zHalfWidth   << endreq; 
    msg() << MSG::DEBUG << " MinJetEt     = " << m_minJetEt     << endreq; 
    msg() << MSG::DEBUG << " MaxJetEta    = " << m_maxJetEta    << endreq; 
    msg() << MSG::DEBUG << " LogRatio  = " << m_logRatio    << endreq; 
    msg() << MSG::DEBUG << " ReversedCut  = " << m_reversedCut    << endreq; 
}

  return HLT::OK;
}


//** ----------------------------------------------------------------------------------------------------------------- **//


TrigJetSplitter::~TrigJetSplitter(){}


//** ----------------------------------------------------------------------------------------------------------------- **//


HLT::ErrorCode TrigJetSplitter::hltExecute(std::vector<std::vector<HLT::TriggerElement*> >& inputTEs, unsigned int output) {

  if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Running TrigJetSplitter::hltExecute" << endreq;

  beforeExecMonitors().ignore();

  if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " inputTEs.size() " << inputTEs.size() << endreq;
  
  if (inputTEs.size() != 1) {
    msg() << MSG::WARNING << "Wrong number of input TEs" << endreq;
    afterExecMonitors().ignore();
    return HLT::ErrorCode(HLT::Action::ABORT_CHAIN, HLT::Reason::MISSING_FEATURE);
  }
  
  // -----------------------
  // Retrieve jets
  // -----------------------

  std::vector<HLT::TriggerElement*>& jetTE = inputTEs.at(0);

  if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " jetTE.size() " << jetTE.size() << endreq;

  if (jetTE.size() == 0) {
    msg() << MSG::WARNING << "Got an empty inputTE (jets)" << endreq;
    afterExecMonitors().ignore();
    return HLT::MISSING_FEATURE; 
  }

  const xAOD::JetContainer* jets = 0;
  HLT::ErrorCode statusJets = getFeature(jetTE.front(), jets);

  if (statusJets != HLT::OK) {
    if (msgLvl() <= MSG::WARNING) msg() << MSG::WARNING << "Failed to retrieve features (jets)" << endreq;
    return HLT::NAV_ERROR;
  }

  if(jets==0) {
    if (msgLvl() <= MSG::WARNING) msg() << MSG::WARNING << "Missing feature (jets)." << endreq;
    return HLT::MISSING_FEATURE;
  }


  // -----------------------------------
  // Retrieve calo cells  --> this is now done in TrigBHremoval.cxx
  // -----------------------------------
  /*
  std::vector<const CaloCellContainer*> vectorOfCellContainers;

  if(getFeatures(jetTE.front(), vectorOfCellContainers, "") != HLT::OK) {
    msg() << MSG::WARNING << "Failed to get TrigCells" << endreq;   
    return HLT::OK;
  }
 
  if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Got vector with " << vectorOfCellContainers.size() << " CellContainers" << endreq;
 
  // if no containers were found, just leave the vector empty and leave
  if ( vectorOfCellContainers.size() < 1) {
    msg() << MSG::ERROR << "No cells to analyse, leaving!" << endreq;
    return HLT::OK;
  }

  // get last container to be put in vector (should also be the only one)
   const CaloCellContainer* theCellCont = vectorOfCellContainers.back();

  if(msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG << " Retrieved a Cell Container of Size= " << theCellCont->size() << endreq;
  }
  */
  if (msgLvl() <= MSG::DEBUG)
    msg() << MSG::DEBUG << "Found " << jets->size() << " jets, creating corresponding RoIs" << endreq; 

  HLT::TriggerElement* initialTE = config()->getNavigation()->getInitialNode();
  
  unsigned int i = 0;
  for ( xAOD::JetContainer::const_iterator jetitr=jets->begin() ; jetitr!=jets->end() ; jetitr++ ) { 
  
    ++i;
    auto jet = *jetitr;

    float jetEt  = jet->p4().Et()*0.001;
    float jetEta = jet->eta();
    float jetPhi = jet->phi();
    double jetEMF = jet->getAttribute<float>("EMFrac");
    
    double zero = 0., one =1., jetRatio = -1.;
    if (CxxUtils::fpcompare::greater(jetEMF,zero)){
      if(CxxUtils::fpcompare::greater_equal(jetEMF,one)) jetRatio = -999.;
      else jetRatio = log10(double(1./jetEMF - 1.));
    } else {
      jetRatio = 999;
    }

    if (jetEt < m_minJetEt) {
      if (msgLvl() <= MSG::DEBUG)
	msg() << MSG::DEBUG << "Jet "<< i << " below the " << m_minJetEt << " GeV threshold; Et " << jetEt << "; skipping this jet." << endreq;
      continue;
    }
    if (fabs(jetEta) > m_maxJetEta) {
      if (msgLvl() <= MSG::DEBUG)
	msg() << MSG::DEBUG << "Jet "<< i << " outside the |eta| < 2.5 requirement; Eta = " << jetEta << "; skipping this jet." << endreq;
      continue;
    }
    if(!m_reversedCut) {
      if ( jetRatio < m_logRatio) {
	if (msgLvl() <= MSG::DEBUG)
	  msg() << MSG::DEBUG << "Jet "<< i << " below the " << m_logRatio << " threshold for the log-ratio cut; logRatio = " << jetRatio << "; skipping this jet." << endreq;
	continue;
      }
    } else {
      if ( jetRatio > m_logRatio) {
	if (msgLvl() <= MSG::DEBUG)
	  msg() << MSG::DEBUG << "Jet "<< i << " above the " << m_logRatio << " threshold for the log-ratio cut; logRatio = " << jetRatio << "; skipping this jet." << endreq;
	continue;
      } 
    }
    int countCaloCell=0;
    /* --> this is now done in TrigBHremoval.cxx
    //calculate number of cells in line of fire
    for(CaloCellContainer::const_iterator celliter = theCellCont->begin(); celliter != theCellCont->end(); ++celliter){
    //LoF cell selection:
    //-tile
    //-provenance
    //-energy
      if((*celliter)->caloDDE()->is_tile() && (*celliter)->energy() > 240.0){
    	
        //-dPhi
        //-dR
        float d_phi = HLT::wrapPhi((*celliter)->phi() - jetPhi);
        float d_eta = (*celliter)->eta() - jetEta;

        if(fabs(d_phi) < 0.2 && sqrt(d_eta*d_eta + d_phi*d_phi) > 0.3){
          //-early
          float t = (*celliter)->time();
          if(t < -2.0){
            //-halo time
            float x = (*celliter)->x();
            float y = (*celliter)->y();
            float z = (*celliter)->z();
            float c = 299.792458;//mm per ns
            float r = sqrt(x*x + y*y);
            if((fabs(t - (z-sqrt(z*z + r*r))/c) < 5.0) || (fabs(t - (-z-sqrt(z*z + r*r))/c) < 5.0)){
	      msg() << MSG::DEBUG << " cell E = " << (*celliter)->energy() << " cell phi = " << (*celliter)->phi() << " cell eta = " << (*celliter)->eta() << " cell r = " << r << endreq;
	          countCaloCell++;
            }
          }
        }
      }
    }

    if (msgLvl() <= MSG::DEBUG)
      msg() << MSG::DEBUG << "Jet "<< i << "; Et " << jetEt << "; eta "<< jetEta << "; phi " << jetPhi <<"; logRatio " << jetRatio << "; LoF Cells " << countCaloCell << endreq;
    
    */
    // Create an output TE seeded by an empty vector
    HLT::TriggerElement* outputTE = config()->getNavigation()->addNode( initialTE, output );
    outputTE->setActiveState(true);

    // Make deep copy of vertex and attach to output TE

    /// create RoI correspondinding to the jet
    double phiMinus = HLT::wrapPhi(jetPhi-m_phiHalfWidth); 
    double phiPlus  = HLT::wrapPhi(jetPhi+m_phiHalfWidth); 

    double etaMinus = jetEta-m_etaHalfWidth;  
    double etaPlus  = jetEta+m_etaHalfWidth; 

    TrigRoiDescriptor* roi =  new TrigRoiDescriptor(jetEta,   etaMinus, etaPlus, 
						    jetPhi,   phiMinus, phiPlus);

    HLT::ErrorCode hltStatus = attachFeature(outputTE, roi, m_jetOutputKey);
    if ( hltStatus != HLT::OK ) {
      msg() << MSG::ERROR << "Failed to attach TrigRoiDescriptor as feature " << *roi << endreq;
      return hltStatus;
    }

    // Ideally would have liked to attach a view container
    // but these cannot be persistified at the moment...
    // ConstDataVector<xAOD::JetContainer>* jc = new ConstDataVector<xAOD::JetContainer>(SG::VIEW_ELEMENTS);
    // jc->clear(SG::VIEW_ELEMENTS); 
    // jc->push_back ( *jetitr );

    // ... so for the time being do a deep copy
    xAOD::JetTrigAuxContainer trigJetTrigAuxContainer;
    xAOD::JetContainer* jc = new xAOD::JetContainer;
    jc->setStore(&trigJetTrigAuxContainer);
    jc->push_back ( new xAOD::Jet(**jetitr) );
      
    // for checking number of cells passing LoF requirements
    TrigOperationalInfo* trigInfoCellLoF = new TrigOperationalInfo();
    trigInfoCellLoF->set("CellLoF", countCaloCell);
    HLT::ErrorCode hltCellStatus = attachFeature(outputTE, trigInfoCellLoF, "CellLoFInfo"); 
    if (hltCellStatus != HLT::OK) {
      msg() << MSG::ERROR << "Failed to attach TrigOperationalInfo (number of calo cells) as feature" << endreq;
      return hltCellStatus;
    }

    hltStatus = attachFeature(outputTE, jc, m_jetOutputKey); 
    if (hltStatus != HLT::OK) {
      msg() << MSG::ERROR << "Failed to attach xAOD::JetContainer (" << m_jetOutputKey << ") as feature jet eta, phi " << jet->eta() << ", " << jet->phi() << endreq;
      return hltStatus;
    }
  }

  afterExecMonitors().ignore();

  return HLT::OK;
}


//** ----------------------------------------------------------------------------------------------------------------- **//


HLT::ErrorCode TrigJetSplitter::hltFinalize() {

  if ( msgLvl() <= MSG::INFO )
    msg() << MSG::INFO << "in finalize()" << endreq;

  return HLT::OK;
}



