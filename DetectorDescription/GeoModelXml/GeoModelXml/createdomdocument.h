/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef CREATEDOMDOCUMENT_H
#define CREATEDOMDOCUMENT_H
#include <xercesc/util/XercesDefs.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/DOMLSParser.hpp>
#include <iostream>

xercesc::DOMDocument *createDOMDocument(std::string xmlFile, xercesc::DOMLSParser *parser, unsigned int flags = 0);

#endif // CREATEDOMDOCUMENT_H
