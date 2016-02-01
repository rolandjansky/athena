/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// XMLUtil.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef GEOXMLUTIL_H
#define GEOXMLUTIL_H

// STD library
#include <iostream>
#include <string>
#include <vector>
#include <map>
// XML library
#include <xercesc/dom/DOM.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>

using namespace xercesc;

class GeoXMLUtils {
 public:
  GeoXMLUtils();
  virtual ~GeoXMLUtils();
  
  // AlgTool initialize and finalize methods
  bool InitializeXML();
  bool TerminateXML();
  void PrintErrorXML(const XMLException &e, std::string message);
  const char* getString(DOMNode *node) const;
  const char* getStringNoTrim(DOMNode *node) const;
  std::vector<double> getVectorDouble(DOMNode *node) const;
  XMLCh* transcode(std::string s);
  
  bool ParseFile(std::string xmlfile);
  std::string getChildValue(const char* parentTag,
		       int parentIndex,
		       const char* childTag,
		       int childIndex = 0) const;
  std::string getChildValue(const char* parentTag,
			    int parentIndex,
			    const char* childTag,
			    int childIndex,
			    const char* child2Tag,
			    int childIndex2=0) const;
  int getChildValue_Index(const char* parentTag,
			  const char* indexTag,
			  int parentIndex,
			  std::string parentValue = "") const;
  std::vector<int> getChildValue_Indices(const char* parentTag,
					 const char* indexTag,
					 int parentIndex,
					 std::string parentValue="") const;
  std::string getChildAttribute(const char* parentTag,
			   int parentIndex, const char* childTag, int childIndex,
			   const char* attributeTag) const;
  int getChildCount(const char* parentTag, int parentIndex,
		    const char* childTag) const;

  std::vector<std::string> getNodeList(const char* parentTag) const;

  std::string getString(const char* parentTag,int parentIndex,
			const char* childTag, int childIndex=0,
			std::string defaultValue="") const;
  std::string getString(const char* parentTag,std::vector<int> parentIndex,
			const char* childTag, int childIndex=0,
			std::string defaultValue="") const;
  double getDouble(const char* parentTag,int parentIndex,
		   const char* childTag, int childIndex=0,
		   double defaultValue=999999.) const;
  double getDouble(const char* parentTag,std::vector<int> parentIndex,
		   const char* childTag, int childIndex=0,
		   double defaultValue=999999.) const;
  int getInt(const char* parentTag,int parentIndex,
	     const char* childTag, int childIndex=0,
	     int defaultValue=99999) const;
  bool getBoolean(const char* parentTag,int parentIndex,
		  const char* childTag, int childIndex=0) const;


  std::vector<double> getVectorDouble(const char* parentTag,int parentIndex,
				      const char* childTag, int childIndex=0) const;
  std::vector<int> getVectorInt(const char* parentTag,int parentIndex,
				const char* childTag, int childIndex=0) const;
  std::vector<std::string> getVectorString(const char* parentTag,int parentIndex,
					    const char* childTag, int childIndex=0) const;

 protected:
  void releaseStrings();
  XercesDOMParser *m_ConfigFileParser;
  std::map<std::string, XMLCh *> m_Strings;

  DOMDocument* m_doc;

};

#endif //LOIXMLUTIL_H
