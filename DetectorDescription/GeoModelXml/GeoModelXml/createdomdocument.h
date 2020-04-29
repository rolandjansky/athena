/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/DOMLSParser.hpp>
#include <iostream>

using namespace std;
using namespace xercesc;

DOMDocument *createDOMDocument(string xmlFile, DOMLSParser *parser, unsigned int flags = 0);
