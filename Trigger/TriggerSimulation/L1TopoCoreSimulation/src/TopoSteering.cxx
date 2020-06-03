/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//
//  TopoSteering.cxx
//  L1TopoCoreSimulation
//
//  Created by Joerg Stelzer on 11/20/12.
//

#include "L1TopoCoreSimulation/TopoSteering.h"
#include "L1TopoCoreSimulation/Exception.h"
#include "L1TopoCoreSimulation/Connector.h"
#include "L1TopoCoreSimulation/ConfigurableAlg.h"
#include "L1TopoCoreSimulation/ParameterSpace.h"
#include "L1TopoCoreSimulation/SortingAlg.h"
#include "L1TopoCoreSimulation/DecisionAlg.h"
#include "L1TopoCoreSimulation/GenericTOB.h"

using namespace std;

TCS::TopoSteering::TopoSteering() :
m_isConfigured(false)
{}

// Configuration

TCS::StatusCode
TCS::TopoSteering::setupMockConfiguration() {
   m_isConfigured = true;
   
   m_connectors.push_back(new Connector("Clusters", TCS::CLUSTER) ); // input connectors don't have algorithm or predecessors
   
   m_connectors.push_back(new Connector("SortedClusters", TCS::Connector::SORTED,
                                        TCS::Connector::vectorize("ClusterSortingAlg/ClusterSorting"),
                                        TCS::Connector::vectorize("Clusters")) );
   
   m_connectors.push_back(new Connector("OppositeClusters", TCS::Connector::DECISION,
                                        TCS::Connector::vectorize("DeltaPhiLeading/ClusterDeltaPhi_230"),
                                        TCS::Connector::vectorize("SortedClusters")) );
   
   m_output.push_back("OppositeClusters");
   
   StatusCode sc = instantiateAlgorithms();
   
   sc &= linkConnectors();
   
   ParameterSpace parSpace;
   parSpace.addParameterArray("ClusterSorting")
   .addParameter("NumberOfClusters", 3)
   .addParameter("ReverseOrder", 1);
   
   setAlgorithmParameters(parSpace);

   ConfigurableAlg * alg = AlgFactory::instance().algorithm("ClusterDeltaPhi_230");
   if(alg->isDecisionAlg()) {
      DecisionAlg * decAlg = (DecisionAlg *) alg;
      decAlg->decision().setFirstBit(4);
      decAlg->decision().setNBits(3);
   }

   checkConfigurationConsistency();

   m_simulationResult.setConfiguration(m_connectors);

   return sc;
}

TCS::StatusCode
TCS::TopoSteering::checkConfigurationConsistency() const {
   typedef std::vector<TCS::DecisionAlg*> vecDecAlg_t;

   vecDecAlg_t algVec;

   for(std::vector<TCS::Connector*>::const_iterator conn = m_connectors.begin();
       conn != m_connectors.end(); ++conn ) {
      for(std::vector<TCS::ConfigurableAlg*>::const_iterator alg = (*conn)->algorithms().begin();
          alg != (*conn)->algorithms().end(); ++alg ) {
         if( (*alg)->isDecisionAlg() ) {
            algVec.push_back((DecisionAlg*) *alg);
         }
      }
   }

   for(vecDecAlg_t::const_iterator alg1 = algVec.begin();
       alg1 != algVec.end(); ++alg1)
   {
      vecDecAlg_t::const_iterator alg2 = alg1; ++alg2;
      for(; alg2 != algVec.end(); ++alg2) {
         const DecisionAlg * a1 = *alg1;
         const DecisionAlg * a2 = *alg2;
         bool noOverlap = (a1->decision().firstBit() > a2->decision().lastBit()) ||
         (a2->decision().firstBit() > a1->decision().lastBit());
         if(! noOverlap) {
            TCS_EXCEPTION("Overlap in decision bit range between algorithms " << a1->name() << " and " << a2->name())
         }
      }
   }
   return StatusCode::SUCCESS;
}


