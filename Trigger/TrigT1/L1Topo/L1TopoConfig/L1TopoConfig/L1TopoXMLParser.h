/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//--------------------------------
// This is the L1TopoXMLParser class. It containts all the methods required to
// parse the configuration XML. The parsed information is stored in a container  
// class L1TopoConfigAlg. This container is sent to the TopoSteering and its contents 
// are unloaded and used to configurate the decision algorithm.
//-------------------------------

#ifndef L1TOPOXMLPARSER_HPP
#define L1TOPOXMLPARSER_HPP

#include "TrigConfBase/TrigConfMessaging.h"

#include "L1TopoConfig/L1TopoConfigAlg.h"
#include "L1TopoConfig/L1TopoMenu.h"

#include <iostream>
#include <stdio.h>
#include <vector>
#include <string>

// boost libraries
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

namespace TXC {

   class L1TopoXMLParser : public TrigConf::TrigConfMessaging {
  
   public:

      //Constructor
      L1TopoXMLParser();
  
      //Destructor
      ~L1TopoXMLParser();

      // print the parsed configuration
      void printConfiguration() const;
  
      // set verbosity (level out output to screen)
      // something like 3=high, 2=med, 1=low
      void setVerbosity(int v) { m_verbosity=v; }
 
      // method to get a single algorithm from the parsed configuration file
      const L1TopoConfigAlg& getAlgorithm(int i) const; 

      // method to get a vector of all algorithms
      const std::vector<L1TopoConfigAlg> & getAlgorithms() const;

      // read input configuration and check for errors
      void readConfiguration(const std::string & inputfile);

      // read input configuration and check for errors
      void setConfiguration(boost::property_tree::ptree topomenu) { m_menuPT = topomenu; }

      // parse configuration and fill algorithm containers
      void parseConfiguration();

      const L1TopoMenu& menu() const { return m_menu; }

      L1TopoMenu takeMenu() { 
         return std::move(m_menu);
      }

   private:
       
      // methods to set parameters for menu objects
      void setAlgoParameters(boost::property_tree::ptree menuElement, L1TopoConfigAlg &algo);
      void setTopoConfigParameters(boost::property_tree::ptree menuElement);
      void setOutputListParameters(boost::property_tree::ptree menuElement);
      void setAlgoBits(boost::property_tree::ptree subsubElement, L1TopoConfigAlg &algo);

      // input validation variables
      bool m_isValidConfigurtion {false};
      int  m_verbosity {0};
  
      // configuration variables
      boost::property_tree::ptree m_menuPT;
      L1TopoMenu m_menu;
   };

}

#endif
