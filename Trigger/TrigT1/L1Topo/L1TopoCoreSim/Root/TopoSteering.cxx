//  TopoSteering.cxx
//  Framework to execute simulation algorithms

#include "L1TopoInterfaces/AlgFactory.h" 

#include "L1TopoInterfaces/ConfigurableAlg.h"
#include "L1TopoInterfaces/ParameterSpace.h"
#include "L1TopoInterfaces/SortingAlg.h"
#include "L1TopoInterfaces/DecisionAlg.h"
#include "L1TopoCommon/Exception.h"
#include "L1TopoEvent/GenericTOB.h"

#include "L1TopoConfig/L1TopoMenu.h"
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
using boost::lexical_cast;


TopoSteering::TopoSteering()
{}

TopoSteering::~TopoSteering() {
   AlgFactory::destroy_instance();
}



StatusCode
TopoSteering::setupFromConfiguration(const TXC::L1TopoMenu& menu) {

   StatusCode sc = m_structure.setupFromMenu( menu );

   // configure layout of the simulation result
   sc &= m_simulationResult.setupFromMenu( menu, m_structure.outputConnectors() );

   return sc;
}



StatusCode
TopoSteering::reset() {

   inputEvent().clear();

   m_structure.reset();

   GenericTOB::clearHeap();
   ClusterTOB::clearHeap();
   JetTOB::clearHeap();
   MuonTOB::clearHeap();
   CompositeTOB::clearHeap();

   m_simulationResult.reset();
   
   return StatusCode::SUCCESS;
}


StatusCode
TopoSteering::initializeAlgorithms() {
   cout << "L1 TopoSteering: initializing algorithms" << endl;
   if( ! structure().isConfigured() ) {
      TCS_EXCEPTION("L1Topo Steering has not been configured, can't run")
   }

   for(auto conn: m_structure.connectors()) {
      TCS::ConfigurableAlg * alg = conn->algorithm();
      if(alg) alg->initialize();
   }

   return TCS::StatusCode::SUCCESS;
}


StatusCode
TopoSteering::executeEvent() {

   cout << "L1 TopoSteering: start executing event" << endl;
   if( ! structure().isConfigured() ) {
      TCS_EXCEPTION("L1Topo Steering has not been configured, can't run")
   }

   // execute all connectors
   for(auto outConn: m_structure.outputConnectors()) {
      cout << "L1 TopoSteering: executing output decision " << outConn.first << endl;
      executeConnector(outConn.second);
   }   

   m_simulationResult.collectResult();
   cout << "L1 TopoSteering: finished executing event" << endl;
   return TCS::StatusCode::SUCCESS;
}



StatusCode
TopoSteering::executeTrigger(const std::string & TrigName) {
   if( ! structure().isConfigured() ) {
      TCS_EXCEPTION("TopoSteering has not been configured, can't run");
   }
   
   DecisionConnector * outConn = m_structure.outputConnector(TrigName);
   
   return executeConnector(outConn);
}



StatusCode
TopoSteering::executeConnector(TCS::Connector *conn) {

   // caching
   if(conn->isExecuted())
      return conn->executionStatusCode();
  
   StatusCode sc(StatusCode::SUCCESS);

   cout << "  ... executing connector '" << conn->name() << "'" << endl;

   if(conn->isInputConnector()) {
      sc = executeInputConnector(dynamic_cast<InputConnector*>(conn));
   } else if(conn->isSortingConnector()) {
      sc = executeSortingConnector(dynamic_cast<SortingConnector*>(conn));
   } else {
      sc = executeDecisionConnector(dynamic_cast<DecisionConnector*>(conn));
   }

   conn->setIsExecuted(true);
   conn->setExecutionStatusCode(sc);
  
   return sc;
}



StatusCode
TopoSteering::executeInputConnector(TCS::InputConnector *conn) {

   StatusCode sc(StatusCode::SUCCESS);

   // attaching data from inputEvent to input connector, depending on the configured input type
   const InputTOBArray * inputData = inputEvent().inputTOBs( conn->inputTOBType() );

   conn->attachOutputData( inputData );

   cout << "  ... attaching input data '" << inputData->name() << "' of size " << inputData->size() << " to connector '" << conn->name() << "'" << endl;

   return sc;
}



