/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include <cassert>
#include <cstdlib> /* atoi */
#include <iostream>
#include <vector>

#include "GaudiKernel/IJobOptionsSvc.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ISvcLocator.h"

#include "AthenaKernel/DefaultKey.h"
#include "AthenaKernel/IClassIDSvc.h"

#include "AthenaKernel/IClassIDSvc.h"
#include "AthenaKernel/ClassID_traits.h"

#include "SGFolder.h"

/* #define SGFOLDER_DEBUG 1 */

using namespace SG;



//-----------------------------------------------------------------------------
Folder::Folder( const std::string& type, 
                const std::string& name,
                const IInterface* parent) : 
  AthAlgTool( type, name, parent ), 
  m_pCLIDSvc("ClassIDSvc", name),
  m_checkItems(false)
{
  declareProperty("ItemList", m_itemList,
		  " list of data objects identified by a class name (or clid)#key pairs. One can use '*' as key value to add all objects of a given type to the Folder.  If the type name ends with !, then write the object as exactly that type (and not as any derived class). ");
  m_itemList.declareUpdateHandler(&Folder::decodeItemList, this);
  declareProperty("CheckItems", m_checkItems, 
		  "check if item types are known to ClassIDSvc");
}

//-----------------------------------------------------------------------------
Folder::~Folder()
{}

//-----------------------------------------------------------------------------
StatusCode Folder::queryInterface(const InterfaceID& riid, void** ppvIf) {
  if ( riid == SG::IFolder::interfaceID() ) {
    *ppvIf = (IFolder*)this;
    addRef();
    return StatusCode::SUCCESS;
  }
  return AthAlgTool::queryInterface( riid, ppvIf );
}

//-----------------------------------------------------------------------------
StatusCode Folder::initialize() {
  return m_pCLIDSvc.retrieve();
}

//-----------------------------------------------------------------------------
void Folder::updateItemList(bool checkValid) {
  std::vector< std::string >::const_iterator 
    i(m_itemList.value().begin()), e(m_itemList.value().end());
  while (i != e) decodeItem(*i++, checkValid);
}

void Folder::decodeItem(const std::string& item, bool checkValid) {
  assert( !item.empty() );
  assert( m_pCLIDSvc );
#ifdef SGFOLDER_DEBUG
  //can't use MsgStream (log level still not defined)
  std::cout << "Folder::decodeItem("<< item<<") called" << std::endl;
#endif
  std::string::size_type sep(item.rfind('#'));
  std::string typeName (item.substr(0,sep));
  std::string skey;
  if (sep != std::string::npos) skey = item.substr(sep+1);

  //item contains a typename OR a CLID. Try the CLID hypothesis first 
  CLID clid(atoi(typeName.c_str()));  
  //atoi would return 0 if "typeName" is not a stringified number
  if ( 0 == clid) {
    //notice that if the typename is not yet in the ClassIDSvc registry the entry will be ignored
    add(typeName, skey).ignore();
  } else add(clid, skey, checkValid, false).ignore();
}

StatusCode 
Folder::add(const std::string& typeName, const std::string& skey) {
  bool exact = false;
  std::string tn = typeName;
  if (tn.size() > 0 && tn[tn.size()-1] == '!') {
    exact = true;
    tn.erase (tn.end()-1);
  }
  
  CLID clid;
  //let's see if the typename is known to ClassIDSvc
  StatusCode sc(m_pCLIDSvc->getIDOfTypeName(tn, clid));
  //if so, add the corresponding clid (no point in checking it again)
  if (sc.isSuccess()) sc=add(clid, skey, false, exact);
  else {
    MsgStream log(msgSvc(), name());
    log << MSG::WARNING << "add: can not find type ["
	<< typeName << "] in clid db" << endmsg;
  }
  return sc;
}

StatusCode
Folder::add(const CLID& clid, const std::string& skey,
            bool checkValid, bool exact)
{
  StatusCode sc(StatusCode::FAILURE);
  if ( !checkValid || m_pCLIDSvc->isIDInUse(clid) ) {
    m_list.insert(FolderItem(clid, skey, exact));
    sc = StatusCode::SUCCESS;
  } else if (0 != clid) {
    MsgStream log(msgSvc(), name());
    log << MSG::WARNING << "add: can not find clid "
	<< clid << " in clid db" << endmsg;
  }
#ifdef SGFOLDER_DEBUG
    std::cout << "SG::Folder::add(" << clid << ",\"" << skey << "\") returns " 
	      << (sc.isSuccess() ? "SUCCESS" : "FAILURE") << std::endl;
#endif     
  return sc;
}
