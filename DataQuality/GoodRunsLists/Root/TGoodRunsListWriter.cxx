/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/**********************************************************************************
 * Class  : TGoodRunsListWriter                                                   *
 *                                                                                *
 * Authors (alphabetical):                                                        *
 *      Max Baak <mbaak@cern.ch> - CERN, Switzerland                              *
 * 
 * Inspiration from: http://194.199.20.115/examples/testWriter.c
 **********************************************************************************/

#include <iostream>
#include <stdlib.h>
// #include <stdio.h>
#include <string.h>
#include "TROOT.h"

#include "GoodRunsLists/TGoodRunsListWriter.h"
#include "GoodRunsLists/TGoodRunsList.h"
#include "GoodRunsLists/TGoodRun.h"
#include "GoodRunsLists/TLumiBlockRange.h"

#ifndef __NOLIBXML__
#ifdef __clang__
# define LIBXML_ATTR_ALLOC_SIZE(x)
#endif
#include <libxml/encoding.h>
#include <libxml/xmlwriter.h>
#endif

#define MY_ENCODING "ISO-8859-1"

ClassImp(Root::TGoodRunsListWriter)

using namespace std;


Root::TGoodRunsListWriter::TGoodRunsListWriter()
 : TObject()
 , m_logger     ( "TGoodRunsListWriter" )
 , m_mergegrls(false)
 , m_individuals(false)
{
}


Root::TGoodRunsListWriter::TGoodRunsListWriter( const Root::TGoodRunsList& goodrunslist, const TString& dataCardName )
 : TObject()
 , m_dataCardName( dataCardName )
 , m_logger     ( "TGoodRunsListWriter" )
 , m_mergegrls(false)
 , m_individuals(false)
{
  m_grlvec.push_back(goodrunslist);
}


Root::TGoodRunsListWriter::~TGoodRunsListWriter()
{
  this->reset();
}


void
Root::TGoodRunsListWriter::reset()
{
  m_grlvec.Reset();
  m_grl.clear();
  m_mergegrls = false;
  m_individuals = false;
  m_prefix="";
  m_xmlstring="";
  m_xmlstringVec.clear();
}


Bool_t 
Root::TGoodRunsListWriter::WriteXMLFiles()
{
  if (m_grlvec.IsEmpty()) {
    m_logger << kWARNING << "GoodRunsList is empty - nothing to write. Return false." << GEndl;
    return kFALSE;
  }

  m_individuals=true;
  TString olddatacardname=m_dataCardName; 

  /// loop over goodrunslists
  std::vector< Root::TGoodRunsList >::const_iterator litr = m_grlvec.begin();
  for (int i=0; litr!=m_grlvec.end(); ++litr, ++i) {
    /// set grl to store
    m_grl = (*litr);
    // write xml for this goodrunslist
    m_dataCardName = m_prefix + Form("merged_%d_",i) + litr->GetSuggestedName() + ".xml" ;
    (void) this->WriteXMLFile();
  }

  // reset back to original values
  m_individuals=false;
  m_dataCardName=olddatacardname;

  return kTRUE;
}


