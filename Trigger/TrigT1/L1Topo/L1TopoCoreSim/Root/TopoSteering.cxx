//  TopoSteering.cxx
//  Framework to execute simulation algorithms

#include "L1TopoInterfaces/AlgFactory.h" 
#include "L1TopoInterfaces/IL1TopoHistSvc.h"


#include "L1TopoInterfaces/ConfigurableAlg.h"
#include "L1TopoInterfaces/ParameterSpace.h"
#include "L1TopoInterfaces/SortingAlg.h"
#include "L1TopoInterfaces/DecisionAlg.h"
#include "L1TopoCommon/Exception.h"
#include "L1TopoEvent/GenericTOB.h"

#include "L1TopoConfig/L1TopoMenu.h"
#include "L1TopoConfig/L1TopoConfigGlobal.h"
#include "L1TopoConfig/LayoutConstraints.h"

#include "L1TopoCoreSim/TopoSteering.h"
#include "L1TopoCoreSim/Connector.h"
#include "L1TopoCoreSim/InputConnector.h"
#include "L1TopoCoreSim/SortingConnector.h"
#include "L1TopoCoreSim/DecisionConnector.h"

// c++ libraries
#include <iomanip>
#include <string>

// boost libraries
#include <boost/lexical_cast.hpp>

using namespace std;
using namespace TCS;
using namespace TrigConf;
using boost::lexical_cast;


TopoSteering::TopoSteering() :
   TrigConfMessaging("TopoSteering")
{}

TopoSteering::~TopoSteering() {
   AlgFactory::destroy_instance();
}



StatusCode
TopoSteering::setupFromConfiguration(const TXC::L1TopoMenu& menu) {
   
   if(m_useBitwise){ TRG_MSG_INFO("Will be using bitwise implementation of algorithms");}
   else{TRG_MSG_INFO("Will NOT be using bitwise implementation of algorithms");}


   StatusCode sc = m_structure.setupFromMenu( menu );

   // configure layout of the simulation result
   sc &= m_simulationResult.setupFromMenu( menu, m_structure.outputConnectors() );

   return sc;
}



StatusCode
TopoSteering::reset() {

   inputEvent().clear();

   m_structure.reset();

   ClusterTOB::clearHeap();
   JetTOB::clearHeap();
   MuonTOB::clearHeap();
   LateMuonTOB::clearHeap();
   MuonNextBCTOB::clearHeap();
   MetTOB::clearHeap();
   GenericTOB::clearHeap();
   CompositeTOB::clearHeap();

   m_simulationResult.reset();
   
   return StatusCode::SUCCESS;
}


StatusCode
TopoSteering::initializeAlgorithms() {
   TRG_MSG_INFO("initializing algorithms");
   if( ! structure().isConfigured() ) {
      TCS_EXCEPTION("L1Topo Steering has not been configured, can't run")
   }

   for(auto conn: m_structure.connectors()) {
      TCS::ConfigurableAlg * alg = conn->algorithm();
      if(alg) {
         TRG_MSG_INFO("initializing algorithm " << alg->name());
         if(m_histSvc) {
            alg->setL1TopoHistSvc(m_histSvc);
         }
         alg->initialize();
      }

   }

   return TCS::StatusCode::SUCCESS;
}


StatusCode
TopoSteering::setHistSvc(std::shared_ptr<IL1TopoHistSvc> histSvc) {
   TRG_MSG_INFO("setting L1TopoHistSvc ");
   m_histSvc = histSvc;
   return TCS::StatusCode::SUCCESS;
}


StatusCode
TopoSteering::saveHist() {
   if(m_histSvc) {
      m_histSvc->save();
   } else {
      TRG_MSG_WARNING("saveHist called without an L1TopoHistSvc being available");
   }
   return TCS::StatusCode::SUCCESS;
}


