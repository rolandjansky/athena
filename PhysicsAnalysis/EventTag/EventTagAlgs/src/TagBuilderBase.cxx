/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "EventTagAlgs/TagBuilderBase.h"

#include "AthenaPoolUtilities/AthenaAttributeListSpecification.h"
#include "AthenaPoolUtilities/AthenaAttributeSpecification.h"

#include <sstream>
#include <stdexcept>

// initialize class members
int TagBuilderBase::s_maxNumOfBuilder = 0;
int TagBuilderBase::s_numOfBuilder    = 0;

AthenaAttributeListSpecification* TagBuilderBase::m_attribListSpec = 0;

TagBuilderBase::TagBuilderBase() : m_check(false)
{
  incMaxNumOfBuilder();

  //Supported Types
  m_supportedTypes.insert("bool");
  m_supportedTypes.insert("short");
  m_supportedTypes.insert("unsigned short");
  m_supportedTypes.insert("int");
  m_supportedTypes.insert("unsigned int");
  m_supportedTypes.insert("long");
  m_supportedTypes.insert("unsigned long");
  m_supportedTypes.insert("float");
  m_supportedTypes.insert("double");
  m_supportedTypes.insert("unsigned long long");

}


void TagBuilderBase::incMaxNumOfBuilder()
{
  ++s_maxNumOfBuilder;
}

void TagBuilderBase::setNumOfBuilderToMax()
{
  s_numOfBuilder = s_maxNumOfBuilder;
}

void TagBuilderBase::decNumOfBuilder()
{
  --s_numOfBuilder;
}

bool TagBuilderBase::lastBuilder()
{
  return (s_numOfBuilder>0) ? false : true;
}

std::string TagBuilderBase::tail(std::string const& source, const unsigned int length) const {
  //return last "length" characters of a std::string
  if (length >= source.size()) { return source; }
  return source.substr(source.size() - length);
}

/** fill the list of tag attributes with data */
void TagBuilderBase::fillAttribute(TagAthenaAttributeList* attribList,
                                   const TagFragmentCollection& tagFragment) const
{
  fill (attribList, tagFragment.attrData());
}

/** fill the attributes */
void TagBuilderBase::fill(TagAthenaAttributeList* attribList, 
                          const std::vector<TagFragmentCollection::DataPair>& attributeData) const {

   int max = attributeData.size();
   for (int i=0; i<max; ++i) {
     std::string name = attributeData[i].first;
     double val = attributeData[i].second;
     std::map<std::string, std::string>::const_iterator index = m_attrMap.find(name);
     if (index != m_attrMap.end() ) {
        std::string dataType = (*index).second;
        if (dataType == "bool")
	  (*attribList)[ (*index).first ].data<bool>() = static_cast<bool>(val);
        else if (dataType == "short")
	  (*attribList)[ (*index).first ].data<short>() = static_cast<short>(val);
        else if (dataType == "unsigned short")
	  (*attribList)[ (*index).first ].data<unsigned short>() = static_cast<unsigned short>(val);
        else if (dataType == "int")
	  (*attribList)[ (*index).first ].data<int>() = static_cast<int>(val);
        else if (dataType == "unsigned int")
	  (*attribList)[ (*index).first ].data<unsigned int>() = static_cast<unsigned int>(val);
        else if (dataType == "long")
	  (*attribList)[ (*index).first ].data<long>() = static_cast<long>(val);
        else if (dataType == "unsigned long")
	  (*attribList)[ (*index).first ].data<unsigned long>() = static_cast<unsigned long>(val);
        else if (dataType == "float")
	  (*attribList)[ (*index).first ].data<float>() = static_cast<float>(val);
        else if (dataType == "double")
	  (*attribList)[ (*index).first ].data<double>() = static_cast<double>(val);
        else if (dataType == "unsigned long long")
          (*attribList)[ (*index).first ].data<unsigned long long>() = static_cast<unsigned long long>(val);
	else
	  throw std::runtime_error("Tried to fill unsupported attribute type:"+dataType+" for "+(*index).first);
     }
   }
}

bool TagBuilderBase::checkAttribute(const std::string attr, const std::string * p, 
                                    const int n, const int k) const {

  /** do not do any check if not enforced */
  if (m_check == false) return true;

  /** check that the attribute is defined 
      user and groups can just add anything to the TAG on the fly and at will */

  bool check = false;
  unsigned int sSize= attr.size();

  for (int i=0; i<n; ++i) {

    unsigned int iSize = (*(p+i)).size();

    if (attr == *(p+i) ) {
       check = true;
       break;
       //    } else if (attr.find( *(p+i) ) == 0 && (sSize-iSize) == 1) {
    } else if (attr.find( *(p+i) ) == 0 && ( (sSize-iSize) == 1 || ( (sSize-iSize) == 2 && k >= 10 ) ) ) {

      std::string num = tail(attr, (sSize-iSize));

      //      char chars[1];
      //      attr.copy(chars, (sSize-iSize), iSize);
      //      std::istringstream s(chars);
      std::istringstream s(num);
      int val;
      s >> val;
      //int val = std::atoi(chars);  Does not work for 64 bit
      if ( ( (val >=1 && val <=k ) || (val >= 0 && val < k) ) ) {
      check = true;
      break;
      }
    }
  }
  return check;
}

bool TagBuilderBase::checkAttribute(const std::string attr, 
                                    const std::string * p, 
                                    const int n) const 
{

  /** do not do any check if not enforced */
  if (m_check == false) return true;

  /** check that the attribute is defined 
      user and groups can just add anything to the TAG on the fly and at will */

  bool check = false;

  for (int i=0; i<n; ++i) {
    if (attr == *(p+i) ) {
       check = true;
       break;
    }
  }
  return check;
}

bool TagBuilderBase::checkAttribute(const std::string attr, 
                                    const AthenaAttributeSpecification * p, 
                                    const int n) const 
{

  /** do not do any check if not enforced */
  if (m_check == false) return true;

  /** check that the attribute is defined 
      user and groups can just add anything to the TAG on the fly and at will */

  bool check = false;

  for (int i=0; i<n; ++i) {
    if (attr == p[i].name() ) {
       check = true;
       break;
    }
  }
  return check;
}


// add attribute to attributeListSpec
void TagBuilderBase::addAttribute(const std::string& attName, 
                                  const std::string& attType, 
                                  const std::string& attUnitName, 
                                  const std::string& attGroupName)
{
   AthenaAttributeType typeobj(attType,attUnitName,attGroupName);
   addAttribute(attName,typeobj);
}


void TagBuilderBase::addAttribute(const std::string & attName, AthenaAttributeType & typeobj) {

   if ( m_supportedTypes.find(typeobj.typeName()) != m_supportedTypes.end() ) {
       bool check2 = (m_attrMap.find(attName)==m_attrMap.end());
       if (check2) {
          m_attrMap[attName] = typeobj.typeName(); 
          m_attribListSpec->extend(attName,typeobj);
       }
       else { 
          throw std::runtime_error(attName+" already in use " );
       }
   } 
   else {
       throw std::runtime_error("Tried to add unsupported attribute type:"+typeobj.typeName()+" for "+attName );
   }

}
