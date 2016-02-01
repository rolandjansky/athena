/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// XMLUtils.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "PixelLayoutUtils/GeoXMLUtils.h"
#include <sstream>
#include <fstream>
#include <algorithm>

GeoXMLUtils::GeoXMLUtils():
  m_doc(0)
{
  m_ConfigFileParser = 0;
}

GeoXMLUtils::~GeoXMLUtils()
{
}

bool GeoXMLUtils::InitializeXML()
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


bool GeoXMLUtils::TerminateXML()
{
  releaseStrings();
  if (m_ConfigFileParser) delete m_ConfigFileParser;
  //  if (m_doc) m_doc->release();

  try {
    XMLPlatformUtils::Terminate();  // Terminate after release of memory
  } catch(XMLException& e) {
    PrintErrorXML(e,"Error in XMLPlatformUtils::Terminate()");
    return false;
  }

  return true;
}

void GeoXMLUtils::PrintErrorXML(const XMLException &e, std::string message)
{
  char* err_message = XMLString::transcode( e.getMessage() );
  std::cerr << "XML: " << message << ": " << err_message << std::endl;
  //XMLString::release(&err_message); 
}

const char* GeoXMLUtils::getString(DOMNode *node) const
{
  XMLCh* xmlString = (XMLCh*) node->getTextContent();
  XMLString::trim(xmlString);
  const char* name = XMLString::transcode(xmlString);
  // No release because getTextContent return const XMLCh* // XMLString::release(&xmlString); 
  return name;
}

const char* GeoXMLUtils::getStringNoTrim(DOMNode *node) const
{
  XMLCh* xmlString = (XMLCh*) node->getTextContent();
  const char* name = XMLString::transcode(xmlString);
  // No release because getTextContent return const XMLCh* // XMLString::release(&xmlString); 
  return name;
}

std::vector<double> GeoXMLUtils::getVectorDouble(DOMNode *node) const
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

XMLCh* GeoXMLUtils::transcode(std::string s)
{
  std::map<std::string, XMLCh*>::iterator iter = m_Strings.find(s);
  if(iter!=m_Strings.end()) {
    return iter->second;
  }

  XMLCh* ch = XMLString::transcode(s.c_str());
  m_Strings[s]=ch;
  return ch;
}

void GeoXMLUtils::releaseStrings()
{
  for(std::map<std::string, XMLCh*>::iterator iter=m_Strings.begin();iter!=m_Strings.end();++iter) {
    XMLCh* ch = iter->second;
    if(ch==0) continue;
    try {
      XMLString::release(&ch);
    } catch(const XMLException& e) {
      PrintErrorXML(e,"Release error in GeoXMLUtils");
    }
  }
}

bool GeoXMLUtils::ParseFile(std::string xmlfile)
{
  //  m_ConfigFileParser->parse(xmlfile.c_str());

  
  // Read xml file content
  // ------------------------------------
  std::ifstream ifs (xmlfile.c_str());
  if(!ifs) return false;
  std::filebuf* inbuf  = ifs.rdbuf();

  // get file size using buffer's members
  std::size_t size = inbuf->pubseekoff (0,ifs.end,ifs.in);
  inbuf->pubseekpos (0,ifs.in);

  // allocate memory to contain file data
  char* buffer=new char[size];

  // get file data
  inbuf->sgetn (buffer,size);
  ifs.close();

  // Check if corresponding GEO file exists
  // -----------------------------------------
  char* bufferGeo=0;
  std::size_t sizeGeo =0;
  xmlfile.replace(xmlfile.length()-4,4,"Geo.xml");
  std::ifstream ifsGeo(xmlfile.c_str());
  if(ifsGeo){
    std::filebuf* inbuf  = ifsGeo.rdbuf();
    
    // get file size using buffer's members
    sizeGeo = inbuf->pubseekoff (0,ifsGeo.end,ifsGeo.in);
    inbuf->pubseekpos (0,ifsGeo.in);
    
    // allocate memory to contain file data
     bufferGeo=new char[sizeGeo];
    
    // get file data
    inbuf->sgetn (bufferGeo,sizeGeo);
    ifsGeo.close();
  }


  std::string myxml = std::string(buffer,size);
  std::size_t firstChar = myxml.find(std::string("<"));
  std::size_t lastChar = myxml.rfind(std::string(">"));
  myxml = myxml.substr(firstChar,lastChar-firstChar+1);
  delete buffer;

  if(bufferGeo){
    std::string myxmlGeo = std::string(bufferGeo,sizeGeo);
    std::size_t firstGeo = myxmlGeo.find(std::string(">"))+1;
    std::size_t lastGeo = myxmlGeo.rfind(std::string("<"));
    std::size_t lastXML = myxml.rfind("<");

    myxml = myxml.insert(lastXML, myxmlGeo.substr(firstGeo,lastGeo-firstGeo));
    delete bufferGeo;
  }

  //  std::cout<<"---- XML buffer ------------------------------------------------"<<std::endl;
  //  std::cout<<myxml<<std::endl;
  //  std::cout<<"----------------------------------------------------"<<std::endl;

  MemBufInputSource myxml_buf((const XMLByte*)myxml.c_str(), myxml.size()-1,"dummy");
  m_ConfigFileParser->parse(myxml_buf);

  m_doc = m_ConfigFileParser->getDocument();

  return true;
}


