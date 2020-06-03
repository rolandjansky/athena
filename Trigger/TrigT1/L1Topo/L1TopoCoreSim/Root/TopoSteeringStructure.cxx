/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#include "L1TopoCoreSim/TopoSteeringStructure.h"

#include "L1TopoCommon/StringUtils.h"
#include "L1TopoCommon/Exception.h"
#include "L1TopoCommon/Types.h"

#include "L1TopoConfig/LayoutConstraints.h"
#include "L1TopoConfig/L1TopoMenu.h"

#include "L1TopoInterfaces/DecisionAlg.h"
#include "L1TopoInterfaces/SortingAlg.h"

#include "L1TopoCoreSim/Connector.h"
#include "L1TopoCoreSim/InputConnector.h"
#include "L1TopoCoreSim/SortingConnector.h"
#include "L1TopoCoreSim/DecisionConnector.h"

#include "L1TopoHardware/L1TopoHardware.h"

#include <set>
#include <iomanip>
#include <boost/lexical_cast.hpp>

using namespace std;
using namespace TCS;
using boost::lexical_cast;

namespace {
   uint32_t interpretGenericParam(const std::string& parvalue) {
      uint32_t val;
      try {
         val  = lexical_cast<uint32_t, string>(parvalue);
      }
      catch(const boost::bad_lexical_cast & bc) {
         if( parvalue.size()>=3 && parvalue[0]==':' and parvalue[parvalue.size()-1]==':' ) {

            auto x = L1TopoHardwareParameters.find(parvalue.substr(1,parvalue.size()-2));

            string parname = parvalue.substr(1,parvalue.size()-2);

            if( x != L1TopoHardwareParameters.end()) {
               val = x->second.value;
            } else {
               cout << "Hardware constrained parameters are" << endl;
               for(auto & x : L1TopoHardwareParameters)
                  cout << "   " << x.first << endl;

               TCS_EXCEPTION("Generic parameter value " << parvalue << " has the hardware contrained parameter format, but '" << parname << "' is not listed in L1TopoHardware.cxx");
            }
         } else {
            TCS_EXCEPTION("Generic parameter value " << parvalue << " is not a uint32_t and does not match the hardware contrained parameter specification ':<parname>:' ");
         }
      }
      return val;
   }
}


TCS::TopoSteeringStructure::TopoSteeringStructure() :
   m_parameters(2 * LayoutConstraints::maxComponents(), nullptr)
{}


TopoSteeringStructure::~TopoSteeringStructure() {
   for( Connector* c: m_connectors ) 
      delete c;
   for( ParameterSpace * ps : m_parameters )
      delete ps;
}


StatusCode
TopoSteeringStructure::reset() {
   for(Connector* conn: m_connectors)
      conn->reset();
   return StatusCode::SUCCESS;
}


void
TCS::TopoSteeringStructure::print(std::ostream & o) const {
   o << "Topo Steering Structure" << endl
     << "-----------------------" << endl;

   o << "Output summary:" << endl;
   for(auto conn: outputConnectors() ) {
      o << "  " << *conn.second << endl;
   }

   o << endl 
     << "Algorithm detail:" << endl;
   for(auto nc: outputConnectors() ) {
      DecisionConnector * conn = nc.second;
      unsigned int firstBit          = conn->decision().firstBit();
      unsigned int lastBit           = conn->numberOutputBits() + firstBit - 1;
      const ConfigurableAlg* alg     = conn->algorithm();
      const Connector* inputconn     = conn->inputConnectors().back();
      const ConfigurableAlg* sortalg = inputconn->algorithm();
      o << std::setw(2) << "bits " << firstBit << "-" << lastBit << ": " << conn->name() << " [input " << inputconn->name() <<"]" << endl;
      o << *alg << endl << endl;
      o << *sortalg << endl;
   }
}


void
TCS::TopoSteeringStructure::printParameters(std::ostream & o) const {
   unsigned int idx(0);
   for(const ParameterSpace* ps: m_parameters) {
      if(ps && ps->isInitialized())
         o << "pos " << std::setw(2) << idx << ": " << ps << endl;
      idx++;
   }
}




