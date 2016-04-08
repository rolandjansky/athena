//  -*- c++ -*- 

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETSELECTORTOOLS_JETSELECTORATTRIBUTE_H
#define JETSELECTORTOOLS_JETSELECTORATTRIBUTE_H

#include "JetInterface/IJetSelector.h"
#include "AsgTools/AsgTool.h"
/////////////////////////////////////////////////////////
/// \file JetAttributeSelector.h 
/// \brief define simple IJetSelector based on jet attributes
///
/// \class JetAttributeSelector
/// \brief a IJetSelector implementation which select according to an attribute
///
/// This implementation select jets for which an attribute is in a given range.
/// Properties : 
///   * "Attribute" (string) the attribute name
///   * "AttributeType" (string) the attribute type (supported : "float", "int", "vector<float>", "vector<int>")
///   * "VectorIndex" (int) when the type is a vector, only the value of vector at this index will be considered. default= 0.
///   * "CutMin" (float) the min value. Defaults to -std::numeric_limits<float>::max() so that unset is equivalent to no minimun value.
///   * "CutMax" (float) the max value. Defaults to +std::numeric_limits<float>::max() so that unset is equivalent to no maximum value.
///
///
/// \class JetAttributeRatioSelector
/// \brief a IJetSelector implementation which select according to ratio of attributes
///
/// This class is typically usefull for ratios commonly used (like Tau3/Tau2) and which are stored as separate attributes.
/// Same properties as above plus   
///   * "Attribute2" (string) the denominator attribute name
///   * "AttributeType2" (string) the denominator attribute type
///   * "VectorIndex" (int)  the index in the denominator vector.
///
////////////////////////////////////////////////////////////////////////

class JetAttributeSelector : public asg::AsgTool , virtual public IJetSelector
 { 
   ASG_TOOL_CLASS(JetAttributeSelector,IJetSelector)
   public:

   /// \class SelValueRetriever retrieves the value on which this tool select jets.
   /// concrete classes are used internally to retrieve the attribute according to its type. 
   struct SelValueRetriever {
     virtual float value(const xAOD::Jet& j) = 0;
     virtual ~SelValueRetriever(){};
   };
   
   JetAttributeSelector(const std::string &t);
   virtual ~JetAttributeSelector();

   virtual StatusCode initialize() ;
     
   virtual int keep(const xAOD::Jet& jet) const;

 protected:
   SelValueRetriever * buildValueRetriever(const std::string& type, const std::string& name, int index);
   
   float m_min;
   float m_max;

   std::string m_attName;
   std::string m_attType;
   
   int m_vectorAttIndex; /// if the attribute is a vector we'll use the value at this index. else it is ignored.
   SelValueRetriever *m_vretriever; /// A helper object which implement the attribute extraction details (i.e. according to the type)
   
};


class JetAbsAttributeSelector : public JetAttributeSelector {
  
  ASG_TOOL_CLASS0(JetAbsAttributeSelector)
public:
  JetAbsAttributeSelector(const std::string &t);
  virtual int keep(const xAOD::Jet& jet) const;

};

class JetAttributeRatioSelector : public JetAttributeSelector {
  
  ASG_TOOL_CLASS0(JetAttributeRatioSelector)
public:
  JetAttributeRatioSelector(const std::string &t);

   virtual StatusCode initialize() ;
     
   virtual int keep(const xAOD::Jet& jet) const;
 protected:

   std::string m_attName2;
   std::string m_attType2;
   
   int m_vectorAttIndex2; /// if the attribute is a vector we'll use the value at this index. else it is ignored.
   SelValueRetriever *m_vretriever2;
  
};




#endif
