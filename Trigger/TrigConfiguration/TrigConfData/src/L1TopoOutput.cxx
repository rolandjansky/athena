/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigConfData/L1TopoOutput.h"

TrigConf::L1TopoOutput::L1TopoOutput()
{}

TrigConf::L1TopoOutput::L1TopoOutput(const boost::property_tree::ptree & data) 
   : DataStructure(data)
{}

TrigConf::L1TopoOutput::~L1TopoOutput()
{}

const std::string &
TrigConf::L1TopoOutput::algName() const
{
   return data().get_child("algName").data();
}

unsigned int
TrigConf::L1TopoOutput::algId() const
{
   return data().get_child("algId").get_value<unsigned int>();
}

unsigned int
TrigConf::L1TopoOutput::clock() const
{
   return data().get_child("clock").get_value<unsigned int>();
}

unsigned int
TrigConf::L1TopoOutput::firstBit() const
{
   return data().get_child("firstBit").get_value<unsigned int>();
}

unsigned int
TrigConf::L1TopoOutput::fpga() const
{
   return data().get_child("fpga").get_value<unsigned int>();
}

unsigned int
TrigConf::L1TopoOutput::board() const
{
   return data().get_child("board").get_value<unsigned int>();
}

const std::string &
TrigConf::L1TopoOutput::triggerLines() const
{
   return data().get_child("triggerLines").data();
}