Bool_t 
Root::TGoodRunsListWriter::WriteXMLFile()
{
  if (m_grlvec.IsEmpty()) {
    m_logger << kWARNING << "GoodRunsList is empty - nothing to write to <" << m_dataCardName << ">. Return false." << GEndl;
    return kFALSE;
  }
  if (m_dataCardName.IsNull()) {
    m_logger << kINFO << "Output filename not set. Will be generated automatically." << GEndl;
  }

#ifndef __NOLIBXML__

  int rc;
  xmlTextWriterPtr writer;
  xmlDocPtr doc;
  
  /* Create a new XmlWriter for DOM, with no compression. */
  writer = xmlNewTextWriterDoc(&doc, 0);
  if (writer == NULL) {
    m_logger << kWARNING << "testXmlwriterDoc: Error creating the xml writer" << GEndl;
    return kFALSE;
  }
  
  /* Write out goodrunslist here */
  this->WriteLumiRangeCollection(writer);

  /* Here we could close the elements NamedLumiRange and LumiRangeCollection using the
   * function xmlTextWriterEndElement, but since we do not want to
   * write any other elements, we simply call xmlTextWriterEndDocument,
   * which will do all the work. */
  rc = xmlTextWriterEndDocument(writer);
  if (rc < 0) {
    m_logger << kWARNING << "testXmlwriterDoc: Error at xmlTextWriterEndDocument" << GEndl;
    return kFALSE;
  }

  xmlFreeTextWriter(writer);
  //xmlSaveFileEnc(m_dataCardName.Data(), doc, MY_ENCODING);
  xmlSaveFormatFile(m_dataCardName.Data(), doc, 1);
  xmlFreeDoc(doc);

  m_logger << kINFO << "GoodRunsList stored as        : " << m_dataCardName << GEndl;

#else

  m_logger << kWARNING << "GoodRunsLists package compiled without libxml2" << GEndl;
  m_logger << kWARNING << "can not write out GoodRunsList" << GEndl;
  
#endif

  return kTRUE;
}


const std::vector<TString>&
Root::TGoodRunsListWriter::GetXMLStrings()
{
  m_xmlstringVec.clear();

  if (m_grlvec.IsEmpty()) {
    m_logger << kWARNING << "GoodRunsList is empty - nothing to write. Return false." << GEndl;
    return m_xmlstringVec;
  }

  // individual strings
  m_individuals=true;

  /// loop over goodrunslists
  std::vector< Root::TGoodRunsList >::const_iterator litr = m_grlvec.begin();
  for (int i=0; litr!=m_grlvec.end(); ++litr, ++i) {
    /// set grl to store
    m_grl = (*litr);
    // get xml string for this goodrunslist
    m_xmlstringVec.push_back( this->GetXMLString() );
  }

  // reset back to original values
  m_individuals=false;

  return m_xmlstringVec;
}


const TString& 
Root::TGoodRunsListWriter::GetXMLString()
{
  m_xmlstring="";
  
  if (m_grlvec.IsEmpty()) {
    m_logger << kWARNING << "GoodRunsList is empty - nothing to write. Return false." << GEndl;
    return m_xmlstring;
  }

#ifndef __NOLIBXML__

  int rc;
  xmlTextWriterPtr writer;
  xmlBufferPtr buf;

  /* Create a new XML buffer, to which the XML document will be
   * written */
  buf = xmlBufferCreate();
  if (buf == NULL) {
    m_logger << kWARNING << "testXmlwriterMemory: Error creating the xml buffer" << GEndl;
    return m_xmlstring;
  }

  /* Create a new XmlWriter for memory, with no compression.
   * Remark: there is no compression for this kind of xmlTextWriter */
  writer = xmlNewTextWriterMemory(buf, 0);
  if (writer == NULL) {
    m_logger << kWARNING << "testXmlwriterMemory: Error creating the xml writer" << GEndl;
    return m_xmlstring;
  }

  /* Write out goodrunslist here */
  this->WriteLumiRangeCollection(writer);

  /* Here we could close the elements ORDER and EXAMPLE using the
   * function xmlTextWriterEndElement, but since we do not want to
   * write any other elements, we simply call xmlTextWriterEndDocument,
   * which will do all the work. */
  rc = xmlTextWriterEndDocument(writer);
  if (rc < 0) {
    m_logger << kWARNING << "testXmlwriterMemory: Error at xmlTextWriterEndDocument" << GEndl;
    return m_xmlstring;
  }

  xmlFreeTextWriter(writer);
  m_xmlstring = (const char *) buf->content;
  xmlBufferFree(buf);

#else

  m_logger << kWARNING << "GoodRunsLists package compiled without libxml2" << GEndl;
  m_logger << kWARNING << "can not write out GoodRunsList" << GEndl;
  
#endif

  return m_xmlstring;
}


