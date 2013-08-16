/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/



#include "ExpatCoreParser.h"

int main (int /*argc*/, char* argv[])
{ 
  CoreParser::DOMNode* doc = ExpatCoreParser::parse (argv[1]);
           
  if (doc != 0) doc->print ("============ ALL =============");

  return (0);
} 
