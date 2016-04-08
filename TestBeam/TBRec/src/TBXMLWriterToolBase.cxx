/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "TBRec/TBXMLWriterToolBase.h"

#include <iostream>
#include <string>
#include <vector>

///////////////////////////////
// XML writer implementation //
///////////////////////////////

std::string TBXMLWriterToolBase::m_doubleQuote = "\042"; 

// dictionary
void
TBXMLWriterToolBase::openDictionary(std::ostream& outStream,
				    const std::string& dictName,
				    const std::vector<std::string>&
				    listOfElements)
{
  // clean up
  m_elementList.clear();
  m_dictionary = dictName;

  // write header
  outStream << "<?XML version=" << m_doubleQuote
	    << "1" << m_doubleQuote << "?>" << std::endl;
  
  // write begin of dictionary
  outStream << "<!DOCTYPE " << dictName
	    << " [" << std::endl;

  // add doctype as element
  this->addDictElement(outStream,dictName,listOfElements);
}      
      
// template <typename DATA>
void
TBXMLWriterToolBase::openDictionary(std::ostream& outStream,
				    const std::string& dictName)
{
  std::vector<std::string> emptyList;
  this->openDictionary(outStream,dictName, emptyList);
}

// template <typename DATA>
void
TBXMLWriterToolBase::closeDictionary(std::ostream& outStream)
{
  outStream << "]>" << std::endl;
}

//template <typename DATA>
void
TBXMLWriterToolBase::addDictElement(std::ostream& outStream,
				    const std::string& theElement,
				    const std::vector<std::string>&
				    listOfElements)
{
  outStream << "<!ELEMENT " << theElement;
  // add document as element
  if ( listOfElements.size() == 0 )
    {
      outStream << ">" << std::endl;
    }
  else
    {
      // store elements
      if ( listOfElements[0] == "ANY"   ||
	   listOfElements[0] == "EMPTY" ||
	   listOfElements[0] == "ALL" )
	{
	  outStream << " " << listOfElements[0] << " >" << std::endl;
	}
      else
	{
	  outStream << " (" << listOfElements[0];
	  for ( unsigned int i=1; i<listOfElements.size(); i++ )
	    {
	      outStream << "," << listOfElements[i];
	    }
	  outStream << ") >" << std::endl;
	}
    }
}

// template <typename DATA>
void
TBXMLWriterToolBase::addAttributes(std::ostream& outStream,
				   const std::string& theElement,
				   const std::vector<std::string>&
				   listOfAttr)
{
  outStream << "  <!ATTLIST " << theElement << std::endl;
  for ( unsigned int i=0; i<listOfAttr.size(); i++ )
    {
      outStream << "            " << listOfAttr[i] << std::endl;
    }
  outStream << ">" << std::endl;
}

// template <typename DATA>
void
TBXMLWriterToolBase::openElement(std::ostream& outStream,
				 const std::string& theElement,
				 const std::vector<std::string>&
				 listOfAttr,
				 const std::vector<std::string>&
				 listOfValues)
{
  m_elementList.push_back(theElement);
  outStream << "<" << theElement;
  if ( listOfAttr.size() > 0 && listOfValues.size() == listOfAttr.size() )
    {
      for ( unsigned int i=0; i<listOfAttr.size(); i++ )
	{
	  outStream << " " << listOfAttr[i] << "="
		    << m_doubleQuote 
		    << listOfValues[i]
		    << m_doubleQuote;
	}
    }
  outStream << ">" << std::endl;
}

// template <typename DATA>
void
TBXMLWriterToolBase::closeElement(std::ostream& outStream)
{
  unsigned int thisSize = m_elementList.size() - 1;
  outStream << "</" << m_elementList[thisSize] << ">" << std::endl;
  m_elementList.resize(thisSize);
}

// template <typename DATA>
void
TBXMLWriterToolBase::closeElement(std::ostream& outStream,
				  const std::string& theElement)
{
  std::vector<std::string>::iterator inVec = 
    find( m_elementList.begin(), m_elementList.end(), theElement);
  if ( inVec != m_elementList.end() )
    {
      outStream << "</" << (*inVec) << ">" << std::endl;
      m_elementList.erase(inVec);
    }
  else
    {
      outStream << "<!-- XML Structural Problem! \n\n"
		<< "     cannot find an element " << theElement << "!\n\n"
		<< "-->" << std::endl;
    }
}

// template <typename DATA>
void
TBXMLWriterToolBase::closeAllElements(std::ostream& outStream)
{
  for ( int i=m_elementList.size()-1; i>=0; i-- ) 
    {
      outStream << "</" << m_elementList[i] << ">" << std::endl;
    }
  m_elementList.clear();
}

