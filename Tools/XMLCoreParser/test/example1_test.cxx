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

  const char* dir = getenv("XMLCOREPARSER_DATA");
  if (!dir)
    dir = "../data";
  std::string file = dir;
  if (argc > 1)
    file = argv[1]; 
  else
    file += "/ATLAS_AGDD.xml";

  parser.parse (file);
  return (0);
}
