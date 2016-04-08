/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//$Id: FCEntry.h 622332 2014-10-17 14:10:37Z ssnyder $
#ifndef POOL_FCENTRY_H
#define POOL_FCENTRY_H
#include <string>
#include <iostream>
#include <sstream>
#include "CoralBase/AttributeList.h"
#include "CoralBase/AttributeListSpecification.h"
#include "CoralBase/Attribute.h"

namespace pool{
  /** @class PFNEntry FCEntry.h FileCatalog/FCEntry.h
      Structure of one PFN table entry
      @author Zhen Xie
  */
  namespace fc{
    template <class T>
      bool from_string(T& t, const std::string& s, std::ios_base& (*f)(std::ios_base&)){
      std::istringstream iss(s);
      return !(iss >> f >> t).fail();
    }
    template <class T>
      std::string 
      to_string(T& t){
      std::stringstream o;
      o<<t;
      return o.str();
    }
  }//ns fc
  class PFNEntry{
  public:
    ///default constructor
    PFNEntry();
    ~PFNEntry(){}
    /**PFNEntry constructor
       @param ipfname [IN] PFN 
       @param iguid [IN] fileid
       @param ifiletype [IN] file technology type
     */ 
    PFNEntry(const std::string& ipfname, 
             const std::string& iguid,
	     const std::string& ifiletype 
	     );
    // Default copy constructor
    /**fill the PFNEntry struct
     */
    void setValue(const std::string& ipfname, 
		  const std::string& iguid, 
		  const std::string& ifiletype 
		  );
    std::string guid() const;
    const std::string pfname() const;
    const std::string filetype() const;
    void toOutputStream(std::ostream& s) const;
  private:
    coral::AttributeList m_attrs;
  };
  
  inline std::ostream& operator << (std::ostream& s, const PFNEntry& p){
    p.toOutputStream(s);
    return s;
  } 

  class LFNEntry{
  public:
    ///default constructor
    LFNEntry();
    /**constructor
       @param ilfname [IN] LFN 
       @param iguid [IN] fileid
    */
    LFNEntry(const std::string& ilfname, const std::string& iguid);
    // Default copy constructor.
    void setValue( const std::string& ilfname, const std::string& iguid);
    std::string guid() const;
    const std::string lfname() const;
    void toOutputStream(std::ostream& s) const;
  private:
    coral::AttributeList m_attrs;
  };
  
  inline std::ostream& operator << (std::ostream& s, const LFNEntry& l){
    l.toOutputStream(s);
    return s;
  } 

  class MetaDataEntry{
  public:
    MetaDataEntry();
    // Default copy constructor.
    unsigned int size() const;
    std::string guid() const;
    void addAttributeSpec(const std::string& attr_name, 
			  const std::string& type_name );
    void setGuidValue(const std::string& iguid);
    template<class T>
      void setAttributeValue( const std::string& attr_name, const T& t){
      m_attrs[attr_name].data<T>()=t;
    }
    //void setAttributeValueAsString( const std::string& attr_name, const std::string& t );
    std::string getGuidValue() const;
    //std::string getAttributeValueAsString( const std::string& attr_name ) const;
    void toOutputStream(std::ostream& s) const;
    // MetaDataEntry& append_and_merge_spec( MetaDataEntry& mentry );
    coral::AttributeList& attrs();
    template<class T>
      T getAttributeValue( const std::string& attr_name ) const{
      return m_attrs[attr_name].data<T>();
    }
  private:
    coral::AttributeList m_attrs;
    
  };

  inline std::ostream& operator << (std::ostream& s, const MetaDataEntry& m){
    m.toOutputStream(s);
    return s;
  } 
}//ns pool
#endif