TCS::StatusCode
TCS::TopoSteeringStructure::setupFromMenu(const TXC::L1TopoMenu& menu, bool debug) {

   if(debug)
      cout << "/***************************************************************************/" << endl
           << "           L1Topo steering structure: configuring from L1 Topo Menu          " << endl
           << "/***************************************************************************/" << endl;

   set<TCS::inputTOBType_t> neededInputs;
   if(debug)
      cout << "... building sorting connectors" << endl;

   for( const TXC::L1TopoConfigAlg & configalgo: menu.getL1TopoConfigAlgs() ) {   

      if(!configalgo.isSortAlg()) continue;
      
      // Input
      for(string & algoInput : configalgo.getInputNames()) {
         TCS::inputTOBType_t en = inputType(algoInput);
         neededInputs.insert(en);
      }

      // create connector
      SortingConnector * conn = new SortingConnector(configalgo.getInputNames()[0], configalgo.fullname(), configalgo.output());
      if(debug)
         cout << "Adding sorting connector " << "[" << *conn << "]" << endl;
      addSortingConnector( conn );
   }




   if(debug)
      cout << "... building output connectors" << endl;
   for( const TXC::L1TopoConfigAlg & configalgo: menu.getL1TopoConfigAlgs() ) {     

      if(!configalgo.isDecAlg()) continue;

      for( const TXC::InputElement & input : configalgo.getInputs() ) {
         if( sortingConnector(input.value) == 0 ) {
            TCS_EXCEPTION("L1TopoSteering: Decision algo ( " << configalgo.fullname() << " ) input is not defined: " << input.value);
         }
      }

      DecisionConnector * conn = new DecisionConnector(configalgo.name(), configalgo.getInputNames(), configalgo.fullname(), configalgo.getOutputNames());
      conn->m_decision.setNBits( configalgo.getOutputs().size() );

      for(const std::string & output: conn->outputNames()) {
         const TXC::TriggerLine & trigger = menu.getL1TopoConfigOutputList().getTrigger(output);
         if(trigger.name()!="UNDEF")
            conn->m_triggers.push_back(trigger);
      }
      if(debug)
         cout << "Adding decision connector " << "[" << *conn << "]" << endl;
      addDecisionConnector( conn );

   }


   if(debug)
      cout << "... building input connectors" << endl;
   for(auto sortConn : m_sortedLookup) {
      const string & in = sortConn.second->inputNames()[0]; // name of input

      if( m_inputLookup.count(in) > 0 ) continue; // InputConnector already exists

      InputConnector * conn = new InputConnector(in);
      m_connectors.push_back(conn);
      m_inputLookup[in] = conn;
      if(debug)
         cout << "Adding input connector " << "[" << *conn << "]" << endl;
   }

   
   // link the connector objects together
   StatusCode sc = linkConnectors();
   
   // instantiate the algorithms from the algorithm names in the connectors
   if(debug)
      cout << "... instantiating algorithms" << endl;
   sc &= instantiateAlgorithms(debug);
   

   // iterate through OutputList elements
   if(debug) {
      cout << "... checking output list" << endl;
      cout << menu.getL1TopoConfigOutputList().getOutputList().size() << " output algorithms for " 
           << menu.getL1TopoConfigOutputList().getTriggerLines().size() << " trigger lines." << endl;
   }
   
   // set algorithm parameters
   if(debug)
      cout << "... setting algorithm parameters" << endl;
   
   for( const TXC::L1TopoConfigAlg & configalgo: menu.getL1TopoConfigAlgs() ) {

      ConfigurableAlg * alg = AlgFactory::instance().algorithm(configalgo.name());

      alg->setAlgoId( configalgo.algoID() );

      if(debug)
         cout << "Algorithm " << alg->name() << endl << "  (reading parameters)" << endl;

      if(alg->isDecisionAlg())
         ((DecisionAlg *) alg)->setNumberOutputBits(configalgo.getOutputs().size());

      // create ParameterSpace for this algorithm
      ParameterSpace * ps = new ParameterSpace(alg->name());

      for(TXC::RegisterParameter pe: configalgo.getParameters()) {
         
         string   name = pe.name;
         uint32_t val = lexical_cast<uint32_t, string>(pe.value);
         uint32_t pos  = pe.position;
         uint32_t sel  = pe.selection;
         
         if(debug)
            cout << "  parameter " << pos << ": " << setw(20) << left << name << " value = " << setw(3) << left << val << " (selection " << sel << ")" << endl;
         ps->addParameter( name, val, sel);
      }


      for(TXC::FixedParameter pe: configalgo.getFixedParameters()) {
         
         string   name = pe.name;
         uint32_t val = interpretGenericParam(pe.value);
         if(name=="NumResultBits") {
            if(val != configalgo.getOutputs().size()) {
               TCS_EXCEPTION("Algorithm " << name << " parameter OutputBits (" << val << ") is different from output size (" << configalgo.getOutputs().size() << ")");
            }
            continue; // ignore this, because it is defined through the output list
         }
         if(debug)
            cout << "  fixed parameter : " << setw(20) << left << name << " value = " << setw(3) << left << val << endl;
         ps->addParameter( name, val);
      }


      if(debug)
         cout << "  (setting parameters)" << endl;
      alg->setParameters( *ps );

      if( alg->isDecisionAlg() ) {
         if( m_parameters[alg->algoId()] != nullptr ) {
            TCS_EXCEPTION("Decision algorithm " << alg->name() << " has algoId " << alg->algoId() << " which is already used");
         }
         m_parameters[alg->algoId()] = ps;
      } else if( alg->isSortingAlg() ) {
         if( m_parameters[alg->algoId() + LayoutConstraints::maxComponents()] != nullptr ) {
            TCS_EXCEPTION("Sorting algorithm " << alg->name() << " has algoId " << alg->algoId() << " which is already used");
         }
         m_parameters[alg->algoId() + LayoutConstraints::maxComponents()] = ps;
      } else {
	// newed parameters unused so delete to avoid memory leak
	delete ps;
	ps=0;
      }
   }

   m_isConfigured = true;

   if(debug)
      cout << "... L1TopoSteering successfully configured" << endl;
   
   return sc;
}