std::string GeoXMLUtils::getString(const char* parentTag,int parentIndex,
				const char* childTag, int childIndex,
				std::string defaultValue) const
{

  std::string res = getChildValue( parentTag, parentIndex, childTag, childIndex);
  if(res=="") res=defaultValue;

  return res;
}

std::string GeoXMLUtils::getString(const char* parentTag,std::vector<int> parentIndexList,
				const char* childTag, int childIndex,
				std::string defaultValue) const
{
  std::string res=defaultValue;
  
  try {
    for(int i=0; i<(int)parentIndexList.size(); i++){
      std::string tmp = getChildValue( parentTag, parentIndexList[i], childTag, childIndex);
      if(tmp.length()>0) return tmp;
    }
  }
  catch(...){
    std::cout << "XML: " << "Could not find "<<parentTag<<" "<<childTag<<" in XML file"<<std::endl;
  }
  
  return res;
}

double GeoXMLUtils::getDouble(const char* parentTag,int parentIndex,
				const char* childTag, int childIndex,
				double defaultValue) const
{
  double res=defaultValue;

  try {
    std::string tmp = getChildValue( parentTag, parentIndex, childTag, childIndex);
    if(tmp.length()>0)
      res = atof(tmp.c_str());
    else {
      //     std::cerr << "XML: " << "Could not find "<<parentTag<<" "<<childTag<<" in XML file"<<std::endl;
      std::cout << "XML: " << "Could not find "<<parentTag<<" "<<childTag<<" in XML file"<<std::endl;
    }
  }
  catch(...){
    std::cout << "XML: " << "Could not find "<<parentTag<<" "<<childTag<<" in XML file"<<std::endl;
  }

  return res;
}

std::vector<double> GeoXMLUtils::getVectorDouble(const char* parentTag,int parentIndex,
					      const char* childTag, int childIndex) const
{
  std::vector<double> res;
  
  try {
    std::string tmp = getChildValue( parentTag, parentIndex, childTag, childIndex);
    if(tmp.length()>0){
      std::istringstream s(tmp);
      double d;
      while (s >> d) res.push_back(d);
    }
    else {
      //     std::cerr << "XML: " << "Could not find "<<parentTag<<" "<<childTag<<" in XML file"<<std::endl;
      std::cout << "XML: " << "Could not find "<<parentTag<<" "<<childTag<<" in XML file"<<std::endl;
    }
  }
  catch(...){
    std::cout << "XML: " << "Could not find "<<parentTag<<" "<<childTag<<" in XML file"<<std::endl;
  }

  return res; 
}

