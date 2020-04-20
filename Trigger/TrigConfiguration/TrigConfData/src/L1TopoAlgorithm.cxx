/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigConfData/L1TopoAlgorithm.h"

TrigConf::L1TopoAlgorithm::L1TopoAlgorithm()
{}

TrigConf::L1TopoAlgorithm::L1TopoAlgorithm(const std::string & algoName, AlgorithmType algoType, const boost::property_tree::ptree & data) 
   : DataStructure(data),
     m_type(algoType)
{
   m_name = algoName;
   update();
}

TrigConf::L1TopoAlgorithm::~L1TopoAlgorithm()
{}

std::string
TrigConf::L1TopoAlgorithm::className() const {
   return "L1TopoAlgorithm";
}

void
TrigConf::L1TopoAlgorithm::update()
{
   if(! isInitialized() || empty() ) {
      return;
   }
   if( m_type == AlgorithmType::DECISION ) {
      for( auto & o : getList("output")) {
         m_outputs.push_back(o.getValue());
      }
   } else {
      m_outputs.push_back(getAttribute("output"));
   }
}


unsigned int
TrigConf::L1TopoAlgorithm::algId() const
{
   return getAttribute<unsigned int>("algId");
}

TrigConf::L1TopoAlgorithm::AlgorithmType
TrigConf::L1TopoAlgorithm::type() const
{
   return m_type;
}

const std::string &
TrigConf::L1TopoAlgorithm::klass() const
{
   return getAttribute("type");
}


std::vector<TrigConf::DataStructure> 
TrigConf::L1TopoAlgorithm::inputs() const
{
   return getList("fixedParameters.inputs");
}

const std::vector<std::string> & 
TrigConf::L1TopoAlgorithm::outputs() const
{
   return m_outputs;
}

std::vector<TrigConf::DataStructure> 
TrigConf::L1TopoAlgorithm::generics() const
{
   return getList("fixedParameters.generics");
}

std::vector<TrigConf::DataStructure> 
TrigConf::L1TopoAlgorithm::parameters() const
{
   return getList("variableParameters.parameters");
}

