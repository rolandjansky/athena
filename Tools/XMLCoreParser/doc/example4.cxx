/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/



#include "XMLCoreParser/XMLCoreParser.h"

int main (int argc, char* argv[])
{
  XMLCoreParser parser;

  parser.register_text_entity ("ABCD", "<abcd/>");

  if (argc > 1)
    {
      const char* file = argv[1]; 

      parser.parse (file);
    }

  return (0);
}