std::vector<int> GeoXMLUtils::getVectorInt(const char* parentTag,int parentIndex,
					const char* childTag, int childIndex) const
{
  std::vector<int> res;
  
  try {
    std::string tmp = getChildValue( parentTag, parentIndex, childTag, childIndex);
    if(tmp.length()>0){
      std::istringstream s(tmp);
      int d;
      while (s >> d) res.push_back(d);
    }
    else {
      //     std::cerr << "XML: " << "Could not find "<<parentTag<<" "<<childTag<<" in XML file"<<std::endl;
      std::cout << "XML: " << "Could not find "<<parentTag<<" "<<childTag<<" in XML file"<<std::endl;
    }
  }
  catch(...){
    std::cout << "XML: " << "Could not find "<<parentTag<<" "<<childTag<<" in XML file"<<std::endl;
  }

  return res; 
}

std::vector<std::string> GeoXMLUtils::getVectorString(const char* parentTag,int parentIndex,
						   const char* childTag, int childIndex) const
{
  std::vector<std::string> res;
  
  try {
    std::string tmp = getChildValue( parentTag, parentIndex, childTag, childIndex);
    if(tmp.length()>0){
      std::istringstream s(tmp);
      std::string d;
      while (s >> d) res.push_back(d);
    }
    else {
      //     std::cerr << "XML: " << "Could not find "<<parentTag<<" "<<childTag<<" in XML file"<<std::endl;
      std::cout << "XML: " << "Could not find "<<parentTag<<" "<<childTag<<" in XML file"<<std::endl;
    }
  }
  catch(...){
    std::cout << "XML: " << "Could not find "<<parentTag<<" "<<childTag<<" in XML file"<<std::endl;
  }

  return res; 
}


double GeoXMLUtils::getDouble(const char* parentTag,std::vector<int> parentIndexList,
				const char* childTag, int childIndex,
				double defaultValue) const
{
  double res=defaultValue;
  
  try {
    for(int i=0; i<(int)parentIndexList.size(); i++){
      std::string tmp = getChildValue( parentTag, parentIndexList[i], childTag, childIndex);
      if(tmp.length()>0){
	res = atof(tmp.c_str());
	return res;
      }
    }
  }
  catch(...){
    std::cout << "XML: " << "Could not find "<<parentTag<<" "<<childTag<<" in XML file"<<std::endl;
  }
  
  return res;
}


int GeoXMLUtils::getInt(const char* parentTag,int parentIndex,
		     const char* childTag, int childIndex,
		     int defaultValue) const
{
  int res=defaultValue;

  std::string tmp=getChildValue( parentTag, parentIndex, childTag, childIndex);
  if(tmp.length()>0)
    res = atoi(tmp.c_str());
  else {
    std::cerr << "XML: " << "Could not find "<<parentTag<<" "<<childTag<<" in XML file"<<std::endl;
  }

  return res;
}

bool GeoXMLUtils::getBoolean(const char* parentTag,int parentIndex,
			  const char* childTag, int childIndex) const

{
  bool res = false;
  std::string tmp=getChildValue( parentTag, parentIndex, childTag, childIndex);
  tmp.erase(std::remove(tmp.begin(),tmp.end(),' '),tmp.end());
  if(tmp.length()>0){
    if(tmp.compare("true")==0||tmp.compare("True")==0||tmp.compare("1")==0) res = true;
  }
  else {
    std::cerr << "XML: " << "Could not find "<<parentTag<<" "<<childTag<<" in XML file"<<std::endl;
  }

  return res;
}

std::vector<std::string> GeoXMLUtils::getNodeList(const char* parentTag) const
{
  std::vector<std::string> nodeList;

  XMLCh* temp = XMLString::transcode(parentTag);
  DOMNodeList* list = m_doc->getElementsByTagName(temp);
  XMLString::release(&temp);
  
  int parentIndex = 0;
  DOMElement* parent = dynamic_cast<DOMElement*>(list->item(parentIndex));

  DOMNodeList *kids = parent->getChildNodes();

  int j, n = kids->getLength();
  DOMNode *nod = 0;
  DOMElement* ele = 0;
  for (j = 0; j < n; j++)
    {
      nod = kids->item(j);

      if (nod->getNodeType() == DOMNode::ELEMENT_NODE)
	{
	  ele = dynamic_cast<xercesc::DOMElement *> (nod);
	  char *ptr = XMLString::transcode(ele->getTagName());
	  nodeList.push_back(std::string(ptr));
	}
    }

  return nodeList;
}