TCS::StatusCode
TCS::TopoSteering::linkConnectors() {
   for(std::vector<TCS::Connector*>::iterator conn = m_connectors.begin(); conn != m_connectors.end(); ++conn ) {
      for(std::vector<std::string>::const_iterator alg = (*conn)->inputNames().begin();
          alg != (*conn)->inputNames().end(); ++alg )
      {
         (*conn)->inputConnectors().push_back( findConnector(*alg) );
      }
   }
   return StatusCode::SUCCESS;
}



TCS::StatusCode
TCS::TopoSteering::instantiateAlgorithms() {

   StatusCode sc(StatusCode::SUCCESS);

   // go through all connectors
   for(std::vector<TCS::Connector*>::iterator conn = m_connectors.begin();
       conn != m_connectors.end(); ++conn ) {

      // go through all algorithm names of each connector,
      // instantiate them, and add to connector
      for(std::vector<std::string>::const_iterator alg = (*conn)->algorithmNames().begin();
          alg != (*conn)->algorithmNames().end(); ++alg ) {
         
         std::string algType(*alg, 0, alg->find('/'));
         std::string algName(*alg, alg->find('/')+1);

         ConfigurableAlg * algInstance = TCS::AlgFactory::create(algType, algName);

         (*conn)->algorithms().push_back(algInstance);

      }
   }
   return sc;
}


TCS::StatusCode
TCS::TopoSteering::setAlgorithmParameters(const TCS::ParameterSpace & parameterSpace) {
   
   for(std::vector<TCS::Connector*>::iterator conn = m_connectors.begin();
       conn != m_connectors.end(); ++conn ) {
      
      // go through all algorithm names of each connector,
      // instantiate them, and add to connector
      for(std::vector<TCS::ConfigurableAlg*>::const_iterator alg = (*conn)->algorithms().begin();
          alg != (*conn)->algorithms().end(); ++alg ) {
         
         if(!parameterSpace.contains((*alg)->name())) {
            continue;
         }
         
         // setting alg parameters
         (*alg)->setParameters( parameterSpace.parameters( (*alg)->name() ) );
      }
   }
   
   return StatusCode::SUCCESS;
}


// ==============================================================================
//
// event Execution
//
// ==============================================================================

TCS::StatusCode
TCS::TopoSteering::reset() {

   // clear the inputEvent
   inputEvent().clear();

   for(std::vector<TCS::Connector*>::iterator conn = m_connectors.begin();
       conn != m_connectors.end(); ++conn)
   {
      (*conn)->reset();
   }

   GenericTOB::clearHeap();
   ClusterTOB::clearHeap();
   JetTOB::clearHeap();
   CompositeTOB::clearHeap();

   m_simulationResult.reset();
   
   return StatusCode::SUCCESS;
}


TCS::StatusCode
TCS::TopoSteering::executeEvent() {
   if( ! m_isConfigured) {
      TCS_EXCEPTION("TopoSteering has not been configured, can't run")
   }
   for(std::vector<std::string>::const_iterator out = m_output.begin(); out != m_output.end(); ++out) {
      Connector * outConn = findConnector(*out);
      executeConnector(outConn);
//      DecisionAlg * dec = dynamic_cast<DecisionAlg*>(outConn->algorithms().back());
//      cout << dec->decision() << endl;
   }

   m_simulationResult.collectResult();

   return TCS::StatusCode::SUCCESS;
}


TCS::StatusCode
TCS::TopoSteering::executeConnector(TCS::Connector *conn) {
   // caching
   if(conn->isExecuted()) {
      return conn->executionStatusCode();
   }
   
   conn->setIsExecuted(true);
   
   StatusCode sc = StatusCode::SUCCESS;

   // execute all the prior connectors
   for(std::vector<TCS::Connector*>::iterator inputConn = conn->inputConnectors().begin();
       inputConn != conn->inputConnectors().end(); ++inputConn) {
      sc &= executeConnector(*inputConn);
   }
   
   // execute
   cout << "executing connector " << conn->name() << endl;
   if(conn->isInputConnector()) {
      // attaching data from inputEvent to input connector, depending on the configured input type
      conn->attachOutputData(inputEvent().inputTOBs(conn->inputTOBType()));
      cout << "attached " << conn->outputData()->name() << endl;
   } else {
      for(std::vector<TCS::ConfigurableAlg*>::iterator alg = conn->algorithms().begin();
          alg != conn->algorithms().end(); ++alg) {
         sc &= executeAlgorithm(*alg, conn);
      }
   }
   
   conn->setExecutionStatusCode(sc);
   
   return sc;
}


