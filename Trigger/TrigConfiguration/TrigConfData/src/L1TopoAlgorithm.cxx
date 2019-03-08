/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigConfData/L1TopoAlgorithm.h"

TrigConf::L1TopoAlgorithm::L1TopoAlgorithm()
{}

TrigConf::L1TopoAlgorithm::L1TopoAlgorithm(const boost::property_tree::ptree & data) 
   : DataStructure(data)
{}

TrigConf::L1TopoAlgorithm::~L1TopoAlgorithm()
{}

unsigned int
TrigConf::L1TopoAlgorithm::algId() const
{
   return data().get_child("algId").get_value<unsigned int>();
}

const std::string &
TrigConf::L1TopoAlgorithm::name() const
{
   return data().get_child("name").data();
}

const std::string &
TrigConf::L1TopoAlgorithm::type() const
{
   return data().get_child("type").data();
}


std::vector<TrigConf::DataStructure> 
TrigConf::L1TopoAlgorithm::inputs() const
{
   return getList("fixedParameters.inputs");
}

TrigConf::DataStructure 
TrigConf::L1TopoAlgorithm::outputs() const
{
   return getObject("fixedParameters.outputs");
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