StatusCode
TopoSteering::executeEvent() {


   TRG_MSG_INFO("L1 TopoSteering: start executing event " << m_evtCounter << "-----------------------------------");


   if( ! structure().isConfigured() ) {
      TRG_MSG_INFO("L1Topo Steering has not been configured, can't run");
      TCS_EXCEPTION("L1Topo Steering has not been configured, can't run");
   }

   TRG_MSG_INFO("LateMuonTOB::heap().size = "<<LateMuonTOB::heap().size());
   inputEvent().print();

   // execute all connectors
   StatusCode sc = StatusCode::SUCCESS;
   TRG_MSG_INFO("Going to execute " << m_structure.outputConnectors().size() << " connectors");
   for(auto outConn: m_structure.outputConnectors()) {
      TRG_MSG_INFO("executing trigger line " << outConn.first);
      sc |= executeConnector(outConn.second);
      TRG_MSG_INFO("result of trigger line " << outConn.first << " : " << outConn.second->decision().decision());
   }   
               
   sc |= m_simulationResult.collectResult();

   m_simulationResult.globalDecision().print();

   TRG_MSG_INFO("finished executing event " << m_evtCounter++);
   return StatusCode::SUCCESS;
}



StatusCode
TopoSteering::executeTrigger(const std::string & TrigName) {
   if( ! structure().isConfigured() )
      TCS_EXCEPTION("TopoSteering has not been configured, can't run");
   
   DecisionConnector * outConn = m_structure.outputConnector(TrigName);

   StatusCode sc = executeConnector(outConn);

   m_simulationResult.collectResult(outConn);

   return sc;
}






StatusCode
TopoSteering::executeConnector(TCS::Connector *conn) {

   if (conn == NULL) {
     return StatusCode::FAILURE;
   }

   // caching
   if(conn->isExecuted())
      return conn->executionStatusCode();
  
   StatusCode sc(StatusCode::SUCCESS);

   if(conn->isInputConnector()) {
      //TRG_MSG_DEBUG("  ... executing input connector '" << conn->name() << "'");
      sc = executeInputConnector(dynamic_cast<InputConnector*>(conn));
   } else if(conn->isSortingConnector()) {
      //TRG_MSG_DEBUG("  ... executing sorting connector '" << conn->name() << "'");
      sc = executeSortingConnector(dynamic_cast<SortingConnector*>(conn));
   } else {
      //TRG_MSG_DEBUG("  ... executing decision connector '" << conn->name() << "'");
      sc = executeDecisionConnector(dynamic_cast<DecisionConnector*>(conn));
   }

   conn->setIsExecuted(true);
   conn->setExecutionStatusCode(sc);
  
   return sc;
}



StatusCode
TopoSteering::executeInputConnector(TCS::InputConnector *conn) {

   if (conn == NULL) {
     return StatusCode::FAILURE;
   }

   StatusCode sc(StatusCode::SUCCESS);

   // attaching data from inputEvent to input connector, depending on the configured input type

   const InputTOBArray * inputData = inputEvent().inputTOBs( conn->inputTOBType() );
   const bool hasInputOverflow = inputEvent().hasInputOverflow(conn->inputTOBType());
   conn->attachOutputData( inputData );
   conn->toggleInputOverflow(hasInputOverflow);

   TRG_MSG_DEBUG("  ... executing input connector '" << conn->name() << "' -> attaching '" << inputData->name() << "' of size " << inputData->size());

   return sc;
}



StatusCode
TopoSteering::executeSortingConnector(TCS::SortingConnector *conn) {

   if (conn == NULL) {
     return StatusCode::FAILURE;
   }

   StatusCode sc = StatusCode::SUCCESS;
  
   // execute all the prior connectors
   for( TCS::Connector* inputConn: conn->inputConnectors() ){
      sc &= executeConnector(inputConn);
      conn->toggleInputOverflow(conn->hasInputOverflow() ||
                                inputConn->hasInputOverflow());
   }
   TCS::SortingAlg* alg = conn->sortingAlgorithm();

   TOBArray * sortedOutput = new TOBArray(conn->outputName());

   sc &= executeSortingAlgorithm(alg, conn->inputConnector(), sortedOutput);

   TRG_MSG_DEBUG("  ... executing sorting connector '" << conn->name() << "' -> attaching '" << sortedOutput->name() << "' of size " << sortedOutput->size());

   conn->attachOutputData(sortedOutput);

   return sc;
}