TCS::StatusCode
TopoSteeringStructure::addSortingConnector(SortingConnector * conn) {
   m_connectors.push_back(conn);
   for( const string & output : conn->outputNames() )
      m_sortedLookup[output] = conn;
   return StatusCode::SUCCESS;
}

TCS::StatusCode
TopoSteeringStructure::addDecisionConnector(DecisionConnector * conn) {
   m_connectors.push_back(conn);
   for( const string & output : conn->outputNames() )
      m_outputLookup[output] = conn;
   return StatusCode::SUCCESS;
}


TCS::StatusCode
TopoSteeringStructure::linkConnectors() {

   for(TCS::Connector * conn: m_connectors)
      for(const std::string & inconn: conn->inputNames())
         conn->inputConnectors().push_back( connector(inconn) );

   return StatusCode::SUCCESS;
}


TCS::StatusCode
TCS::TopoSteeringStructure::instantiateAlgorithms(bool debug) {

   for(TCS::Connector* conn: m_connectors) {

      if(conn->isInputConnector()) continue;

      // for each connector instantiate the algorithm and add to connector
      const std::string & alg = conn->algorithmName();
       
      // split into name and type
      std::string algType(alg, 0, alg.find('/'));
      std::string algName(alg, alg.find('/')+1);

      ConfigurableAlg * algInstance = TCS::AlgFactory::instance().algorithm(algName);
      if(algInstance==0) {
         if(debug)
            cout << "Instantiating " << alg << endl;
         algInstance = TCS::AlgFactory::create(algType, algName);
      } else {
         if(algInstance->className() != algType) {
            TCS_EXCEPTION("L1 TopoSteering: duplicate algorithm names:  algorithm " << algName << " has already been instantiated but with different type");
         }
      }
      conn->setAlgorithm(algInstance);
   }
   return StatusCode::SUCCESS;
}


TCS::Connector *
TCS::TopoSteeringStructure::connector(const std::string & connectorName) const {
   for( TCS::Connector* conn: m_connectors ) {
      if( conn->name() == connectorName )
         return conn;
   }
   TCS_EXCEPTION("L1Topo Steering: can not find connector of name " << connectorName << ". Need to abort!");
   return 0;
}


SortingConnector* 
TopoSteeringStructure::sortingConnector(const std::string & connectorName) const {
   SortingConnector * sc = nullptr;
   for( TCS::Connector* conn: m_connectors ) {
      if( conn->name() == connectorName ) {
         sc = dynamic_cast<SortingConnector*>(conn);
         if(sc==nullptr) {
            TCS_EXCEPTION("TopoSteeringStructure: connector of name " << connectorName << " exists, but is not a SortingConnector. Need to abort!");            
         }
      }
   }
   if(sc==nullptr) {
      TCS_EXCEPTION("TopoSteeringStructure: can not find SortingConnector of name " << connectorName << ". Need to abort!");
   }
   return sc;
}


TCS::DecisionConnector *
TCS::TopoSteeringStructure::outputConnector(const std::string & output) const {
   auto c = m_outputLookup.find(output);
   if( c != m_outputLookup.end() )
      return c->second;
   TCS_EXCEPTION("L1Topo Steering: can not find output connector of that produces " << output << ". Need to abort!");
   return 0;
}
