/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigHLTJetHypoTool.h"
#include "xAODJet/JetContainer.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/xAODJetAsIJet.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/groupsMatcherFactory.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/lineSplitter.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/CleanerFactory.h"

#include <limits>
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

class xAODJetAsIJetFactory{
  /* Create a HypoJet::IJet from a xAODJet while remembering index info
     (for example position in a Data vector of the xAODJet) */

public:
  xAODJetAsIJetFactory(): m_ind(-1){}
  pHypoJet operator() (const xAOD::Jet* j){
    return new HypoJet::xAODJetAsIJet(j, ++m_ind);}
private:
  unsigned int m_ind;
};

TrigHLTJetHypoTool::TrigHLTJetHypoTool( const std::string& type, 
                                        const std::string& name,
                                        const IInterface* parent ): AthAlgTool( type, name, parent ), m_id(name) {

  declareProperty("AcceptAll",      m_acceptAll=false);
  declareProperty("chain_name",      m_chainName="Unknown");
  
  // cleaning
  declareProperty("cleaningAlg", m_cleaningAlg = "noCleaning");
  
  // matching. Legal: maximumBipartite, orderedCollections, selectedJets
  declareProperty("matchingAlg", m_matchingAlg = "maximumBipartite");
 
 //basic cleaning  
  declareProperty("n90CleaningThreshold", m_n90Threshold = 2 );
  declareProperty("presamplerCleaningThreshold", m_presamplerThreshold = 0.9 );
  declareProperty("negativeECleaningThreshold", m_negativeEThreshold = -60e3 ); // 60 GeV
  declareProperty("qmeanCleaningThreshold", m_qmeanThreshold = 0.8 );
  declareProperty("HECQCleaningThreshold", m_hecQThreshold = 0.5 );
  declareProperty("HECfCleaningThreshold", m_hecFThreshold = 0.5 );
  declareProperty("LArQCleaningThreshold", m_larQThreshold = 0.8 );
  declareProperty("EMfCleaningThreshold", m_emFThreshold = 0.95 );
  //loose cleaning
  declareProperty("fracSamplingMaxLooseThreshold", m_fSampMaxLooseThreshold = 0.8 );
  declareProperty("etaLooseThreshold", m_etaLooseThreshold = 2.0 );
  declareProperty("EMfLowLooseThreshold", m_emfLowLooseThreshold = 0.10 );
  declareProperty("EMfHighLooseThreshold", m_emfHighLooseThreshold = 0.99 );
  declareProperty("HECfLooseThreshold", m_hecfLooseThreshold = 0.85 );
  //tight cleaning
  declareProperty("fracSamplingMaxTightThreshold", m_fSampMaxTightThreshold = 0.8 );
  declareProperty("etaTightThreshold", m_etaTightThreshold = 2.0 );
  declareProperty("EMfLowTightThreshold", m_emfLowTightThreshold = 0.10 );
  declareProperty("EMfHighTightThreshold", m_emfHighTightThreshold = 0.99 );
  declareProperty("HECfTightThreshold", m_hecfTightThreshold = 0.85 );
  //long-lived particle cleaning
  declareProperty("fracSamplingMaxLlpThreshold", m_fSampMaxLlpThreshold = 0.85 );
  declareProperty("negativeELlpThreshold", m_negELlpThreshold = 10e3 ); // 10 GeV
  declareProperty("HECfLlpThreshold", m_hecfLlpThreshold = 0.5 );
  declareProperty("HECQLlpThreshold", m_hecqLlpThreshold = 0.5 );
  declareProperty("AverageLArQFLlpThreshold", m_avLarQFLlpThreshold = 0.8*65535 );
 

  // Monitored variables...
  /*
  declareMonitoredVariable("NJet", m_njet);
  declareMonitoredVariable("Et", m_et);
  declareMonitoredVariable("Eta", m_eta);
  declareMonitoredVariable("Phi", m_phi);
  */
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode TrigHLTJetHypoTool::queryInterface( const InterfaceID& riid, 
                                               void** ppvIf )
{
  if ( riid == ITrigHLTJetHypoTool::interfaceID() )  {
    *ppvIf = (ITrigHLTJetHypoTool*)this;
    addRef();
    return StatusCode::SUCCESS;
  }

  return AthAlgTool::queryInterface( riid, ppvIf );
}

StatusCode TrigHLTJetHypoTool::initialize(){
  ATH_MSG_INFO("in initialize()");
  
  // make and store the jet cleaners
  setCleaners();

  
  ATH_CHECK(checkVals());
  
  setConditions();
  // setJetGrouper();

  ATH_CHECK(!m_conditions.empty());

  auto matcher = groupsMatcherFactory(m_conditions);
  auto grouper = getJetGrouper();
  auto helper = TrigHLTJetHypoHelper2(m_cleaners, 
                                      std::move(grouper), 
                                      std::move(matcher));


  // print out the TrigHLTJetHypoHelper configuration
  ATH_MSG_INFO("Initializing TrigHLTJetHypoTool for chain " 
               << m_chainName);
  std::string line = helper.toString();
  std::vector<std::string> lines = lineSplitter(line, '\n');
  
  for(auto l : lines){
    ATH_MSG_INFO(l);
  }

  ATH_MSG_DEBUG("Tool configured for chain/id: " << m_id);

  return StatusCode::SUCCESS;
}



StatusCode 
TrigHLTJetHypoTool::decide(const xAOD::JetContainer* jets, 
                               bool& pass) const {
  ATH_MSG_DEBUG("Executing " << name() << "...");
  ATH_CHECK(jets != nullptr);

  // resetCounters();

  HypoJetVector hypoJets(jets->size());

  std::transform(jets -> begin(),
                 jets -> end(),
                 hypoJets.begin(),
                 xAODJetAsIJetFactory());
 
  // make a new CleanerMatcher every event
  auto matcher = groupsMatcherFactory(m_conditions);
  auto grouper = getJetGrouper();
  auto helper = TrigHLTJetHypoHelper2(m_cleaners, 
                                      std::move(grouper), 
                                      std::move(matcher));

   /* apply cleaning and hypothesis alg */
  ATH_MSG_DEBUG("hypo helper start... " 
                << name() 
                << " no of jets ... " 
                << jets->size() 
                << "...");

  // steady_clock::time_point t =  steady_clock::now();

  try{
    pass = !jets->empty() && (m_acceptAll || helper.pass(hypoJets));
  } catch(std::exception& e){
    ATH_MSG_ERROR("Exception raised by the TrigHLTJetHypoHelper2: " 
                  << e.what());
    return StatusCode::FAILURE;
  }

  // accumulateTime(steady_clock::now() - t);
  
  ATH_MSG_DEBUG("hypo testing done... " << name() << "...");
  
  if(m_dumpJets){writeDebug(pass, helper.passedJets(), helper.failedJets());}

  // delete the xAOD::Jet wrappers
  for(auto i : hypoJets){delete i;}

  return StatusCode::SUCCESS;
}


void  TrigHLTJetHypoTool::setCleaners() {

  if (m_cleaningAlg != "noCleaning"){

    CleanerFactory cleanerFactory(//basic cleaning
                                  m_n90Threshold, 
                                  m_presamplerThreshold,
                                  m_negativeEThreshold,
                                  //loose cleaning
                                  m_fSampMaxLooseThreshold,
                                  m_etaLooseThreshold,
                                  m_emfLowLooseThreshold,
                                  m_emfHighLooseThreshold,
                                  m_hecfLooseThreshold,
                                  //tight cleaning
                                  m_fSampMaxTightThreshold,
                                  m_etaTightThreshold,
                                  m_emfLowTightThreshold,
                                  m_emfHighTightThreshold,
                                  m_hecfTightThreshold,
                                  //long-lived particle cleaning
                                  m_fSampMaxLlpThreshold,
                                  m_negELlpThreshold,
                                  m_hecfLlpThreshold,
                                  m_hecqLlpThreshold,
                                  m_avLarQFLlpThreshold,
                                  m_cleaningAlg);
    
    m_cleaners.push_back(cleanerFactory.make());
  }

  auto cleaners = getCleaners();  // sub class supplied cleaners
  m_cleaners.insert(m_cleaners.end(), cleaners.begin(), cleaners.end());
  ATH_MSG_INFO("No of Cleaners " << m_cleaners.size());
}

/*
void TrigHLTJetHypoTool::bumpCounters(bool pass, int multiplicity){
   if (pass){
     ++m_accepted;
     m_njet = multiplicity;
   } else {
     ++m_rejected;
   }
 }
*/

void TrigHLTJetHypoTool::writeDebug(bool pass,
                                    const HypoJetVector& passedJets,
                                    const HypoJetVector& failedJets
                                    ) const{
  ATH_MSG_INFO("Writing debug start" << name() << "...");
  
  if(pass){
    std::cout<<name()<< " event passed \n";
  } else {
    std::cout<<name()<< " event failed \n";
  }

  for (auto j :  passedJets) {
    auto p4 = j->p4();
    std::cout<<"\nHYPODUMP passed TrigHLTJetHypoTool Et: " 
             << p4.Et() 
             << " eta " 
             << j->eta() 
             << " px "
             << p4.Px()
             << " py "
             << p4.Py()
             << " pz "
             << p4.Pz()
             << " E "
             << p4.E()
             << '\n';
  }
  
  for (auto j :  failedJets) {
    auto p4 = j->p4();
    std::cout<<"\nHYPODUMP failed TrigHLTJetHypoTool Et: " 
             << p4.Et() 
             << " eta " 
             << j->eta() 
             << " px "
             << p4.Px()
             << " py "
             << p4.Py()
             << " pz "
             << p4.Pz()
             << " E "
             << p4.E()
               << '\n';
  }
  
}

/*
void TrigHLTJetHypoTool::resetCounters(){
  m_njet = std::numeric_limits<int>::max();
  m_et = std::numeric_limits<double>::max();
  m_eta = std::numeric_limits<double>::max();
  m_phi = std::numeric_limits<double>::max();
}

void  TrigHLTJetHypoTool::accumulateTime(nanoseconds duration) noexcept{

  auto dtime = duration_cast<microseconds>(duration);
  auto counts = dtime.count();
  //  countssq = counts*counts;

  if (m_nCalls == 0){
    m_nCalls = 1;
    m_chainTimeAv = counts;
    // m_chainTimeSquareAv = countssq;
    return;
  }
  
  m_nCalls += 1;
  m_chainTimeAv = (m_chainTimeAv * (m_nCalls - 1) +  counts)/m_nCalls;
*/
/* error in here somewhere
  m_chainTimeSquareAv += 
    (m_chainTimeSquareAv * (m_nCalls - 1) + countssq)/m_nCalls;
*/
/*
}
*/
void  TrigHLTJetHypoTool::setConditions() {
  m_conditions = getConditions();
}

TrigCompositeUtils::DecisionID TrigHLTJetHypoTool::decisionId() const{
  return m_id.numeric();
}  



/*
void  TrigHLTJetHypoTool::setJetGrouper() {
  m_grouper = getJetGrouper();
}
*/