StatusCode
TopoSteering::executeDecisionConnector(TCS::DecisionConnector *conn) {

   if (conn == NULL) {
     return StatusCode::FAILURE;
   }

   StatusCode sc = StatusCode::SUCCESS;
  
   // execute all the prior connectors
   for( TCS::Connector* inputConn: conn->inputConnectors() ){
      sc &= executeConnector(inputConn);
      conn->toggleInputOverflow(conn->hasInputOverflow() ||
                                inputConn->hasInputOverflow());

   }
   // execute
   TCS::DecisionAlg* alg = conn->decisionAlgorithm();

   // TRG_MSG_DEBUG("  ... executing decision connector '" << conn->name() << "' with " << conn->triggers().size() << " active trigger lines. The algorithm has " << alg->numberOutputBits() << " output bits.");

   // the output is one TOBArray per output line
   vector<TOBArray *> output( alg->numberOutputBits() );

   for(unsigned int i=0; i<alg->numberOutputBits(); ++i)
      output[i] = new TOBArray(conn->triggers()[i].name());

   sc &= executeDecisionAlgorithm(alg, conn->inputConnectors(), output, conn->m_decision);

   TRG_MSG_DEBUG("  ... executing decision connector '" << conn->name() << "' -> attaching output data:");
   for(TOBArray const * outarr : output) {
      TRG_MSG_DEBUG("           data '" << outarr->name() << "' of size " << outarr->size());
   }

   conn->attachOutputData(output);

   conn->setIsExecuted(true);
   conn->setExecutionStatusCode(sc);
   bool sortOverflow = false;
   for(TCS::Connector* inputConnector: conn->inputConnectors()) {
       // TODO DG-2017-04-18 the sort overflow (>10 TOBs) in the SortAlg is not implemented yet
       if(inputConnector->isSortingConnector()) {
           sortOverflow = (sortOverflow ||
                            dynamic_cast<SortingConnector*>(inputConnector)->sortingAlgorithm()->overflow());
       }
   }
   conn->m_decision.setOverflow(conn->hasInputOverflow() || sortOverflow);
   return sc;
}



StatusCode
TopoSteering::executeSortingAlgorithm(TCS::SortingAlg *alg,
                                      TCS::InputConnector* inputConnector,
                                      TCS::TOBArray * & sortedOutput) {
                                           
    TRG_MSG_DEBUG("  ... executing sorting alg '" << alg->fullname() << "'"
                  <<(m_useBitwise?" (bitwise)":""));

   const InputTOBArray * input = inputConnector->outputData();

   if(m_useBitwise) alg->sortBitCorrect(*input, *sortedOutput);
   else alg->sort(*input, *sortedOutput);

   return StatusCode::SUCCESS;
}



StatusCode
TopoSteering::executeDecisionAlgorithm(TCS::DecisionAlg *alg,
                                       const std::vector<Connector*> & inputConnectors,
                                       const std::vector<TCS::TOBArray *> & output,
                                       TCS::Decision & decision) {

    TRG_MSG_DEBUG("  ... executing decision alg '" << alg->fullname() << "'"
                  <<(m_useBitwise?" (bitwise)":""));

   if(inputConnectors.size()<1) {
      TCS_EXCEPTION("L1Topo Steering: Decision algorithm expects at least 1 input array but got 0");
   }

   std::vector<TCS::TOBArray const *> input;  // needs to be optimized
   for(const Connector* inConn: inputConnectors)
   {
      const SortingConnector * sc = dynamic_cast<const SortingConnector *>(inConn);
      if (sc==NULL) {
	 TCS_EXCEPTION("L1Topo Steering: Decision algorithm " << alg->name() << " could not cast as SortingConnector* the input connector " << inConn->name());
      }
      const TOBArray * tobA = dynamic_cast<const TOBArray *>( sc->outputData());
      if(tobA==NULL) {
         TCS_EXCEPTION("L1Topo Steering: Decision algorithm " << alg->name() << " expects TOBArray(s) as input, but did not get it from connector " << inConn->name());
      }
      input.push_back( tobA );
   }

   alg->reset();


   if(m_useBitwise) alg->processBitCorrect( input, output, decision);   
   else alg->process( input, output, decision );
   //TRG_MSG_ALWAYS("[XS1234sz]L1Topo Steering alg " << alg->name() << " has decision " << decision.decision());
     
   return StatusCode::SUCCESS;
}



