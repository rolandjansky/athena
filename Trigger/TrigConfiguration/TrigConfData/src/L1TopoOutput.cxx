/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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
   return getAttribute("algName");
}

unsigned int
TrigConf::L1TopoOutput::clock() const
{
   return getAttribute<unsigned int>("clock");
}

unsigned int
TrigConf::L1TopoOutput::firstBit() const
{
   return getAttribute<unsigned int>("firstBit");
}

unsigned int
TrigConf::L1TopoOutput::fpga() const
{
   return getAttribute<unsigned int>("fpga");
}

unsigned int
TrigConf::L1TopoOutput::board() const
{
   return getAttribute<unsigned int>("board");
}

const std::string &
TrigConf::L1TopoOutput::triggerLines() const
{
   return getAttribute("triggerLines");
}

