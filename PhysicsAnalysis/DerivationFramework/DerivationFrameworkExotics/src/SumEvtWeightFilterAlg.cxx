/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SumEvtWeightFilterAlg.cxx
 * @brief Implementation of class SumEvtWeightFilterAlg
 */
  
#include <string>
#include "DerivationFrameworkExotics/SumEvtWeightFilterAlg.h"
#include "DerivationFrameworkExotics/TruthHelper.h"

// EDM includes
#include "xAODCutFlow/CutBookkeeper.h"
#include "xAODEventInfo/EventInfo.h"
 
DerivationFramework::SumEvtWeightFilterAlg::SumEvtWeightFilterAlg(const std::string& name, 
								  ISvcLocator* pSvcLocator) :
  AthFilterAlgorithm(name, pSvcLocator),
  m_eventsProcessed(0),
  m_mcCutIDs()
{
  //
  // Property declaration
  //
}

DerivationFramework::SumEvtWeightFilterAlg::~SumEvtWeightFilterAlg()
{}

StatusCode DerivationFramework::SumEvtWeightFilterAlg::initialize() { 
  ATH_MSG_DEBUG ("Initializing " << this->name() << "...");
 
  cutFlowSvc()->registerTopFilter( this->name(),
				   "Number of processed events before any cut",
				   xAOD::CutBookkeeper::CutLogic::ALLEVENTSPROCESSED,
				   "AllStreams");
return StatusCode::SUCCESS; }