void
TopoSteering::printDebugInfo() {
   TRG_MSG_INFO("Number of ClusterTOB  : " << ClusterTOB::instances() << " (" << ClusterTOB::heap().size() << " on the heap)");
   TRG_MSG_INFO("Number of JetTOB      : " << JetTOB::instances() <<  " (" << JetTOB::heap().size() << " on the heap)");
   TRG_MSG_INFO("Number of GenericTOB  : " << GenericTOB::instances() <<  " (" << GenericTOB::heap().size() << " on the heap)");
   TRG_MSG_INFO("Number of CompositeTOB: " << CompositeTOB::instances() <<  " (" << CompositeTOB::heap().size() << " on the heap)");
   TRG_MSG_INFO("Number of MuonTOB     : " << MuonTOB::instances() <<  " (" << MuonTOB::heap().size() << " on the heap)");
   TRG_MSG_INFO("Number of LateMuonTOB : " << LateMuonTOB::instances() <<  " (" << LateMuonTOB::heap().size() << " on the heap)");
}



void
TopoSteering::printConfiguration(std::ostream & o) const {
   o << "==========================" << endl
     << "TopoSteering configuration" << endl
     << "--------------------------" << endl;
   structure().print(o);
   o << "==========================" << endl;
}



void
TopoSteering::setMsgLevel( TrigConf::MSGTC::Level lvl ) {

   //const char* levelNames[TrigConf::MSGTC::NUM_LEVELS] = {"NIL","VERBOSE","DEBUG","INFO",
   //                                                       "WARNING","ERROR","FATAL","ALWAYS"};
   msg().setLevel( lvl );

   inputEvent().msg().setLevel(lvl);

   m_simulationResult.setMsgLevel( lvl );
}

void
TopoSteering::setAlgMsgLevel( TrigConf::MSGTC::Level lvl ) {

   m_AlgMsgLvl = lvl;

   for( Connector * conn : m_structure.connectors() ) {
      const ConfigurableAlg * alg = conn->algorithm();
      if(alg==nullptr) continue;
      alg->msg().setLevel(lvl);
   }
}
//----------------------------------------------------------
void TopoSteering::setHardwareBits(const std::bitset<numberOfL1TopoBits> &triggerBits,
                                   const std::bitset<numberOfL1TopoBits> &ovrflowBits)
{
    m_triggerHdwBits = triggerBits;
    m_ovrflowHdwBits = ovrflowBits;
}
//----------------------------------------------------------
void TopoSteering::propagateHardwareBitsToAlgos()
{
   for(auto connector : m_structure.outputConnectors()) {
       const string &connectorName = connector.first;
       TCS::DecisionConnector *outCon = connector.second;
        outCon->decisionAlgorithm()->resetHardwareBits();
        unsigned int pos = 0; // for multi-output algorithms pos is the output index
        for(const TXC::TriggerLine &trigger : outCon->triggers()){
            unsigned int bitNumber = trigger.counter();
            outCon->decisionAlgorithm()->setHardwareBits(pos,
                                                         m_triggerHdwBits[bitNumber],
                                                         m_ovrflowHdwBits[bitNumber]);
            pos++;
            TRG_MSG_DEBUG("propagating hardware bit (dec/ovr) "<<bitNumber
                          <<" to algo "<<connectorName<<"["<<pos<<"]"
                          <<" "<<m_triggerHdwBits[bitNumber]<<" /"
                          <<" "<<m_ovrflowHdwBits[bitNumber]);

        }
   }
}
//----------------------------------------------------------
void TopoSteering::setOutputAlgosFillBasedOnHardware(const bool &value)
{
   for(auto connector : m_structure.outputConnectors()) {
       const string &connectorName = connector.first;
       TCS::DecisionConnector *outCon = connector.second;
       if(outCon) {
           outCon->decisionAlgorithm()->setFillHistosBasedOnHardware(value);
       } else {
           TRG_MSG_DEBUG("skipping invalid DecisionConnector '"<<connectorName<<"'");
       }
   }
}
//----------------------------------------------------------
void TopoSteering::setOutputAlgosSkipHistograms(const bool &value)
{
   for(auto connector : m_structure.outputConnectors()) {
       const string &connectorName = connector.first;
       TCS::DecisionConnector *outCon = connector.second;
       if(outCon) {
           outCon->decisionAlgorithm()->setSkipHistos(value);
       } else {
           TRG_MSG_DEBUG("skipping invalid DecisionConnector '"<<connectorName<<"'");
       }
   }
}
//----------------------------------------------------------
