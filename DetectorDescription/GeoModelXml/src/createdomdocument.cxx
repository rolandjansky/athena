/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
//    Read in the xml and create an in-memory copy of it.
//    The first parameter is either the name of a file containing the xml, or a
//    (possibly very long) string with the xml itself. The flags parameter indicates which.
//
//    flags = 0 (default): file-name
//    flags & 1? xml
//    flags & 2? compressed xml
//
#include "GeoModelXml/createdomdocument.h"
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/dom/DOMImplementation.hpp>
#include <xercesc/dom/DOMImplementationLS.hpp>
#include <xercesc/dom/DOMImplementationRegistry.hpp>
#include "GeoModelXml/StrictErrorHandler.h"
#include "GeoModelXml/translate.h"
#include <iostream>
#include <stdexcept>

using namespace std;
using namespace xercesc;

string decompress(const string & compressed);

DOMDocument *createDOMDocument(string xmlFile, DOMLSParser *parser, unsigned int flags) {
//
//    Initialise Xerces-C 
//
    try {
        XMLPlatformUtils::Initialize();
    }
    catch (const xercesc::XMLException &toCatch) {
        char *message = Translate(toCatch.getMessage());
        cout << "XercesC error during initialization: " << message << "\n";
        XMLString::release(&message);
        return 0;
    }
//
//   Get a DOMLS parser (Document Object Model Load Save)
//
    static const XMLCh gLS[] = {chLatin_L, chLatin_S, chNull};
    DOMImplementation *impl = DOMImplementationRegistry::getDOMImplementation(gLS);
    parser = ((DOMImplementationLS*)impl)->createLSParser(DOMImplementationLS::MODE_SYNCHRONOUS, 0);
    DOMConfiguration  *config = parser->getDomConfig();

    config->setParameter(XMLUni::fgDOMNamespaces, true);
    config->setParameter(XMLUni::fgXercesSchema, true);
    config->setParameter(XMLUni::fgXercesHandleMultipleImports, true);
    config->setParameter(XMLUni::fgXercesSchemaFullChecking, true);
    config->setParameter(XMLUni::fgDOMValidateIfSchema, true);
    config->setParameter(XMLUni::fgDOMDatatypeNormalization, true);
// The next is important if you want to insert elements into the tree. Without it, elements loaded 
// from a file included via an !ENTITY are not writable. False should be the default, but for some reason the
// following is still needed
    config->setParameter(xercesc::XMLUni::fgDOMEntities, false);

    // And create our error handler and install it
    StrictErrorHandler errorHandler;
    config->setParameter(XMLUni::fgDOMErrorHandler, &errorHandler);
//
//    Parse the file
//
    XMLCh *xmlCh(0); 
    xercesc::DOMDocument *doc;
    try {
        if (!(flags & 0x1)) { // String is a file name
            doc = parser->parseURI(xmlFile.c_str());
        }
        else {
            string decompressed;
            if (flags & 0x2) { // Decompress it
                decompressed = decompress(xmlFile);
cout << "Decompressed it\n";
            }
            else {
                decompressed = xmlFile;
            }
            xmlCh = Translate(decompressed.c_str());
cout << "Transcoded it\n";
            DOMLSInput *input = impl->createLSInput();
            input->setStringData(xmlCh);            
            doc = parser->parse(input);
cout << "Parsed it\n";
        }
        if (errorHandler.getSawErrors()) {
            cout << "Oh oh, saw errors. \n";
if (flags & 0x1) cout << "\n\n\n" << Translate(xmlCh) << "\n\n\n";
            return 0;
        }
        else {
            if (!(flags & 0x1)) {
                std::cout << "File " << xmlFile << " successfully parsed.\n";
            }
            else {
                std::cout << "Database string successfully parsed.\n";
            }
        }
    }
    catch (const XMLException &toCatch) {
        char *message = Translate(toCatch.getMessage());
        cout << "Parse-file xml exception: \n" << message << "\n";
        XMLString::release(&message);
        return 0;
    }
    catch (const DOMException &toCatch) {
        char *message = Translate(toCatch.msg);
        cout << "DOM exception: \n" << message << "\n";
        XMLString::release(&message);
        return 0;
    }
// You cannot delete the parser, even though you never refer to it again. It is needed 
// to get DOMElement *root etc.
//    delete parser;
    return doc;
}