StatusCode
TopoSteering::executeSortingConnector(TCS::SortingConnector *conn) {

   StatusCode sc = StatusCode::SUCCESS;
  
   // execute all the prior connectors
   for( TCS::Connector* inputConn: conn->inputConnectors() )
      sc &= executeConnector(inputConn);

   TCS::SortingAlg* alg = conn->sortingAlgorithm();

   TOBArray * sortedOutput = new TOBArray(conn->outputName());

   sc &= executeSortingAlgorithm(alg, conn->inputConnector(), sortedOutput);

   cout << "  ... attaching sorted data '" << sortedOutput->name() << "' of size " << sortedOutput->size() << " to connector '" << conn->name() << "'" << endl;

   conn->attachOutputData(sortedOutput);

   return sc;
}



StatusCode
TopoSteering::executeDecisionConnector(TCS::DecisionConnector *conn) {

   StatusCode sc = StatusCode::SUCCESS;
  
   // execute all the prior connectors
   for( TCS::Connector* inputConn: conn->inputConnectors() ) {
      sc &= executeConnector(inputConn);
   }
  
   // execute
   TCS::DecisionAlg* alg = conn->decisionAlgorithm();

   cout << "  ... executing decision connector '" << conn->name() << "' with " << conn->triggers().size() << " active trigger lines. The algorithm has " << alg->numberOutputBits() << " output bits." << endl;

   // the output is one TOBArray per output line
   vector<TOBArray *> output( alg->numberOutputBits() );

   for(unsigned int i=0; i<alg->numberOutputBits(); ++i)
      output[i] = new TOBArray(conn->triggers()[i].name());

   sc &= executeDecisionAlgorithm(alg, conn->inputConnectors(), output, conn->m_decision);

   cout << "  ... attaching output data to connector '" << conn->name() << "'" << endl;
   for(TOBArray const * outarr : output) {
      cout << "      data '" << outarr->name() << "' of size " << outarr->size() << endl;
   }

   conn->attachOutputData(output);

   conn->setIsExecuted(true);
   conn->setExecutionStatusCode(sc);
  
   return sc;
}



StatusCode
TopoSteering::executeSortingAlgorithm(TCS::SortingAlg *alg,
                                      TCS::InputConnector* inputConnector,
                                      TCS::TOBArray * & sortedOutput) {
                                           
   cout << "  ... executing sorting alg '" << alg->fullname() << "'" << endl;

   const InputTOBArray * input = inputConnector->outputData();

   alg->sort(*input, *sortedOutput);

   return StatusCode::SUCCESS;
}



StatusCode
TopoSteering::executeDecisionAlgorithm(TCS::DecisionAlg *alg,
                                       const std::vector<Connector*> & inputConnectors,
                                       const std::vector<TCS::TOBArray *> & output,
                                       TCS::Decision & decision) {
   
   cout << "  ... executing decision alg '" << alg->fullname() << "'" << endl;

   if(inputConnectors.size()<1) {
      TCS_EXCEPTION("L1Topo Steering: Decision algorithm expects at least 1 input array but got 0");
   }

   std::vector<TCS::TOBArray const *> input;  // needs to be optimized
   for(const Connector* inConn: inputConnectors)
   {
      const SortingConnector * sc = dynamic_cast<const SortingConnector *>(inConn);
      const TOBArray * tobA = dynamic_cast<const TOBArray *>( sc->outputData());
      if(tobA==0) {
         TCS_EXCEPTION("L1Topo Steering: Decision algorithm " << alg->name() << " expects TOBArray(s) as input, but did not get it from connector " << inConn->name());
      }
      input.push_back( tobA );
   }

   alg->reset();
   
   alg->process( input, output, decision );
      
   return StatusCode::SUCCESS;
}



void
TopoSteering::printDebugInfo() {
   cout << "Number of ClusterTOB  : " << ClusterTOB::instances() << " (" << ClusterTOB::heap().size() << " on the heap)" << endl;
   cout << "Number of JetTOB      : " << JetTOB::instances() <<  " (" << JetTOB::heap().size() << " on the heap)" << endl;
   cout << "Number of GenericTOB  : " << GenericTOB::instances() <<  " (" << GenericTOB::heap().size() << " on the heap)" << endl;
   cout << "Number of CompositeTOB: " << CompositeTOB::instances() <<  " (" << CompositeTOB::heap().size() << " on the heap)" << endl;
}



void
TopoSteering::printConfiguration(std::ostream & o) const {
   o << "TopoSteering configuration" << endl
     << "==========================" << endl;
   structure().print(o);
}