Bool_t 
Root::TGoodRunsListWriter::WriteLumiRangeCollection(xmlTextWriterPtr writer)
{
#ifndef __NOLIBXML__

  int rc;
  xmlChar *tmp;

  /* Start the document with the xml default for the version,
   * default encoding, and the default for the standalone declaration. */
  rc = xmlTextWriterStartDocument(writer, NULL, NULL/*MY_ENCODING*/, NULL);
  if (rc < 0) {
    m_logger << kWARNING << "testXmlwriterDoc: Error at xmlTextWriterStartDocument" << GEndl;
    return kFALSE;
  }

  /* Write DTD section */
  xmlTextWriterWriteDTD(writer, (xmlChar *)"LumiRangeCollection", NULL,
                        (xmlChar *)"http://atlas-runquery.cern.ch/LumiRangeCollection.dtd", NULL);

  /* Write a comment as child of LumiRangeCollection.
   * Please observe, that the input to the xmlTextWriter functions
   * HAS to be in UTF-8, even if the output XML is encoded
   * in iso-8859-1 */
  tmp = ConvertInput("This document is created by GoodRunsListWriter.", MY_ENCODING);
  rc = xmlTextWriterWriteComment(writer, tmp);
  if (rc < 0) {
    m_logger << kWARNING << "testXmlwriterDoc: Error at xmlTextWriterWriteComment" << GEndl;
    return kFALSE;
  }
  if (tmp != NULL) xmlFree(tmp);

  /* Start an element named "LumiRangeCollection". Since thist is the first
   * element, this will be the root element of the document. */
  rc = xmlTextWriterStartElement(writer, BAD_CAST "LumiRangeCollection");
  if (rc < 0) {
    m_logger << kWARNING << "testXmlwriterDoc: Error at xmlTextWriterStartElement" << GEndl;
    return kFALSE;
  }

  /* Write the actual goodrunslist here. 
   * Use m_grl as global grl currently processed 
   */
  if (m_individuals) {
    // m_grl has already been set.
    this->WriteNamedLumiRange(writer);
  } else {
    // m_grl is set here below
    if (!m_mergegrls) {
      std::vector< Root::TGoodRunsList >::const_iterator litr = m_grlvec.begin();
      for (; litr!=m_grlvec.end(); ++litr) {
        m_grl = (*litr);
        this->WriteNamedLumiRange(writer);
      }
    } else {
      m_grl = this->GetMergedGoodRunsList(); // OR merging
      this->WriteNamedLumiRange(writer);
    }
  }

  /* Close the element */
  rc = xmlTextWriterEndElement(writer);
  if (rc < 0) {
    m_logger << kWARNING << "testXmlwriterDoc: Error at xmlTextWriterEndElement" << GEndl;
    return kFALSE;
  }

#else

  m_logger << kWARNING << "GoodRunsLists package compiled without libxml2" << GEndl;
  m_logger << kWARNING << "can not write out GoodRunsList" << GEndl;
  

#endif

  return kTRUE;
}


