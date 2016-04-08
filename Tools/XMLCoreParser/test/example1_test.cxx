/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/



#include "XMLCoreParser/XMLCoreParser.h"

int main (int argc, char* argv[])
{
  XMLCoreParser parser;

  parser.register_external_entity ("InnerDetector", "OtherInDet.xml");
  parser.register_external_entity ("LArCalorimeter", "NULL");
  parser.register_external_entity ("TileCalorimeter", "NULL");
  parser.register_external_entity ("LVL1Calorimeter", "NULL");
  parser.register_external_entity ("MuonSpectrometer", "NULL");

  const char* file = "../data/ATLAS_AGDD.xml";
  if (argc > 1)
    file = argv[1]; 

  parser.parse (file);
  return (0);
}