std::string GeoXMLUtils::getChildValue(const char* parentTag,
				    int parentIndex,
				    const char* childTag,
				    int childIndex) const
{

  //  std::cout<<"GetChildValue : "<<parentTag<<" "<<parentIndex<<" "<<childTag<<" "<<childIndex<<std::endl;

    XMLCh* temp = XMLString::transcode(parentTag);
    DOMNodeList* list = m_doc->getElementsByTagName(temp);
    XMLString::release(&temp);
    if(list->getLength()==0) return std::string("");
    
    DOMElement* parent = dynamic_cast<DOMElement*>(list->item(parentIndex));
    DOMElement* child = dynamic_cast<DOMElement*>(parent->getElementsByTagName( XMLString::transcode(childTag))->item(childIndex));
    
    std::string value;
    if (child) {
      char* temp2 = XMLString::transcode(child->getTextContent());
      value = temp2;
      XMLString::release(&temp2);
    }
    else {
      value = "";
    }
    return value;
}

std::string GeoXMLUtils::getChildValue(const char* parentTag,
				    int parentIndex,
				    const char* childTag,
				    int childIndex,
				    const char* childTag2,
				    int childIndex2) const
{
    XMLCh* temp = XMLString::transcode(parentTag);
    DOMNodeList* list = m_doc->getElementsByTagName(temp);
    XMLString::release(&temp);
    
    DOMElement* parent = dynamic_cast<DOMElement*>(list->item(parentIndex));
    DOMElement* child = dynamic_cast<DOMElement*>(parent->getElementsByTagName( XMLString::transcode(childTag))->item(childIndex));

    DOMElement* child2 = dynamic_cast<DOMElement*>(child->getElementsByTagName( XMLString::transcode(childTag2))->item(childIndex2));
    
    std::string value;
    if (child) {
      char* temp2 = XMLString::transcode(child2->getTextContent());
      value = temp2;
      XMLString::release(&temp2);
    }
    else {
      value = "";
    }
    return value;
}


int GeoXMLUtils::getChildValue_Index(const char* parentTag,
				     const char* indexTag,
				     int parentIndex,
				     std::string parentValue) const
{
    XMLCh* temp = XMLString::transcode(parentTag);
    DOMNodeList* list = m_doc->getElementsByTagName(temp);
    XMLString::release(&temp);
    
    //    DOMElement* parent = 0;
    int index = 0;
    bool bFound = false;
    bool bEndOfLoop = false;
    DOMElement* child =0;

    // remove whitespaces
    parentValue.erase(std::remove(parentValue.begin(),parentValue.end(),' '),parentValue.end());

    while ( !bFound && ! bEndOfLoop)
      {
	DOMElement* tmp = dynamic_cast<DOMElement*>(list->item(index));

	if(tmp){
	  child = dynamic_cast<DOMElement*>(tmp->getElementsByTagName( XMLString::transcode(indexTag))->item(0));
	  
	  char* temp2 = XMLString::transcode(child->getTextContent());
	  std::string value = temp2;
	  value.erase(std::remove(value.begin(),value.end(),' '),value.end());
	  XMLString::release(&temp2);

	  if(parentIndex>-1&&atoi(value.c_str())==parentIndex){
	    return index;
	  }
	  else if (parentIndex<0&&value.compare(parentValue)==0){
	    return index;
	  }
	}
	else
	  bEndOfLoop = true;
	index++;
      }
    
    return -1;
}