Bool_t 
Root::TGoodRunsListWriter::WriteNamedLumiRange(xmlTextWriterPtr writer)
{
  if (m_dataCardName.IsNull()) {
    m_dataCardName = m_prefix + "merged_" + m_grl.GetSuggestedName() + ".xml" ;
  }

#ifndef __NOLIBXML__

  int rc;
  /* Start an element named "NamedLumiRange" as child of LumiRangeCollection. */
  rc = xmlTextWriterStartElement(writer, BAD_CAST "NamedLumiRange");
  if (rc < 0) {
    m_logger << kWARNING << "testXmlwriterDoc: Error at xmlTextWriterStartElement" << GEndl;
    return kFALSE;
  }

  /* Compress grl before writing to file */
  m_grl.Compress(); // safe space, remove duplicates.
  /* Write an element named "NAME" as child of NamedLumiRange. */
  this->WriteElement(writer,"Name",m_grl.GetName());
  /* Write an element named "VERSION" as child of NamedLumiRange. */
  if (m_grl.GetVersion().Length()>0) {
    this->WriteElement(writer,"Version",m_grl.GetVersion().Data());
  } else { m_logger << kINFO << "<Version> written to xml file <" << m_dataCardName << "> is empty." << GEndl; }
  /* Write Metadata */
  if (!m_grl.GetMetaData().empty()) {
    std::map<TString,TString>::const_iterator mitr = m_grl.GetMetaData().begin();
    for (; mitr!=m_grl.GetMetaData().end(); ++mitr)
      this->WriteElement(writer,"Metadata",mitr->second.Data(),"Name",mitr->first.Data());
  } else { m_logger << kINFO << "<Metadata> written to xml file <" << m_dataCardName << "> is empty." << GEndl; } 
  /* loop over goodruns */
  std::map<Int_t,Root::TGoodRun>::const_iterator gitr = m_grl.begin();
  for (; gitr!=m_grl.end(); ++gitr)
    this->WriteLumiBlockCollection(writer,gitr->second);

  /* Close the element named NamedLumiRange. */
  rc = xmlTextWriterEndElement(writer);
  if (rc < 0) {
    m_logger << kWARNING << "testXmlwriterDoc: Error at xmlTextWriterEndElement" << GEndl;
    return kFALSE;
  }

#else

  m_logger << kWARNING << "GoodRunsLists package compiled without libxml2" << GEndl;
  m_logger << kWARNING << "can not write out GoodRunsList" << GEndl;
  

#endif

  return kTRUE;
}


Bool_t
Root::TGoodRunsListWriter::WriteLumiBlockCollection(xmlTextWriterPtr writer, const Root::TGoodRun& goodrun)
{
#ifndef __NOLIBXML__
  int rc;

  /* Start an element named "NamedLumiRange" as child of LumiRangeCollection. */
  rc = xmlTextWriterStartElement(writer, BAD_CAST "LumiBlockCollection");
  if (rc < 0) {
    m_logger << kWARNING << "testXmlwriterDoc: Error at xmlTextWriterStartElement" << GEndl;
    return kFALSE;
  }

  if (!goodrun.IsEmpty()) { 
    /* Runnumber */
    this->WriteElement(writer,"Run",Form("%d",goodrun.GetRunNumber()));
    /* Start an element named "LBRange" */
    std::vector<TLumiBlockRange>::const_iterator litr = goodrun.begin();
    for (; litr!=goodrun.end(); ++litr)
      if (!litr->IsEmpty()) { 
        if (litr->End()!=2147483647)
          this->WriteElement(writer,"LBRange",0,"Start",Form("%d",litr->Begin()),"End",Form("%d",litr->End()));
        else 
          this->WriteElement(writer,"LBRange",0,"Start",Form("%d",litr->Begin()));
      } 
  }

  /* Close the element named Metadata. */
  rc = xmlTextWriterEndElement(writer);
  if (rc < 0) {
    m_logger << kWARNING << "testXmlwriterDoc: Error at xmlTextWriterEndElement" << GEndl;
    return kFALSE;
  }

#else

  m_logger << kWARNING << "GoodRunsLists package compiled without libxml2" << GEndl;
  m_logger << kWARNING << "can not write out GoodRunsList" << GEndl;
  
#endif

  return kTRUE;
}