StatusCode DerivationFramework::SumEvtWeightFilterAlg::execute() 
{
  ATH_MSG_VERBOSE ("Executing " << this->name() << "...");
  setFilterPassed(true);

  // Get the EventInfo object
  const xAOD::EventInfo* evtInfo = 0;
  ATH_CHECK( evtStore()->retrieve(evtInfo, "EventInfo") );
  // Only try to access the mcEventWeight is we are running on Monte Carlo, duhhh!
  if ( !(evtInfo->eventType(xAOD::EventInfo::IS_SIMULATION)) ) {
    ATH_MSG_DEBUG("We are not running on simulation and thus, nothing to be done here");
    return StatusCode::SUCCESS;
  }

  // preserve the order. this is assumed below
  const std::vector<std::string> filters = {"All","VBFSherpa","BFilter","CFilter","LFFilter",
					    "SherpaPTV90","SherpaPTV100","SherpaPTV500", // for the sherpa photon samples, inclusive, and less than 500 gev
					    "JZVBFFilter","MGVBFFilter","MGVBFORFilter",
					    "MGZnnNp01ORFilter","ZOtherORFilter","MGWFilter",
					    "MGWNoFlavFilter","PhotonFilter","PhotonwIsoFilter"};

  // Set up everything during the first event
  if ( m_eventsProcessed == 0 ){
    const std::size_t nFilters = filters.size();
    m_mcCutIDs.reserve(nFilters);
    for ( std::size_t i=0; i<filters.size(); ++i ) {
      std::stringstream sstm;
      sstm << this->name() << "_" << filters.at(i);
      const std::string cutName = sstm.str();
      std::stringstream sstm2;
      sstm2 << "Apply filter MC event weight number " << filters.at(i);
      const std::string& cutDescription = sstm2.str();
      CutIdentifier cutID = cutFlowSvc()->registerTopFilter( cutName,
							     cutDescription,
							     xAOD::CutBookkeeper::CutLogic::ALLEVENTSPROCESSED,
							     "AllStreams" );
      m_mcCutIDs.push_back(cutID);
    }
  }

  // Collect the variables
    // Read in the truth event
    const xAOD::TruthEventContainer* xTruthEventContainer = nullptr;
    CHECK( evtStore()->retrieve( xTruthEventContainer, "TruthEvents"));
// Read in the truth jets
  std::string m_containerName = "AntiKt4TruthJets";
  // retrieve container
  const xAOD::JetContainer* jets = nullptr;
  ATH_CHECK(evtStore()->retrieve(jets, "AntiKt4TruthJets"));
  double weight = evtInfo->mcEventWeight();
  static SG::AuxElement::ConstAccessor< int >    decoratorFlavourFilter("FlavourFilter");  
  static SG::AuxElement::ConstAccessor< float >  decoratorMGVTruthPt("MGVTruthPt");  
  static SG::AuxElement::ConstAccessor< float >  decoratorSherpaVTruthPt("SherpaVTruthPt");  
  static SG::AuxElement::ConstAccessor< bool >   decoratorin_vy_overlap("in_vy_overlap");
  static SG::AuxElement::ConstAccessor< bool >   decoratorin_vy_overlap_iso("in_vy_overlap_iso");
  int flav=decoratorFlavourFilter(*evtInfo);
  float ptv = decoratorMGVTruthPt(*evtInfo);
  float sherpaptv = decoratorSherpaVTruthPt(*evtInfo);
  bool passPhotonOR = decoratorin_vy_overlap(*evtInfo);
  bool passPhotonORIso = decoratorin_vy_overlap_iso(*evtInfo);
  double JetEtaFilter = 5.0;
  double JetpTFilter = 20e3;
  double MjjFilter = 800.0e3;
  double PhijjFilter = 2.5;
  // After burner filter for the NLO sherpa samples
  bool passJetFilter = DerivationFramework::passTruthFilter(nullptr, jets, JetEtaFilter,JetpTFilter,
							      MjjFilter, PhijjFilter);

  // QCD filter
  bool passQCDFilter=DerivationFramework::passTruthFilter(nullptr, jets, 5.0, 20e3, 1000.0e3, 2.0);
  // MG filtering
  // for the Znn - filtSeq.MissingEtFilter.METCut = 100*GeV
  // only for the Z we need  dR<0.3 and pT>35 GeV
  // filtSeq.VBFMjjIntervalFilter.ElectronJetOverlapRemoval = True
  // filtSeq.VBFMjjIntervalFilter.TauJetOverlapRemoval = True
  // https://gitlab.cern.ch/atlas/athena/blob/master/Generators/GeneratorFilters/src/MissingEtFilter.cxx
  bool passMGJetFilter=DerivationFramework::passTruthFilter(nullptr, jets, JetEtaFilter, 35.0e3,
							    MjjFilter, PhijjFilter);
  // Overlap remove electrons and taus
  bool passMGJetFilterOREleTau = DerivationFramework::passTruthFilter(xTruthEventContainer, jets, JetEtaFilter,35.0e3,
								      MjjFilter, PhijjFilter);
  
  // Increment the non-nominal MC event weight counters
  cutFlowSvc()->addEvent( m_mcCutIDs[0], weight );
  if(passJetFilter)                               cutFlowSvc()->addEvent( m_mcCutIDs[1], weight );
  if(flav==1)                                     cutFlowSvc()->addEvent( m_mcCutIDs[2], weight );
  if(flav==2)                                     cutFlowSvc()->addEvent( m_mcCutIDs[3], weight );
  if(flav==0)                                     cutFlowSvc()->addEvent( m_mcCutIDs[4], weight );
  if(sherpaptv> 90.0e3)                           cutFlowSvc()->addEvent( m_mcCutIDs[5], weight );
  if(sherpaptv>100.0e3)                           cutFlowSvc()->addEvent( m_mcCutIDs[6], weight );
  if(sherpaptv>500.0e3)                           cutFlowSvc()->addEvent( m_mcCutIDs[7], weight );
  if(passQCDFilter)                               cutFlowSvc()->addEvent( m_mcCutIDs[8], weight );
  if(passMGJetFilter)                             cutFlowSvc()->addEvent( m_mcCutIDs[9], weight );
  if(passMGJetFilterOREleTau)                     cutFlowSvc()->addEvent( m_mcCutIDs[10], weight );
  if(ptv>75.0e3 && passMGJetFilterOREleTau)       cutFlowSvc()->addEvent( m_mcCutIDs[11], weight );
  if(ptv>100.0e3 && passMGJetFilterOREleTau)      cutFlowSvc()->addEvent( m_mcCutIDs[12], weight );
  if(flav==0 && ptv>100.0e3 && passMGJetFilter)   cutFlowSvc()->addEvent( m_mcCutIDs[13], weight );
  if(ptv>100.0e3 && passMGJetFilter)              cutFlowSvc()->addEvent( m_mcCutIDs[14], weight );
  if(passPhotonOR)                                cutFlowSvc()->addEvent( m_mcCutIDs[15], weight );
  if(passPhotonORIso)                             cutFlowSvc()->addEvent( m_mcCutIDs[16], weight );
  
  // Increment the event counter
  ++m_eventsProcessed;
  return StatusCode::SUCCESS; 
}

StatusCode DerivationFramework::SumEvtWeightFilterAlg::finalize() { 
  ATH_MSG_DEBUG ("Finalizing " << this->name() << "...");
  return StatusCode::SUCCESS; 
}