std::vector<int> GeoXMLUtils::getChildValue_Indices(const char* parentTag,
						 const char* indexTag,
						 int parentIndex,
						 std::string parentValue) const
{
  std::vector<int> indexList;

  XMLCh* temp = XMLString::transcode(parentTag);
  DOMNodeList* list = m_doc->getElementsByTagName(temp);
  XMLString::release(&temp);
  
  //    DOMElement* parent = 0;
  int index = 0;
  bool bEndOfLoop = false;
  DOMElement* child =0;

  // remove whitespaces
  parentValue.erase(std::remove(parentValue.begin(),parentValue.end(),' '),parentValue.end());
  
  while (!bEndOfLoop)
    {

      DOMElement* tmp = dynamic_cast<DOMElement*>(list->item(index));
      
      if(tmp){
	child = dynamic_cast<DOMElement*>(tmp->getElementsByTagName( XMLString::transcode(indexTag))->item(0));

	
	char* temp2 = XMLString::transcode(child->getTextContent());
	std::string value = temp2;
	value.erase(std::remove(value.begin(),value.end(),' '),value.end());
	XMLString::release(&temp2);

	if(parentIndex>-1&&atoi(value.c_str())==parentIndex){
	  indexList.push_back(index);
	}
	else if(parentIndex<0&&value.compare(parentValue)==0){
	  indexList.push_back(index);
	}
	else if(value.compare("all")==0){
	  indexList.push_back(index);
	}
      }
      else
	bEndOfLoop = true;
      index++;
    }
  
  return indexList;
}

std::string GeoXMLUtils::getChildAttribute(const char* parentTag,
					 int parentIndex, const char* childTag, int childIndex,
					 const char* attributeTag) const
{
  XMLCh* temp = XMLString::transcode(parentTag);
  DOMNodeList* list = m_doc->getElementsByTagName(temp);
  XMLString::release(&temp);
  
  DOMElement* parent = dynamic_cast<DOMElement*>(list->item(parentIndex));
  DOMElement* child = dynamic_cast<DOMElement*>(parent->getElementsByTagName(XMLString::transcode(childTag))->item(childIndex));
  
  std::string value;
  if (child) {
    
    std::cout<<"MATERIAL Child found - "<<childTag<<" "<<childIndex<<"    get "<<attributeTag<<std::endl;

    temp = XMLString::transcode(attributeTag);
    char* temp2 = XMLString::transcode(child->getAttribute(temp));
    value = temp2;
    XMLString::release(&temp2);

  }
  else {
    value = "";
  }
  return value;
}


int GeoXMLUtils::getChildCount(const char* parentTag, int parentIndex,
                                  const char* childTag) const
{
  XMLCh* temp = XMLString::transcode(parentTag);
  DOMNodeList* list = m_doc->getElementsByTagName(temp);
  XMLString::release(&temp);
  
  DOMElement* parent = dynamic_cast<DOMElement*>(list->item(parentIndex));
  DOMNodeList* childList = parent->getElementsByTagName(XMLString::transcode(childTag));
  return (int)childList->getLength();
}


// #include <stdio.h>
// #include <stdlib.h>
// #include <string>
// #include <iostream>
// #include "XmlDomDocument.h"
 
// int main(int argc, char** argv)
// {
//     string value;
//     XmlDomDocument* doc = new XmlDomDocument("./bookstore.xml");
//     if (doc) {
//         for (int i = 0; i < doc->getChildCount("bookstore", 0, "book"); i++) {
//             printf("Book %d\n", i+1);
//             value = doc->getChildAttribute("bookstore", 0, "book", i, "category");
//             printf("book category   - %s\n", value.c_str());
//             value = doc->getChildValue("book", i, "title");
//             printf("book title      - %s\n", value.c_str());
//             value = doc->getChildAttribute("book", i, "title", 0, "lang");
//             printf("book title lang - %s\n", value.c_str);
//             value = doc->getChildValue("book", i, "author");
//             printf("book author     - %s\n", value.c_str());
//             value = doc->getChildValue("book", i, "year");
//             printf("book year       - %s\n", value.c_str());
//             value = doc->getChildValue("book", i, "price");
//             printf("book price      - %s\n", value.c_str());
//         }
//         delete doc;
//     }
//     exit(0);
// }
