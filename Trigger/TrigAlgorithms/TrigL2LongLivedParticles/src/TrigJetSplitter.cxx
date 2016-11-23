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
  declareProperty ("JetPUFixLogRatio",  m_pufixLogRatio    = 1.2);  // minimum log ratio for pileup removal
  declareProperty ("Reversed",     m_reversedCut = false, "reversed cut for collimated photons");
}


//** ----------------------------------------------------------------------------------------------------------------- **//


HLT::ErrorCode TrigJetSplitter::hltInitialize() {

  if (msgLvl() <= MSG::INFO) 
    msg() << MSG::INFO << "Initializing TrigJetSplitter, version " << PACKAGE_VERSION << endmsg;

  //* declareProperty overview *//
  if (msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG << "declareProperty review:" << endmsg;
    msg() << MSG::DEBUG << " JetInputKey  = "  << m_jetInputKey << endmsg; 
    msg() << MSG::DEBUG << " JetOutputKey = " << m_jetOutputKey << endmsg; 
    msg() << MSG::DEBUG << " EtaHalfWidth = " << m_etaHalfWidth << endmsg; 
    msg() << MSG::DEBUG << " PhiHalfWidth = " << m_phiHalfWidth << endmsg; 
    msg() << MSG::DEBUG << " ZHalfWidth   = " << m_zHalfWidth   << endmsg; 
    msg() << MSG::DEBUG << " MinJetEt     = " << m_minJetEt     << endmsg; 
    msg() << MSG::DEBUG << " MaxJetEta    = " << m_maxJetEta    << endmsg; 
    msg() << MSG::DEBUG << " LogRatio  = " << m_logRatio    << endmsg; 
    msg() << MSG::DEBUG << " PUFixLogRatio    = " << m_pufixLogRatio    << endmsg; 
    msg() << MSG::DEBUG << " ReversedCut  = " << m_reversedCut    << endmsg; 
  }

  return HLT::OK;
}


//** ----------------------------------------------------------------------------------------------------------------- **//


TrigJetSplitter::~TrigJetSplitter(){}


//** ----------------------------------------------------------------------------------------------------------------- **//