TCS::StatusCode
TCS::TopoSteering::executeAlgorithm(TCS::ConfigurableAlg *alg, Connector* connector) {
   
   DataArray * output(0);
   
   StatusCode sc;

   // execute algorithms in different ways, depending on type (sort or decision alg)
   
   SortingAlg * sortAlg = dynamic_cast<SortingAlg*>(alg);
   if(sortAlg!=0) {
      sc = executeSortingAlgorithm(sortAlg, connector->inputConnectors(), output);
   } else {
      DecisionAlg * decAlg = dynamic_cast<DecisionAlg*>(alg);
      if(decAlg!=0) {
         sc = executeDecisionAlgorithm(decAlg, connector->inputConnectors(), output);
      } else {
         TCS_EXCEPTION("Found algorithm " << alg->name() << " that is neither sorting nor decision alg")
      }
   }
   
   connector->attachOutputData(output);
   
   return sc;
}


TCS::StatusCode
TCS::TopoSteering::executeSortingAlgorithm(TCS::SortingAlg *alg,
                                           const std::vector<TCS::Connector*> & inputConnectors,
                                           TCS::DataArray * & output) {
                                           
   cout << "executing sorting " << *alg << endl;
   if(inputConnectors.size()!=1) {
      TCS_EXCEPTION("Sorting algorithm expects exactly 1 input array but got " << inputConnectors.size());
   }

   const InputTOBArray * input = dynamic_cast<const InputTOBArray *>(inputConnectors[0]->outputData());
   TOBArray * algOutput = new TOBArray("SortOutput");
   
   alg->sort(*input, *algOutput);

   output = algOutput;
   
   return StatusCode::SUCCESS;
}


TCS::StatusCode
TCS::TopoSteering::executeDecisionAlgorithm(TCS::DecisionAlg *alg,
                                            const std::vector<Connector*> & inputConnectors,
                                            TCS::DataArray * & output) {
   
   cout << "executing decision " << *alg << endl;
   if(inputConnectors.size()<1) {
      TCS_EXCEPTION("Decision algorithm expects at least 1 input array but got 0");
   }

   std::vector<TCS::TOBArray> input;
   for(std::vector<Connector*>::const_iterator inConn = inputConnectors.begin();
       inConn != inputConnectors.end(); ++inConn)
   {
      input.push_back( * dynamic_cast<const TOBArray *>( (*inConn)->outputData()) );
   }

   TOBArray * algOutput = new TOBArray("DecisionOutput");
   
   alg->decision().reset();
   
   alg->process(input, *algOutput, alg->decision());
      
   output = algOutput;
   
   return StatusCode::SUCCESS;
}

// helper functions

TCS::Connector *
TCS::TopoSteering::findConnector(const std::string & connectorName) const {
   for(std::vector<TCS::Connector*>::const_iterator conn = m_connectors.begin();
       conn != m_connectors.end(); ++conn) {
      if( (*conn)->name() == connectorName ) return *conn;
   }
   TCS_EXCEPTION("TopoSteering: can not find connector of name " << connectorName << ". Need to abort!");
   return 0;
}

void
TCS::TopoSteering::printDebugInfo() {
   cout << "Number of ClusterTOB  : " << ClusterTOB::instances() << " (" << ClusterTOB::heap().size() << " on the heap)" << endl;
   cout << "Number of JetTOB      : " << JetTOB::instances() <<  " (" << JetTOB::heap().size() << " on the heap)" << endl;
   cout << "Number of GenericTOB  : " << GenericTOB::instances() <<  " (" << GenericTOB::heap().size() << " on the heap)" << endl;
   cout << "Number of CompositeTOB: " << CompositeTOB::instances() <<  " (" << CompositeTOB::heap().size() << " on the heap)" << endl;
}
