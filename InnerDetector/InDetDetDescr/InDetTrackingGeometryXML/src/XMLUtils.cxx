/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// XMLUtils.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "InDetTrackingGeometryXML/XMLUtils.h"
#include <sstream>
#include <ctype.h>

InDet::XMLUtils::XMLUtils()
{
  m_ConfigFileParser = 0;
}

InDet::XMLUtils::~XMLUtils()
{
}

bool InDet::XMLUtils::InitializeXML()
{
   try {
     XMLPlatformUtils::Initialize(); // Initialize Xerces infrastructure
   } catch(const XMLException& e) {
     PrintErrorXML(e,"Error in XMLPlatformUtils::Initialize()");
     return false;
   }

   m_ConfigFileParser = new XercesDOMParser;

   // Configure XML DOM parser.
   m_ConfigFileParser->setValidationScheme( XercesDOMParser::Val_Never );
   m_ConfigFileParser->setDoNamespaces( false );
   m_ConfigFileParser->setDoSchema( false );
   m_ConfigFileParser->setLoadExternalDTD( false );

   return true;
}


bool InDet::XMLUtils::TerminateXML()
{
  releaseStrings();
  if (m_ConfigFileParser) delete m_ConfigFileParser;

  try {
    XMLPlatformUtils::Terminate();  // Terminate after release of memory
  } catch(XMLException& e) {
    PrintErrorXML(e,"Error in XMLPlatformUtils::Terminate()");
    return false;
  }

  return true;
}

void InDet::XMLUtils::PrintErrorXML(const XMLException &e, std::string message)
{
  char* err_message = XMLString::transcode( e.getMessage() );
  std::cerr << "XML: " << message << ": " << err_message << std::endl;
  //XMLString::release(&err_message); 
}

const char* InDet::XMLUtils::getString(DOMNode *node)
{
  XMLCh* xmlString = (XMLCh*) node->getTextContent();
  XMLString::trim(xmlString);
  const char* name = XMLString::transcode(xmlString);
  // No release because getTextContent return const XMLCh* // XMLString::release(&xmlString); 
  return name;
}

const char* InDet::XMLUtils::getStringNoTrim(DOMNode *node)
{
  XMLCh* xmlString = (XMLCh*) node->getTextContent();
  const char* name = XMLString::transcode(xmlString);
  // No release because getTextContent return const XMLCh* // XMLString::release(&xmlString); 
  return name;
}

bool InDet::XMLUtils::getBoolean(DOMNode *node)
{
  std::string str = getString(node);
  // Change string to lower-case
  unsigned int n = str.size();
  for(unsigned int i=0;i<n;i++) str[i] = tolower((unsigned char) str[i]);
  if(str.compare("true") == 0 ) return true;
  return false;

}

std::vector<std::string> InDet::XMLUtils::getVectorString(DOMNode *node)
{
  std::string str = getStringNoTrim(node);
  std::stringstream ss(str);
  std::string item;

  std::vector<std::string> v;

  while (std::getline(ss, item, ' ')) {
    if(item.size()> 0) v.push_back(item);
  }

  return v;
}

std::vector<double> InDet::XMLUtils::getVectorDouble(DOMNode *node)
{
  std::string str = getStringNoTrim(node);

  std::vector<double> v;
  std::istringstream s(str);
  double d;
  while (s >> d) {
    v.push_back(d);
  }

  return v;
}

std::vector<int> InDet::XMLUtils::getVectorInt(DOMNode *node)
{
  std::string str = getStringNoTrim(node);

  std::vector<int> v;
  std::istringstream s(str);
  int d;
  while (s >> d) {
    v.push_back(d);
  }

  return v;
}

XMLCh* InDet::XMLUtils::transcode(std::string s)
{
  std::map<std::string, XMLCh*>::iterator iter = m_Strings.find(s);
  if(iter!=m_Strings.end()) {
    return iter->second;
  }

  XMLCh* ch = XMLString::transcode(s.c_str());
  m_Strings[s]=ch;
  return ch;
}

void InDet::XMLUtils::releaseStrings()
{
  for(std::map<std::string, XMLCh*>::iterator iter=m_Strings.begin();iter!=m_Strings.end();++iter) {
    XMLCh* ch = iter->second;
    if(ch==0) continue;
    try {
      XMLString::release(&ch);
    } catch(const XMLException& e) {
      PrintErrorXML(e,"Release error in XMLUtils");
    }
  }
}


void InDet::XMLUtils::parseFile(std::string filename, std::string tag, std::string section)
{
  // Process XML description file
  // std::cout << "INFO XMLUtils: Opening XML description file " << filename << std::endl;

  // Create XML 16 bit tag string
  XMLCh* TAG_section = transcode(tag.c_str());
  XMLCh* TAG_node    = transcode(section.c_str());

  // Parse XML file
  try { 
    m_ConfigFileParser->parse(filename.c_str());
    DOMDocument* xmlDoc       = m_ConfigFileParser->getDocument();
    if(!xmlDoc) {
      std::cerr << "ERROR XMLUtils: xmlDoc bad pointer, Wrong XML file name " << filename <<  std::endl;
      return;
    }
    DOMNodeList* xmlSections  = xmlDoc->getElementsByTagName(TAG_section);
    if(!xmlSections) {
      std::cout << "WARNING: Sections " <<  tag.c_str() << " not found" << std::endl;
      std::cerr << "ERROR XMLUtils: No tag " << tag << std::endl;
      return;
    }
    
    DOMNode* currentSection = xmlSections->item(0);

    DOMNodeList* xmlList = currentSection->getChildNodes();
    if(!xmlList) {
      std::cerr << "ERROR XMLUtils: xmlList bad pointer, Wrong XML file type" << std::endl;
      return;
    }
    const XMLSize_t nodeCount = xmlList->getLength();

    if(nodeCount==0) {
      std::cerr << "ERROR XMLUtils: No section " << section << std::endl;
      return;
    } else

    // Loop over all subsections
    for( XMLSize_t xx = 0; xx < nodeCount; ++xx ) {
      DOMNode* currentNode = xmlList->item(xx);
      if(!currentNode->getNodeType()) continue; // NULL node
      if(currentNode->getNodeType() != DOMNode::ELEMENT_NODE) continue; // not an element

      // Found node which is an Element. Re-cast node as element
      DOMElement* currentElement = dynamic_cast< xercesc::DOMElement* >( currentNode );
      if( XMLString::equals(currentElement->getTagName(), TAG_node)) parseNode(section,currentElement);
    } // End of loop on subsections
    
  } catch (XMLException& e) {
    PrintErrorXML(e,"Parse error in XMLUtils::parseFile()");
    return;
  }
}

void InDet::XMLUtils::parseNode(std::string section, DOMNode *)
{
  std::cout << "ERROR XMLUtils: No parseNode defined for section " << section << std::endl;
  return;
}

