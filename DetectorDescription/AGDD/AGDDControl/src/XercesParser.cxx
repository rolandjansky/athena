/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDDControl/XercesParser.h"
#include "AGDDControl/XMLHandler.h"
#include "AGDDControl/XMLHandlerStore.h"
#include "PathResolver/PathResolver.h"
#include "AthenaKernel/getMessageSvc.h"
#include "GaudiKernel/MsgStream.h"

#include "xercesc/util/PlatformUtils.hpp"
#include "xercesc/util/XMLException.hpp"
#include <xercesc/dom/DOM.hpp>
#include <xercesc/framework/StdOutFormatTarget.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/util/XMLUni.hpp>
#include <xercesc/util/OutOfMemoryException.hpp>

#include <iostream>

using namespace xercesc;

XercesParser::~XercesParser()
{
	delete m_parser;
	m_parser=0;
	Finalize();
}

XercesParser::XercesParser(XMLHandlerStore& xs)
  : IAGDDParser(),m_doc(0),m_parser(0),m_initialized(false),
    m_xs(xs)
{
}

XercesParser::XercesParser(XMLHandlerStore& xs, const std::string& s)
  : IAGDDParser(s),m_doc(0),m_parser(0),m_initialized(false),
    m_xs(xs)
{
}

bool XercesParser::ParseFile(const std::string& s_in)
{
	bool errorsOccured = false;
        m_fileName=s_in;
        std::string s=PathResolver::find_file(s_in,"XMLPATH",PathResolver::RecursiveSearch);
	if (s.empty()) {
		MsgStream log(Athena::getMessageSvc(), "XercesParser");
		log<<MSG::WARNING<<"ParseFile() - something wrong, could not find XML file "<<s<<endmsg;
		errorsOccured = true;
	}
	else
	{
		if (!m_initialized) Initialize();
        m_parser = new XercesDOMParser;
    	try
    	{
        	m_parser->parse(s.c_str());
    	}
    	catch (const OutOfMemoryException&)
    	{
        	XERCES_STD_QUALIFIER cerr << "OutOfMemoryException" << XERCES_STD_QUALIFIER endl;
        	errorsOccured = true;
    	}
    	catch (const XMLException& e)
    	{
        	XERCES_STD_QUALIFIER cerr << "An error occurred during parsing\n   Message: "
             	<< XMLString::transcode(e.getMessage()) << XERCES_STD_QUALIFIER endl;
        	errorsOccured = true;
    	}
        catch (const DOMException& e)
        {
            const unsigned int maxChars = 2047;
        	XMLCh errText[maxChars + 1];

        	XERCES_STD_QUALIFIER cerr << "\nDOM Error during parsing: '" << s << "'\n"
            	 << "DOMException code is:  " << e.code << XERCES_STD_QUALIFIER endl;

        	if (DOMImplementation::loadDOMExceptionMsg(e.code, errText, maxChars))
             	XERCES_STD_QUALIFIER cerr << "Message is: " << XMLString::transcode(errText) << XERCES_STD_QUALIFIER endl;

        	errorsOccured = true;
    	}
    	catch (...)
    	{
        	XERCES_STD_QUALIFIER cerr << "An error occurred during parsing\n " << XERCES_STD_QUALIFIER endl;
        	errorsOccured = true;
    	}  
		m_doc=m_parser->getDocument();
		return errorsOccured;
	}
    return errorsOccured;
}
bool XercesParser::ParseFileAndNavigate(AGDDController& c,
                                        const std::string& s)
{
    bool errorOccured = ParseFile(s);
    if (errorOccured) return false;
	navigateTree(c);
	return true;
}
bool XercesParser::ParseString(const std::string& s)
{
	const char* str=s.c_str();
	MemBufInputSource* memBuf = new MemBufInputSource((const XMLByte*)str,strlen(str),"prodInfo",false);
    m_parser = new XercesDOMParser;
    bool errorsOccured = false;
	if (!m_initialized) Initialize();
    try
    {
    	m_parser->parse(*memBuf);
    }
    catch (const OutOfMemoryException&)
    {
    	XERCES_STD_QUALIFIER cerr << "OutOfMemoryException" << XERCES_STD_QUALIFIER endl;
    	errorsOccured = true;
    }
    catch (const XMLException& e)
    {
    	XERCES_STD_QUALIFIER cerr << "An error occurred during parsing\n   Message: "
             	<< XMLString::transcode(e.getMessage()) << XERCES_STD_QUALIFIER endl;
    	errorsOccured = true;
    }
    catch (const DOMException& e)
    {
        const unsigned int maxChars = 2047;
    	XMLCh errText[maxChars + 1];

    	XERCES_STD_QUALIFIER cerr << "\nDOM Error during parsing: '" << s << "'\n"
            	 << "DOMException code is:  " << e.code << XERCES_STD_QUALIFIER endl;

   		if (DOMImplementation::loadDOMExceptionMsg(e.code, errText, maxChars))
             	XERCES_STD_QUALIFIER cerr << "Message is: " << XMLString::transcode(errText) << XERCES_STD_QUALIFIER endl;

    	errorsOccured = true;
    }
    catch (...)
    {
    	XERCES_STD_QUALIFIER cerr << "An error occurred during parsing\n " << XERCES_STD_QUALIFIER endl;
    	errorsOccured = true;
    }  
	m_doc=m_parser->getDocument();
	return errorsOccured;
}
bool XercesParser::ParseStringAndNavigate(AGDDController& c,
                                          const std::string& s)
{
    bool errorOccured = ParseString(s);
    if (!errorOccured) navigateTree(c);
    return true;
}

