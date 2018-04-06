/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTTAGALGS_TAGBUILDERBASE_H
#define EVENTTAGALGS_TAGBUILDERBASE_H

#include "AthenaPoolUtilities/TagAthenaAttributeList.h"
#include "TagEvent/TagFragmentCollection.h"

#include <string>
#include <map>
#include <vector>
#include <set>

/**
   Base class of Tag Builder for AttributeListSpecification
     and builder count
*/

class AthenaAttributeSpecification;
class AthenaAttributeListSpecification;

class TagBuilderBase
{
public:

  // constructor
  TagBuilderBase ();
  
  // destructor.
  virtual ~TagBuilderBase() {}
  
private:

  // increment maximum number of builder
  void incMaxNumOfBuilder();
  
  // maximum number of builder
  static int s_maxNumOfBuilder;

  // number of builder to be executed
  static int s_numOfBuilder;

  // Supported Types
  std::set<std::string> m_supportedTypes;

  // AttributeList specification
  std::map<std::string,std::string> m_attrMap;

protected:

  // set the number of builder to the max. 
  // This method must be called by EventTagBuilder 
  void setNumOfBuilderToMax();

  // decrement the number of builder
  void decNumOfBuilder();

  //return last "length" characters of a std::string
  std::string tail(const std::string& source, const unsigned int length) const;

  // test if this is the last builder 
  bool lastBuilder();

  // add attribute to attributeListSpec
  void addAttribute(const std::string& attName, 
                    const std::string& attType, 
                    const std::string& attUnitName="UNKNOWN", 
                    const std::string& attGroupName="UNKNOWN");
  void addAttribute(const std::string & attName, AthenaAttributeType & typeobj);

  // Return Supported Types
  inline std::set<std::string> supportedTypes() {return m_supportedTypes;}

  // fill the list of tag attributes with data
  void fillAttribute(TagAthenaAttributeList* attribList, const TagFragmentCollection& tagFragment) const;
  void fill(TagAthenaAttributeList* attribList, 
            const std::vector<TagFragmentCollection::DataPair>& attributeData) const;

  bool checkAttribute(const std::string attr, const std::string * p, const int n, const int k) const; 
  bool checkAttribute(const std::string attr, const std::string * p, const int n) const;
  bool checkAttribute(const std::string attr, const AthenaAttributeSpecification * p, const int n) const;

  // check the attribute list
  bool m_check;

  // Attribute list specification object
  // This must be initialized by EventTagBuilder 
  static AthenaAttributeListSpecification* m_attribListSpec;

};

#endif