HLT::ErrorCode TrigJetSplitter::hltExecute(std::vector<std::vector<HLT::TriggerElement*> >& inputTEs, unsigned int output) {

  if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Running TrigJetSplitter::hltExecute" << endmsg;

  beforeExecMonitors().ignore();

  if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " inputTEs.size() " << inputTEs.size() << endmsg;
  
  if (inputTEs.size() != 1) {
    msg() << MSG::WARNING << "Wrong number of input TEs" << endmsg;
    afterExecMonitors().ignore();
    return HLT::ErrorCode(HLT::Action::ABORT_CHAIN, HLT::Reason::MISSING_FEATURE);
  }
  
  // -----------------------
  // Retrieve jets
  // -----------------------

  std::vector<HLT::TriggerElement*>& jetTE = inputTEs.at(0);

  if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " jetTE.size() " << jetTE.size() << endmsg;

  if (jetTE.size() == 0) {
    msg() << MSG::WARNING << "Got an empty inputTE (jets)" << endmsg;
    afterExecMonitors().ignore();
    return HLT::MISSING_FEATURE; 
  }

  const xAOD::JetContainer* jets = 0;
  HLT::ErrorCode statusJets = getFeature(jetTE.front(), jets);

  if (statusJets != HLT::OK) {
    if (msgLvl() <= MSG::WARNING) msg() << MSG::WARNING << "Failed to retrieve features (jets)" << endmsg;
    return HLT::NAV_ERROR;
  }

  if(jets==0) {
    if (msgLvl() <= MSG::WARNING) msg() << MSG::WARNING << "Missing feature (jets)." << endmsg;
    return HLT::MISSING_FEATURE;
  }


  // -----------------------------------
  // Retrieve calo cells  --> this is now done in TrigBHremoval.cxx
  // -----------------------------------
  /*
    std::vector<const CaloCellContainer*> vectorOfCellContainers;

    if(getFeatures(jetTE.front(), vectorOfCellContainers, "") != HLT::OK) {
    msg() << MSG::WARNING << "Failed to get TrigCells" << endmsg;   
    return HLT::OK;
    }
 
    if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Got vector with " << vectorOfCellContainers.size() << " CellContainers" << endmsg;
 
    // if no containers were found, just leave the vector empty and leave
    if ( vectorOfCellContainers.size() < 1) {
    msg() << MSG::ERROR << "No cells to analyse, leaving!" << endmsg;
    return HLT::OK;
    }

    // get last container to be put in vector (should also be the only one)
    const CaloCellContainer* theCellCont = vectorOfCellContainers.back();

    if(msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG << " Retrieved a Cell Container of Size= " << theCellCont->size() << endmsg;
    }
  */
  if (msgLvl() <= MSG::DEBUG)
    msg() << MSG::DEBUG << "Found " << jets->size() << " jets, creating corresponding RoIs" << endmsg; 

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
	msg() << MSG::DEBUG << "Jet "<< i << " below the " << m_minJetEt << " GeV threshold; Et " << jetEt << "; skipping this jet." << endmsg;
      continue;
    }
    if (fabs(jetEta) > m_maxJetEta) {
      if (msgLvl() <= MSG::DEBUG)
	msg() << MSG::DEBUG << "Jet "<< i << " outside the |eta| < 2.5 requirement; Eta = " << jetEta << "; skipping this jet." << endmsg;
      continue;
    }
    if(!m_reversedCut) {
      if ( jetRatio < m_pufixLogRatio) {
	if (msgLvl() <= MSG::DEBUG)
	  msg() << MSG::DEBUG << "Jet "<< i << " below the " << m_pufixLogRatio << " threshold for the log-ratio cut; logRatio = " << jetRatio << "; skipping this jet." << endmsg;
	continue;
      }

      double pufixLR = -1; // Will recalculate the jet's logRatio if it is not already > 1.2

      if ( jetRatio < m_logRatio ) {
	if (msgLvl() <= MSG::DEBUG)
	  msg() << MSG::DEBUG << "Jet "<< i << " is below the " << m_logRatio << " threshold for the log-ratio cut and above the " << m_pufixLogRatio << " threshold; logRatio = " << jetRatio << "; performing pileup removal." << endmsg;

	size_t nClusters = jet->numConstituents();
	double clusterPU_sumEEM = 0; double clusterPU_sumE = 0;

	for (size_t clust = 0; clust < nClusters; clust++) {

	  const xAOD::CaloCluster * aCluster = dynamic_cast<const xAOD::CaloCluster*> (jet->rawConstituent(clust));

	  double clusEEM = 0;
	  clusEEM+=(aCluster)->eSample(CaloSampling::EMB1);
	  clusEEM+=(aCluster)->eSample(CaloSampling::EMB2);
	  clusEEM+=(aCluster)->eSample(CaloSampling::EMB3);
	  clusEEM+=(aCluster)->eSample(CaloSampling::EME1);
	  clusEEM+=(aCluster)->eSample(CaloSampling::EME2);
	  clusEEM+=(aCluster)->eSample(CaloSampling::EME3);
	  clusEEM+=(aCluster)->eSample(CaloSampling::FCAL1);

	  double lambda = aCluster->getMomentValue(xAOD::CaloCluster::CENTER_LAMBDA);

	  if (lambda > 500) continue;

	  double d_eta = aCluster->rawEta() - jetEta;
	  double d_phi = HLT::wrapPhi(aCluster->rawPhi() - jetPhi);

	  double d_R2 = d_eta*d_eta + d_phi*d_phi;

	  if (d_R2 < 0.15*0.15) continue;

	  clusterPU_sumEEM+=clusEEM/1000.;
	  clusterPU_sumE+=aCluster->rawE()/1000.;

	}

	double jetEEM_EMscale = 0; double jetE_EMscale = 0;  //Working on EM scale because calE() doesn't always return correct EEM and cluster moment EMF not accessable during testing

	std::vector<double> samplingEnergy = jet->getAttribute<std::vector<double> >("EnergyPerSampling");  

	for(size_t s=0; s<samplingEnergy.size(); s++) {

	  double samplingE = 0.001*(samplingEnergy.at(s));

	  if ( s < 8 || (s > 20 && s < 28) ) jetEEM_EMscale+=samplingE; // EM layers 0-7 and 21-27
	  jetE_EMscale+=samplingE; 

	}

	double pufixEMF = (jetEEM_EMscale - clusterPU_sumEEM)/(jetE_EMscale - clusterPU_sumE);

	if (CxxUtils::fpcompare::greater(pufixEMF,zero)){
	  if(CxxUtils::fpcompare::greater_equal(pufixEMF,one)) pufixLR = -999.;
	  else pufixLR = log10(double(1./pufixEMF - 1.));
	} else {
	  pufixLR = 999;
	}

	if ( pufixLR < m_logRatio) {
	  if (msgLvl() <= MSG::DEBUG)
	    msg() << MSG::DEBUG << "Jet "<< i << " is still below the " << m_logRatio << " threshold for the log-ratio cut; recalculated logRatio = " << pufixLR << "; skipping this jet." << endmsg;
	  continue;
	}

	if (msgLvl() <= MSG::DEBUG){
	  msg() << MSG::DEBUG << "Jet "<< i << " old logRatio: " << jetRatio << "  new logRatio: " << pufixLR << endmsg;
	  msg() << MSG::DEBUG << "This is the jet with ET: " << jetEt << " eta: " << jetEta << " phi: " << jetPhi << endmsg;
	}
      }
      jetRatio = pufixLR;
    }
    else {
      if ( jetRatio > m_logRatio) {
	if (msgLvl() <= MSG::DEBUG)
	  msg() << MSG::DEBUG << "Jet "<< i << " above the " << m_logRatio << " threshold for the log-ratio cut; logRatio = " << jetRatio << "; skipping this jet." << endmsg;
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
    msg() << MSG::DEBUG << " cell E = " << (*celliter)->energy() << " cell phi = " << (*celliter)->phi() << " cell eta = " << (*celliter)->eta() << " cell r = " << r << endmsg;
    countCaloCell++;
    }
    }
    }
    }
    }

    if (msgLvl() <= MSG::DEBUG)
    msg() << MSG::DEBUG << "Jet "<< i << "; Et " << jetEt << "; eta "<< jetEta << "; phi " << jetPhi <<"; logRatio " << jetRatio << "; LoF Cells " << countCaloCell << endmsg;
    
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
      msg() << MSG::ERROR << "Failed to attach TrigRoiDescriptor as feature " << *roi << endmsg;
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
      msg() << MSG::ERROR << "Failed to attach TrigOperationalInfo (number of calo cells) as feature" << endmsg;
      return hltCellStatus;
    }

    hltStatus = attachFeature(outputTE, jc, m_jetOutputKey); 
    if (hltStatus != HLT::OK) {
      msg() << MSG::ERROR << "Failed to attach xAOD::JetContainer (" << m_jetOutputKey << ") as feature jet eta, phi " << jet->eta() << ", " << jet->phi() << endmsg;
      return hltStatus;
    }
  }

  afterExecMonitors().ignore();

  return HLT::OK;
}


//** ----------------------------------------------------------------------------------------------------------------- **//


HLT::ErrorCode TrigJetSplitter::hltFinalize() {

  if ( msgLvl() <= MSG::INFO )
    msg() << MSG::INFO << "in finalize()" << endmsg;

  return HLT::OK;
}



