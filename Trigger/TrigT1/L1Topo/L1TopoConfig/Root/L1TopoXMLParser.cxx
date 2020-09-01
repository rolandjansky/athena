/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// L1Topo libraries
#include "L1TopoConfig/L1TopoXMLParser.h"
#include "L1TopoConfig/L1TopoConfigAlg.h"
#include "L1TopoConfig/L1TopoConfigOutputList.h"
#include "L1TopoConfig/L1TopoConfigGlobal.h"
#include "L1TopoConfig/L1TopoMenu.h"
#include "L1TopoConfig/L1TopoXMLHelper.h"
#include "L1TopoCommon/Exception.h"

// cpp libraries
#include <iostream>
#include <vector>
#include <string>

// boost libraries
#include <boost/foreach.hpp>

using namespace std;
using namespace TXC;
using boost::property_tree::ptree;

namespace pt = boost::property_tree;


L1TopoXMLParser::L1TopoXMLParser() :
   TrigConfMessaging("L1TopoXMLParser")
{}

L1TopoXMLParser::~L1TopoXMLParser()
{}


void
L1TopoXMLParser::printConfiguration() const {
  cout << "************************** L1TopoXMLParser **********************" << endl;
  m_menu.print();
  cout << "*****************************************************************" << endl;
}


void
L1TopoXMLParser::readConfiguration(const std::string & inputfile) {
   // populate the ptree m_configuration with the structure of the xml input file
   ptree inputTree;
   read_xml(inputfile, inputTree); // 3 --> trim_whitespace

   m_menuPT = inputTree.get_child("TOPO_MENU");

   m_isValidConfigurtion = true;
   TRG_MSG_INFO("Read " << inputfile << " successfully!");
}


void TXC::L1TopoXMLParser::parseConfiguration() {

   m_menu.setName(getAttribute(m_menuPT,"menu_name"));
   m_menu.setVersion(getAttribute(m_menuPT,"menu_version"));
  
   // iterate through elements of topomenu
   for(const boost::property_tree::ptree::value_type &x: m_menuPT) {

      string menuElementName = x.first;
      if (menuElementName=="<xmlattr>" || menuElementName=="<xmlcomment>") continue;
      ptree menuElement = x.second;

      // get branch attributes
      if( menuElementName=="OutputList" ) {

         setOutputListParameters(menuElement);

      } 

      else if( menuElementName=="TopoConfig" ) {

         setTopoConfigParameters(menuElement);

      }

      else if( menuElementName=="SortAlgo" ) {

         TXC::L1TopoConfigAlg algo( getAttribute(menuElement,"name"), getAttribute(menuElement,"type") ); 
         algo.setAlgKind( L1TopoConfigAlg::SORT );
         setAlgoParameters( menuElement, algo );
         algo.setAlgOutput( getAttribute(menuElement,"output") );
         algo.setAlgoID( getUIntAttribute(menuElement, "algoId") );
         m_menu.addAlgorithm(std::move(algo));

      }

      else if( menuElementName=="DecisionAlgo" ) {

         L1TopoConfigAlg algo( getAttribute(menuElement,"name"), getAttribute(menuElement,"type") ); 
         algo.setAlgKind( L1TopoConfigAlg::DECISION );
         setAlgoParameters(menuElement, algo);
         algo.setAlgoID( getUIntAttribute(menuElement, "algoId") );
         m_menu.addAlgorithm(std::move(algo));

      } else {
         TRG_MSG_FATAL("Unknown element" << menuElementName);
      }
   }

   m_menu.setTriggerList();

   TRG_MSG_INFO("Parsing Successful!");

   if(msg().level() <= TrigConf::MSGTC::INFO)
      m_menu.print();

}



