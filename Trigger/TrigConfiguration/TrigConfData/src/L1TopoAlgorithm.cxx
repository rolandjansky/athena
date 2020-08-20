/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigConfData/L1TopoAlgorithm.h"
#include <map>

TrigConf::L1TopoAlgorithm::L1TopoAlgorithm(const std::string & algoName, AlgorithmType algoType, const std::string & algoCategory, const boost::property_tree::ptree & data) 
   : DataStructure(data),
     m_type(algoType),
     m_category(algoCategory)
{
   if( m_category != "TOPO" && m_category != "MUTOPO" && m_category != "R2TOPO" && m_category != "MULTTOPO") {
      throw std::runtime_error("Algorithm category must be TOPO, R2TOPO, MUTOPO or MULTTOPO, but is '" + algoCategory + "'");
   }
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
   if( m_type == AlgorithmType::DECISION ) { // DECISION algo
      if( hasChild("input") ) {
         for( auto & inp : getList("input")) {
            m_inputs.push_back(inp.getValue());
         }
      } else if( hasChild("fixedParameters.inputs") ) { // backwards compatibility, to be removed when we stop using DEV db
         for( auto & inp : getList("fixedParameters.inputs")) {
            m_inputs.push_back(inp["value"]);
         }
      }
      for( auto & o : getList("output")) {
         m_outputs.push_back(o.getValue());
      }
   } else if( m_type == AlgorithmType::MULTIPLICITY ) { // MULTIPLICITY algo
      m_inputs.push_back(getAttribute("input"));
      m_outputs.push_back(getAttribute("output"));
   } else { // SORTING algo
      if( hasAttribute("input") ) {
         m_inputs.push_back( getAttribute("input") );
      } else if( hasChild("fixedParameters.input") ) { // backwards compatibility, to be removed when we stop using DEV db
         auto inp = getObject("fixedParameters.input");
         for( auto & k : inp.getKeys() ) {
            m_inputs.push_back(inp[k]);
            break;
         }
      }
      m_outputs.push_back(getAttribute("output"));
   }

   if( m_type == AlgorithmType::DECISION || m_type == AlgorithmType::SORTING ) {
      for( auto & p : getList("variableParameters") ) {
         m_parameters.emplace_back(p["name"], p.getAttribute<int>("value"), p.getAttribute<unsigned int>("selection", /*ignore-if-missing*/true, 0));
      }
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
TrigConf::L1TopoAlgorithm::category() const
{
   return m_category;
}

const std::string &
TrigConf::L1TopoAlgorithm::klass() const
{
   if(hasAttribute("klass")) {
      return getAttribute("klass");
   }
   return getAttribute("type");
}

const std::vector<std::string> & 
TrigConf::L1TopoAlgorithm::inputs() const
{
   return m_inputs;
}

const std::vector<std::string> & 
TrigConf::L1TopoAlgorithm::outputs() const
{
   return m_outputs;
}

std::vector<std::string>
TrigConf::L1TopoAlgorithm::fullOutputs() const
{
   std::vector<std::string> out;
   for( auto & s : m_outputs ) {
      out.push_back(m_category + "_" + s);
   }
   return out;
}

std::string
TrigConf::L1TopoAlgorithm::genericParameter(const std::string & parName) const
{
   return operator[]("fixedParameters.generics." + parName + ".value");
}

TrigConf::DataStructure
TrigConf::L1TopoAlgorithm::generics() const
{
   return getObject("fixedParameters.generics");
}

const std::vector<TrigConf::L1TopoAlgorithm::VariableParameter> &
TrigConf::L1TopoAlgorithm::parameters() const
{
   return m_parameters;
}


void
TrigConf::L1TopoAlgorithm::print(std::ostream & os) const
{
   os << "Algorithm " << name() << " (class " << klass() << ", category " << m_category << ")" << std::endl;
   os << "  input:" << std::endl;
   for( auto & input : inputs() ) {
      os << "    " << input << std::endl;
   }
   os << "  output:" << std::endl;
   for( auto & output : outputs() ) {
      os << "    " << output << std::endl;
   }
   os << "  full output:" << std::endl;
   for( auto & output : fullOutputs() ) {
      os << "    " << output << std::endl;
   }
   if(type() == AlgorithmType::MULTIPLICITY) {
      os << "  threshold definition: " << getAttribute("threshold") << std::endl;
      os << "  number of output bits: " << getAttribute<unsigned int>("nbits") << std::endl;
   } else {
      os << "  generic parameters:" << std::endl;
      auto genPars = generics();
      for( auto & k : genPars.getKeys() ) {
         os << "    " << k << "  ==>  " <<  genericParameter(k) << std::endl;
      }
      os << "  parameters:" << std::endl;
      auto pars = parameters();
      unsigned int idx{0};
      for( auto & p : pars ) {
         os << "    " << idx++ << "  " << p.name << "[" << p.selection << "]  ==>  " <<  p.value << std::endl;
      }
   }
   os << std::endl;
}