bool XercesParser::WriteToFile(const std::string& s)
{
	XMLCh tempStr[100];
	XMLString::transcode("LS 3.0 Core 2.0", tempStr, 99);
	DOMImplementation* implementation = DOMImplementationRegistry::getDOMImplementation(tempStr);
	DOMLSSerializer*   serializer     = ((DOMImplementationLS*)implementation)->createLSSerializer();
	// if one wants a nicely indented file -- not in this case as it goes to the DB and be compressed
	// DOMConfiguration*  domconfig     = serializer->getDomConfig();
	// domconfig->setParameter(XMLUni::fgDOMWRTFormatPrettyPrint, true);
	XMLFormatTarget* target    = new LocalFileFormatTarget(s.c_str());
	DOMLSOutput*     domoutput = ((DOMImplementationLS*)implementation)->createLSOutput();
	// remove all comments
	if( m_doc->getDOMConfig()->canSetParameter(XMLUni::fgDOMComments, true) ) m_doc->getDOMConfig()->setParameter(XMLUni::fgDOMComments, false);
	m_doc->normalizeDocument();

	domoutput->setByteStream(target);
	serializer->write(m_doc, domoutput);
	domoutput->release();
	serializer->release();
	delete target;

	return true;
}

void XercesParser::navigateTree(AGDDController& c)
{
    if (!m_doc) {
        MsgStream log(Athena::getMessageSvc(), "XercesParser");
        log<<MSG::WARNING<<"navigateTree() - something is wrong! no document set! doing nothing!"<<endmsg;
        return;
    }
    DOMNode* node = 0;
    node = dynamic_cast<DOMNode*>(m_doc->getDocumentElement());
    if( !node ) std::abort();
    elementLoop(c, node);
}

void XercesParser::elementLoop() {}

void XercesParser::elementLoop(AGDDController& c,
                               DOMNode *e)
{
	if (!e) {
		MsgStream log(Athena::getMessageSvc(), "XercesParser");
		log<<MSG::WARNING<<"Calling elementLoop() with NULL pointer!!!"<<endmsg;
		return;
	}
	if (!(e->getNodeType()==DOMNode::ELEMENT_NODE)) return;
	XMLHandler *h = m_xs.GetHandler(e);
	bool stopLoop=false;
	if (h)
	{
		h->Handle(c, e);
		stopLoop=h->IsLoopToBeStopped();
	}
	DOMNode *child;
	std::string sName;
	if (!stopLoop && e)
	{
		char* name=XMLString::transcode(e->getNodeName());
		sName=name;
		XMLString::release(&name);
		for (child=e->getFirstChild();child!=0;child=child->getNextSibling())
		{
			if (child->getNodeType()==DOMNode::ELEMENT_NODE) {
				elementLoop(c, child);
			}
		}
	}
}

bool XercesParser::Initialize()
{
    try {
        XMLPlatformUtils::Initialize();
    }

    catch(const XMLException &toCatch) {
        XERCES_STD_QUALIFIER cerr << "Error during Xerces-c Initialization.\n"
         	<< "  Exception message:"
         	<< XERCES_STD_QUALIFIER endl;
        return 1;
    }
    m_initialized=true;
    return 0;
}

bool XercesParser::Finalize()
{
	XMLPlatformUtils::Terminate();
	m_initialized=false;
	return 0;
}