void TXC::L1TopoXMLParser::setTopoConfigParameters(ptree menuElement) {
   for(const boost::property_tree::ptree::value_type &c: menuElement) {
    
      string configName = c.first;
      if (configName=="<xmlattr>" || configName=="<xmlcomment>") continue;
      ptree configElement = c.second;
    
      m_menu.m_topoconfig.addTopoConfigElement( TopoConfigElement(getAttribute(configElement,"name"), 
                                                                  getAttribute(configElement,"value")));

   }
}

void TXC::L1TopoXMLParser::setOutputListParameters(ptree menuElement) {
   for(const boost::property_tree::ptree::value_type &o : menuElement) {

      string outputName = o.first;
      if (outputName=="<xmlattr>" || outputName=="<xmlcomment>") continue;
      ptree outputElement = o.second;

      m_menu.addL1TopoXMLOutput( OutputListElement(getAttribute(outputElement,"algname"),
                                                   getUIntAttribute(outputElement,"algoId"),
                                                   getUIntAttribute(outputElement,"module"),
                                                   getUIntAttribute(outputElement,"fpga"),
                                                   getUIntAttribute(outputElement,"clock"),
                                                   getUIntAttribute(outputElement,"firstbit") ) );
   }
}


void TXC::L1TopoXMLParser::setAlgoParameters(ptree menuElement, L1TopoConfigAlg &algo) {
  
   // iterate through elements of menu algorithms
   for(const boost::property_tree::ptree::value_type &y: menuElement) {
    
      // initialize elements of menu algorithms
      string subElementName = y.first;
      if (subElementName=="<xmlattr>" || subElementName=="<xmlcomment>") continue;
      ptree parameterGroup = y.second;

      if(subElementName=="Fixed") {

         for(boost::property_tree::ptree::value_type &z: parameterGroup) {
	
            string subsubElementName = z.first;
            if (subsubElementName=="<xmlattr>" || subsubElementName=="<xmlcomment>") continue;
            ptree subsubElement = z.second;
	
            if(subsubElementName=="Input") {
               algo.addInput(getAttribute(subsubElement,"name"), 
                             getAttribute(subsubElement,"value"), 
                             getUIntAttribute(subsubElement,"position", 0));
            }
            if(subsubElementName=="Output") {
               if(hasAttribute(subsubElement,"name") && 
                  hasAttribute(subsubElement,"value")) {
                  algo.addOutput(getAttribute(subsubElement,"name"), 
                                 getAttribute(subsubElement,"value"), 
                                 0, "", 0);
               } else {
                  setAlgoBits(subsubElement, algo);
               }
            }
            if(subsubElementName=="Generic") {
               algo.addFixedParameter( getAttribute(subsubElement,"name"), 
                                       getAttribute(subsubElement,"value") );
            }
         }
      }

      if(subElementName=="Variable") {
      
         for(boost::property_tree::ptree::value_type &z : parameterGroup) {
	
            string subsubElementName = z.first;
            if (subsubElementName=="<xmlattr>" || subsubElementName=="<xmlcomment>" ) continue;
            ptree subsubElement = z.second;

            if(subsubElementName=="Parameter") {
               algo.addParameter(getAttribute(subsubElement,"name"), 
                                 getAttribute(subsubElement,"value"), 
                                 getUIntAttribute(subsubElement,"pos", 0), 
                                 getUIntAttribute(subsubElement,"selection", 0) );
            }
         } // loop
      } // var

   } // end algo loop
}



void TXC::L1TopoXMLParser::setAlgoBits(ptree subsubElement, L1TopoConfigAlg &algo) {

   for(boost::property_tree::ptree::value_type &b: subsubElement) {
    
      string subsubsubElementName = b.first;
      if (subsubsubElementName=="<xmlattr>" || subsubsubElementName=="<xmlcomment>") continue;
      ptree subsubsubElement = b.second;
    
      if(subsubsubElementName=="Bit") {
         algo.addOutput(getAttribute(subsubElement,"name"), 
                        "", 
                        getUIntAttribute(subsubElement,"bits"),
                        getAttribute(subsubsubElement,"name"),
                        getUIntAttribute(subsubsubElement,"selection"));
      }
   }
}
