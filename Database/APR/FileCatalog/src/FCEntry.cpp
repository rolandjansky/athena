/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FileCatalog/FCEntry.h"
#include "FileCatalog/FCException.h"
namespace pool{
  
  PFNEntry::PFNEntry(){
      m_attrs.extend<std::string>("guid");
      m_attrs.extend<std::string>("pfname");
      m_attrs.extend<std::string>("filetype");
  }
  PFNEntry::PFNEntry(const std::string& ipfname, 
		     const std::string& iguid, 
	             const std::string& ifiletype){
    m_attrs.extend<std::string>("guid");
    m_attrs.extend<std::string>("pfname");
    m_attrs.extend<std::string>("filetype");
    this->setValue(ipfname,iguid,ifiletype);
  }
  void PFNEntry::setValue(const std::string& ipfname, 
			  const std::string& iguid, 
			  const std::string& ifiletype
			  ){
    m_attrs["guid"].data<std::string>()=iguid;
    m_attrs["pfname"].data<std::string>()=ipfname;
    m_attrs["filetype"].data<std::string>()=ifiletype;
  }
  std::string PFNEntry::guid() const{
    return m_attrs["guid"].data<std::string>();
  }
  const std::string PFNEntry::pfname() const{
    return m_attrs["pfname"].data<std::string>();
  }
  const std::string PFNEntry::filetype() const{
    return m_attrs["filetype"].data<std::string>();
  }
  void PFNEntry::toOutputStream(std::ostream& s) const{
    m_attrs.toOutputStream(s);
  }

  LFNEntry::LFNEntry(){
    m_attrs.extend<std::string>("guid");
    m_attrs.extend<std::string>("lfname");
  }
  LFNEntry::LFNEntry(const std::string& ilfname, const std::string& iguid){
    //std::cout<<"LFNEntry constructor"<<std::endl;
    m_attrs.extend<std::string>("guid");
    m_attrs.extend<std::string>("lfname");
    this->setValue(ilfname,iguid);
  }
  void LFNEntry::setValue( const std::string& ilfname, const std::string& iguid){
    m_attrs["guid"].data<std::string>()=iguid;
    m_attrs["lfname"].data<std::string>()=ilfname;
  }
  std::string LFNEntry::guid() const{
    return m_attrs["guid"].data<std::string>();
  }
  const std::string LFNEntry::lfname() const{
    return m_attrs["lfname"].data<std::string>();
  }
  void LFNEntry::toOutputStream(std::ostream& s) const{
    m_attrs.toOutputStream(s);
  }
  MetaDataEntry::MetaDataEntry(){
    m_attrs.extend<std::string>("guid");
  }
  unsigned int MetaDataEntry::size() const{
    return m_attrs.size();
  }
  std::string MetaDataEntry::guid() const{
    return m_attrs["guid"].data<std::string>();
  }
  coral::AttributeList& MetaDataEntry::attrs(){
    return m_attrs;
  }
  void MetaDataEntry::addAttributeSpec(const std::string& attr_name, const std::string& type_name ){
    if( type_name != "string" && type_name!="short" && type_name!="unsigned short"&& type_name!="int" && type_name!="unsigned int" && type_name!="long" && type_name!="unsigned long" && type_name!="long long" && type_name!="unsigned long long" && type_name!="float" && type_name!="double" && type_name!="long double"){
      throw pool::FCbackendException("MetaDataEntry::addAttributeSpec", std::string("unsuported type ")+type_name);
    }
    m_attrs.extend( attr_name, type_name);
  }
  void MetaDataEntry::setGuidValue(const std::string& iguid){
    m_attrs["guid"].data<std::string>()=iguid;
  }
  //void MetaDataEntry::setAttributeValueAsString( const std::string& attr_name, const std::string& t ){
    //m_attrs[attr_name].data<std::string>()=t;
  //}
  std::string MetaDataEntry::getGuidValue() const {
    return m_attrs["guid"].data<std::string>();
  }
  //std::string MetaDataEntry::getAttributeValueAsString( const std::string& attr_name ) const {
  //  return m_attrs[attr_name].data<std::string>();
  //}
  void MetaDataEntry::toOutputStream(std::ostream& s) const{
    m_attrs.toOutputStream(s);
  }
}//ns pool
