/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
// @file Cool2Json.h
// Header for Cool2Json class
// @author Shaun Roe
// @date 04 October 2017
#ifndef IOVDbSvc_Cool2Json_h
#define IOVDbSvc_Cool2Json_h

#include <string>
#include <iostream>

#include "CoolKernel/ValidityKey.h"//its a typedef
#include "CoolKernel/IFolder.h"
#include "CoolKernel/pointers.h" //IFolderPtr etc, typedefs

namespace cool{
  class ChannelSelection;
}


namespace IOVDbNamespace {
  //@brief Formatter helper class
  //Formats cool attributes, lists to json string representation and provides JSON
  class Cool2Json{
    //@stream insertion
    friend std::ostream& operator<<(std::ostream& o, const Cool2Json &c);
    //
    public:
    //class methods
    ///Produce a representation of a coral::Attribute as a json string
    static std::string jsonAttribute(const coral::Attribute&);
    
    ///Produce a representation of a coral::AttributeList as a json string
    static std::string jsonAttributeList(const coral::AttributeList&);
    
    //@Constructor sets internal variables
    Cool2Json(const cool::IFolderPtr & pFolder, const cool::ValidityKey & since, 
        const cool::ValidityKey & until, const cool::ChannelSelection & m_chansel, 
        const std::string& folderTag);
        
    ///Folder description string
    std::string description() const;
    
    ///Payload specification for this folder
    std::string payloadSpec() const;
    
    ///Payload (data for the given channel selection and iov)
    std::string payload();
    
    ///Formatted iov as '[<since>, <until>]'
    std::string iov() const;
    
    ///Number of channels in the folder
    unsigned int nchans() const;
    
    ///'time' (ns of epoch) or 'run-lumi'
    std::string iovBase() const;
    
    ///Simply the tag string passed to the constructor
    std::string tag() const;
    
    ///json open tag, '{'
    static std::string open();
    
    ///json close tag, '}'
    static std::string close();
    
    ///json standard delimiter ', '
    static std::string delimiter();
  
  
    private:
    enum FolderType {
      AttrList=0,
      AttrListColl,
      PoolRef,
      PoolRefColl,
      CoraCool,
      CoolVector,
      JSON,
      NUM_FOLDER_TYPES
    };
    const cool::IFolderPtr  m_pFolder;
    const cool::ValidityKey m_start;
    const cool::ValidityKey m_stop;
    const cool::ChannelSelection m_chansel;
    const std::string m_tag;
    const std::string m_desc;
    const std::string m_spec;
    unsigned int m_nchans;
    std::string formatCvp(const cool::IObjectIteratorPtr & itr);
    std::string formatAttrList(const cool::IObjectIteratorPtr & itr);
    std::string formatPoolRef(const  cool::IObjectIteratorPtr & itr);
  };
  
  
  std::ostream &operator<<(std::ostream &o, const Cool2Json &c);
  
  
}


#endif