Bool_t
Root::TGoodRunsListWriter::WriteElement(xmlTextWriterPtr writer, 
                                        const char* name, const char* value, 
                                        const char* atr1, const char* val1, const char* atr2, const char* val2)
{
#ifndef __NOLIBXML__

  int rc;
  /* Start an element named 'name' as child of previous element. */
  rc = xmlTextWriterStartElement(writer, BAD_CAST (name));
  if (rc < 0) {
    m_logger << kWARNING << "testXmlwriterDoc: Error at xmlTextWriterStartElement" << GEndl;
    return kFALSE;
  }
  /* Add an attribute 'atr1' and value 'val1' to element. */
  if ((atr1!=0) && (val1!=0)) {
    rc = xmlTextWriterWriteAttribute(writer, BAD_CAST (atr1), BAD_CAST (val1));
    if (rc < 0) {
      m_logger << kWARNING << "testXmlwriterDoc: Error at xmlTextWriterWriteAttribute" << GEndl;
      return kFALSE;
    }
  }
  /* Add an attribute 'atr2' and value 'val2' to element. */
  if ((atr2!=0) && (val2!=0)) {
    rc = xmlTextWriterWriteAttribute(writer, BAD_CAST (atr2), BAD_CAST (val2));
    if (rc < 0) {
      m_logger << kWARNING << "testXmlwriterDoc: Error at xmlTextWriterWriteAttribute" << GEndl;
      return kFALSE;
    }
  }
  /* Add value 'value' to the element */
  if ((value!=0)) {
    rc = xmlTextWriterWriteString(writer, BAD_CAST (value));
    if(rc < 0) {
      m_logger << kWARNING << "testXmlwriterDoc: Error at xmlTextWriterWriteString" << GEndl;
      return kFALSE;
    }
  }
  /* Close the element */
  rc = xmlTextWriterEndElement(writer);
  if (rc < 0) {
    m_logger << kWARNING << "testXmlwriterDoc: Error at xmlTextWriterEndElement" << GEndl;
    return kFALSE;
  }

#else

  m_logger << kWARNING << "GoodRunsLists package compiled without libxml2" << GEndl;
  m_logger << kWARNING << "can not write out GoodRunsList" << GEndl;
  

#endif

  return kTRUE;
}


/**
 * ConvertInput:
 * @in: string in a given encoding
 * @encoding: the encoding used
 *
 * Converts @in into UTF-8 for processing with libxml2 APIs
 *
 * Returns the converted UTF-8 string, or NULL in case of error.
 */
xmlChar *
Root::TGoodRunsListWriter::ConvertInput(const char *in, const char *encoding)
{
  xmlChar* out(0);

#ifndef __NOLIBXML__

  int ret;
  int size;
  int out_size;
  int temp;
  xmlCharEncodingHandlerPtr handler;
  
  if (in == 0)
    return 0;
  
  handler = xmlFindCharEncodingHandler(encoding);
  
  if (!handler) {
    m_logger << kWARNING << "ConvertInput: no encoding handler found for " << (encoding ? encoding : "") << GEndl;
    return 0;
  }
  
  size = (int) strlen(in) + 1;
  out_size = size * 2 - 1;
  out = (unsigned char *) xmlMalloc((size_t) out_size);
  
  if (out != 0) {
    temp = size - 1;
    ret = handler->input(out, &out_size, (const xmlChar *) in, &temp);
    if ((ret < 0) || (temp - size + 1)) {
      if (ret < 0) {
	m_logger << kWARNING << "ConvertInput: conversion wasn't successful." << GEndl;
      } else {
	m_logger << kWARNING << "ConvertInput: conversion wasn't successful. Converted: " << temp << " octets." << GEndl;
      }
      
      xmlFree(out);
      out = 0;
    } else {
      out = (unsigned char *) xmlRealloc(out, out_size + 1);
      out[out_size] = 0;  /*null terminating out */
    }
  } else {
    m_logger << kWARNING << "ConvertInput: no mem" << GEndl;
  }

#else

  m_logger << kWARNING << "GoodRunsLists package compiled without libxml2" << GEndl;
  m_logger << kWARNING << "can not write out GoodRunsList" << GEndl;
  

#endif
  
  return out;
}


